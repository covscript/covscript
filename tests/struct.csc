struct foo
    var a=10
    function test()
        --this.a
        if(this.a>0)
            this.test()
        end
    end
end
var a as foo
var b=a
system.println(current.a.a)
system.println(current.b.a)
a.test()
system.println(current.a.a)
system.println(current.b.a)
