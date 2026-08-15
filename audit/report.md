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
