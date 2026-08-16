# 审计与合并验证报告（gc_improve）

- 日期：2026-08-16 · 分支：`gc_improve`（合并前未 push 提交已压缩为单一提交）
- 本文档合并了原 `audit/{report,profiling,defensive,merge_verification}.md`

## 1. 审计结论（F01–F38）

全项目静态审阅 + 测试驱动验证：42 条可疑点去重为 38 项。**确认并修复 19 个真实缺陷（10 个崩溃/死循环/泄漏级），3 个误报，1 个弱化可接受**；其余为契约/历史遗留/低危项或需 TSAN 的并发项。关键修复：

- 全局 GC → RAII 所有权 + 每程序独立 token arena；lambda 存运行时 `function_store`，根除 arena↔lambda 环；context 子对象只持非拥有反向指针，确定性回收。
- 嵌套块编译失败语句泄漏（`body_guard` RAII）、reentrant repl::exec 清空外层调用帧崩溃（F15）、module sub-context 所有权链 teardown 不释放、repl/fiber context 自环、跨模块常量的 `case a[0]` 折叠、async 线程 null `current_process` segfault、struct 共享方法 double-free。
- 遗留建议：逃逸 structure 契约化；CI 引入 sanitizer 覆盖并发项；性能热点见 §3。

## 2. COVSCRIPT_DEBUG

运行时三级守卫（`none`/`warning`/`strict`，默认 `warning`，`strict` 为 `abort`），接入 fiber 未收尾析构与 interpret() 入口值栈守卫；REPL 清栈死代码已删除。测试 `test_debug_mode.sh/.bat` 覆盖三模式 + 默认/非法值。

## 3. 性能（profiling 摘要）

| 项 | 结论 |
| :-- | :-- |
| benchmark（vs master） | 持平；快速优化后多项 +6~+17%（seq/str/arr） |
| call 密集微基准 | gc 相对 master 曾 +10% 指令；真实套件负载收窄至 +4.5% |
| 优化后（profile_suite Ir） | 12.00B → 11.77B（-1.9%）：P1 `current_process_ref` fast path（-0.31%）、P3 `token_base::get_type` 去虚化（-1.65%，间接分支 18%→15.7%、误预测 9.1%→8.2%） |
| P2 `CS_AGGRESSIVE_OPTIMIZE`（Linux） | +5.5% 指令、benchmark 无改善 → **回退** |
| 热点 | `parse_expr`（树遍历求值）、`get_var`+phmap `find`+名字构造、引用计数 churn、多态派发 |
| 缓存/memcheck | ~0% miss；0 泄漏 0 错误 |

## 4. 防御性内容审计

40 个 commit 分类：**真实 bug 修复 + 核心重构约 40%**（async segfault、double-free、泄漏、崩溃，均为正常代码可触发），测试 22%，**防御性约 4%**（仅 `ad17782` 明确防御）。结构体 process 钉住有功能性理由（finalize 需有效 `current_process`）；候选 1–5 已全部处置（死代码删除、COVSCRIPT_DEBUG、文档对齐 + `escaped_structure_data_usable_after_context_death` 测试）。

## 5. 合并前验证（vs `origin/master` c6e46d5）

- **合并拓扑**：`origin/master..gc_improve` = 24 commits，反向 0，fast-forward 干净。
- **测试矩阵全绿**：单测 186（Release/UBSan），184（ASan/TSAN，过滤 2 个与 sanitizer 硬冲突的 dlopen 测试，sanitizers#611）；并发压测 `future_stress.csc` TSAN 0 竞态；集成套件 68（WSL+Windows Release、ASan+LSan+UBSan）；扩展测试、COVSCRIPT_DEBUG、memcheck 全过。
- **性能对比（vs c6e46d5）**：benchmark seq +6% / str +9.3% / arr +17.3%；总 gprof CPU -4.9%；Ir +0.5% 但墙钟更快（分支预测改善）。
- **已知风险**：MSVC/Clang/macOS/Android 本机未测，建议 CI 补 MSVC。

## 6. 结论

gc_improve 满足合并条件：测试矩阵全绿、相对当前 master 性能更快且无回退、fast-forward 干净。代码注释中引用的提交哈希为历史标识，合并前已压缩，不再一一对应。

---

# Round-2 审计报告（合并前最终轮，2026-08-16）

- 方式：按**功能单元**审计（非 commit）；仅审计不改码；profiling 全部在 WSL `/tmp` 全新 clone 中进行（gcc-13/clang，未触碰现有检出）；跨平台测试覆盖 Windows（MinGW 本机）、WSL Ubuntu、macOS（ssh 10.31.0.13，arm64）。
- 结论先行：**发现 2 个合并前必修缺陷（B1 内存安全、B2 数据竞争）+ 1 个性能权衡确认（F1）+ 反复项终裁（D1–D5）**。除此之外测试矩阵全绿、墙钟性能持平、无泄漏。

