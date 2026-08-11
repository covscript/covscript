# CovScript 代码审阅报告（fix_import 分支终审）

日期：2026-08-11
分支：`fix_import`（HEAD `1451e08`，72 提交，约 2100 行改动）
范围：核心对象模型与内存（`variable.hpp`/`components.hpp`/`core.hpp`）、编译器管线（`compiler`/`codegen`/`parser`/`lexer`）、解释器·实例·运行库（`instance`/`interpreter`）、fiber 运行时（`system/common`/`win32`/`unix`）、调试器与 CLI、新增单元测试与回归脚本（`test_variable`/`test_cni`/`test_regressions`）。

## 0. 概要

对 `fix_import` 分支进行终审。逐文件通读全量 diff，并对关键结论用实际构建（`cs`/`cs_dbg`/`cs_unit_tests`/扩展）与运行验证核实：133 个单元测试全部通过，CSP 回归脚本全部通过，扩展在新 ABI 下可正常加载。

总体结论：本分支修复了大量真实缺陷（值模型强异常保证、fiber 值栈隔离、导入缓存、循环导入、`int_pow`/除法/模零、`hash_set.clear`、REPL `loop_depth`、编译期 `break`/`continue` 校验、常量折叠副作用等），架构与内存语义经推演与实测一致，**未发现新的严重/高危问题**。审阅中发现并修复的 4 项问题为：

1. **1 项实测确认的正确性回归**（NaN 混合 int/float 比较 `>`/`>=`，见 R1）；
2. **3 项 fiber 运行时缺陷**（见 R2/R3/R4，均为评审中发现并已在本次审阅中修复，附回归测试）；
3. **若干刻意取舍 / 行为变更**（foreach 原地迭代、`switch` 内 `break` 编译期拒绝、悬置 fiber 断言、常量数组负下标可写、三层 fiber 混用值栈残余）——需在文档中显式声明，避免 reviewer 重复提出。

## 1. 待修复问题

### R1 [中] NaN 混合 int/float 比较 `>` / `>=` 结果错误（实测回归 · 已修复）

`include/covscript/core/components.hpp`：`compare_int_float`/`compare_float_int` 对 NaN 返回哨兵值 `2`，原 `abocmp`（`>`）/`aeqcmp`（`>=`）以 `> 0`/`>= 0` 判定，导致 `2 > 0` 为真：

```
float-float: NaN > 1   false   （IEEE 正确）
int-float:   1 > NaN   true    ← 错误（修复前）
int-float:   1 >= NaN  true    ← 错误（修复前）
```

- 引入自 `compare_int_float` 重构；master 上为 `data._num > rhs.data._int`（NaN 直比 → false）。`<`/`<=`/`==`/`!=` 均正确。
- NaN 脚本可达：`1 > (0.0/0.0)`、`sqrt(-1)` 等。
- **修复**：新增 `compare_greater`（`c == 1`）与 `compare_greater_equal`（`c == 0 || c == 1`）两个排序判定辅助，`>`/`>=` 的混合 int/float 分支改用之，NaN 统一为"无序"（false）。已实测 C++ 与脚本双路径：NaN 六种比较与 float-float 参考一致，且精确排序（`9007199254740993 > 9007199254740992.0`、`2 > 1.5`、`1 >= 1.0`）不受影响。新增回归测试 `numeric_nan_comparisons_unordered`（`unit_tests/test_regressions.cpp`）。

### R2 [中] 原生 fiber resume 脚本 fiber 后 `current_process` 未恢复（评审发现 · 已修复）

`sources/system/win32/common.cpp` / `sources/system/unix/common.cpp` 的 `fiber::resume`：子 fiber 挂起/结束后，栈非空时仅调用剩余调用者的 `cs_swap_in()`。脚本 fiber 的 `cs_swap_in` 会切到其 fork 出的 `process`，而原生 fiber **没有私有 process**，其 `cs_swap_in` 为 no-op，于是 resume 返回后 `current_process` 仍停留在被 resume 的脚本 fiber 的进程上。

