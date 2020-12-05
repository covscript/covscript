Covariant Mozart Utility Library Guide
-----------------------------------------------
**智锐科创：莫扎特 实用工具库指南**

### 1:License(使用协议)
**Licensed under the Covariant Innovation General Public License,**  
**Version 1.0 (the "License");**  
**you may not use this file except in compliance with the License.**  
**You may obtain a copy of the License at**  
  
**https://covariant.cn/licenses/LICENSE-1.0**  
  
**Unless required by applicable law or agreed to in writing, software**  
**distributed under the License is distributed on an "AS IS" BASIS,**  
**WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.**  
**See the License for the specific language governing permissions and**  
**limitations under the License.**  

**Copyright (C) 2020 Michael Lee(李登淳)**  

Email: mikecovlee@163.com  
Github: https://github.com/mikecovlee  

### 2:Warning Codes and Error Codes Reference(警告码与错误码对照表)

##### English:
>  
*Note:  
I am a Chinese guy.My English is very poor.So I can't complete this reference by myself.  
Please contact me if you want to help me translate this guide. Thanks!*  

Code|Detail
:---:|:---:
W0001|Tag type is different from condition type  
W0002|Duplicate tag  
E0001|Covariant Mozart Utility Library requires your compiler to support the C++ programming language.  
E0002|Covariant Mozart Utility Library requires your compiler to support C++11 or higher standards. Please check that you have forgotten the `-std=c++11` compiler option.  
E0003|Objects are not executable.  
E0004|Calls the member function with a null pointer.  
E0005|Uses uninitialized objects.  
E0006|The requested type is different from the object type.  
E0007|Parameter list error.  
E0008|Parameter type error.  
E0009|Parameter is incorrect.  
E000A|Does not support receiving any parameters.  
E000B|Type match failed.  
E000D|Does not support conversion of the specified type to `std::string`  
E000E|Tree node is empty  
E000F|Does not support the specified type of hash operation  
E000G|Constant tagged objects can not be copied  
E000H|Stack is empty  
E000I|Stack overflow  
E000J|The variable has been protected.  
E000K|It is not allowed to change the value of a constant.  
E000L|Duplicate singleton objects are not allowed.  
E000M|Does not support conversion of the specified type to `long`.  

##### 简体中文:

代码|含义
:---:|:---:
W0001|标签类型与条件类型不同  
W0002|重复定义标签  
E0001|Covariant Mozart Utility Library需要您的编译器支持C++编程语言。  
E0002|Covariant Mozart Utility Library需要您的编译器支持C++11或者更高标准。请检查您否忘记了`-std=c++11`编译选项。  
E0003|对象不可执行。  
E0004|使用空指针调用成员函数。  
E0005|使用了未初始化的对象。  
E0006|请求的类型与对象类型不同。  
E0007|参数列表错误。  
E0008|参数类型错误。  
E0009|参数数量错误。  
E000A|不支持接收任何参数。  
E000B|类型匹配失败。  
E000D|不支持指定类型向`std::string`的转换  
E000E|树节点为空  
E000F|不支持指定类型的hash操作  
E000G|不允许降低变量的保护级别  
E000H|栈为空  
E000I|栈溢出  
E000J|变量已被保护  
E000K|不允许更改常量的值  
E000L|不允许复制单例对象  
E000M|不支持指定类型向`long`的转换
E000N|合并非本树的节点