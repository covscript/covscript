![](https://github.com/covscript/covscript/raw/master/icon/covariant_script_wide.png)
# Covariant Script 编程语言：解释器(**教育版**) #
[![Build Status](https://travis-ci.org/scu-covariant/covscript.svg?branch=master)](https://travis-ci.org/scu-covariant/covscript)
[![](https://img.shields.io/badge/license-Covariant%20Innovation%20GPL-blue.svg)](https://github.com/covscript/covscript/blob/master/LICENSE)
[![](https://img.shields.io/github/languages/top/covscript/covscript.svg)](http://www.cplusplus.com/)  
**欢迎使用Covariant Script编程语言!**  
**Covariant Script**是一种开源的跨平台编程语言  
此项目为官方维护的Covariant Script解释器(**教育版**)
## 切换语言 ##
- [简体中文](https://github.com/covscript/covscript/blob/master/docs/README.zh_CN.md)
- [English](https://github.com/covscript/covscript/blob/master/README.md)
## 关于教育版 ##
**教育版**是在标准版本基础上修改的版本，移除了一些第三方组件和不太稳定的新功能。  
但随着主项目的更新，此版本也会跟进较为稳定的功能和安全修复。
## 特点 ##
+ 跨平台，支持大部分主流操作系统
+ 专为教育目的而设计，更稳定
+ 使用C++11编写，兼容性更好
+ 独立、高效的编译器前端
+ 可导出编译结果
+ 支持编译期优化
+ 高效的内存管理系统
+ 引用计数垃圾回收器
+ 强大易用的扩展系统
+ C/**C++** 原生接口 (CNI)
## 扩展 ##
+ [Dear ImGui图形用户界面](https://github.com/covscript/covscript-imgui)
+ [Darwin通用字符图形库](https://github.com/covscript/covscript-darwin)
+ [SQLite3数据库](https://github.com/covscript/covscript-sqlite)
+ [Base64编解码](https://github.com/covscript/covscript-codec)
+ [ASIO网络库](https://github.com/covscript/covscript-network)
+ [正则表达式](https://github.com/covscript/covscript-regex)
+ [流式API](https://github.com/covscript/covscript-streams)
## 文档 ##
[CovScript在线文档](http://covscript.org/docs/)  
## 安装 ##
+ 在[Latest Release](https://github.com/covscript/covscript/releases/latest)里下载预编译的二进制文件
+ 使用CMake工具链直接编译源代码
### Mac OS ###
编译Covariant Script的方法与Linux相同。目前Covariant Script已经在MacOS Sierra 10.12.5中测试通过。
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
#### Options ####
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
## 感谢 ##
**按贡献排名。**  
+ 顾问：[@ice1000](https://github.com/ice1000/), [@imkiva](https://github.com/imkiva/)
+ 测试：史为成，[@imkiva](https://github.com/imkiva/)
+ 文档：[@imkiva](https://github.com/imkiva/), [@ice1000](https://github.com/ice1000/)
+ 流式API扩展：[@imkiva](https://github.com/imkiva/)
+ IntelliJ插件：[@ice1000](https://github.com/ice1000/)
## 评论 ##
[@ice1000](https://github.com/ice1000/)的评论：
>这是我的朋友李登淳创造的一门编程语言，用 C++ 实现，具体介绍在官网还是比较详细的，我在这里就只写点我自己的看法吧。  
这门语言本身是类 C 的（至少表达式和函数调用是 C 风格），而函数、结构体、命名空间定义等是 Ruby 风格的 end 结尾，并有 package import using 的概念（也就是模块系统）。  
老李的 Parser 是手写的（可以说复杂度很高了，虽然 Parser 是很 trivial 的），让我对他的耐力很佩服（同为高二学生， 我在写 Lice 的时候就没有这份心思，只是做了个很灵活的 AST evaluator 而已）。  
这门语言支持调用 C++ 实现的函数，形式类似 JNI 。  
CovScript 的类 C 语法让它能写起来很愉快，再加上对 lambda 的支持，可以说是一个 real-world 的编程语言了（ 不像 Lice 这种，是以辅助另一门语言为目标的）。  
最大的特色应该是它功能齐全的标准库，有数据库、 GUI （有个命令行的图形库，感觉碉堡）、网络编程、正则等库，感觉写程序很方便。