![](https://github.com/covscript/covscript/raw/master/docs/covariant_script_wide.png)
# Covariant Script 编程语言：解释器 #
![](https://github.com/covscript/covscript/workflows/build/badge.svg)
[![](https://img.shields.io/github/stars/covscript/covscript?logo=GitHub)](https://github.com/covscript/covscript/stargazers)
[![](https://img.shields.io/github/license/covscript/covscript)](http://www.apache.org/licenses/LICENSE-2.0)
[![](https://img.shields.io/github/v/release/covscript/covscript)](https://github.com/covscript/covscript/releases/latest)
![](https://github.com/covscript/covscript/workflows/CodeQL/badge.svg)
[![](https://img.shields.io/github/languages/top/covscript/covscript)](http://www.cplusplus.com/)  
**欢迎使用Covariant Script编程语言!**  
**Covariant Script**是一种开源的跨平台编程语言  
此项目为官方维护的Covariant Script解释器  
## 切换语言 ##
- [简体中文](./README-zh.md)
- [English](./README.md)
## 特点 ##
+ 全平台可用，支持大多数主流操作系统
  + 使用 C++ 14 标准编写，支持版本较老的编译器
  + Linux and Unix: x86, ARM, MIPS, Loongson ISA
  + Microsoft Windows: x86
+ 强大易用的扩展系统
  + C/C++ Native Interface(CNI) 系统：能够翻译 C/C++ 和 Covariant Script 之间的双向调用
  + Interpreter Bootstrap：在 C++ 项目中集成 Covariant Script 运行时环境的帮手
  + CNI 组成宏：将扩展系统与CNI系统有机结合，现已纳入语言标准
+ 自研高效编译系统
  + 无依赖、高效的编译器前端
  + 支持编译期优化
  + 可导出编译结果
+ 高性能运行时环境
  + 热点分析优化算法
  + 高效的内存管理系统
  + 引用计数垃圾回收器
## 库环境 ##
### 标准库 ##
+ [Dear ImGui 图形用户界面](https://github.com/covscript/covscript-imgui)
+ [Darwin 通用字符图形库](https://github.com/covscript/covscript-darwin)
+ [Base64/Json 编解码](https://github.com/covscript/covscript-codec)
+ [LibMozart 进程库](https://github.com/covscript/covscript-process)
+ [SQLite3 数据库](https://github.com/covscript/covscript-sqlite)
+ [Zip 压缩文件](https://github.com/covscript/covscript-zip)
+ [ASIO 网络库](https://github.com/covscript/covscript-network)
+ [cURL 网络库](https://github.com/covscript/covscript-curl)
+ [正则表达式](https://github.com/covscript/covscript-regex)
+ [流式 API](https://github.com/covscript/covscript-streams)
### 平台依赖 ###
+ [wiringPi](https://github.com/covscript/covscript-wiringpi)
## 文档 ##
[CovScript官方文档(PDF)](http://mirrors.covariant.cn/covscript/comm_dist/cs_docs.pdf)  
## 安装 ##
+ **[建议]** 从 [covscript.org.cn](http://covscript.org.cn) 下载安装包
+ 下载 [64位类Debian系统滚动构建安装包](https://github.com/covscript/csbuild/releases/download/schedule/covscript-amd64.deb)
+ 在 [Latest Release](https://github.com/covscript/covscript/releases/latest) 里下载预编译的二进制文件
+ 使用 CMake 工具链直接编译源代码
## 运行 ##
### 解释器 ###
`cs [选项...] <文件> [参数...]`  
`cs [选项...]`
#### 选项 ####
##### 解释器 #####
选项|助记符|功能
:---:|:---:|:--:
`--compile-only`|`-c`|仅编译
`--dump-ast`|`-d`|导出高级语法树
`--dependency`|`-r`|导出模块依赖

##### 交互式解释器(Repl) #####
选项|助记符|功能
:---:|:---:|:--:
`--silent`|`-s`|关闭命令提示符
`--args <...>`|`-a <...>`|设置程序参数

**注意，在选项`--args`或其助记符`-a`之后设置的每一项都将被视为参数**
##### 通用选项 #####
选项|助记符|功能
:---:|:---:|:--:
`--no-optimize`|`-o`|禁用优化器
`--help`|`-h`|显示帮助信息
`--version`|`-v`|显示版本信息
`--wait-before-exit`|`-w`|等待进程退出
`--log-path <PATH>`|`-l <PATH>` |设置日志输出路径
`--import-path <PATH>`|`-i <PATH>`|设置引入查找路径

**注意，若不设置日志输出路径，将直接输出至标准输出流**
### 调试器 ###
`cs_dbg [选项...] <文件>`
#### 选项 ####
选项|助记符|功能
:---:|:---:|:--:
`--help`|`-h`|显示帮助信息
`--version`|`-v`|显示版本信息
`--wait-before-exit`|`-w`|等待进程退出
`--log-path <PATH>`|`-l <PATH>`|设置日志路径
`--import-path <PATH>`|`-i <PATH>`|设置引入查找路径

**若不设置日志路径，将直接输出至标准输出流**
## 示例 ##
`examples` 文件夹包含了数个使用 Covariant Script 编写的示例程序。
## 版权 ##
**Covariant Script 编程语言的作者是 [@mikecovlee](https://github.com/mikecovlee/)，基于 Apache 2.0 协议分发。**  
**版权所有 © 2017-2020 李登淳(Michael Lee)**  
*该软件已在中华人民共和国国家版权局注册（登记号：2020SR0408026），受中华人民共和国著作权法保护。*
## 感谢 ##
[四川大学](http://scu.edu.cn/), [freeCodeCamp 成都社区](https://china.freecodecamp.one/?city=chengdu)  
[@imkiva](https://github.com/imkiva/), [@ice1000](https://github.com/ice1000/), [@MouriNaruto](https://github.com/MouriNaruto), [@Access-Rend](https://github.com/Access-Rend), Weicheng Shi