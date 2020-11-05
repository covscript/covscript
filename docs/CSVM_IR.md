# Covariant Script 3+ IR Standard
## IR Literals
Literal|Convention|Function
:---:|:---:|:---:
ID|[A-Za-z_][A-Za-z0-9_]*|标签
INT|[0-9]+|非负整数
NUM|[0-9]+(\\.[0-9]+){0,1}|数值
STR|"(\\\\a\|\\\\b\|\\\\f\|\\\\n\|\\\\r\|\\\\t\|\\\\v\|\\\\\\\\\|\\\\'\|\\\\"\|\\\\0\|[^\\\\'"])*"|文字
## Scope
Mnemonic|Argument|Function
:---:|:---:|:---:
BEG_CHILD|[TAG:ID]|创建子程序
END_CHILD|[TAG:ID]|结束子程序
PUSH_SCOPE|[TYPE:NORMAL\|LOOP\|EXCEPT]|插入作用域
POP_SCOPE|无|弹出作用域
CLS_SCOPE|无|清除作用域

## Variable
Mnemonic|Argument|Function
:---:|:---:|:---:
DEF_VAR|[NAME:ID], [DATA:NUM\|STR]|创建变量
PUSH_VAR|[NAME:ID]|插入变量

## Jump
Mnemonic|Argument|Function
:---:|:---:|:---:
JUMP|[TAG:ID]|无条件跳转
COND|[TAG:ID], [COND:True\|False]|条件跳转
CALL|[TAG:ID], [COUNT:INT]|调用子程序，COUNT为参数数量
RET|无|结束子程序调用

## Exception
Mnemonic|Argument|Function
:---:|:---:|:---:
CATCH|无|在EXCEPT作用域中注册异常处理标签
THROW|无|抛出异常