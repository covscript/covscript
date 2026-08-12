function named(a, b)
    return a + b
end
var l = [](x)->x * 2
var co = fiber.create(l, 21)
co.resume()
system.out.println(runtime.argument_count(named))
system.out.println(runtime.argument_count(l))
system.out.println(l(21))
