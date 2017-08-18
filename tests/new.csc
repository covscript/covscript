struct foo
    var a=10
end
var a=new pointer
if a==null
    system.out.println("Null")
end
a=gcnew foo
++a->a
system.out.println((*a).a)
