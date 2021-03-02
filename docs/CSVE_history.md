# Covariant Script Vulnerabilities & Exposures #

*本文件列出了影响较大或至少影响一个Release版本的Bug，各类信息仅供参考*  
*为了避免出现不可预知的错误，请尽量更新至最新版本*

## CSVE-2018-01-01 ##

+ 触发方法：在三目运算符中进行递归
+ 漏洞原理：CovScript在计算三目运算符时会将两个目标值全部计算从而可能造成无限递归从而触发栈溢出
+ 漏洞状态：已修复
+ 影响范围：CovScript 1.1.0-1.2.1(Beta1)

## CSVE-2018-01-02 ##

+ 触发方法：在函数调用后直接使用点运算符
+ 漏洞原理：CovScript编译器对运算符的推断出现了误判导致AST构建错误，CovScript解释器未检查AST格式的情况下直接解释导致段错误
+ 漏洞状态：已修复
+ 影响范围：CovScript 1.2.0-1.2.1(Beta2)

## CSVE-2018-01-03 ##

+ 触发方法：在三目运算符中使用两个常量
+ 漏洞原理：CovScript优化器在对AST进行剪枝时会误剪掉符合条件的三目运算符的两个分支，导致AST格式丢失报错
+ 漏洞状态：已修复
+ 影响范围：CovScript 1.2.1(Beta3)

## CSVE-2018-01-04 ##

+ 触发方式：嵌套Import并调用其中的函数
+ 漏洞原理：CovScript在去年八月的大重构中忽略了更换嵌套的Import实例的宿主环境(注：宿主环境在解释完需要引入的包之后会自动销毁)，导致资源不可用
+ 漏洞状态：已修复
+ 影响范围：CovScript 1.1.1-1.2.1(Beta4)

## CSVE-2018-01-05 ##

+ 触发方法：在`->`运算符中引用当前作用域中存在的常量名
+ 漏洞原理：未正确处理优化器的优化规则导致优化器误判名称作用域
+ 漏送状态：已修复
+ 影响范围：CovScript 1.0.0-1.2.1(Beta5)

## CSVE-2018-01-06 ##

+ 触发方法：在某些场景中使用括号
+ 漏洞原理：AST构建索引时漏掉了对根结点的更改
+ 漏洞状态：已修复
+ 影响范围：CovScript 1.0.0-1.2.1(Beta6)

## CSVE-2018-02-01 ##

+ 触发方法：在NDK上编译CovScript
+ 漏洞原理：语法不严谨
+ 漏洞状态：已修复
+ 影响范围：CovScript 1.2.1(Beta2)-1.2.1(Release1)

## CSVE-2018-02-02 ##

+ 触发方法：CNI抛出错误
+ 漏洞原理：CNI错误处理程序访问了不可达的指针
+ 漏洞状态：已修复
+ 影响范围：CovScript 1.2.0-1.2.1(Release1)

## CSVE-2018-03-01 ##

+ 触发方法：在`->`运算符中引用结构体中存在的变量名
+ 漏洞原理：优化器缺少优化规则导致无法自动推断`this`
+ 漏洞状态：已修复
+ 影响范围：CovScript 1.2.1(Beta5)-1.2.2(Beta4)

## CSVE-2018-08-01 ##

+ 触发方法：使用运行时类型信息(RTTI)时类型ID冲突
+ 漏洞原理：运行时类型信息系统的类型ID生成逻辑不严谨
+ 漏洞状态：已修复
+ 影响范围：CovBasic 2.1.2.3-CovScript 1.3.0 Panthera uncia(Unstable) Build 4

## CSVE-2018-08-02 ##

+ 触发方法：使用`import`，`using`，`const var`后污染`global`名称空间
+ 漏洞原理：代码生成器设计失误
+ 漏洞状态：已修复
+ 影响范围：CovScript 1.2.2(Beta6)-1.3.0 Panthera uncia(Unstable) Build 2

## CSVE-2018-12-01 ##

+ 触发方法：函数的参数与上层常量重名时会忽略函数参数的声明
+ 漏洞原理：代码生成器未给予优化器正确的参数
+ 漏洞状态：已修复
+ 影响范围：CovScript 1.2.1-3.0.0 Pantholops hodgsonii(Unstable) Build 3

## CSVE-2019-04-01 ##

+ 触发方法：任何场景
+ 漏洞原理：编译器重复调用不必要的预处理函数，仅影响编译器性能
+ 漏洞状态：已修复
+ 影响范围：CovScript 1.0.3-3.2.0 John von Neumann(Preview) Build 3

## CSVE-2019-04-02 ##

+ 触发方法：开启优化器且上层空间存同名在常量时会忽略当前作用域中的变量
+ 漏洞原理：优化器规则设计失误
+ 漏洞状态：已修复
+ 影响范围：CovScript 1.2.2(Beta4)-3.2.0 John von Neumann(Preview) Build 3

## CSVE-2019-10-01 ##

+ 触发方法：在结构体中声明变参函数
+ 漏洞原理：参数入栈时未考虑this参数
+ 漏洞状态：已修复
+ 影响范围：CovScript 3.1.2 Ovis ammon(Unstable) Build 2-3.2.1 Giant panda(Stable) Build 12

## CSVE-2019-11-01 ##

+ 触发方法：声明多于一个常量，并以动态形式访问
+ 漏洞原理：代码生成器错误，生成多个重复的语句(修复CSVE-2018-08-02时带来的问题)
+ 漏洞状态：已修复
+ 影响范围：CovScript 1.3.0 Panthera uncia(Unstable) Build 2-3.3.1 Manis pentadactyla(Unstable) Build 5