- 触发：原生 fiber 体内 resume 一个脚本 fiber（脚本侧 `fiber.create(fiber.resume, sf)`，或 `fiber.get_future(sf).get()` 等）。`current_process` 错误持续到原生 fiber 结束，期间其返回值栈/`poll_event`/`fiber_cxt` 等全部指向错误的进程，并可能污染被挂起脚本 fiber 的保存栈。
- 已用 SDK 独立程序实测确认（native fiber body 内 resume 前后 `current_process` 从主进程变为脚本 fiber 进程）。
- **修复**：pop 后先无条件调用被 resume 的 `fi->cs_swap_out()`（恢复 `resumer_process`），再重新绑定剩余调用者；已按此修改 win32/unix 两处，并新增回归测试 `native_fiber_resumes_script_fiber_restores_process`（`unit_tests/test_fiber.cpp`）。修复后全套 129 项单元测试及 fiber 相关 CSP 脚本通过。

### R3 [中] fiber `fork()` 的退出/中断转发监听器捕获父进程裸指针（评审发现 · 已修复）

`sources/covscript.cpp`（`process_context::fork`）：原实现子进程 `on_process_exit`/`on_process_sigint` 的转发监听器以裸指针捕获 `current_process`（父进程）。当父进程是另一个 fiber fork 出的进程时，父 fiber 先消亡则指针悬垂 → 子 fiber 后续调用 `system.exit`/SIGINT 触发 use-after-free。

- 触发：脚本 fiber A 创建脚本 fiber B 后 A 被销毁，B 存活并调用 `system.exit`（已实测：销毁 A 后 B 的转发链指向已释放的 A 进程；未崩溃属内存未复用的侥幸 UB）。
- **修复（生成链保活）**：`process_context` 持有 `std::shared_ptr<process_context> m_parent`（强引用生成链），`fork()` 经 `current_owner()` 沿现有 fiber 链解析父进程的 shared_ptr 并强持有之；转发监听器直接 `parent->touch`，`parent == null` 即"父进程是根"，转发给 `this_process`。父进程随存活后代保留（保活语义），无兜底、无悬垂；fiber 创建的 fiber 仍经父进程转发（父进程上的用户监听器照常触发）。`this_process` 保持值对象不变。
- 新增回归测试：`grandchild_fiber_exit_after_parent_destroyed`（父 fiber 消亡后子 fiber 退出码仍可达根）、`fiber_exit_forwards_through_live_parent`（父进程上的监听器在子 fiber 退出时触发，链语义保留）。

### R4 [中] fiber 内 `system.exit`：退出码丢失 + CS_EXIT 哨兵被双重包裹丢失（评审发现 · 已修复）

两层独立缺陷叠加，均因 fiber 进程隔离引入：

1. `include/covscript/core/cni.hpp:684`（`cni::operator()`）：CNI 抛出的任何 `std::exception`（**含已带文件/行定位的 `cs::exception`**）都被 `std_eh_callback` 转成 `forward_exception(e.what())`，丢失裸消息。主路径 exit 因 `strip_prefix("Fatal Error: CS_EXIT")` 恰好恢复 "CS_EXIT" 而幸存；fiber 内 exit 会被**二次包裹**（fiber 内 call_rr 一次 + CNI 边界一次），最终 `raw_error_message` 得到 "File …, line …: CS_EXIT" 而非哨兵 → 报错、`EXIT=-1`。
2. `sources/interpreter.cpp:267/338`、`sources/debugger.cpp:587` 的 exit 监听器写 `cs::current_process->exit_code`，fiber 内 `current_process` 是 fiber 的进程 → `main()` 读到主进程的 `exit_code=0`，退出码丢失。

