![](https://github.com/covscript/covscript/raw/master/icon/covariant_script_wide.png)
# Covariant Script Programming Language: The Interpreter #
[![Action Status](https://github.com/covscript/covscript/workflows/build/badge.svg)](https://github.com/covscript/covscript/actions)
[![Schedule Status](https://github.com/covscript/csbuild/workflows/schedule/badge.svg)](https://github.com/covscript/csbuild/actions)
[![](https://img.shields.io/badge/license-Covariant%20Innovation%20GPL-blue.svg)](https://github.com/covscript/covscript/blob/master/LICENSE)
[![](https://img.shields.io/github/languages/top/covscript/covscript.svg)](http://www.cplusplus.com/)  
**Welcome to the Covariant Script programming language!**  
**Covariant Script** is an open source, cross-platform programming language.  
This project is an officially maintained Covariant Script interpreter.  
## Switch language ##
- [简体中文](./README-zh.md)
- [English](./README.md)
## Features ##
+ Cross-platform, supporting most mainstream Operating Systems
  + GNU/Linux: x86, ARM, MIPS, Loongson 3A3000/4000
  + Microsoft Windows: x86
  + Apple macOS: x86
+ Powerful and bloat-free extension system
  + C/**C++** Native Interface(CNI) System: translate invoking from both C/C++ and Covariant Script
  + Interpreter Bootstrap: A portable way to access the Covariant Script Runtime from C++
  + CNI Composer Macro: Combined the extension system and CNI system organically
+ Good compatibility -- written in C++14
+ Dependency-free and efficient compiler front-end
+ Exportable compilation results
+ Support compile-time optimization
+ Runtime hotspot optimization
+ Efficient memory management system
+ Compile-time optimization
+ Reference count garbage collection system
## Extensions ##
+ [Darwin Universal CGL](https://github.com/covscript/covscript-darwin)
+ [Base64/Json Codec](https://github.com/covscript/covscript-codec)
+ [Regular Expression](https://github.com/covscript/covscript-regex)
+ [LibMozart Process](https://github.com/covscript/covscript-process)
+ [SQLite3 Database](https://github.com/covscript/covscript-sqlite)
+ [Dear ImGui GUI](https://github.com/covscript/covscript-imgui)
+ [ASIO Network](https://github.com/covscript/covscript-network)
+ [Streams API](https://github.com/covscript/covscript-streams)
+ [wiringPi](https://github.com/covscript/covscript-wiringpi)
## Documentation ##
[CovScript Official Document(PDF)](http://mirrors.covariant.cn/covscript/comm_dist/cs_docs.pdf)  
## Installation ##
+ **[Recommend]** Download installation package from [covscript.org](http://covscript.org)
+ Download [nightly build installation package](https://github.com/covscript/csbuild/releases/download/schedule/covscript-amd64.deb) for debian-like system on x86_64 machine
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
`--dependency`|`-r`|Export module dependency

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
`--log-path <PATH>`|`-l <PATH>` |Set the log path
`--import-path <PATH>`|`-i <PATH>`|Set the import path

**Note that if you do not set the log path, it will be directly output to the standard output stream.**
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
## Examples ##
The [examples](./examples) folder contains several example programs written by CovScript.
## Copyright ##
**The author of the Covariant Script Programming Language is [@mikecovlee](https://github.com/mikecovlee/).**  
**Copyright © 2020 Michael Lee**
## By me a coffee #
See [中文文档/捐献](https://github.com/covscript/covscript/blob/master/docs/README.zh_CN.md#%E6%8D%90%E7%8C%AE)
## Thanks ##
**Ordered by contribution.**  
+ Consultant: [@imkiva](https://github.com/imkiva/), [@ice1000](https://github.com/ice1000/)
+ Testing: [@imkiva](https://github.com/imkiva/), Weicheng Shi
+ Douments: [@imkiva](https://github.com/imkiva/), [@ice1000](https://github.com/ice1000/)
+ Streams API Extension: [@imkiva](https://github.com/imkiva/)
+ IntelliJ Plugin: [@ice1000](https://github.com/ice1000/)
+ Cross-platform Support: [@MouriNaruto](https://github.com/MouriNaruto)
