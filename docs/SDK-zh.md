# Covariant Script SDK

本文档描述 Covariant Script 的嵌入（C++）API，以及每位嵌入者都必须遵守的**资源所有权规约**。Covariant Script 使用确定性的 RAII 管理全部内存，因此对象的生命周期是精确的，但必须被正确理解。

## 目录

+ [快速开始](#快速开始)
+ [所有权模型](#所有权模型)
+ [资源规约](#资源规约)
  + [1. Context 生命周期](#1-context-生命周期)
  + [2. `current_process` 与线程](#2-current_process-与线程)
  + [3. 结构体终结器（finalize）](#3-结构体终结器finalize)
  + [4. `var` 生命周期](#4-var-生命周期)
  + [5. Token arena 与重编译](#5-token-arena-与重编译)
  + [6. 运行时诊断（`COVSCRIPT_DEBUG`）](#6-运行时诊断covscript_debug)
+ [迁移指南（ABI 2608xx → ABI 2609xx）](#迁移指南abi-2608xx--abi-2609xx)
+ [迁移指南（ABI 2609xx → ABI 2610xx）](#迁移指南abi-2609xx--abi-2610xx)

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
| `process_context` | context | context 生命周期 |
| `instance_type` | context（`context->instance`） | context 生命周期 |
| `compiler_type` | context（`context->compiler`） | context 生命周期（与子 context 共享） |
| token arena（`compile_unit`） | instance、函数与 struct_builder | 直到最后一个属主释放 |
| `var` 值 | 引用计数 | 直到最后一个 `var` 引用被释放 |
| 模块子 context | context 的 `subcontexts` 池 | context 生命周期 |

## 所有权模型

Covariant Script 移除了历史遗留的全局垃圾回收器，改为显式、确定性的所有权：

+ **语句**构成一棵树；每个父节点删除其子节点。
+ **token** 存放在每个程序独立的 bump arena（`compile_unit`）中，整块释放。
+ **`var`** 是对堆上值的引用计数、写时复制句柄。拷贝一个 `var` 代价极低（引用计数 +1）；`cs::copy(var)` / `var::clone()` 执行深拷贝。
+ **context 是运行时根属主**，拥有 process、compiler、instance 及其子对象。脚本对象内部都只是指向其所属 context 的裸指针；一旦 context 销毁，这些指针全部失效，再使用属于未定义行为。

## 资源规约

以下是嵌入者必须遵守的规则。脚本对象内部持有的是指向所属 context 的裸指针，一旦 context 销毁即全部失效；context 销毁后再使用任何逃逸出来的脚本对象，都是未定义行为。

### 1. Context 生命周期

**所有脚本对象均假定定义它的 context 存活。**

在 context 销毁后调用脚本函数、结构体方法等均是**未定义行为**——不执行检查，不保证抛出异常。

因此：

```cpp
cs::var f;
{
    auto ctx = cs::create_context({...});
    f = cs::eval(ctx, "[](x)->x+1");   // 逃逸一个 lambda
}                                      // 此处 ctx 已析构
cs::invoke(f, 1); // 未定义行为——切勿如此
```

在仍使用对象期间，务必保持 `context_t` 存活：

```cpp
auto ctx = cs::create_context({...});
cs::var f = cs::eval(ctx, "[](x)->x+1");
// ... 在 ctx 存活期间自由使用 f ...
```

只要还用着逃逸出来的对象，就必须保持 context 存活；context 销毁后，逃逸出来的对象（含结构体成员数据）一律不可再用。

### 2. `current_process` 与线程

`cs::current_process` 是由线程局部存储支持的 `process_context_ref`；它可转换为 `process_context*`，当前线程无运行实例时为 `nullptr`。`cs::process_run_scope(ctx)` 可以在作用域内注册 Context 所持有的进程，并在离开作用域时自动注销。同一进程上的嵌套作用域是透明的；**不同**的活动进程会抛出异常，因此一个线程不能并发运行两个实例。原生代码调用 `callable::call()` 时需自行持有 `process_run_scope`。

推论：

+ 原生（CNI）回调与异步 future 可能观察到 `current_process == nullptr`；SDK 的错误路径已对此容错，但读取 `current_process` 的扩展必须做空判断。
+ 两个 context 可在**不同线程**上并发运行。`var` 的 proxy 分配器池是**每线程独立**的（`thread_local`）且按需增长，因此不同 context 永不共享池槽；值在其它线程释放时回退到直接分配路径（`std::allocator` 实例间可互换）。但 context 之间共享状态是危险的，因为 `var` 的引用计数是非原子的。

需要独立编译环境（独立存储、命名空间或编译产物）的扩展 DLL 应使用**子上下文**而非独立上下文：

```cpp
// 扩展函数内——宿主进程活跃时
auto ctx = cs::create_subcontext(host_ctx); // 共享进程，独立存储
ctx->instance->compile(in);
ctx->instance->interpret();
// ctx 离开作用域：子上下文销毁，宿主不受影响
```

子上下文共享父进程（`process_run_scope` 透明）和父上下文的编译器（编译期间绑定到子上下文），但拥有独立的存储。这与模块导入使用的机制相同。

### 3. 结构体终结器（finalize）

结构体的 `finalize` 方法在其**运行时仍存活时**被释放才会执行。对于块作用域变量，这在 `interpret` 期间自然发生；但全局变量只有在全局域被清空时才会释放。

对一次跑完的脚本，高层入口已替你处理：

+ `cs::bootstrap::~bootstrap()` 与 `cs` 命令行在执行后调用
  `instance->storage.clear_global()`。
+ REPL 在退出时清空全局域。

如果你手动驱动 `create_context` + `compile` + `interpret`，且析构时同一线程上没有无关活动 process，终结器会在 context teardown 期间执行。为获得确定性行为，应在 context 自身 process 活动时调用 `ctx->instance->storage.clear_global()` 后再释放；若析构时另一个无关 process 仍活动，则必须显式清理。

### 4. `var` 生命周期

`cs::var` 是一个指针大小的句柄（64 位平台上为 8 字节）；拷贝它只会让引用计数 +1，值在最后一个引用释放时被回收。要把值与其原始存储分离，请使用 `cs::copy(var)`（深拷贝）。值一旦逃逸出 context，context 销毁后即不可再用。脚本 callable 内部只是指向函数的裸指针，context 销毁后再调用是未定义行为；结构体的成员数据与类型身份同理。

### 5. Token arena 与重编译

每次 `compile()` 都会生成一个程序，其 token 存放在每个实例独立的 arena 中。再次编译（或调用 `release_statements()`）会丢弃上一个程序及其 arena。

从上一个程序注册到函数存储的脚本函数/lambda 会通过 `function::m_unit` 保持该 arena 存活，因此它在重编译后仍可调用——**前提是其 context 仍存活**（§1）。这是唯一保留的保持存活机制，它只关乎 arena，与 context 无关。

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

## 迁移指南（ABI 2609xx → ABI 2610xx）

所有扩展必须重新编译。

### 破坏性变更

- **`function_ptr` 不再持有 `owner`**。`function_ptr` 从
  `{function*, shared_ptr<function>}` 精简为裸指针 `{function*}`。
  直接构造 `function_ptr(f, owner)` 或访问 `.owner` 的代码必须改掉。
- **`contains_callable` 全家移除**。`callable_contains_function`、
  `callable_is_member_function` 等检测 callable 是否由脚本函数支撑的
  函数已删除。
- **逃逸行为变更：UB 替代抛异常**。逃逸的脚本对象（函数、结构体方法、
  类型构造器、fiber 等）在 context 销毁后使用，不再抛
  `runtime_error`，而是未定义行为。`catch` 这类异常的代码不再生效。
- **`function::get_context()` 返回裸指针**。原返回
  `shared_ptr<context_type>`，现返回 `context_type*`。
- **`process_context::teardown_ctx()` 已移除**。
- **`structure::m_process` 改为 `std::weak_ptr<process_context>`**。原为
  `shared_ptr<process_context>`；weak 引用仅作存活探测（不钉住 process）。
  `run_finalize` 在 process 已死时跳过并通过 `debug_guard` 报告。
- **命名函数改为编译时注册**。`statement_function::mFunc` 从
  `unique_ptr<function>` 改为 `function*`（由 `function_store` 持有）。

### 新增 API

- `cs::invoke(func, args...)` —— 统一的 callable 调用入口，替代
  `func.val<cs::callable>().call(args)`。
