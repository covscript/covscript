##Covariant Basic编程语言--入门手册
###Covariant Basic简介
**Covariant Basic**编程语言是经典的Basic编程语言的变种，在传统Basic的基础上混合了现代编程语言的风格，易于入门与使用。初学者可以将**Covariant Basic**编程语言作为入门语言，学习曲线平缓，向高级语言过渡更容易（相对于传统Basic语言，**Covariant Basic**与C/C++/Java等高级语言更类似）。此外，**Covariant Basic**编程语言支持与C++编程语言无缝对接，用户可以将Covariant Basic编程语言作为主程序的配置脚本，方便强大。  
链接：**[Covariant Basic官方解释器项目](https://github.com/mikecovlee/covbasic.git)**  
###第一章：基础组件
**在这一章我们将会学习Covariant Basic语言基础的输入输出和变量功能。**
####第一节：Hello World！
在学习Covariant Basic之前您首先需要到**[Covariant Basic官方解释器项目](https://github.com/mikecovlee/covbasic.git)**中下载Covariant Basic解释器。  
**Windows 32位**请下载CovBasic[版本号]_win32_i386.exe  
**Windows 64位**请下载CovBasic[版本号]_win32_amd64.exe  
下载的.exe文件直接双击打开即可。  
**Linux 32位**请下载CovBasic[版本号]_linux_i386  
**Linux 64位**请下载CovBasic[版本号]_linux_amd64  
下载的二进制可执行文件您需要进入**终端模拟器**并转到文件所在目录输入**./文件名**执行程序。  
打开Covariant Basic解释器后直接输入代码运行即可。您也可以将代码保存在文本文件中并在Covariant Basic解释器中输入**`Exec("文件名")`**并回车执行。但是一般来说我们习惯将Covariant Basic源文件后缀名改为**.cbs**(Covariant Basic Source)。
  
所有的准备工作完成后，现在您需要在解释器中直接输入或保存在文件中使用**`Exec("文件名")`**指令执行下面的程序并观察效果。  
**例程1.1:**  
``
Print("Hello world")
``  
您可以看见窗口上显示出一行字，如下图所示：
![](https://github.com/mikecovlee/covbasic/raw/master/doc/images/Ex1-1.png)
  
就像你看到的，``Print``函数是CovBasic的**输出函数**，通过这个函数我们可以把一些内容打印到屏幕上。  
``Print``函数可以接收数量不定的参数，参数之间用逗号隔开。比如：  
``Print("Hello"," world")``  
这个语句与刚才的效果是一样的。
####第二节：输出问候语句
在上一节中我们学习了如何输出一个句子，本节中我们将学习如何定义变量以及获取输入。  
**例程1.2:**  
``
Print("Please enter your name:")
Define name
Input(name)
Print("Hello,",name,"!")
``  
效果如下：  
![](https://github.com/mikecovlee/covbasic/raw/master/doc/images/Ex1-2.png)