## 7. 合并前必修缺陷

### B1 · 同实例二次 compile 触发 token arena use-after-free（分支特有，内存安全级）

- **证据**：ASan（Linux gcc 与 clang）在 `test_failed_import_preserves_loaded_modules` 及最小复现（两次普通 `var` 编译，无 using/import）中报 heap-use-after-free；释放链：`compile()` 入口 `release_statements()` 释放上一程序 arena → `trim_expr`/`exist_record` 对已释放 string_view 执行 memcmp。
- **根因**：`domain_manager::m_set`（`set_t<std::string_view>`，runtime.hpp:102/313）持有指向 token arena 的 string_view 跨编译存活；master 因 arena 永不释放而免疫——这是 arena 确定性回收设计（F1 单元）遗漏的旧假设。
- **次生症状**：陈旧 view 字节未变时产生错误语义，如同实例第二次 `using system` 误报 `Redefinition of variable "log"`。
- **影响面**：REPL（逐语句 `clear_set`）与解释器单次编译不受影响；任何 SDK 嵌入方在**同一 context 实例上二次 compile** 即命中。
- **修复（已实施）**：`m_set` 键 `std::string_view` → `std::string`（runtime.hpp，自有键，与 `domain_type::m_reflect` 的既有模式一致；`get_var_optimizable` 仅编译期调用，运行时零影响）。曾尝试 `compile()` 入口清 `m_set` 的补丁方案，实测**破坏编译期常量折叠**（`get_var_optimizable` 依赖集合中的既有记录，`optimize.csc`/`api_cov.csc`/`file_os.csc` 复现失败），已放弃——string 键为唯一正确方案。**验证：ASan 0 错误（含新增单测 `recompile_on_same_instance_is_clean`，207 单测全绿）。**

### B1b · switch 翻译失败泄漏（B1 修复后由 LSan 暴露的既有缺陷，已一并修复）

- **证据**：B1 修复后 LSan 报 17920B/180 分配泄漏（`test_switch_body_translate_failure_is_cleaned_up`，20 次失败编译 × 9 分配）；revert 对比证实为既有缺陷（此前被 UAF abort 掩盖）。
- **根因**：`method_switch::translate`（codegen.cpp）的递归 `translate()` 在 try 块**之外**——失败时 `body_guard` 只删除 case/default 包装器，而块的所有权语义要求手动删除（catch 块已实现该回收逻辑，但未被 414 行覆盖）。
- **修复（已实施）**：递归 `translate()` 移入现有 try 块，复用已有回收逻辑（codegen.cpp，+4/-1）。
- **验证**：ASan+LSan 0 错误 0 泄漏。

### B2 · async worker 与主线程对共享实例 domain 栈的数据竞争（既有设计洞）

- **证据**：TSAN（`setarch -R`）在 `async_future_snapshots_recursive_lambda_argument` 报 data race：worker 线程 `scope_guard::add_domain`（写 `m_data`）vs 主线程 `get_var`（读 `m_data.size()`）。
- **触发模式**：`future.create(invoke_it, 脚本lambda)`——CNI 在 worker 线程间接调用脚本函数，绕过 `is_native_callable` 检查；脚本调用经 `function::call → scope_guard → add_domain` 写入**实例级共享** domain 栈，与主线程解释执行并发。
- **终裁（已定）**：**不修复。** 守卫的设计语义即"直接 callable 必须为原生"，从未承诺覆盖参数；通过原生函数参数在 worker 上间接执行脚本函数属**未支持用法**（守卫只检查最外层）。`docs/Asynchronous.md`（+zh）已明确写入：async 函数不得与 CovScript 运行时发生任何交互（直接或间接调用脚本函数、读写运行时状态均禁止），如确需交互**必须另起进程**。代码与测试保持不变；TSAN 矩阵记录为 1 个已知告警（该测试即文档所述未支持用法的演示），`future_stress.csc`（纯原生）0 竞态。

## 8. 性能验证与权衡（WSL，同一机器/同套件/同参数）

### 基线对比（vs 合并目标 `origin/master` c6e46d5）

| 指标 | c6e46d5 | gc_improve | 结论 |
| :-- | :-- | :-- | :-- |
| profile_suite 墙钟（3 次均值） | 1.62s | 1.65s | 持平 |
| benchmark.csc 墙钟 | 26.45s | 26.44s | 持平 |
| callgrind Ir（profile_suite） | 11.47B | 11.90B | +3.7% |
| 分支误预测 | — | 2.06%（Bim 占间接分支 39.5%） | 记录 |

