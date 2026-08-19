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
  + [6. 运行时诊断（`COVSCRIPT_DEBUG`）](#6-运行时诊断covscript_debug)
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
| `process_context` | context 与逃逸的 structure | 直到最后一个属主释放 |
| `instance_type` | context（`context->instance`） | context 生命周期 |
| `compiler_type` | context（`context->compiler`） | context 生命周期（与子 context 共享） |
| token arena（`compile_unit`） | instance 与逃逸的函数 | 直到最后一个属主释放 |
| `var` 值 | 引用计数 | 直到最后一个 `var` 引用被释放 |
| 模块子 context | context 的 `subcontexts` 池 | context 生命周期 |

## 所有权模型

Covariant Script 移除了历史遗留的全局垃圾回收器，改为显式、确定性的所有权：

+ **语句**构成一棵树；每个父节点删除其子节点。
+ **token** 存放在每个程序独立的 bump arena（`compile_unit`）中，整块释放。
+ **`var`** 是对堆上值的引用计数、写时复制句柄。拷贝一个 `var` 代价极低（引用计数 +1）；`cs::copy(var)` / `var::clone()` 执行深拷贝。
+ **context 是运行时根属主**，拥有 process、compiler、instance 及其子对象。大多数内部子对象使用受 context 生命周期约束的裸非 owning 反向引用；逃逸的脚本函数使用 `std::weak_ptr`。逃逸函数与 structure 可分别延长 token arena 或 process 的生命周期，但不会保活 context，也不会形成所有权环。

结果是：**每项资源都在其最后一个 owning 引用消失时被精确回收**——没有 `collect_garbage()`，没有延迟清扫。

## 资源规约

以下是嵌入者必须遵守的规则。由弱引用支持的 API 在 context 销毁后会抛出 `runtime_error`；裸非 owning SDK 引用不得比其属主活得更久。

### 1. Context 生命周期（逃逸对象）

**需要执行脚本代码的逃逸对象，要求创建它的 context 保持存活。**

运行时返回的对象并不具有完全相同的生命周期行为：

+ 逃逸的脚本函数 / lambda 持有 `std::weak_ptr<context_type>`（`function::mContext`）；context 销毁后调用会抛出 `runtime_error`；
+ `structure` 会 pin 住其所属的 process，因此类型身份与成员数据在 context 销毁后仍有效；其脚本方法仍要求定义它的 context 存活，销毁后调用会抛出异常。
+ 逃逸的**类型**（`type_t`）携带同样的反向引用；context 销毁后调用其构造器（`type_t::constructor()`）会抛出 "the struct's context has been destroyed"。

因此：

```cpp
cs::var f;
{
    auto ctx = cs::create_context({...});
    f = cs::eval(ctx, "[](x)->x+1");   // 逃逸一个 lambda
}                                      // 此处 ctx 已析构
f.const_val<cs::callable>().call(...); // 抛出 runtime_error
```

在仍使用对象期间，务必保持 `context_t` 存活：

```cpp
auto ctx = cs::create_context({...});
cs::var f = cs::eval(ctx, "[](x)->x+1");
// ... 在 ctx 存活期间自由使用 f ...
```

逃逸对象需要执行脚本代码时，应保持 context 存活；如上所述，自包含的值数据可独立继续使用。

### 2. `current_process` 与线程

`cs::current_process` 是由线程局部存储支持的 `process_context_ref`；它可转换为 `process_context*`，当前线程无运行实例时为 `nullptr`。`cs::process_run_scope(ctx)` 在作用域内安装 `ctx` 的进程。同一进程上的嵌套作用域是透明的；**不同**的活动进程会抛出异常，因此一个线程不能并发运行两个实例。原生代码调用 `callable::call()` 时需自行持有 `process_run_scope`。

推论：

+ 原生（CNI）回调与异步 future 可能观察到 `current_process == nullptr`；SDK 的错误路径已对此容错，但读取 `current_process` 的扩展必须做空判断。
+ 两个 context 可在**不同线程**上并发运行。`var` 的 proxy 分配器池是**每线程独立**的（`thread_local`）且按需增长，因此不同 context 永不共享池槽；值在其它线程释放时回退到直接分配路径（`std::allocator` 实例间可互换）。

需要独立编译环境（独立存储、命名空间或编译产物）的扩展 DLL 应使用**子上下文**而非独立上下文：

```cpp
// 扩展函数内——宿主进程活跃时
auto ctx = cs::create_subcontext(host_ctx); // 共享进程，独立存储
ctx->instance->compile(in);
ctx->instance->interpret();
// ctx 离开作用域：子上下文销毁，宿主不受影响
```

子上下文共享父进程（`process_run_scope` 透明）和父上下文的编译器（编译期间绑定到子上下文），但拥有独立的存储和 token arena。这与模块导入使用的机制相同。

### 3. 结构体终结器（finalize）

结构体的 `finalize` 方法在其**运行时仍存活时**被释放才会执行。对于块作用域变量，这在 `interpret` 期间自然发生；但全局变量只有在全局域被清空时才会释放。

对一次跑完的脚本，高层入口已替你处理：

+ `cs::bootstrap::~bootstrap()` 与 `cs` 命令行在执行后调用
  `instance->storage.clear_global()`。
+ REPL 在退出时清空全局域。

如果你手动驱动 `create_context` + `compile` + `interpret`，且析构时同一线程上没有无关活动 process，终结器会在 context teardown 期间执行。为获得确定性行为，应在 context 自身 process 活动时调用 `ctx->instance->storage.clear_global()` 后再释放；若析构时另一个无关 process 仍活动，则必须显式清理。

### 4. `var` 生命周期

`cs::var` 是一个指针大小的句柄（64 位平台上为 8 字节）；拷贝它只会让引用计数 +1，值在最后一个引用释放时被回收。要把值与其原始存储分离，请使用 `cs::copy(var)`（深拷贝）。逃逸出 context 的自包含值数据仍然安全；脚本 callable 使用弱 context 引用并在 context 销毁后调用时抛出异常，structure 数据仍有效，但其脚本方法遵循相同规则。

### 5. Token arena 与重编译

每次 `compile()` 都会生成一个程序，其 token 存放在每个实例独立的 arena 中。再次编译（或调用 `release_statements()`）会丢弃上一个程序及其 arena。

从上一个程序逃逸的脚本函数/lambda 会通过 `function::m_unit` 保活该 arena，因此它在重编译后仍可调用——**前提是其 context 仍存活**（§1）。这是唯一保留下来的保活机制，因为它针对的是 arena，而非 context。

### 6. 运行时诊断（`COVSCRIPT_DEBUG`）

防御性运行时守卫（例如销毁未完成的 fiber、程序入口处函数值栈非空，或结构体 finalize 方法在清理阶段抛出异常）按照 `COVSCRIPT_DEBUG` 环境变量行事：

| 取值 | 行为 |
| :-- | :-- |
| `none` | 静默忽略：程序继续运行，接受部分泄漏 |
| `warning` | 向 stderr 打印警告并继续（默认） |
| `strict` | 打印警告后立即中止（fail-fast） |

取值大小写不敏感；未设置或非法值默认按 `warning` 处理。守卫仅用于诊断，不改变上述所有权契约。

## 迁移指南（ABI 2608xx → ABI 2609xx）

所有扩展必须重新编译。

- `cs::current_process` 类型改为 `process_context_ref`——用法不变
  （`->`、`*`、`== nullptr` 均兼容）。
- `process_context::raise_sigint()` / `raise_exit()` 已移除——改用
  `cs::global_signals.raise_sigint()` / `raise_exit()`。
- DLL 入口签名变更——使用新头文件重新编译即可。
- `COVSCRIPT_DEBUG` 环境变量控制运行时诊断级别
  （`none` / `warning` / `strict`）。
- `process_context::on_process_sigint` 默认处理器从"退出"改为"忽略"。
  嵌入方如需 Ctrl+C 终止，应添加监听器：
  `ctx->on_process_sigint.add_listener([](void*) -> bool { /* 关闭逻辑 */ return true; })`。
- 信号标志为 OS 进程级（`cs::global_signals`）。多嵌入进程并发时，
  先 poll 的进程消费挂起信号；无进程 poll 时信号保留至下次 poll。
- `cs::invoke` 对非 callable 对象改抛 `cs::lang_error`（原为
  `cs::runtime_error`），与脚本层行为一致。注意 `cs::lang_error`
  不继承 `std::exception`。
- `constant` 声明和 `case` 标签中，值包含脚本函数或方法的现在编译期
  拒绝（此前值会被静默折叠进 token 树，形成 arena↔function 环）。
- `process_activation` 已移除。原生代码调用 `callable::call()` 时需
  自行持有 `process_run_scope`。扩展 DLL 通过访问器共享宿主的线程
  局部槽——读写统一，无需手动管理访问器。

### 新增 API

- `cs::process_run_scope` —— `current_process` 管理的 RAII 守卫。
- `cs::global_signals` —— 全局信号控制（替代已移除的
  `process_context::raise_sigint/exit`）。
- `cs::fiber::schedule_parameters` / `get_schedule_parameters()` /
  `set_schedule_parameters()` —— 调整 fiber 退避参数。
- `cs::callable::argument_count()` —— 查询函数参数数量。
- `cs::create_context` 新增可选的 `stack_size` 参数。
