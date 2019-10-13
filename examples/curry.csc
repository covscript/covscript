struct curry_impl
    var func = null
    var args = {}
    function exec(...arg)
        foreach it in arg do args.push_back(it)
        if args.size() == runtime.argument_count(func)
            return func((this.args)...)
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
function test(a, b, c)
    return a + b + c
end
system.out.println(curry(test)(1)(2)(3))
curry(iostream.ostream.println)(system.out)("Hello")