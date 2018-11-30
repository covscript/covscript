![](https://github.com/covscript/covscript/raw/master/icon/covariant_script_wide.png)
# Covariant Script Programming Language: The Interpreter #
[![Build Status](https://travis-ci.org/covscript/covscript.svg?branch=master)](https://travis-ci.org/covscript/covscript)
[![](https://img.shields.io/github/license/covscript/covscript.svg)](https://github.com/covscript/covscript/blob/master/LICENSE)
[![](https://img.shields.io/github/languages/top/covscript/covscript.svg)](http://www.cplusplus.com/)  
**Welcome to the Covariant Script programming language!**  
**Covariant Script** is an open source, cross-platform programming language  
This project is an officially maintained Covariant Script interpreter
## Switch language ##
- [简体中文](https://github.com/covscript/covscript/blob/master/README.zh_CN.md)
- [English](https://github.com/covscript/covscript/blob/master/README.md)
## Features ##
+ Cross-platform, supporting most mainstream operating systems
+ The extension system is powerful and easy to use
+ Written in C++ 11, which compatibility is better
+ Independent and efficient compiler front end
+ The compilation results can be exported
+ Efficient memory management system
+ Support compile-time optimization
+ Reference count garbage collector
+ C/C++ Native Interface(CNI)
## Extensions ##
+ [Darwin Universal CGL](https://github.com/covscript/covscript-darwin)
+ [Regular Expression](https://github.com/covscript/covscript-regex)
+ [SQLite3 Database](https://github.com/covscript/covscript-sqlite)
+ [Dear ImGui GUI](https://github.com/covscript/covscript-imgui)
+ [ASIO Network](https://github.com/covscript/covscript-network)
+ [Streams API](https://github.com/covscript/covscript-streams)
## Documentations ##
[CovScript Online Documents](http://covscript.org/docs/)  
## Installation ##
+ Download the precompiled executable on the [Latest Release](https://github.com/covscript/covscript/releases/latest)
+ Directly compile source code with CMake toolchain
### Mac OS ###
All the steps are the same as Linux, tested under macOS Sierra 10.12.5.
## Run ##
### Interpreter ###
`cs [arguments..] <file> <args...>`  
#### Arguments ####
+ `--compile-only` Only compile
+ `--no-optimize` Disable optimizer
+ `--dump-ast` Export abstract syntax tree
+ `--wait-before-exit` Wait before process exit
+ `--log-path PATH` Set the log and AST exporting path
+ `--import-path PATH` Set the import path

**Note that if you do not set the log and AST exporting path, both will be output directly to the standard output stream.**
### Repl ###
`cs_repl [arguments..]`  
#### Arguments ####
+ `--args` Specify the operating parameters
+ `--silent` Does not display the version info
+ `--wait-before-exit` Wait before process exit
+ `--log-path PATH` Set the log path
+ `--import-path PATH` Set the import path

**Note that if you do not set a log path, it will be output directly to the standard output stream.**
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
+ Consultant: [@ice1000](https://github.com/ice1000/), [@imkiva](https://github.com/imkiva/)
+ Testing: Shi Youcheng, [@imkiva](https://github.com/imkiva/)
+ Douments: [@imkiva](https://github.com/imkiva/), [@ice1000](https://github.com/ice1000/)
+ Streams API Extension: [@imkiva](https://github.com/imkiva/)
+ IntelliJ Plugin: [@ice1000](https://github.com/ice1000/)
## Comments ##
Comments by [@ice1000](https://github.com/ice1000/):
>This is a programming language created by my friend Michael Lee, written in C++. The detailed description is on it’s website so I’ll just put some comments here. This is a C-like language (at least the function call syntax is of C style), while functions, structs, namespaces are ends with end like Ruby, and it has the concepts of package import using (the module system).  
Its parser is hand-written (very complex, although parsers are trivial), which impressed me (I didn’t even think of that when working on Lice, I just wrote an AST evaluator).  
This language supports invoking functions written in C++, like JNI.  
CovScript mostly features its powerful standard library, which has database, GUI, web access, regular expressions.