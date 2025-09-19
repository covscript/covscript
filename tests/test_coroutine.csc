var channel = new fiber.channel
var co_b_exit = false

function co_a()
    var i = channel.pop()
    system.out.println(i + ", from co_a")
    i = channel.pop()
    system.out.println(i + ", from co_a")
end

function co_b(i)
    system.out.println(i + ", from co_b")
    fiber.yield()
    system.out.println("21, async waiting for input")
    var val = runtime.await(system.in.getline)
    system.out.println("22, input = " + val)
    co_b_exit = true
end

var rt1 = fiber.create(co_a), rt2 = fiber.create(co_b, "20")

system.console.gotoxy(0,0)

system.out.println("00, resume co_a")
rt1.resume()

system.out.println("01, resume co_b")
rt2.resume()

system.out.println("02, push 10")
channel.push("10")

system.out.println("03, resume co_b")
rt2.resume()

system.out.println("04, push 11")
channel.push("11")

system.out.println("05, waiting for co_b exit")
var count = 0
loop
    ++count
    rt2.resume()
until co_b_exit
var blank = new string
foreach i in range(20) do blank += " "
system.out.print(blank + "\r")
system.out.println("06, count = " + count)
