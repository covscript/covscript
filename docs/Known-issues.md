# 已知遗留问题（按决定缓办 / 已接受的既有行为）

> 来源：2026-08-10 第一轮审计（Audit-2026-08.md，已归档删除）。以下为按决定缓办或已接受的开放项。

## 待处理

### F4 [medium] `member_visitor` 持裸引用（缓办）

- `include/covscript/core/cni.hpp:713-758`（`_Class &obj` 裸引用）、`sources/instance/runtime.cpp:163-176`。
- 宿主对象销毁后 visitor 悬垂。触发：`var v = p.first; p = {...}; v.get()`。
- 修复方向：visitor 持宿主 var 共享引用。
- **缓办原因**：改动核心 visitor 机制、风险高。

### J1 [info] `runtime.import` / `source_import` 吞异常

- `sources/instance/type_ext.cpp:1790-1805`：`catch (...)` 返回 `null_pointer`，脚本侧无法区分失败原因。
- 建议：至少保留 `lang_error`。

### J2 [info] `source_import` 不注册 csym

- `source_import` 从不调用 `import_csym`，源导入模块的报错无法 csym 重定位（功能缺口）。

### J3 [info] Windows 非 ASCII 路径

- 依赖 `ifstream(std::string)` 代码页行为，跨工具链不确定（仓库有 unicode-support 分支）。

### J4 [info] Unix 文件名含 `:`

- 破坏 import path 切分（定界符设计固有限制，同 `$PATH`）。

## 已接受的行为边界（非回归）

- `--no-optimize` 下常量数组可通过 `constant a={...}` 定义，但方法级变更（如 `a.push_front`）报"不允许修改常量"；正常模式该调用被编译期折叠（副作用前移），二者行为不一致。既有语言不一致，非本次引入。
- 全局 GC 使调试器重复 `run` / 嵌入式反复 bootstrap 时已编译代码内存累积至进程退出（已接受 trade-off）。
