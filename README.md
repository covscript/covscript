![](https://github.com/covscript/covscript/raw/master/icon/covariant_script_wide.png)
# The Covariant Script Programming Language #
[![Build Status](https://travis-ci.org/covscript/covscript.svg?branch=master)](https://travis-ci.org/covscript/covscript)
[![](https://img.shields.io/badge/GUI%20build-passing-blue.svg)](https://github.com/covscript/covscript-gui/releases/latest)
[![](https://img.shields.io/github/languages/top/covscript/covscript.svg)](http://www.cplusplus.com/)
[![](https://img.shields.io/github/license/covscript/covscript.svg)](https://github.com/covscript/covscript/blob/master/LICENSE)  
**Welcome to the Covariant Script programming language!**  
**Covariant Script** is an open source,cross-platform programming language.
## Switch language ##
- [简体中文](https://github.com/covscript/covscript/blob/master/README.zh_CN.md)
- [English](https://github.com/covscript/covscript/blob/master/README.md)

***Due to academic stress, the author may not be able to fix all the bugs in time. If you have some problems, please raise in the issue, thank you for your understanding.***

## Features ##
1. Fast compiler front end
2. Compile-time optimization
3. Reference count garbage collector
4. C/C++ Native Interface
5. Regular Expression
6. Darwin Universal CGL
7. SQLite3 Database
8. ASIO Network

## Documentations ##
[CovScript Document](https://github.com/covscript/covscript-docs)  
[CovScript Tutorial](https://github.com/covscript/covscript-tutorial)  
## Installation ##
### Windows ###
Download the precompiled executable on the [Latest Release](https://github.com/covscript/covscript/releases/latest).   
You can also use other tools (like MinGW) to compile. If you already have a G++ compiler installed, you can use `make.bat`. Make sure the compiler supports C++11.  
**Note: Covariant Script does not compile with Microsoft CL compiler magically.**
### Linux ###
First, download source from [CovScript Github Page](https://github.com/covscript/covscript).  
Then you have two choices:
+ With G++ and makefile
```sh
$ sh ./make.sh
```
+ With CMake toolchain  
We've provided a `CMakeLists.txt`.

Make sure your compiler supports C++11.  

**Note: Some distributions' G++ are too old to support C++11, make sure you G++ compiler is up to date.**
### Mac OS ###
All the steps are the same as Linux, tested under macOS Sierra 10.12.5.
## Run ##
### Interpreter ###
`cs [arguments..] <file> <args...>`  
#### Arguments ####
`--compile-only` Only compile.  
`--wait-before-exit` Wait before process exit.  
`--log-path PATH` Set the log path.  
`--import-path PATH` Set the import path.  
### Repl ###
`cs_repl [arguments..]`  
#### Arguments ####
`--args` Specify the operating parameters.  
`--silent` Does not display the version info.  
`--wait-before-exit` Wait before process exit.  
`--log-path PATH` Set the log path.  
`--import-path PATH` Set the import path.  
### GUI ###
If you hate command line you can use [Covariant Script GUI](https://github.com/covscript/covscript-gui/releases/latest).
### IntelliJ Plugin ###
The IntelliJ Plugin is available now:
+ [Source Code](https://github.com/covscript/covscript-intellij)
+ The Plugin is also available in [Plugin Repository](https://plugins.jetbrains.com/plugin/10326-covscript)!
## Examples ##
The `examples` folder contains several example programs written by CovScript.
## Copyright ##
**The author of the Covariant Script Programming Language is Michael Lee(mikecovlee@github.com,mikecovlee@163.com).**  
**Copyright (C) 2018 Michael Lee (李登淳)**  

## Thanks ##
**Ranked by contribution.**  
Testing: Shi Youcheng, Kiva (zt515)  
IntelliJ Plugin: [@ice1000](https://github.com/ice1000/)
## Comments ##
Comments by [@ice1000](https://github.com/ice1000/):

> This is a programming language created by my friend Michael Lee, written in C++. The detailed description is on it’s website so I’ll just put some comments here. This is a C-like language (at least the function call syntax is of C style), while functions, structs, namespaces are ends with end like Ruby, and it has the concepts of package import using (the module system). <br/>
Its parser is hand-written (very complex, although parsers are trivial), which impressed me (I didn’t even think of that when working on Lice, I just wrote an AST evaluator). <br/>
This language supports invoking functions written in C++, like JNI. <br/><br/>
CovScript mostly features its powerful standard library, which has database, GUI, web access, regular expressions.
