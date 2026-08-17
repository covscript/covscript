# Covariant Script SDK

本文档描述 Covariant Script 的嵌入（C++）API，以及每位嵌入者都必须遵守的**资源所有权规约**。Covariant Script 使用确定性的 RAII 管理全部内存——没有垃圾回收器——因此对象的生命周期是精确的，但必须被正确理解。

## 目录

+ [快速开始](#快速开始)
+ [所有权模型](#所有权模型)
+ [资源规约](#资源规约)
  + [1. Context 生命周期（逃逸对象）](#1-context-生命周期逃逸对象)
  + [2. `current_process` 与线程](#2-current_process-与线程)
  + [3. 结构体终结器（finalize）](#3-结构体终结器finalize)
  + [4. `var` 生命周期](#4-var-生命周期)
  + [5. Token arena 与重编译](#5-token-arena-与重编译)
+ [迁移指南（ABI 2608xx → ABI 2609xx）](#迁移指南abi-2608xx--abi-2609xx)

---

## 快速开始

最简单的嵌入方式是 `cs::bootstrap`：

```cpp
#include <covscript/covscript.hpp>

int main()
{
    cs::bootstrap env;                       // 持有一个 context
    env.run("script.csc");                   // 编译 + 解释执行
    // ... 通过 cs::eval(env.context, "...") 或 cs::function_invoker 使用 ...
}
```

如需更细粒度的控制，可直接构建 context：

```cpp
cs::array args;
args.push_back(cs::var::make<cs::string>("<MY_APP>"));
auto ctx = cs::create_context(args);         // context_t = shared_ptr<context_type>
ctx->instance->compile("script.csc");
ctx->instance->interpret();
```

核心句柄是 `cs::context_t`（`std::shared_ptr<cs::context_type>`），其余一切都挂在它上面：

| 对象 | 属主 | 生命周期 |
|---|---|---|
| `context_type` | 嵌入者的 `context_t` | 直到最后一个 `context_t` 被释放 |
| `process_context` | context（`context->process`） | context 生命周期（fork 出的 fiber 会保活父进程） |
| `instance_type` | context（`context->instance`） | context 生命周期 |
| `compiler_type` | context（`context->compiler`） | context 生命周期（与子 context 共享） |
| token arena（`compile_unit`） | instance | 直到重编译或 instance 析构 |
| `var` 值 | 引用计数 | 直到最后一个 `var` 引用被释放 |
| 模块子 context | context 的 `subcontexts` 池 | context 生命周期 |

## 所有权模型

Covariant Script 移除了历史遗留的全局垃圾回收器，改为显式、确定性的所有权：

+ **语句**构成一棵树；每个父节点删除其子节点。
+ **token** 存放在每个程序独立的 bump arena（`compile_unit`）中，整块释放。
+ **`var`** 是对堆上值的引用计数、写时复制句柄。拷贝一个 `var` 代价极低（引用计数 +1）；`cs::copy(var)` / `var::clone()` 执行深拷贝。
+ **context 是唯一属主**，拥有 process、compiler、instance 及其子对象。这些子对象对 context 的反向引用都是裸（非 owning）指针，因此释放最后一个外部 `context_t` 即可无环地回收整棵树。

结果是：**一切资源都在最后一个 owning 引用消失的那一刻被精确回收**——没有 `collect_garbage()`，没有延迟清扫。

## 资源规约

以下是嵌入者必须遵守的规则。违反它们即未定义行为（通常是 use-after-free）。

### 1. Context 生命周期（逃逸对象）

**逃逸对象要求其定义它的 context 保持存活。**

运行时返回的对象——脚本函数、lambda、`structure` 实例、模块命名空间——都**不拥有**它们的 context，而是持有指向它的非 owning 反向引用：

+ 脚本函数 / lambda 持有裸的 `context_type*`（`function::mContext`）；
+ `structure` 钉住其 owning process（因此类型身份节点与成员数据在 context 销毁后仍有效），但其*方法*是脚本函数，仍持有裸的 `function::mContext` 反向引用——调用它们要求 context 存活（否则抛 "the function's context has been destroyed"）。
+ 逃逸的**类型**（`type_t`）携带同样的反向引用；在 context 销毁后调用其构造器（`type_t::constructor()`）会抛出 "the struct's context has been destroyed"。

因此：

```cpp
cs::var f;
{
    auto ctx = cs::create_context({...});
    f = cs::eval(ctx, "[](x)->x+1");   // 逃逸一个 lambda
}                                      // 此处 ctx 已析构
f.const_val<cs::callable>().call(...); // 未定义行为
```

在仍使用对象期间，务必保持 `context_t` 存活：

```cpp
auto ctx = cs::create_context({...});
cs::var f = cs::eval(ctx, "[](x)->x+1");
// ... 在 ctx 存活期间自由使用 f ...
```

此规约对函数、lambda、结构体、模块命名空间一视同仁。（其他语言的运行时同样不保证逃逸值能比它的定义域活得更久；Covariant Script 只是把这一要求显式化。）

### 2. `current_process` 与线程

`cs::current_process` 是一个 `thread_local process_context*`，除非当前线程上有运行中的实例，否则为 `nullptr`。它由两个 RAII 守卫安装：

+ `cs::process_run_scope(ctx)` —— 在作用域内安装 `ctx` 的进程。同一进程上的嵌套作用域是透明的；**不同**的活动进程会抛出异常，因此一个线程不能并发运行两个实例。
+ `cs::process_activation(ctx)` —— 仅当无活动进程时才激活（用于裸原生调用与异步工作线程）。

推论：

+ 原生（CNI）回调与异步 future 可能观察到 `current_process == nullptr`；SDK 的错误路径已对此容错，但读取 `current_process` 的扩展必须做空判断。
+ 两个 context 可在**不同线程**上并发运行。`var` 的 proxy 分配器池是**每线程独立**的（`thread_local`）且按需增长，因此不同 context 永不共享池槽；值在其它线程释放时回退到直接分配路径（`std::allocator` 实例间可互换）。

### 3. 结构体终结器（finalize）

结构体的 `finalize` 方法在其**运行时仍存活时**被释放才会执行。对于块作用域变量，这在 `interpret` 期间自然发生；但全局变量只有在全局域被清空时才会释放。

对一次跑完的脚本，高层入口已替你处理：

+ `cs::bootstrap::~bootstrap()` 与 `cs` 命令行在执行后调用
  `instance->storage.clear_global()`。
+ REPL 在退出时清空全局域。

如果你手动驱动 `create_context` + `compile` + `interpret` 后直接丢弃 context 而不清空全局域，全局结构体将在 context 析构期间被释放——此时 process/instance 已开始消亡——其 `finalize` 将无法正确执行。请在 context 存活时（进程仍活动）调用 `ctx->instance->storage.clear_global()` 以确定性地执行终结器。

### 4. `var` 生命周期

`cs::var` 是一个 8 字节句柄；拷贝它只会让引用计数 +1，值在最后一个引用释放时被回收。要把值与其原始存储分离，请使用 `cs::copy(var)`（深拷贝）。逃逸出 context 的值作为“值”本身是安全的（它们自包含），但 `callable`/`structure` 值**同时**还携带着 §1 所述的裸反向引用，因此 context 规约依然适用于它们。

### 5. Token arena 与重编译

每次 `compile()` 都会生成一个程序，其 token 存放在每个实例独立的 arena 中。再次编译（或调用 `release_statements()`）会丢弃上一个程序及其 arena。

从上一个程序逃逸的脚本函数/lambda 会通过 `function::m_unit` 保活该 arena，因此它在重编译后仍可调用——**前提是其 context 仍存活**（§1）。这是唯一保留下来的保活机制，因为它针对的是 arena，而非 context。

### 6. 运行时诊断（`COVSCRIPT_DEBUG`）

防御性运行时守卫（例如销毁未完成的 fiber，或程序入口处函数值栈非空）按照 `COVSCRIPT_DEBUG` 环境变量行事：

| 取值 | 行为 |
| :-- | :-- |
| `none` | 静默忽略：程序继续运行，接受部分泄漏 |
| `warning` | 向 stderr 打印警告并继续（默认） |
| `strict` | 打印警告后立即中止（fail-fast） |

取值大小写不敏感；未设置或非法值默认按 `warning` 处理。守卫仅用于诊断，不改变上述所有权契约。

## 迁移指南（ABI 2608xx → ABI 2609xx）

所有扩展必须重新编译。

- `cs::current_process` 类型改为 `current_process_ref`——用法不变
  （`->`、`*`、`== nullptr` 均兼容）。
- `process_context::raise_sigint()` / `raise_exit()` 已移除——改用
  `cs::global_signals.raise_sigint()` / `raise_exit()`。
- DLL 入口签名变更——使用新头文件重新编译即可。
- `COVSCRIPT_DEBUG` 环境变量控制运行时诊断级别
  （`none` / `warning` / `strict`）。

### 新增 API

- `cs::process_run_scope` / `cs::process_activation` —— `current_process`
  管理的 RAII 守卫。
- `cs::global_signals` —— 全局信号控制（替代已移除的
  `process_context::raise_sigint/exit`）。
- `cs::fiber::schedule_parameters` / `get_schedule_parameters()` /
  `set_schedule_parameters()` —— 调整 fiber 退避参数。
- `cs::callable::argument_count()` —— 查询函数参数数量。
- `cs::create_context` 新增可选的 `stack_size` 参数。
