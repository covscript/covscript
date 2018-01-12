![](https://github.com/covscript/covscript/raw/master/icon/covariant_script_wide.png)
# Covariant Script 编程语言 #
[![Build Status](https://travis-ci.org/covscript/covscript.svg?branch=master)](https://travis-ci.org/covscript/covscript)
[![](https://img.shields.io/badge/GUI%20build-passing-blue.svg)](https://github.com/covscript/covscript-gui/releases/latest)
[![](https://img.shields.io/github/languages/top/covscript/covscript.svg)](http://www.cplusplus.com/)
[![](https://img.shields.io/github/license/covscript/covscript.svg)](https://github.com/covscript/covscript/blob/master/LICENSE)  
**欢迎使用Covariant Script编程语言!**  
**Covariant Script**是一种开源的跨平台编程语言。
## 切换语言 ##
- [简体中文](https://github.com/covscript/covscript/blob/master/README.zh_CN.md)
- [English](https://github.com/covscript/covscript/blob/master/README.md)

***由于学业紧张，作者可能无法及时的修复所有Bug。如果你遇到了一些问题，请在issue中提出，感谢您的理解。***

## 语言特性 ##
1. 高效的编译器前端
2. 编译期优化
3. 引用计数垃圾回收器
4. C/C++原生接口
5. 正则表达式
6. Darwin通用字符图形库
7. SQLite3数据库
8. ASIO网络库

## 文档 ##
[CovScript文档](https://github.com/covscript/covscript-docs)  
[CovScript教程](https://github.com/covscript/covscript-tutorial)
## 安装 ##
### Microsoft Windows ###
请在[Latest Release](https://github.com/covscript/covscript/releases/latest)上下载预编译可执行文件。  
你也可以使用MinGW或相应的工具来直接编译源代码。如果您的系统中已经安装了G++编译器，则可以直接执行`make.bat`。 您需要确保编译器支持C++11标准。  
**注意：Covariant Script不支持Microsoft CL编译器，具体原因还不太清楚，可能是它对C++标准的支持不完整。**
### Linux ###
首先你需要从[CovScript Github主页](https://github.com/covscript/covscript)下载源代码。
然后你有两种选择：
+ 使用G++编译器和编译脚本
```sh
$ sh ./make.sh
```
+ 使用CMake工具链

我们提供了`CMakeLists.txt`，以便您执行`cmake`命令来构建。

您需要确保编译器支持C++11标准。

**注意：某些发行版的G++版本太旧，无法支持C++11标准，请下载最新版本的G++编译器。**
### Mac OS ###
编译Covariant Script的方法与Linux相同。目前Covariant Script已经在MacOS Sierra 10.12.5中测试通过。
## 运行 ##
### 解释器 ###
`cs [参数..] <文件> <运行参数...>`  
#### 参数 ####
`--compile-only` 仅编译。  
`--wait-before-exit` 等待进程退出。  
`--log-path PATH` 设置日志路径。  
`--import-path PATH` 设置import路径。  
### 交互式解释器(Repl) ###
`cs_repl [参数..]`  
#### 参数 ####
`--args` 指定运行参数。  
`--silent` 不显示版本信息。  
`--wait-before-exit` 等待进程退出。  
`--log-path PATH` 设置日志路径。  
`--import-path PATH` 设置import路径。  
### GUI ###
你也可以下载[Covariant Script GUI](https://github.com/covscript/covscript-gui/releases/latest)来简化您的工作。
### IntelliJ插件 ###
IntelliJ插件现已可用：
+ [源代码](https://github.com/covscript/covscript-intellij)
+ [Plugin Repository](https://plugins.jetbrains.com/plugin/10326-covscript)
## 示例 ##
`examples`文件夹包含了数个使用CovScript编写的示例程序。
## 版权 ##
**Covariant Script编程语言的作者是Michael Lee(mikecovlee@github.com,mikecovlee@163.com).**  
**版权所有 (C) 2018 Michael Lee(李登淳)** 

## 感谢 ##
**按贡献排名。**  
测试：史为成，Kiva (zt515)  
IntelliJ插件：[@ice1000](https://github.com/ice1000/)
## 评论 ##
[@ice1000](https://github.com/ice1000/)的评论：

>这是我的朋友李登淳创造的一门编程语言，用 C++ 实现，具体介绍在官网还是比较详细的，我在这里就只写点我自己的看法吧。  
这门语言本身是类 C 的（至少表达式和函数调用是 C 风格），而函数、结构体、命名空间定义等是 Ruby 风格的 end 结尾，并有 package import using 的概念（也就是模块系统）。  
老李的 Parser 是手写的（可以说复杂度很高了，虽然 Parser 是很 trivial 的），让我对他的耐力很佩服（同为高二学生， 我在写 Lice 的时候就没有这份心思，只是做了个很灵活的 AST evaluator 而已）。  
这门语言支持调用 C++ 实现的函数，形式类似 JNI 。  
CovScript 的类 C 语法让它能写起来很愉快，再加上对 lambda 的支持，可以说是一个 real-world 的编程语言了（ 不像 Lice 这种，是以辅助另一门语言为目标的）。  
最大的特色应该是它功能齐全的标准库，有数据库、 GUI （有个命令行的图形库，感觉碉堡）、网络编程、正则等库，感觉写程序很方便。