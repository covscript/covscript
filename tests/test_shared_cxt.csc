import test_shared_cxt as test

function co_main(queue, msg)
    system.out.println(msg)
    foreach i in range(10)
        queue.yield(i)
        if queue.avail()
            system.out.println(queue.get()[0])
        end
    end
    system.out.println("Bye~")
end

var co = new test.coroutine
co.construct(co_main)

co.join("Hello")
var val = 0
loop
    if co.queue.avail()
        val = co.get()[0]
        system.out.println(val)
    end
until !co.resume(val + 1)
