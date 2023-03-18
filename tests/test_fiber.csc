var channel = context.create_channel()

function async_func()
    foreach i in range(3)
        system.out.println("[async_func] Waiting for... lasting " + (3 - i) + "s")
        runtime.delay(1000)
    end
    return 22
end

function co_a()
    var i = channel.pop()
    system.out.println(i)
    i = channel.pop()
    system.out.println(i)
end

function co_b()
    system.out.println(20)
    context.yield()
    system.out.println(21)
    var val = context.await(async_func)
    system.out.println(val)
end

var rt1 = context.create_fiber(co_a), rt2 = context.create_fiber(co_b)

system.out.println("00")
context.resume(rt1)

system.out.println("01")
context.resume(rt2)

system.out.println("02")
channel.push(10)

system.out.println("03")
context.resume(rt2)

system.out.println("04")
channel.push(11)

system.out.println("05")
foreach i in range(6)
    system.out.println("[main] Waiting for... lasting " + (6 - i) + "s")
    runtime.delay(1000)
end
context.resume(rt2)
