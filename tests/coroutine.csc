var co_swap = null

function send(val)
    co_swap := val
    context.yield()
end

function receive(co)
    context.resume(co)
    return co_swap
end

function worker()
    var i = 0
    loop
        send(++i)
    until co_swap == null
end

foreach it in range(10)
    var co_list = new array
    foreach i in range(10) do co_list.push_back(context.create_co(worker))
    var i = 0, ts = runtime.time()
    while runtime.time() - ts < 1000
        foreach co in co_list do i += receive(co)
    end
    system.out.println(i)
    co_swap = null
    foreach co in co_list do context.resume(co)
    co_list = null
end
