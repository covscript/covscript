var test_uid = 0
class test
    var uid = null
    function initialize()
        uid = ++test_uid
        system.out.println("Call initializer: " + uid)
    end
    function duplicate(orig)
        uid = ++test_uid
        system.out.println("Call duplicator: " + orig.uid)
    end
    function finalize()
        system.out.println("Call destroyer: " + uid)
    end
    # Comparators
    function equal(rhs)
        system.out.println("Call comparator: " + uid + " == " + rhs.uid)
        return true
    end
    function greater(rhs)
        system.out.println("Call comparator: " + uid + " > " + rhs.uid)
        return true
    end
    function less(rhs)
        system.out.println("Call comparator: " + uid + " < " + rhs.uid)
        return true
    end
    function greater_equal(rhs)
        system.out.println("Call comparator: " + uid + " >= " + rhs.uid)
        return true
    end
    function less_equal(rhs)
        system.out.println("Call comparator: " + uid + " <= " + rhs.uid)
        return true
    end
    # Operators
    function op_add(rhs)
        system.out.println("Call operator: " + uid + " + " + rhs.uid)
        return this
    end
    function op_sub(rhs)
        system.out.println("Call operator: " + uid + " - " + rhs.uid)
        return this
    end
    function op_mul(rhs)
        system.out.println("Call operator: " + uid + " * " + rhs.uid)
        return this
    end
    function op_div(rhs)
        system.out.println("Call operator: " + uid + " / " + rhs.uid)
        return this
    end
    function op_mod(rhs)
        system.out.println("Call operator: " + uid + " % " + rhs.uid)
        return this
    end
    function op_pow(rhs)
        system.out.println("Call operator: " + uid + " ^ " + rhs.uid)
        return this
    end
    function op_inc()
        system.out.println("Call operator: ++" + uid)
    end
    function op_dec()
        system.out.println("Call operator: --" + uid)
    end
    function op_index(idx)
        system.out.println("Call operator: " + uid + "[" + idx + "]")
        return 0
    end
    function op_call(...args)
        system.out.println("Call operator: " + uid + "(" + args + ")")
        return 0
    end
    # Type Support
    function to_string()
        system.out.println("Call to_string: " + uid)
        return "Hello" + uid
    end
    function hash()
        system.out.println("Call hash: " + uid)
        return uid
    end
end
var lhs = new test, rhs = new test
system.out.print("Swap<")
swap(lhs, rhs)
system.out.println(">Swap")
lhs = rhs
rhs = lhs
lhs == rhs
lhs > rhs
lhs < rhs
lhs >= rhs
lhs <= rhs
lhs + rhs
lhs - rhs
lhs * rhs
lhs / rhs
lhs % rhs
lhs ^ rhs
++lhs
rhs++
--lhs
rhs--
lhs[2333]
rhs(1, 2, 3, 4)
system.out.println("String value = " + to_string(lhs))
system.out.println("Hash value = " + runtime.hash(rhs))