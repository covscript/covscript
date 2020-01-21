![](https://github.com/covscript/covscript/raw/master/icon/covariant_script_wide.png)
# Covariant Script 编程语言：解释器 #
[![Build Status](https://travis-ci.org/covscript/covscript.svg?branch=master)](https://travis-ci.org/covscript/covscript)
[![Action Status](https://github.com/covscript/covscript/workflows/build/badge.svg)](https://github.com/covscript/covscript/actions)
[![Schedule Status](https://github.com/covscript/csbuild/workflows/schedule/badge.svg)](https://github.com/covscript/csbuild/actions)
[![](https://img.shields.io/badge/license-Covariant%20Innovation%20GPL-blue.svg)](https://github.com/covscript/covscript/blob/master/LICENSE)
[![](https://img.shields.io/github/languages/top/covscript/covscript.svg)](http://www.cplusplus.com/)  
**欢迎使用Covariant Script编程语言!**  
**Covariant Script**是一种开源的跨平台编程语言  
此项目为官方维护的Covariant Script解释器  
你可以在 http://dev.covariant.cn/ 中尝试Covariant Script 
## 切换语言 ##
- [简体中文](./README-zh.md)
- [English](./README.md)
## 特点 ##
+ 跨平台，支持大部分主流操作系统
+ 使用C++14编写，兼容性更好
+ 独立、高效的编译器前端
+ 可导出编译结果
+ 支持编译期优化
+ 运行时热点优化
+ 高效的内存管理系统
+ 引用计数垃圾回收器
+ 强大易用的扩展系统
+ C/**C++** 原生接口(CNI)
## 扩展 ##
+ [Dear ImGui图形用户界面](https://github.com/covscript/covscript-imgui)
+ [Darwin通用字符图形库](https://github.com/covscript/covscript-darwin)
+ [Base64/Json编解码](https://github.com/covscript/covscript-codec)
+ [SQLite3数据库](https://github.com/covscript/covscript-sqlite)
+ [ASIO网络库](https://github.com/covscript/covscript-network)
+ [正则表达式](https://github.com/covscript/covscript-regex)
+ [流式API](https://github.com/covscript/covscript-streams)
## 文档 ##
[CovScript在线文档](https://covscript.org/docs/)  
[CovScript官方文档(PDF)](http://mirrors.covariant.cn/covscript/comm_dist/cs_docs.pdf)  
## 安装 ##
+ **[建议]** 从[covscript.org](http://covscript.org)下载安装包
+ 下载[64位类Debian系统滚动构建安装包](https://github.com/covscript/csbuild/releases/download/schedule/covscript-amd64.deb)
+ 在[Latest Release](https://github.com/covscript/covscript/releases/latest)里下载预编译的二进制文件
+ 使用CMake工具链直接编译源代码
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
`--log-path <PATH>`|`-l <PATH>` |设置日志和语法树导出路径
`--import-path <PATH>`|`-i <PATH>`|设置引入查找路径

**注意，若不设置日志和导出AST路径，这两者将直接输出至标准输出流**
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
`examples`文件夹包含了数个使用CovScript编写的示例程序。
## 版权 ##
**Covariant Script编程语言的作者是[@mikecovlee](https://github.com/mikecovlee/).**  
**版权所有 © 2020 李登淳**
## 捐献 #
![](https://github.com/covscript/covscript/raw/master/docs/Donation.png)
## 感谢 ##
**按贡献排名。**  
+ 顾问：[@ice1000](https://github.com/ice1000/), [@imkiva](https://github.com/imkiva/)
+ 测试：史为成，[@imkiva](https://github.com/imkiva/)
+ 文档：[@imkiva](https://github.com/imkiva/), [@ice1000](https://github.com/ice1000/)
+ 流式API扩展：[@imkiva](https://github.com/imkiva/)
+ IntelliJ插件：[@ice1000](https://github.com/ice1000/)
+ 跨平台支持：[@MouriNaruto](https://github.com/MouriNaruto)