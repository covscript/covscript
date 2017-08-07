struct foo
    var a=10
end
var a as pointer
if a==null
    system.println("Null")
end
a=new foo
++a->a
system.println((*a).a)
