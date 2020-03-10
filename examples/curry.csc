struct curry_impl
    var func = null
    var args = {}
    function exec(...arg)
        foreach it in arg do args.push_back(it)
        if args.size >= runtime.argument_count(func)
            return func(args...)
        else
            return exec    
        end
    end
end
function curry(func)
    var cur = new curry_impl
    cur.func = func
    return cur.exec
end
struct bind_placeholder
    var posit = 0
end
function placeholder(n)
    var ph = new bind_placeholder
    ph.posit = n
    return ph
end
struct bind_impl
    var func = null
    var phs = {}
    function exec(...args)
        foreach it in phs
            if typeid it == typeid bind_placeholder
                it = args.at(it.posit - 1)
            end
        end
        return func(phs...)
    end
end
function bind(...args)
    if args.size < 2
        throw runtime.exception("Bind needs arguments")
    end
    var bnd = new bind_impl
    bnd.func = args.front
    args.pop_front()
    if args.size != runtime.argument_count(bnd.func)
        throw runtime.exception("Must bind all of the arguments")
    end
    bnd.phs = args
    return bnd.exec
end
function test(a, b, c)
    return a + b + c
end
system.out.println(curry(test)(1, 2)(3))
curry(iostream.ostream.println)(system.out)("Hello") 
system.out.println(bind(test, placeholder(2), 2, placeholder(1))(1, 3))
bind(iostream.ostream.println, placeholder(1), "Hello")(system.out)