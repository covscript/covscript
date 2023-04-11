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
    + Apple macOS: Universal binary with x86_64 and arm64
    + Linux and Unix: x86, ARM, MIPS, LoongArch
    + Microsoft Windows: x86
+ Powerful and bloat-free extension system
    + C/C++ Native Interface(CNI) System: translate invoking from both C/C++ and Covariant Script
    + Interpreter Bootstrap: A portable way to access the Covariant Script Runtime from C++
    + CNI Composer Macro: Combined the extension system and CNI system organically
    + CFFI Support: Directly calling functions from shared library written in C
+ Self-developed efficient compilation system
    + Dependency-free and efficient compiler front-end
    + Unicode support optimized for Chinese
    + Support compile-time optimization
    + Exportable compilation results
+ High-performance runtime environment
    + Stackful asymmetric coroutine
    + Hotspot optimization algorithm
    + Efficient memory management system
    + Reference count garbage collection system

## Documents ##

Please visit [Covariant Script Organization README](https://github.com/covscript/README)

## Installation ##

+ **[Recommend]** Download installation package from [covscript.org.cn](http://covscript.org.cn)
+ Download the precompiled executable on the [Latest Release](https://github.com/covscript/covscript/releases/latest)
+ Directly compile source code with CMake toolchain

## Run ##

### Basic command ###

`cs [options...] <FILE|STDIN> [arguments...]`  
`cs [options...]`

When replace `FILE` with `STDIN`, interpreter will directly reads inputs from standard input stream.

#### Options ####

##### Interpreter #####

Option|Mnemonic|Function
:---:|:---:|:--:
`--compile-only`|`-c`|Only compile
`--dump-ast`|`-d`|Export abstract syntax tree
`--dependency`|`-r`|Export module dependency
`--csym <FILE>`|`-g <FILE>`|Read cSYM from file

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
`--stack-resize <SIZE>`|`-S <SIZE>`|Reset the size of runtime stack
`--log-path <PATH>`|`-l <PATH>` |Set the log path
`--import-path <PATH>`|`-i <PATH>`|Set the import path

The default stack size is **1000**. When the stack size is set larger than the default value, the stack size of the coroutine will be dynamically adjusted to one tenth of the set size; when the stack
size is set smaller than the default value, the stack size of the coroutine will be will remain at **100**.

**Note that if you do not set the log path, it will be directly output to the standard output stream.**

### Debugger ###

`cs_dbg [options...] <FILE>`

#### Options ####

         Option          |Mnemonic|Function
:-----------------------:|:---:|:--:
        `--help`         |`-h`|Show help infomation
       `--version`       |`-v`|Show version infomation
       `--silent`        |`-s`|Close the command prompt
  `--wait-before-exit`   |`-w`|Wait before process exit
     `--csym <FILE>`     |`-g <FILE>`|Read cSYM from file
 `--stack-resize <SIZE>` |`-S <SIZE>`|Reset the size of runtime stack
   `--log-path <PATH>`   |`-l <PATH>`|Set the log path
 `--import-path <PATH>`  |`-i <PATH>`|Set the import path

The stack size policy is the same as for the interpreter.

**Note that if you do not set the log path, it will be printed to stdout.**

## Examples ##

The [covscript-example](https://github.com/covscript/covscript-example) repository contains several example programs written by Covariant Script.

## Copyright ##

**The author of the Covariant Script Programming Language is [@mikecovlee](https://github.com/mikecovlee/), licensed
under Apache 2.0**  
**Copyright © 2017-2023 Michael Lee(李登淳)**  
*This software is registered with the National Copyright Administration of the People's Republic of China(Registration
Number: 2020SR0408026) and is protected by the Copyright Law of the People's Republic of China.*

## Thanks ##

[Sichuan University](http://scu.edu.cn/), KDDE Lab (Prof. Lei Duan), freeCodeCamp Chengdu Community

[@imkiva](https://github.com/imkiva/), [@ice1000](https://github.com/ice1000/),
[@MouriNaruto](https://github.com/MouriNaruto), Weicheng Shi
