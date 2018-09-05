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
system.out.println(local.a.a)
system.out.println(local.b.a)
a.test()
system.out.println(local.a.a)
system.out.println(local.b.a)
