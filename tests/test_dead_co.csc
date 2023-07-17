function dummy_func()
    return
end

var co = null

foreach i in range(10)
    co = context.create_co(dummy_func)
    loop
        var state = context.resume(co)
    until state == -2
end
