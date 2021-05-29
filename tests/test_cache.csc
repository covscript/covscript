var a = 0
function test(_a)
    system.out.println("begin")
    system.out.println(a)
    var a = _a + 1
    system.out.println(a)
    if a < 10
        test(a)
    end
    system.out.println(a)
end
test(0)