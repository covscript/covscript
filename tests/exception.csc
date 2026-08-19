using system
# exception handling: throw, catch, .what, propagation, nesting
function boom(msg)
    throw runtime.exception(msg)
end
try
    boom("kaboom")
    system.out.println("no throw: FAIL")
catch e
    system.out.println("caught: " + e.what)
end
function outer()
    boom("deep")
end
try
    outer()
    system.out.println("no propagate: FAIL")
catch e
    system.out.println("propagated: " + e.what)
end
try
    try
        boom("inner")
        system.out.println("inner no throw: FAIL")
    catch e
        system.out.println("inner caught: " + e.what)
    end
    system.out.println("after inner")
catch e
    system.out.println("outer caught (unexpected): " + e.what)
end
