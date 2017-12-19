![](https://github.com/covscript/covscript/raw/master/icon/covariant_script_wide.png)
# Covariant Script Programming Language #
[![Build Status](https://travis-ci.org/covscript/covscript.svg?branch=master)](https://travis-ci.org/covscript/covscript) [![](https://img.shields.io/badge/GUI%20build-passing-blue.svg)](https://github.com/covscript/covscript-gui/releases/latest) [![](https://img.shields.io/badge/language-C%2B%2B-blue.svg)](http://www.cplusplus.com/) [![](https://img.shields.io/badge/license-AGPL--3.0-blue.svg)](https://github.com/covscript/covscript/blob/master/LICENSE)  
**Welcome to the Covariant Script programming language!**  
**Covariant Script** is an open source,cross-platform programming language.
## Switch language ##
- [简体中文](https://github.com/covscript/covscript/blob/master/README.zh_CN.md)
- [English](https://github.com/covscript/covscript/blob/master/README.md)

***Due to academic stress, the author may not be able to fix all the bugs in time. If you have some problems, please raise in the issue, thank you for your understanding.***
## Language features ##
1. Complete compiler front end
2. Dynamic type
3. Compile-time optimization
4. Reference count garbage collector
5. C/C++ Native Interface
6. Regular Expression
7. Darwin Universal CGL
8. SQLite3 Database
9. ASIO Network

## Use the Covariant Script programming language ##
[CovScript Document](https://github.com/covscript/covscript-docs)  
[CovScript Tutorial](https://github.com/covscript/covscript-tutorial)  
## Download and install ##
### Microsoft Windows ###
Please download the precompiled executable on the [Latest Release](https://github.com/covscript/covscript/releases/latest).   
You can also use MinGW or the appropriate tools to compile the source code directly. If you already have a G++ compiler installed in your system, you can execute `make.bat` directly. You need to make sure that the compiler supports the C++11 standard.  
**Attention! Covariant Script does not support the use of Microsoft CL compiler, the specific reason is not yet clear, probably because of its support for the C++ standard is not complete.**
### Linux ###
First you need to download the source code from [CovScript Github Page](https://github.com/covscript/covscript).  
After downloading the source code, if you have installed the G++ compiler on your system, you can type `sh ./make.sh` directly in the terminal.  
If you have the CMake tool installed, we provided the `CMakeLists.txt`, so that you can execute the `cmake` command to build.  
You need to make sure that the compiler supports the C++11 standard.  

**Attention! Some distributions of the G++ version are too old to support the C++11 standard, please download the latest version of the G++ compiler.**
### Mac OS ###
Building Covariant Script is the same as Linux. And now Covariant Script has been tested on macOS Sierra 10.12.5.
## Run ##
### Interpreter ###
`cs [arguments..] <file> <args...>`  
#### Arguments ####
`--compile-only` Only compile.  
`--wait-before-exit` Wait before process exit.  
`--log-path PATH` Set the log path.  
`--import-path PATH` Set the import path.  
### Repl Interpreter ###
`cs_repl [arguments..]`  
#### Arguments ####
`--args` Specify the operating parameters.  
`--wait-before-exit` Wait before process exit.  
`--log-path PATH` Set the log path.  
`--import-path PATH` Set the import path.  
### GUI ###
You can also download the [Covariant Script GUI](https://github.com/covscript/covscript-gui/releases/latest) to simplify your work.
## Examples ##
The `examples` folder contains several example programs written by CovScript.
## Copyright ##
**The author of the Covariant Script Programming Language is Michael Lee(mikecovlee@github.com,mikecovlee@163.com).**  
**Copyright (C) 2017 Michael Lee(李登淳)**  
## Thanks ##
**Ranking by contribution.**  
Testing: Shi Youcheng,Kiva(zt515)  