- 实测：fiber 内 `system.exit(3)` 之前输出 "File …, line 5: File …, line 3: CS_EXIT" 且 `EXIT=-1`（应为 3）。
- **修复**：① `cni::operator()` 对 `cs::exception` 直接 `throw;`（保留定位与裸消息）；② 三个 exit 监听器捕获注册时的主进程指针写 `exit_code`。修复后 fiber 与主路径 `EXIT=3` 均正确。新增回归测试 `system_exit_from_fiber_dispatches_code`、`fiber_exit_sentinel_keeps_bare_message`。

## 2. 既定取舍与行为变更（建议文档化，供 reviewer 知悉）

以下均为本分支**有意为之**或**既有边界**，代码中已有注释，但建议收敛到文档，避免重复评审：

### D1 [低] `foreach` 恢复原地迭代（撤销快照拷贝）
`sources/instance/statement.cpp:770-793`：撤销了 18e1a51 的快照修复，注释明确说明性能取舍。循环体内修改容器（push/pop/insert）会失效 deque 迭代器，可能触发堆损坏（快照修复当初即因此引入）。建议在语言文档明确"循环内修改容器需显式 clone"。

### D2 [低] `switch` 不视为循环：非循环包裹的 `switch` 内 `break` 现为编译错误
`sources/compiler/codegen.cpp:498-509`（`method_break/continue::translate`）＋ `include/covscript/impl/compiler.hpp:258`（`is_loop_block` 不含 `switch_`）。`break` 在 `switch` 内仅当存在外层循环时合法（语义为控制外层循环）。回归测试 `break_in_switch_without_loop_rejected` 已钉住该行为。属语言行为收紧，需在变更说明中声明。

### D3 [低] 悬置 fiber 析构：Debug 构建 `assert(false)` 会中断进程
`sources/system/win32/common.cpp:232-246`（unix 同）：`var f = fiber.create(fn); f.resume(); f = null;` 为合法脚本场景。当前 Release 构建（`-DNDEBUG`）仅打印警告并 `DeleteFiber`（泄漏修复有效），但真正 Debug 构建将直接 abort。建议将 assert 降级为纯警告。

### D4 [低] 常量数组负下标读取会"静默修改"常量
`include/covscript/impl/type_ext.hpp:1311-1327`（`index_ref<cs::array>`）：`constant a={1,2,3}; a[-4]` 读取时经 `cindex→index→index_ref` 的 `const_cast` 链路在常量前补零（`a[0]` 变 0）。master 上此处为无界 `emplace_back`（OOM/挂死），本分支转为"已定义但可写常量"，属既有 const 可变性漏洞的延伸。建议在 `index_ref` 中校验 `protect_level`。

### D5 [Info] 缓办问题清单文档被删除
`docs/Known-issues.md`（含 F4 `member_visitor` 裸引用悬垂、J1 `runtime.import`/`context.import`/`source_import` 吞异常返回 `null`、J2 `source_import` 不注册 csym、全局 GC 内存随 bootstrap 累积等）与历次审计文档均被删除。开放项仍存在，reviewer 可能重新发现。建议保留一份精简 Known-issues 文档。

### D6 [Info] 已接受的既定边界
- 全局 GC：`collect_garbage`/`on_process_exit_default_handler` 不再回收 statement/token/method 池，内存累积至进程退出（`sources/covscript.cpp` 注释已说明），换取 function_ptr 无 UAF。
- lambda `function` 进入进程生命周期池（`sources/compiler/compiler.cpp:32`），永不回收。
- 类型节点 `alloc_type_node` 进程生命周期池（`sources/covscript.cpp:181`），地址永不复用；非线程安全，依赖"结构体仅单线程定义"。
- `runtime.import` 吞异常返回 `null` 为既有 API 契约（`test_import_fail_cache.csc` 依赖）。

