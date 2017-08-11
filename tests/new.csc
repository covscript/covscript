struct foo
    var a=10
end
var a=new pointer
if a==null
    system.println("Null")
end
a=gcnew foo
++a->a
system.println((*a).a)
