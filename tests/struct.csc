struct foo
    var a=10
    function test()
        --this.a
        if(this.a>0)
            this.test()
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
