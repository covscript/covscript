import my_ext
constant a=10
constant b=my_ext.test0(to_string(a))
constant c=my_ext.test1(b)
system.out.println(a+b+c)