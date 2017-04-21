![](https://github.com/mikecovlee/covbasic/blob/master/icon/basic2.0.jpg)
# Covariant Basic Programming Language #
**欢迎使用Covariant Basic编程语言！**  
**Covariant Basic**是一种开源的跨平台编程语言
## 下载与安装 ##
### Microsoft Windows ###
请访问[Covariant Basic Programming Language Main Page](http://ldc.atd3.cn/cbs)下载预编译的可执行文件。你也可以使用MinGW或合适的工具直接编译源码，如你的系统中已经安装了G++编译器你可以直接执行`make.bat`，你需要确保编译器支持C++14标准。  
**注意！Covariant Basic不支持使用Microsoft CL编译，具体原因目前还不清楚，可能是因为其对C++14标准的支持并不完全。**
### Linux ###
下载源码后，如果你的系统中已安装G++编译器你可以直接在终端中输入`sh ./make.sh`来编译，你需要确保编译器支持C++14标准。  
**注意！有些发行版的G++版本太老，不支持C++14标准，请一定下载最新版G++编译器。**
### Mac OS ###
暂时没有测试，应该与Linux无异。
## 运行 ##
`cbs2 <file> <args...>`
## 测试 ##
在`tests`文件夹中包含了数个由CovBasic编写的测试程序。
## 版权 ##
**Covariant Basic编程语言**的作者是李登淳(Michael Lee,mikecovlee@github.com)