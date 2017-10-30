struct foo
    var a=10
    function test()
        --a
        if(a>0)
            test()
        end
    end
end
var a=new foo
var b=a
system.out.println(current.a.a)
system.out.println(current.b.a)
a.test()
system.out.println(current.a.a)
system.out.println(current.b.a)
