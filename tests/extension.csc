import my_ext
constant a=10
var      b=my_ext.test(a)
constant c=my_ext.child.test(a)
system.out.println(a+b+c)