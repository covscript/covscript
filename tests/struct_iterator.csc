struct wrapper_iterator
    var ref = null
    function set_ref(obj)
        ref = &obj
        return this
    end
    function next()
        if ++ref->counter > 10
            return {ref->counter, true}
        else
            return {ref->counter, false}
        end
    end
end

struct wrapper
    var counter = 0
    function get_iterator()
        return (new wrapper_iterator).set_ref(this)
    end
end

var a = new wrapper
foreach it in a.get_iterator()
    system.out.println("Iteration: " + it)
    system.out.println("Counter: " + a.counter)
end

system.out.println("Final Counter: " + a.counter)
