![](https://github.com/covscript/covscript/raw/master/icon/covariant_script_wide.png)
# Covariant Script 编程语言：解释器 #
[![Build Status](https://travis-ci.org/covscript/covscript.svg?branch=master)](https://travis-ci.org/covscript/covscript)
[![](https://img.shields.io/badge/license-Covariant%20Innovation%20GPL-blue.svg)](https://github.com/covscript/covscript/blob/master/LICENSE)
[![](https://img.shields.io/github/languages/top/covscript/covscript.svg)](http://www.cplusplus.com/)  
**欢迎使用Covariant Script编程语言!**  
**Covariant Script**是一种开源的跨平台编程语言  
此项目为官方维护的Covariant Script解释器
## 切换语言 ##
- [简体中文](https://github.com/covscript/covscript/blob/master/docs/README.zh_CN.md)
- [English](https://github.com/covscript/covscript/blob/master/README.md)
## 特点 ##
+ 跨平台，支持大部分主流操作系统
+ 使用C++14编写，兼容性更好
+ 独立、高效的编译器前端
+ 可导出编译结果
+ 支持编译期优化
+ 运行时热点优化
+ 高效的内存管理系统
+ 引用计数垃圾回收器
+ 强大易用的扩展系统
+ C/**C++** 原生接口(CNI)
## 扩展 ##
+ [Dear ImGui图形用户界面](https://github.com/covscript/covscript-imgui)
+ [Darwin通用字符图形库](https://github.com/covscript/covscript-darwin)
+ [SQLite3数据库](https://github.com/covscript/covscript-sqlite)
+ [Base64编解码](https://github.com/covscript/covscript-codec)
+ [ASIO网络库](https://github.com/covscript/covscript-network)
+ [正则表达式](https://github.com/covscript/covscript-regex)
+ [流式API](https://github.com/covscript/covscript-streams)
## 文档 ##
[CovScript在线文档](https://covscript.org/docs/)  
## 安装 ##
+ 在[Latest Release](https://github.com/covscript/covscript/releases/latest)里下载预编译的二进制文件
+ 使用CMake工具链直接编译源代码
## 运行 ##
### 解释器 ###
`cs [选项...] <文件> [参数...]`
#### 选项 ####
选项|助记符|功能
:---:|:---:|:--:
`--compile-only`|`-c`|仅编译
`--no-optimize`|`-o`|禁用优化器
`--help`|`-h`|显示帮助信息
`--version`|`-v`|显示版本信息
`--wait-before-exit`|`-w`|等待进程退出
`--dump-ast`|`-d`|导出高级语法树
`--log-path <PATH>`|`-l <PATH>` |设置日志和语法树导出路径
`--import-path <PATH>`|`-i <PATH>`|设置引入查找路径

**注意，若不设置日志和导出AST路径，这两者将直接输出至标准输出流**
### 交互式解释器(Repl) ###
`cs_repl [选项...]`
#### 选项 ####
选项|助记符|功能
:---:|:---:|:--:
`--help`|`-h`|显示帮助信息
`--version`|`-v`|显示版本信息
`--silent`|`-s`|关闭命令提示符
`--wait-before-exit`|`-w`|等待进程退出
`--args <...>`|`-a <...>`|设置程序参数
`--log-path <PATH>`|`-l <PATH>`|设置日志路径
`--import-path <PATH>`|`-i <PATH>`|设置引入查找路径

**注意：**
- 在选项`--args`或其助记符`-a`之后设置的每一项都将被视为参数。
- 若不设置日志路径，将直接输出至标准输出流
### 调试器 ###
`cs_dbg [选项...] <文件>`
#### 选项 ####
选项|助记符|功能
:---:|:---:|:--:
`--help`|`-h`|显示帮助信息
`--version`|`-v`|显示版本信息
`--wait-before-exit`|`-w`|等待进程退出
`--log-path <PATH>`|`-l <PATH>`|设置日志路径
`--import-path <PATH>`|`-i <PATH>`|设置引入查找路径

**若不设置日志路径，将直接输出至标准输出流**
### 安装器 ###
Covariant Script安装器为Windows平台提供了简易的工具链配置方法，您可以一键安装，升级，卸载工具链。你可以在[CovScript编程语言主页](http://covscript.org)下载最新版安装器，详情请访问[Covariant Script Installer](https://github.com/covscript/covscript-installer)。
### GUI ###
Covariant Script为Windows平台提供了一站式的代码编辑，运行，调试体验。CovScript安装器会自动为您下载CovScript GUI，详情请访问[Covariant Script GUI](https://github.com/covscript/covscript-gui)。
### 包管理器 ###
`cspkg`是使用Covariant Script编写的包管理器，其职能主要是管理Covariant Script的扩展或者是包。`cspkg`会与最新版本Covariant Script共同发行，详情请访问[Covariant Script Package Manager](https://github.com/covscript/cspkg)。
### IntelliJ插件 ###
IntelliJ插件现已可用：
+ [源代码](https://github.com/covscript/covscript-intellij)
+ [Plugin Repository](https://plugins.jetbrains.com/plugin/10326-covscript)
## 示例 ##
`examples`文件夹包含了数个使用CovScript编写的示例程序。
## 版权 ##
**Covariant Script编程语言的作者是[@mikecovlee](https://github.com/mikecovlee/).**  
**版权所有 © 2019 李登淳**
## 捐献 #
![](https://github.com/covscript/covscript/raw/master/docs/Donation.png)
## 感谢 ##
**按贡献排名。**  
+ 顾问：[@ice1000](https://github.com/ice1000/), [@imkiva](https://github.com/imkiva/)
+ 测试：史为成，[@imkiva](https://github.com/imkiva/)
+ 文档：[@imkiva](https://github.com/imkiva/), [@ice1000](https://github.com/ice1000/)
+ 流式API扩展：[@imkiva](https://github.com/imkiva/)
+ IntelliJ插件：[@ice1000](https://github.com/ice1000/)
+ 跨平台支持：[@MouriNaruto](https://github.com/MouriNaruto)