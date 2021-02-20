![](https://github.com/covscript/covscript/raw/master/docs/covariant_script_wide.png)
# Covariant Script Programming Language: The Interpreter #
![](https://github.com/covscript/covscript/workflows/build/badge.svg)
[![](https://img.shields.io/github/stars/covscript/covscript?logo=GitHub)](https://github.com/covscript/covscript/stargazers)
[![](https://img.shields.io/github/license/covscript/covscript)](http://www.apache.org/licenses/LICENSE-2.0)
[![](https://img.shields.io/github/v/release/covscript/covscript)](https://github.com/covscript/covscript/releases/latest)
![](https://github.com/covscript/covscript/workflows/CodeQL/badge.svg)
[![](https://img.shields.io/github/languages/top/covscript/covscript)](http://www.cplusplus.com/)  
**Welcome to the Covariant Script programming language!**  
**Covariant Script** is an open source, cross-platform programming language.  
This project is an officially maintained Covariant Script interpreter.  
## Switch language ##
- [简体中文](./README-zh.md)
- [English](./README.md)
## Features ##
+ Cross-platform, supporting most mainstream Operating Systems
  + Compatible with legacy compilers -- written in C++ 14
  + Linux and Unix: x86, ARM, MIPS, Loongson ISA
  + Microsoft Windows: x86
+ Powerful and bloat-free extension system
  + C/C++ Native Interface(CNI) System: translate invoking from both C/C++ and Covariant Script
  + Interpreter Bootstrap: A portable way to access the Covariant Script Runtime from C++
  + CNI Composer Macro: Combined the extension system and CNI system organically
+ Self-developed efficient compilation system
  + Dependency-free and efficient compiler front-end
  + Support compile-time optimization
  + Exportable compilation results
+ High-performance runtime environment
  + Hotspot optimization algorithm
  + Efficient memory management system
  + Reference count garbage collection system
## Libraries ##
### Standard Library ##
+ [Darwin Universal CGL](https://github.com/covscript/covscript-darwin)
+ [Base64/Json Codec](https://github.com/covscript/covscript-codec)
+ [Regular Expression](https://github.com/covscript/covscript-regex)
+ [LibMozart Process](https://github.com/covscript/covscript-process)
+ [SQLite3 Database](https://github.com/covscript/covscript-sqlite)
+ [Zip File Support](https://github.com/covscript/covscript-zip)
+ [Dear ImGui GUI](https://github.com/covscript/covscript-imgui)
+ [ASIO Network](https://github.com/covscript/covscript-network)
+ [cURL Network](https://github.com/covscript/covscript-curl)
+ [Streams API](https://github.com/covscript/covscript-streams)
### Platform Specified ###
+ [wiringPi](https://github.com/covscript/covscript-wiringpi)
## Documentation ##
[CovScript Wiki](http://wiki.csman.info)  
## Installation ##
+ **[Recommend]** Download installation package from [covscript.org.cn](http://covscript.org.cn)
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
The [examples](./examples) folder contains several example programs written by Covariant Script.
## Copyright ##
**The author of the Covariant Script Programming Language is [@mikecovlee](https://github.com/mikecovlee/), licensed under Apache 2.0**  
**Copyright © 2017-2021 Michael Lee(李登淳)**  
*This software is registered with the National Copyright Administration of the People's Republic of China(Registration Number: 2020SR0408026) and is protected by the Copyright Law of the People's Republic of China.*
## Thanks ##
[Sichuan University](http://scu.edu.cn/), [freeCodeCamp Chengdu Community](https://china.freecodecamp.one/?city=chengdu)  
[@imkiva](https://github.com/imkiva/), [@ice1000](https://github.com/ice1000/), [@MouriNaruto](https://github.com/MouriNaruto), [@Access-Rend](https://github.com/Access-Rend), Weicheng Shi