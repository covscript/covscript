# 系统环境变量

本文档描述了 Covariant Script 用于定位运行时文件、模块和开发工具的环境变量。

## 运行时路径

**环境变量**：`COVSCRIPT_HOME`

运行时路径是 Covariant Script 存放用户数据和标准库模块的位置。

**默认位置**：

+ **Windows**：`%USERPROFILE%\Documents\CovScript`（通常为 `C:\Users\<用户名>\Documents\CovScript`）
+ **Linux & macOS**：`~/.covscript`（用户主目录下）

**优先级**：如果在编译期定义了 `COVSCRIPT_HOME` 预处理宏，则宏的值优先于环境变量 —— 运行时的 `getenv` 调用会被完全跳过，直接使用编译期硬编码的路径。这种情况常见于发行版打包时通过 CMake 写死了安装前缀。

## 导入路径

**环境变量**：`CS_IMPORT_PATH`

导入路径是一组目录列表，Covariant Script 在解析 `import` 语句时会按顺序在这些目录中搜索模块文件。

多个路径之间使用平台分隔符隔开：**Windows** 用 `;`（分号），**Linux / macOS** 用 `:`（冒号）。与系统 `PATH` 变量的惯例一致。

**搜索顺序**（依次查找，靠前的优先）：

1. 当前执行脚本文件所在的目录（运行时自动前置）
2. `.`（当前工作目录，`import_path` 的初始默认值）
3. `-i` / `--import-path` 命令行参数指定的路径（只能使用一次，多个路径用分隔符隔开）
4. `$CS_IMPORT_PATH`（用户自定义导入路径）
5. `$COVSCRIPT_HOME/imports`（用户模块目录）
6. `$COVSCRIPT_PLATFORM_HOME/imports`（系统模块目录）

命令行用法：

```bash
cs -i /path/to/modules main.csc
cs -i "/path/a;/path/b" main.csc    # Windows：多个路径用分号分隔
cs -i "/path/a:/path/b" main.csc    # Linux / macOS：多个路径用冒号分隔
```

`-i` 只能使用一次，如需指定多个路径，在同一值内用平台分隔符隔开。`-i` 路径和 `$CS_IMPORT_PATH` 均会被包含在最终的导入路径中，其中 `-i` 指定的路径优先级高于 `$CS_IMPORT_PATH`。

**各平台系统模块默认路径**：

+ **Windows**：`C:\Program Files (x86)\CovScript`
+ **macOS**：`/Applications/CovScript.app/Contents/MacOS/covscript`
+ **Linux**：`/usr/share/covscript`

## SDK 路径（开发路径）

**环境变量**：`CS_DEV_PATH`

SDK 路径是一个 CMake 层面的约定，供第三方扩展项目在构建过程中定位 CovScript SDK。扩展项目通常通过此变量找到 `csbuild.cmake`、头文件及其他 SDK 资源：

```cmake
include($ENV{CS_DEV_PATH}/csbuild.cmake)
```

**注意**：此变量**不由** CovScript 解释器或运行时消费，仅由外部扩展的构建系统读取。
