![](https://github.com/mikecovlee/covscript/raw/master/icon/covariant_script_wide.png)
# Covariant Script Programming Language #
[![Build Status](https://travis-ci.org/mikecovlee/covscript.svg?branch=master)](https://travis-ci.org/mikecovlee/covscript) [![](https://img.shields.io/badge/GUI%20build-passing-blue.svg)](https://github.com/mikecovlee/covscript-gui/releases/latest) [![](https://img.shields.io/badge/language-C%2B%2B-blue.svg)](http://www.cplusplus.com/) [![](https://img.shields.io/badge/license-GPLv3-blue.svg)](https://github.com/mikecovlee/covscript/blob/master/LICENSE)  
**Welcome to the Covariant Script programming language!**  
**Covariant Script** is an open source,cross-platform programming language.
## Switch language ##
- [简体中文](https://github.com/mikecovlee/covscript/blob/master/README.zh_CN.md)
- [English](https://github.com/mikecovlee/covscript/blob/master/README.md)

## Language features ##
1. Complete compiler front end
2. Dynamic type
3. Compile-time optimization
4. Reference count garbage collector
5. C/C++ Native Interface

## Use the Covariant Script programming language ##
The `docs` folder contains several documents for the CovScript programming language.
## Download and install ##
### Microsoft Windows ###
Please download the precompiled executable on the [Latest Release](https://github.com/mikecovlee/covscript/releases/latest). You can also use MinGW or the appropriate tools to compile the source code directly. If you already have a G++ compiler installed in your system, you can execute `make.bat` directly. You need to make sure that the compiler supports the C++11 standard.  
**Attention! Covariant Script does not support the use of Microsoft CL compiler, the specific reason is not yet clear, probably because of its support for the C++ standard is not complete.**
### Linux ###
After downloading the source code, if you have installed the G++ compiler on your system, you can type `sh./make.sh` directly in the terminal.If you have the Make tool installed, you can also execute the `make` command.You need to make sure that the compiler supports the C++11 standard.  
**Attention! Some distributions of the G++ version are too old to support the C++11 standard, please download the latest version of the G++ compiler.**
### Mac OS ###
Building Covariant Script is the same as Linux. And now Covariant Script has been tested on macOS Sierra 10.12.5.
## Run ##
`cs <file> <args...>`  
You can also download the [Covariant Script GUI](https://github.com/mikecovlee/covscript-gui/releases/latest) to simplify your work.
## Examples ##
The `examples` folder contains several example programs written by CovScript.
## Copyright ##
**The author of the Covariant Script Programming Language is Michael Lee(mikecovlee@github.com,mikecovlee@163.com).**  
**Copyright (C) 2017 Michael Lee(李登淳)**  
## Thanks ##
**Ranking by contribution.**  
Testing: Shi Youcheng,Kiva(zt515)  
Tutorial: Zaozhuang No.8 Middle School (North Campus) Computer Community Teaching Group(Li Hui,Pan Yuguang,Shi Youcheng)  
