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
+ Efficient compiler front end
+ Compile-time optimization
+ Reference count garbage collector
+ C/C++ Native Interface

## Extensions ##
+ Darwin Universal CGL
+ Regular Expression
+ SQLite3 Database
+ Dear ImGui GUI
+ ASIO Network
+ Streams API

## Documentations ##
[CovScript Document](http://covscript.org/docs/)  
## Installation ##
### Windows ###
Download the precompiled executable on the [Latest Release](https://github.com/covscript/covscript/releases/latest).   
You can also use other tools (like MinGW) to compile. If you already have a G++ compiler installed, you can use `make.bat`. Make sure the compiler supports C++11.  
**Note: The Covariant Script interpreter supports the MSVC compiler, but due to the restrictions imposed by the Windows system on DLL export symbols, the current Covariant Script extension mechanism is almost completely paradoxical on MSVC. Although we provide `make_msvc.bat`, it is not recommended and is for reference only. Windows developers who need to embed Covariant Script please compile the source code directly, but all extension libraries are temporarily unavailable.**
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
### Installer ###
Covariant Script Installer provides a simple toolchain configuration method for the Microsoft Windows platform. You can download, install, upgrade and uninstall the toolchain by simply push a button. You can download the latest installer from the [CovScript Programming Language Home](http://covscript.org). For more information, visit the [Covariant Script Installer](https://github.com/covscript/covscript-installer).
### GUI ###
Covariant Script provides a one-stop code editing, running, debugging experience for the Microsoft Windows platform . The CovScript Installer will automatically downloads the CovScript GUI for you. For more information, please visit [Covariant Script GUI](https://github.com/covscript/covscript-gui).
### Package Manager ###
`cspkg` is a package manager written in Covariant Script whose main function is to manage extensions or packages for Covariant Script. `cspkg` will be released with the latest version of Covariant Script. For more information, please visit [Covariant Script Package Manager](https://github.com/covscript/cspkg).
### IntelliJ Plugin ###
The IntelliJ Plugin is available now:
+ [Source Code](https://github.com/covscript/covscript-intellij)
+ [Plugin Repository](https://plugins.jetbrains.com/plugin/10326-covscript)
## Examples ##
The `examples` folder contains several example programs written by CovScript.
## Copyright ##
**The author of the Covariant Script Programming Language is [@mikecovlee](https://github.com/mikecovlee/).**  
**Copyright © 2018 Michael Lee**
## Thanks ##
**Ranked by contribution.**  
Testing: Shi Youcheng, [@imkiva](https://github.com/imkiva/)  
Douments: [@imkiva](https://github.com/imkiva/), [@ice1000](https://github.com/ice1000/)  
Streams API Extension: [@imkiva](https://github.com/imkiva/)  
IntelliJ Plugin: [@ice1000](https://github.com/ice1000/)
## Comments ##
Comments by [@ice1000](https://github.com/ice1000/):

>This is a programming language created by my friend Michael Lee, written in C++. The detailed description is on it’s website so I’ll just put some comments here. This is a C-like language (at least the function call syntax is of C style), while functions, structs, namespaces are ends with end like Ruby, and it has the concepts of package import using (the module system).  
Its parser is hand-written (very complex, although parsers are trivial), which impressed me (I didn’t even think of that when working on Lice, I just wrote an AST evaluator).  
This language supports invoking functions written in C++, like JNI.  
CovScript mostly features its powerful standard library, which has database, GUI, web access, regular expressions.