### D7 [Info] 值栈（`instance->fiber_stack`）在"脚本 fiber→原生 fiber→脚本 fiber"三层混用场景不还原
脚本 fiber A 内 resume 原生 fiber N，N 再 resume 脚本 fiber B，B 挂起返回后：`fi->cs_swap_out()` 将 `instance->fiber_stack` 置空，而 N（无 cs_context）的 `cs_swap_in` 不恢复它。`current_process`（R2 已修）正确，但 `fiber_stack` 为 null 而非 A 的 `cs_stack`。仅当 N 在该窗口内执行脚本代码（经 CNI 调 `cs::invoke` 等）才受影响（变量作用域错位，非崩溃）；N 结束回到 resume(N) 时会经 A 的 `cs_swap_in` 恢复。场景极难触发，权衡后缓办，可在 `cs_swap_in/out` 中保存/还原先前 `fiber_stack` 彻底修复。

## 3. 已验证项（关键风险点逐一核实，未发现问题）

- `basic_var` 值模型：`copy_store` 强异常保证、SVO/堆 `op_move`（move 后析构源、堆指针转移 + move_store 置空 dispatcher，无双 free）、空 var `swap`、SSO 字符串移动语义、构造抛异常时回收代理/块（`test_variable.cpp` 全绿）。
- 线程局部分配器跨线程 free 安全：底层为 `std::allocator`（全局 new/delete），池仅缓存指针，无归属冲突。
- `int_pow`：`would_overflow` 符号感知边界逐例验证正确（`(-2)^63 == INT64_MIN` 精确、`(-2)^64` 回退 float、`INT_MIN % -1`/`INT_MIN / -1` 处理正确）。
- fiber：`current_process`/`resumer_process` 恢复路径（含嵌套 resume、native/script fiber 混用）、值栈隔离（`swap_context` 指针切换）、`fiber_function` 持有 `callable owner` 防悬垂，逻辑自洽；`test_fiber_depth2`/`test_fiber_cross_caller` 等通过（三层混用值栈残余见 D7）。
- fiber 退出/中断链路：CNI 边界 `cs::exception` 原样重抛、`fork()` 转发至根进程、`exit_code` 写主进程，均经单元测试与 `cs.exe`/`cs_dbg.exe` 实测（fiber 与主路径 `system.exit(N)` 均返回 N）。
- 导入：`normalize_path` 缓存键、循环/自导入（`test_circular_*`/`test_self_import`）、失败回滚（`modules.erase` + FIFO/`import_base` 截断）、`context_swap_guard` RAII，均正确。
- 编译器：`restore_pool` 作用域安全（statement 持有自己的 var 副本，共享 proxy 的 `mark_constant` 在截断前生效）、`import_results` FIFO 消除共享单例覆盖、`opt_expr` 非 signal 节点早退、写语义运算符不再折叠、lambda `inside_lambda` 用 `value_guard` 还原。
- `type_node` 继承集合物化与 `is_a` O(1) 判定、`clear()` 重建 `m_ref` 使缓存 var_id 失效重解析、`get_var_no_check` 去 `noexcept`。
- `relocate_to_csym` 边界（拒绝 0、允许末行）、`extension` 空指针检查 + 句柄释放、`local_time/utc_time` 空指针检查、`parse_mode` 校验、`chmod_r` 不碰根目录、`copy` 区分目录/文件。
- CLI/调试器：`expect_stack_resize` 参数校验修复、SIGINT 信号安全 `write`、Ctrl+Break 协作式退出、非 tty 跳过确认/等待、精确哨兵匹配（`raw_error_message`）、`add_func` 不再双转发。

## 4. 结论

**R1**（NaN `>`/`>=`）、**R2**（原生 fiber resume 脚本 fiber 后 `current_process` 未恢复）、**R3**（`fork()` 转发监听器悬垂 UAF）、**R4**（fiber 内 `system.exit` 退出码丢失与哨兵双重包裹）均已在本轮审阅中修复并附回归测试（133 项单元测试全部通过，CSP 回归脚本全部通过）。`D1`–`D7` 为有意取舍或低风险遗留（含一处极罕见三层 fiber 混用的值栈残余），建议以文档形式固化，以便后续 reviewer 不再提出。除此之外，本分支未发现其他重要问题。
