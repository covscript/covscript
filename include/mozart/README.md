Covariant Mozart Utility Library Guide
-----------------------------------------------
**智锐随变：莫扎特 实用工具库指南**

### 1:License(使用协议)
**This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or any later version.**  
**This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the GNU General Public License for more details.**  
*You should have received a copy of the GNU General Public License along with this program.  If not, see <http://www.gnu.org/licenses/>.*  

**Copyright (C) 2017 Michael Lee(李登淳)**  

Email: mikecovlee@163.com  
Github: https://github.com/mikecovlee  

### 2:Warning Codes and Error Codes Reference(警告码与错误码对照表)

##### English:
>  
*Note:  
I am a Chinese guy.My English is very poor.So I can't complete this reference by myself.  
Please contact me if you want to help me translate this guide.Thanks!*  

W0001 Tag type is different from condition type  
W0002 Duplicate tag  

E0001 Covariant Mozart Utility Library requires your compiler to support the C++ programming language.  
E0002 Covariant Mozart Utility Library requires your compiler to support C++11 or higher standards. Please check that you have forgotten the `-std=c++11` compiler option.  
E0003 Objects are not executable.  
E0004 Calls the member function with a null pointer.  
E0005 Uses uninitialized objects.  
E0006 The requested type is different from the object type.  
E0007 Parameter list error.  
E0008 Parameter type error.  
E0009 Parameter is incorrect.  
E000A Does not support receiving any parameters.  
E000B Type match failed.  
E000D Does not support conversion of the specified type to `std::string`  
E000E Tree node is empty  
E000F Does not support the specified type of hash operation  
E000G Constant tagged objects can not be copied  

##### 简体中文:
W0001 标签类型与条件类型不同  
W0002 重复定义标签  

E0001 Covariant Mozart Utility Library需要您的编译器支持C++编程语言。  
E0002 Covariant Mozart Utility Library需要您的编译器支持C++11或者更高标准。请检查您否忘记了`-std=c++11`编译选项。  
E0003 对象不可执行。  
E0004 使用空指针调用成员函数。  
E0005 使用了未初始化的对象。  
E0006 请求的类型与对象类型不同。  
E0007 参数列表错误。  
E0008 参数类型错误。  
E0009 参数数量错误。  
E000A 不支持接收任何参数。  
E000B 类型匹配失败。  
E000D 不支持指定类型向`std::string`的转换  
E000E 树节点为空  
E000F 不支持指定类型的hash操作  
E000G 常量标记的对象不可复制  
