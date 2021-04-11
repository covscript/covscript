function test(a)
    ++a
end

function test_unlink(a)
    context.unlink_var("a")
    ++a
end

var a = 10
test(a)
system.out.println(a)
test_unlink(a)
system.out.println(a)

link b = a
++b
system.out.println(a)
context.unlink_var("b")
++b
system.out.println(a)