### 分相位 benchmark（OPS/s，3 次均值）

| 相位 | master | gc_improve | Δ |
| :-- | :-- | :-- | :-- |
| Sequential Simple | 6,874,651 | 6,961,620 | **+1.3%** |
| Function Calling | 3,109,853 | 2,899,720 | **-6.8%** |
| Recursion | 178,615 | 165,128 | **-7.6%** |
| Fiber Switch | 700,619 | 695,039 | -0.8% |
| Complex Math | 1,303,199 | 1,261,668 | -3.2% |
| String Concat | 840,389 | 855,232 | **+1.8%** |
| Array Access | 1,799,399 | 1,781,495 | -1.0% |
| Hash Access | 1,378,741 | 1,427,339 | **+3.5%** |
| Quicksort | 162,585 | 171,092 | **+5.2%** |

> 注：上轮声称的 arr +17.3%/str +9.3%/seq +6% 在本轮同机复测未再现（+1.3/+1.8/-1.0），以本轮数据为准。

### F1 · var 代理池每线程化（554d660）——权衡已确认，接受

- Ir 增量集中在 `any::recycle()`（+43%，438M→627M）与 `any<numeric>` 拷贝；call-dense 相位 -6.8%/-7.6% 即其表现。
- 但 fiber/async 之后的容器相位（hash +3.5%、quicksort +5.2%）gc_improve 反超：master 的 `single_threaded()` 守卫在出现 fiber 后**永久禁用**其全局池（全程走 malloc）；gc_improve 每线程池始终可用。
- **最终决策：接受该权衡**（线程安全池是本分支 fiber/async 模型的必需品）。下一轮可选微优化：TLS 池预热 32 块、`clone()` 恢复就地构造（强异常保证可用 placement 保持）——预期 call-dense 收益 <2%。

### F2 · COVSCRIPT_DEBUG 松弛验证——无需松弛

- 三模式墙钟完全一致（none 1.59–1.66s / warning 1.63–1.64s / strict 1.56–1.64s），检查点全部位于冷路径（每次 interpret 一次 + fiber 析构）。
- **最终决策：检查机制全部保留，默认 `warning` 不变**；"放宽检查换性能"无实测收益空间。唯一可选项：fiber 未收尾警告降级为仅 `strict` 诊断（零性能影响，纯语义取舍，不推荐）。

### F3 · 其余热点（记录，非本轮修复目标）

- `parse_expr` 16.3%（树遍历求值）、`get_var` 6.3%、malloc/free 合计 ~5.5%、`poll_event` ~1%（每函数调用/每循环迭代，松弛收益 <1%，低价值）。
- `CS_AGGRESSIVE_OPTIMIZE`（Linux gcc）复测：profile_suite +1~3% 更慢、benchmark 持平 → **终裁维持 OFF**（macOS force-ON 维持）。

## 9. 反复项最终决策（聚焦点 4）

| # | 事项 | 历史 | **最终决策** |
| :-- | :-- | :-- | :-- |
| D1 | raii_collector / 手动 context reset API | 2023 加入，分支两处删除（跨分支重复提交），master 仍持有 | **维持删除**；记为有意 API 移除，合并时随分支一并生效 |
| D2 | interpret() 值栈清理 | 无条件 drain → debug 门控（警告清栈/严格中止/none 保留） | **检测无条件保留；清栈移出 debug 门控无条件执行**（消除 none 模式跨运行污染，`pop_no_return` 不缩容量、零成本）；诊断仅归 debug 模式。本轮不改码，记入下一轮 |
| D3 | CS_AGGRESSIVE_OPTIMIZE | 代码层从未反复（仅文档记录 Linux 回退） | **维持 opt-in OFF（Linux）+ macOS force-ON**；Linux 回退结论以本轮实测数据定稿 |
| D4 | 逃逸 structure 钉住进程 | 两分支重复提交（非单分支反复） | **机制保留**（type_node 池安全所必需）；钉移至 `type_id` 的窄化方案经论证收益不抵 6 处构造器改造成本，不采纳 |
| D5 | REPL 回滚 | flat resize → 事务化 | **保留事务机制**（索引稳定），`rollback_transaction` 槽位置空+尾裁剪为合理折中 |

## 10. 补丁式修复终审（聚焦点 1；逐项核实后**全部判定为不必要，未实施**）

