![](https://github.com/covscript/covscript/raw/master/icon/covariant_script_wide.png)
# Covariant Script编程语言 #
[![Build Status](https://travis-ci.org/covscript/covscript.svg?branch=master)](https://travis-ci.org/covscript/covscript) [![](https://img.shields.io/badge/GUI%20build-passing-blue.svg)](https://github.com/covscript/covscript-gui/releases/latest) [![](https://img.shields.io/badge/language-C%2B%2B-blue.svg)](http://www.cplusplus.com/) [![](https://img.shields.io/badge/license-AGPL--3.0-blue.svg)](https://github.com/covscript/covscript/blob/master/LICENSE)  
**欢迎使用Covariant Script编程语言!**  
**Covariant Script**是一种开源的跨平台编程语言。
## 切换语言 ##
- [简体中文](https://github.com/covscript/covscript/blob/master/README.zh_CN.md)
- [English](https://github.com/covscript/covscript/blob/master/README.md)

***由于学业紧张，作者可能无法及时的修复所有Bug。如果你遇到了一些问题，请在issue中提出，感谢您的理解。***
## 语言特性 ##
1. 完整的编译器前端
2. 动态类型
3. 编译期优化
4. 引用计数垃圾回收器
5. C/C++原生接口
6. 正则表达式
7. Darwin通用字符图形库
8. SQLite3数据库
9. ASIO网络库

## 使用Covariant Script编程语言 ##
[CovScript文档](https://github.com/covscript/covscript-docs)  
[CovScript教程](https://github.com/covscript/covscript-tutorial)
## 下载和安装 ##
### Microsoft Windows ###
请在[Latest Release](https://github.com/covscript/covscript/releases/latest)上下载预编译可执行文件。  
你也可以使用MinGW或相应的工具来直接编译源代码。如果您的系统中已经安装了G++编译器，则可以直接执行`make.bat`。 您需要确保编译器支持C++11标准。  
**注意！Covariant Script不支持Microsoft CL编译器，具体原因还不太清楚，可能是它对C++标准的支持不完整。**
### Linux ###
首先你需要从[CovScript Github主页](https://github.com/covscript/covscript)下载源代码。
下载源代码后，如果您在系统上安装了G++编译器，则可以在终端中直接键入`sh ./make.sh`。
如果您安装了CMake工具，我们提供了`CMakeLists.txt`，以便您执行`cmake`命令来构建。
您需要确保编译器支持C++11标准。

**注意！某些发行版的G++版本太旧，无法支持C++11标准，请下载最新版本的G++编译器。**
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
`--wait-before-exit` 等待进程退出。  
`--log-path PATH` 设置日志路径。  
`--import-path PATH` 设置import路径。  
### GUI ###
你也可以下载[Covariant Script GUI](https://github.com/covscript/covscript-gui/releases/latest)来简化您的工作。
## 示例 ##
`examples`文件夹包含了数个使用CovScript编写的示例程序。
## 版权 ##
**Covariant Script编程语言的作者是Michael Lee(mikecovlee@github.com,mikecovlee@163.com).**  
**版权所有 (C) 2018 Michael Lee(李登淳)**  
## 感谢 ##
**按贡献排名。**  
测试：史为成，Kiva(zt515)  
