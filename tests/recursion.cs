function test(a)
    system.println(a)
    if(a>0)
        return test(a-1)
    else
        return a
    end
end
system.println("Please enter a intger:")
define in=0
system.input(in)
test(in)
system.println(in)
