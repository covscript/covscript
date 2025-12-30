![](https://github.com/covscript/covscript/raw/master/docs/covariant_script_wide.png)

# Covariant Script Programming Language: The Interpreter #

![Build & Test](https://github.com/covscript/covscript/actions/workflows/build.yml/badge.svg)
![CodeQL Advanced](https://github.com/covscript/covscript/actions/workflows/codeql-analysis.yml/badge.svg)
[![](https://img.shields.io/github/stars/covscript/covscript?logo=GitHub)](https://github.com/covscript/covscript/stargazers)
[![](https://img.shields.io/github/languages/top/covscript/covscript)](http://www.cplusplus.com/)
[![](https://img.shields.io/github/license/covscript/covscript)](http://www.apache.org/licenses/LICENSE-2.0)
[![](https://img.shields.io/github/v/release/covscript/covscript)](https://github.com/covscript/covscript/releases/latest)
[![](https://zenodo.org/badge/79646991.svg)](https://zenodo.org/doi/10.5281/zenodo.10004374)

**Welcome to the Covariant Script programming language!**  
**Covariant Script** is an open source, cross-platform programming language.  
This project is an officially maintained Covariant Script interpreter.

**[Sponsor This Project](SPONSORING.md)** - Support development and contribute to the community's future

## Switch language ##

- [简体中文](./README-zh.md)
- [English](./README.md)

## Features ##

+ Cross-platform, supporting most mainstream Operating Systems
    + Apple macOS: Universal binary with x86_64 (Intel) and ARM64 (Apple Silicon)
    + Linux and Unix: x86, ARM, MIPS, LoongArch
    + Microsoft Windows: x86, ARM
    + [New!] Google Android: x86, ARM (The complete environment is available in [Termux](https://github.com/termux/termux-app) environment)
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

## Codespace ##

You can try CovScript in GitHub Codespace directly.

[![Open in GitHub Codespaces](https://github.com/codespaces/badge.svg)](https://codespaces.new/covscript/codespace?quickstart=1)

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
`--help`|`-h`|Show help information
`--version`|`-v`|Show version information
`--wait-before-exit`|`-w`|Wait before process exit
`--stack-resize <SIZE>`|`-S <SIZE>`|Reset the size of runtime stack
`--log-path <PATH>`|`-l <PATH>` |Set the log and AST exporting path
`--import-path <PATH>`|`-i <PATH>`|Set the import path

The default stack size is **64**. This is the initial reserved size - the stack will grow automatically as needed, with the upper limit determined by system constraints. When the stack size is set to **1000** or larger, the stack size of the coroutine will be dynamically adjusted to one tenth of the set size; when the stack size is set smaller than **1000**, the stack size of the coroutine will remain at **64**.

**Note that if you do not set the log path, it will be directly output to the standard output stream.**

### Debugger ###

`cs_dbg [options...] <FILE>`

#### Options ####

Option|Mnemonic|Function
:-----------------------:|:------:|:--:
`--help`|`-h`|Show help information
`--version`|`-v`|Show version information
`--silent`|`-s`|Close the command prompt
`--wait-before-exit`|`-w`|Wait before process exit
`--csym <FILE>`|`-g <FILE>`|Read cSYM from file
`--stack-resize <SIZE>`|`-S <SIZE>`|Reset the size of runtime stack
`--log-path <PATH>`|`-l <PATH>`|Set the log path
`--import-path <PATH>`|`-i <PATH>`|Set the import path

The stack size policy is the same as for the interpreter.

**Note that if you do not set the log path, it will be directly output to the standard output stream.**

## Examples ##

The [covscript-example](https://github.com/covscript/covscript-example) repository contains several example programs written in Covariant Script.

## Copyright ##

**The author of the Covariant Script Programming Language is [@mikecovlee](https://github.com/mikecovlee/), licensed
under Apache 2.0**  
**Copyright © 2017-2025 Michael Lee(李登淳)**  
*This software is registered with the National Copyright Administration of the People's Republic of China(Registration
Number: 2020SR0408026) and is protected by the Copyright Law of the People's Republic of China.*

### For Contributors

When contributing to Covariant Script:
- Your contributions are always attributed to you
- Your name/GitHub handle is preserved in project history
- Significant contributors may be recognized in version release notes and documentation
- You retain copyright to your contributions under the Apache 2.0 license
- **Active contributors may receive bonuses from sponsorship funds** (distributed annually based on contribution metrics)

For a complete list of contributors, please visit our [Contributors page](CONTRIBUTORS.md). For sponsorship details, see [Sponsoring page](SPONSORING.md).

## Citation ##

Please cite the repo if you use CovScript.
```bibtex
@misc{CovScript,
  author = {Li, Dengchun},
  title = {Covariant Script Programming Language},
  year = {2017},
  publisher = {GitHub},
  howpublished = {\url{https://github.com/covscript/covscript}}
}
```

## Thanks ##

[Sichuan University](http://scu.edu.cn/), KDDE Lab (Prof. Lei Duan), freeCodeCamp Chengdu Community

[@imkiva](https://github.com/imkiva/), [@ice1000](https://github.com/ice1000/),
[@MouriNaruto](https://github.com/MouriNaruto), Weicheng Shi
