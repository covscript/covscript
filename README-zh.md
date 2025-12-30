![](https://github.com/covscript/covscript/raw/master/docs/covariant_script_wide.png)

# Covariant Script 编程语言解释器 #

![Build & Test](https://github.com/covscript/covscript/actions/workflows/build.yml/badge.svg)
![CodeQL Advanced](https://github.com/covscript/covscript/actions/workflows/codeql-analysis.yml/badge.svg)
[![](https://img.shields.io/github/stars/covscript/covscript?logo=GitHub)](https://github.com/covscript/covscript/stargazers)
[![](https://img.shields.io/github/languages/top/covscript/covscript)](http://www.cplusplus.com/)
[![](https://img.shields.io/github/license/covscript/covscript)](http://www.apache.org/licenses/LICENSE-2.0)
[![](https://img.shields.io/github/v/release/covscript/covscript)](https://github.com/covscript/covscript/releases/latest)
[![](https://zenodo.org/badge/79646991.svg)](https://zenodo.org/doi/10.5281/zenodo.10004374)

**欢迎使用Covariant Script编程语言!**  
**Covariant Script**是一种开源的跨平台编程语言  
此项目为官方维护的Covariant Script解释器

## 切换语言 ##

- [简体中文](./README-zh.md)
- [English](./README.md)

## 特点 ##

+ 全平台可用，支持大多数主流操作系统
    + Apple macOS: 通用二进制，同时支持 x86_64 (Intel) 和 ARM64 (Apple Silicon)
    + Linux 和类 Unix: x86, ARM, MIPS, LoongArch
    + Microsoft Windows: x86, ARM
    + [New!] Google Android: x86, ARM (在 [Termux](https://github.com/termux/termux-app) 环境中可运行完整环境)
+ 强大易用的扩展系统
    + C/C++ Native Interface(CNI) 系统：能够翻译 C/C++ 和 Covariant Script 之间的双向调用
    + Interpreter Bootstrap：在 C++ 项目中集成 Covariant Script 运行时环境的帮手
    + CNI 组成宏：将扩展系统与CNI系统有机结合，现已纳入语言标准
    + CFFI 支持: 直接调用使用 C 语言编写的动态库中的函数
+ 自研高效编译系统
    + 针对中文优化的 Unicode 支持
    + 无依赖、高效的编译器前端
    + 支持编译期优化
    + 可导出编译结果
+ 高性能运行时环境
    + 堆栈式非对称协程
    + 热点分析优化算法
    + 高效的内存管理系统
    + 引用计数垃圾回收器
+ 全面支持国产信创环境
    + 处理器：龙芯（MIPS/LoongArch）、鲲鹏/飞腾（ARM）、海光/兆芯（x86）
    + 操作系统：支持大多数 Linux 衍生系统，在 UOS、openEuler 中经过测试
    + 加密算法：支持[主流的国密 SM2、SM3、SM4 算法](https://github.com/covscript/covscript-gmssl)

## 文档 ##

请访问 [Covariant Script OSC README](https://github.com/covscript/README)

## Codespace ##

您可以在 GitHub Codespace 中免安装直接使用 CovScript：

[![Open in GitHub Codespaces](https://github.com/codespaces/badge.svg)](https://codespaces.new/covscript/codespace?quickstart=1)


## 安装 ##

+ **[建议]** 从 [covscript.org.cn](http://covscript.org.cn) 下载安装包
+ 在 [Latest Release](https://github.com/covscript/covscript/releases/latest) 里下载预编译的二进制文件
+ 使用 CMake 工具链直接编译源代码

## 运行 ##

### 解释器 ###

`cs [选项...] <文件|STDIN> [参数...]`  
`cs [选项...]`

当将 `文件` 换成 `STDIN` 时，解释器会直接从标准输入流读取

#### 选项 ####

##### 解释器 #####

选项|助记符|功能
:---:|:---:|:--:
`--compile-only`|`-c`|仅编译
`--dump-ast`|`-d`|导出高级语法树
`--dependency`|`-r`|导出模块依赖
`--csym <文件>`|`-g <文件>`|从文件中读取 cSYM 信息

##### 交互式解释器(Repl) #####

选项|助记符|功能
:---:|:---:|:--:
`--silent`|`-s`|关闭命令提示符
`--args <...>`|`-a <...>`|设置程序参数

**注意，在选项 `--args` 或其助记符 `-a` 之后设置的每一项都将被视为参数**

##### 通用选项 #####

选项|助记符|功能
:---:|:---:|:--:
`--no-optimize`|`-o`|禁用优化器
`--help`|`-h`|显示帮助信息
`--version`|`-v`|显示版本信息
`--wait-before-exit`|`-w`|等待进程退出
`--stack-resize <大小>`|`-S <大小>`|重设运行时栈大小
`--log-path <路径>`|`-l <路径>` |设置日志输出路径
`--import-path <路径>`|`-i <路径>`|设置引入查找路径

默认栈大小为**64**。这只是初始预留大小，栈会根据需要自动增长，其上限取决于系统对栈的限制。当设置的栈大小为**1000**或更大时，协程的栈大小会动态调整为设置大小的十分之一；当设置的栈大小小于**1000**时，协程的栈大小将保持在**64**

**注意，若不设置日志输出路径，将直接输出至标准输出流**

### 调试器 ###

`cs_dbg [选项...] <文件>`

#### 选项 ####

选项|助记符|功能
:--:|:--:|:--:
`--help`|`-h`|显示帮助信息
`--version`|`-v`|显示版本信息
`--silent`|`-s`|关闭命令提示符
`--wait-before-exit`|`-w`|等待进程退出
`--csym <文件>`|`-g <文件>`|从文件中读取 cSYM 信息
`--stack-resize <大小>`|`-S <大小>`|重设运行时栈大小
`--log-path <路径>`|`-l <路径>`|设置日志路径
`--import-path <路径>`|`-i <路径>`|设置引入查找路径

栈大小策略与解释器相同

**若不设置日志路径，将直接输出至标准输出流**

## 示例 ##

仓库 [covscript-example](https://github.com/covscript/covscript-example) 中包含了数个使用 Covariant Script 编写的示例程序。

## 版权 ##

**Covariant Script 编程语言的作者是 [@mikecovlee](https://github.com/mikecovlee/)，基于 Apache 2.0 协议分发。**  
**版权所有 © 2017-2025 李登淳(Michael Lee)**  
*该软件已在中华人民共和国国家版权局注册（登记号：2020SR0408026），受中华人民共和国著作权法保护。*

## 引用 ##

如果 CovScript 为您的工作增添了一些便利，请引用该仓库。
```bibtex
@misc{CovScript,
  author = {Li, Dengchun},
  title = {Covariant Script Programming Language},
  year = {2017},
  publisher = {GitHub},
  howpublished = {\url{https://github.com/covscript/covscript}}
}
```

## 感谢 ##

[四川大学](http://scu.edu.cn/), KDDE Lab (段磊教授), freeCodeCamp 成都社区

[@imkiva](https://github.com/imkiva/), [@ice1000](https://github.com/ice1000/),
[@MouriNaruto](https://github.com/MouriNaruto), Weicheng Shi