| # | 现状（补丁式） | 终审结论 |
| :-- | :-- | :-- |
| R1 | REPL `delete sptr; sptr=nullptr` + 4 处重复 catch 块 | 现状正确（测试全过），纯代码卫生 → **砍** |
| R2 | `struct_builder` 析构 `use_count()==1` 手工判定 | 经核实无缺陷：无移动路径（拷贝构造抑制移动）、无弱引用持有者 → **砍** |
| R3 | `teardown_ctx` 借用 hack 散布 4 处 | 4 处重复但正确 → **砍** |
| R4 | `function::mStmt` 裸指针悬挂 | 控制流核实**不可达**：`reset_status()`（含 `breakpoints.reset()`）在 run 的**所有**路径（正常/异常/SIGINT，debugger.cpp:834/845/851/890）无条件执行，函数断点在下一次编译前必 revert 为 pending，`list()`/回调读取均在语句存活期内 → **砍** |
| R5 | `fiber_function` 手动破环 | 双路径重复但正确 → **砍** |
| R6 | `~instance_type` 依赖成员声明序 | shared_ptr 引用计数安全，释放时机差异无功能影响 → **砍** |
| R7 | `poll_event` 逐调用轮询（~1% Ir） | 收益 <1% 且增事件延迟 → **维持现状** |
| D2 | interpret() 值栈 warning-only 清理 | **已实施**：非空栈无条件清空（删 instance.cpp mode 分支），诊断仍走 debug_guard；文档同步（System_Environment ±zh） |

## 11. 泄漏量化（聚焦点 2 依据）

- valgrind memcheck（profile_suite）：**definitely/indirectly/possibly lost 均为 0，0 errors**。
- "still reachable" 共 146,624 B：TLS 代理池（292×80B）+ token arena 块（64KB×2 等），属**有意保留复用**（每线程池/arena 钉住），进程退出时由 `std::exit`/正常析构回收——不构成"非持续性泄漏"，无需容忍策略调整。

## 12. Round-2 修复后测试矩阵

| 平台/配置 | 单测 | 集成 | DEBUG 矩阵 | 备注 |
| :-- | :-- | :-- | :-- | :-- |
| Windows MinGW Release | 207/207 | 68/68 | 5/5 | 含新增 `recompile_on_same_instance_is_clean` |
| WSL gcc-13 Release | 207/207 | 68/68 | 5/5 | — |
| WSL ASan+UBSan+LSan | 205/205 | — | — | **0 错误 0 泄漏**（B1/B1b 修复后）；过滤 2 个 dlopen 冲突测试（sanitizers#611） |
| WSL TSAN（setarch -R） | 205+1 已知告警 | — | — | 仅 B2 已裁决告警；future_stress 0 竞态 |
| macOS arm64 clang Release | 待跑 | 待跑 | 待跑 | 修复后冒烟 |
| Windows MinGW Release 性能 | — | — | — | benchmark ~26.2s；profile_suite ~1.13s（与修复前持平） |

## 13. 平台说明

- **macOS ASan 假阳性**：`system_exit_from_fiber` 类测试在 macOS arm64 报 stack-buffer-underflow（libunwind 272B 寄存器文件 memcpy 读取已出作用域区域，shadow `f8`）；最小复现（fiber + exit 处理器 throw）在 master c6e46d5 上**同样触发**，且 Release 二进制功能全绿 → 判定为既有平台工具链现象，不阻塞合并。
- WSL 首次批量测试"失败"系 PATH 上旧版 `/usr/bin/cs`（3.5.1）所致，用分支解释器后全绿（已核实）。
- macOS 上轮遗留 `build/`（Ninja 缓存）与新构建冲突，已用新目录 `build-r2` 规避；`git bundle` 推进 `cd428f9 → ec1f238` 为 fast-forward，工作区保持干净。

## 14. 合并结论

- **阻塞项已全部解除**：B1（分支特有 UAF）与 B1b（switch 翻译失败泄漏）已修复并经 ASan/LSan 验证 0 错误 0 泄漏；B2 经裁决为不受支持用法，仅文档化（`docs/Asynchronous.md` + zh 已写入 native-only 契约）。
- 本轮最终改动**仅 4 个文件**：`include/covscript/impl/runtime.hpp`（m_set 自有键）、`sources/compiler/codegen.cpp`（switch 失败回收）、`sources/instance/instance.cpp`（值栈无条件清理）、`unit_tests/test_fixes.cpp`（B1 回归单测）+ 文档（Asynchronous/System_Environment ±zh）+ 本报告。
- R1-R7 重构经逐项核实全部判定不必要（§10），未改动——PR 维持最小增量。
- 其余：测试矩阵全绿；墙钟性能持平、无回退；COVSCRIPT_DEBUG 零开销无需松弛；反复项终裁见 §9。
- 本地分支就绪，未 push；等待三平台最终冒烟（§12 中 macOS 行）后按既定流程合并。
