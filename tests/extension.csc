import my_ext
constant a=30
var      b=my_ext.test(my_ext.val)
constant c=my_ext.child.test(my_ext.child.val)
system.out.println(a+b+c)
system.out.println(a+my_ext.test_v(my_ext.val_v)+my_ext.child.test_v(my_ext.child.val_v))
var foo0=new my_ext.foo
system.out.println(foo0.test(10))
system.out.println(type(foo0))
var foo1=new my_ext.child.foo
system.out.println(foo1.test("Hello"))
system.out.println(type(foo1))