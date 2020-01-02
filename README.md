![](https://github.com/covscript/covscript/raw/master/icon/covariant_script_wide.png)
# Covariant Script Programming Language: The Interpreter #
[![Build Status](https://travis-ci.org/covscript/covscript.svg?branch=master)](https://travis-ci.org/covscript/covscript)
[![](https://img.shields.io/badge/license-Covariant%20Innovation%20GPL-blue.svg)](https://github.com/covscript/covscript/blob/master/LICENSE)
[![](https://img.shields.io/github/languages/top/covscript/covscript.svg)](http://www.cplusplus.com/)  
**Welcome to the Covariant Script programming language!**  
**Covariant Script** is an open source, cross-platform programming language.  
This project is an officially maintained Covariant Script interpreter.  
You can try Covariant Script at http://dev.covariant.cn/ 
## Switch language ##
- [简体中文](https://github.com/covscript/covscript/blob/master/docs/README.zh_CN.md)
- [English](https://github.com/covscript/covscript/blob/master/README.md)
## Features ##
+ Cross-platform, supporting most mainstream Operating Systems
+ Powerful and bloat-free extension system
+ Good compatibility -- written in C++14
+ Dependency-free and efficient compiler front-end
+ Exportable compilation results
+ Support compile-time optimization
+ Runtime hotspot optimization
+ Efficient memory management system
+ Compile-time optimization
+ Reference count garbage collection system
+ C/**C++** Native Interface(CNI)
## Extensions ##
+ [Darwin Universal CGL](https://github.com/covscript/covscript-darwin)
+ [Base64/Json Codec](https://github.com/covscript/covscript-codec)
+ [Regular Expression](https://github.com/covscript/covscript-regex)
+ [SQLite3 Database](https://github.com/covscript/covscript-sqlite)
+ [Dear ImGui GUI](https://github.com/covscript/covscript-imgui)
+ [ASIO Network](https://github.com/covscript/covscript-network)
+ [Streams API](https://github.com/covscript/covscript-streams)
## Documentation ##
[CovScript Online Documents](https://covscript.org/docs/)  
## Installation ##
+ Download the precompiled executable on the [Latest Release](https://github.com/covscript/covscript/releases/latest)
+ Directly compile source code with CMake toolchain
## Run ##
### Basic command ###
`cs [options...] <FILE> [arguments...]`  
`cs [options...]`
#### Options ####
##### Interpreter #####
Option|Mnemonic|Function
:---:|:---:|:--:
`--compile-only`|`-c`|Only compile
`--dump-ast`|`-d`|Export abstract syntax tree

##### Interpreter REPL #####
Option|Mnemonic|Function
:---:|:---:|:--:
`--silent`|`-s`|Close the command prompt
`--args <...>`|`-a <...>`|Set the covscript arguments

**Note that everything you set after option `--args` or its mnemonic `-a` will be regarded as arguments.**
##### Common #####
Option|Mnemonic|Function
:---:|:---:|:--:
`--no-optimize`|`-o`|Disable optimizer
`--help`|`-h`|Show help infomation
`--version`|`-v`|Show version infomation
`--wait-before-exit`|`-w`|Wait before process exit
`--log-path <PATH>`|`-l <PATH>` |Set the log and AST exporting path
`--import-path <PATH>`|`-i <PATH>`|Set the import path

**Note that if you do not set the log and AST exporting path, both will be printed to stdout.**
### Debugger ###
`cs_dbg [options...] <FILE>`
#### Options ####
Option|Mnemonic|Function
:---:|:---:|:--:
`--help`|`-h`|Show help infomation
`--version`|`-v`|Show version infomation
`--wait-before-exit`|`-w`|Wait before process exit
`--log-path <PATH>`|`-l <PATH>`|Set the log path
`--import-path <PATH>`|`-i <PATH>`|Set the import path

**Note that if you do not set the log path, it will be printed to stdout.**
### Installer ###
Covariant Script Installer provides a simple toolchain configuration method for the Microsoft Windows platform. You can download, install, upgrade and uninstall the toolchain by simply pushing a button. You can download the latest installer from the [CovScript Programming Language Home](http://covscript.org). For more information, visit the [Covariant Script Installer](https://github.com/covscript/covscript-installer).
### GUI ###
Covariant Script provides a one-stop code editing, running, debugging experience for the Microsoft Windows platform . The CovScript Installer will automatically download the CovScript GUI for you. For more information, please visit [Covariant Script GUI](https://github.com/covscript/covscript-gui).
### Package Manager ###
`cspkg` is a package manager written in Covariant Script whose main function is to manage extensions or packages for Covariant Script. `cspkg` will be released with the latest version of Covariant Script. For more information, please visit [Covariant Script Package Manager](https://github.com/covscript/cspkg).
### IntelliJ Plugin ###
The IntelliJ Plugin is available now:
+ [Source Code](https://github.com/covscript/covscript-intellij)
+ [Plugin Repository](https://plugins.jetbrains.com/plugin/10326-covscript)
## Examples ##
The [examples](./examples) folder contains several example programs written by CovScript.
## Copyright ##
**The author of the Covariant Script Programming Language is [@mikecovlee](https://github.com/mikecovlee/).**  
**Copyright © 2020 Michael Lee**
## By me a coffee #
See [中文文档/捐献](https://github.com/covscript/covscript/blob/master/docs/README.zh_CN.md#%E6%8D%90%E7%8C%AE)
## Thanks ##
**Ordered by contribution.**  
+ Consultant: [@ice1000](https://github.com/ice1000/), [@imkiva](https://github.com/imkiva/)
+ Testing: Weicheng Shi, [@imkiva](https://github.com/imkiva/)
+ Douments: [@imkiva](https://github.com/imkiva/), [@ice1000](https://github.com/ice1000/)
+ Streams API Extension: [@imkiva](https://github.com/imkiva/)
+ IntelliJ Plugin: [@ice1000](https://github.com/ice1000/)
+ Cross-platform Support: [@MouriNaruto](https://github.com/MouriNaruto)
