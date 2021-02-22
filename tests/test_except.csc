function test_2(a)
    if a > 10
        throw runtime.exception("Hello")
    else
        return a + 1
    end
end
function test_1(i)
    try
        return test_2(i)
    catch e
        system.out.print(e.what)
        return ", World!"
    end
end
system.out.println(test_1(system.in.input()))