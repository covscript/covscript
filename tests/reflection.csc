import reflect
struct foo
    var a = 1
    var b = 2
    function test()
        system.out.println(this.a)
    end
end
var a = new foo
a.test()
function hack(this)
    system.out.println(this.b)
end
reflect.modify_mem_fn(a.test, hack)
a.test()
reflect.modify_mem_fn(a.test, []()->system.out.println(self.a + self.b))
a.test()
reflect.privileged_do(hack, []()->(hack = a.test))
hack()
reflect.proxy(foo, {"test":([]()->system.out.println(self.a++))})
var b = new foo
b.test()
reflect.modify_mem_fn(b.test, a.test)
b.test()
