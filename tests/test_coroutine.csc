var channel = new context.channel
var co_b_exit = false

function co_a()
    var i = channel.pop()
    system.out.println(i + ", from co_a")
    i = channel.pop()
    system.out.println(i + ", from co_a")
end

function co_b(i)
    system.out.println(i + ", from co_b")
    context.yield()
    system.out.println("21, async waiting for input")
    var val = context.await(system.in.getline)
    system.out.println("22, input = " + val)
    co_b_exit = true
end

var rt1 = context.create_co(co_a), rt2 = context.create_co_s(co_b, {"20"})

system.out.println("00, resume co_a")
context.resume(rt1)

system.out.println("01, resume co_b")
context.resume(rt2)

system.out.println("02, push 10")
channel.push("10")

system.out.println("03, resume co_b")
context.resume(rt2)

system.out.println("04, push 11")
channel.push("11")

system.out.println("05, waiting for co_b exit")
var count = 0
loop
    ++count
    context.resume(rt2)
until co_b_exit
system.out.println("06, count = " + count)
