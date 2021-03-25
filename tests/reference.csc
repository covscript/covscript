var a = 10
var b = {20}
context.link_var("a", b[0])
++a
system.out.println(a)
system.out.println(b[0])
a = "Hello"
system.out.println(a)
system.out.println(b[0])
context.unlink_var("a")
a = 10
system.out.println(a)
system.out.println(b[0])