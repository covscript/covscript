![](https://github.com/mikecovlee/covscript/raw/master/icon/covariant_script_wide.png)
# Covariant Script Programming Language #
**Welcome to the Covariant Script programming language!**  
**Covariant Script** is an open source,cross-platform programming language.
## Language features ##
1. Complete compiler front end
2. Dynamic type
3. Compile-time optimization
4. Reference count garbage collector

## Use the Covariant Script programming language ##
Please visit [Covariant Script Wiki](https://github.com/mikecovlee/covscript/wiki)
## Download and install ##
### Microsoft Windows ###
Please download the precompiled executable on the [Latest Release](https://github.com/mikecovlee/covscript/releases/latest). You can also use MinGW or the appropriate tools to compile the source code directly. If you already have a G++ compiler installed in your system, you can execute `make.bat` directly. You need to make sure that the compiler supports the C++11 standard.  
**Attention! Covariant Script does not support the use of Microsoft CL compiler, the specific reason is not yet clear, probably because of its support for the C++ standard is not complete.**
### Linux ###
After downloading the source code, if you have installed the G++ compiler on your system, you can type `sh./make.sh` directly in the terminal. You need to make sure that the compiler supports the C++11 standard.  
**Attention! Some distributions of the G++ version are too old to support the C++11 standard, please download the latest version of the G++ compiler.**
### Mac OS ###
There is no test, it should be the same with Linux.
## Run ##
`cs <file> <args...>`
## Test ##
The `tests` folder contains several test programs written by CScript.
## Copyright ##
**The author of the Covariant Script programming language is Michael Lee(mikecovlee@163.com).**
