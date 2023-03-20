var co_swap = null, co = null

class monitor
    var id = ""
    function finalize()
        #system.out.println("OUT " + id)
    end
end

function make_monitor(id)
    var m = new monitor
    m.id = id
    return move(m)
end

function send(val)
    co_swap := val
    context.yield()
end

function receive()
    context.resume(co)
    return co_swap
end

function worker()
    var i = 0
    loop
        send(++i)
    until co_swap == null
end

#var m = make_monitor("main")

#loop
    co = context.create_fiber(worker)
    #var i = 0, ts = runtime.time()
    #while runtime.time() - ts < 1000
        var i = receive()
    #end
    system.out.println(i)
    co_swap = null
    context.resume(co)
    co = null
#end
