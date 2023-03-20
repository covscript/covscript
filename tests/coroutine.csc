var co_swap = null, co = null

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

loop
    co = context.create_fiber(worker)
    var i = 0, ts = runtime.time()
    while runtime.time() - ts < 1000
        i = receive()
    end
    system.out.println(i)
    co_swap = null
    context.resume(co)
    co = null
end
