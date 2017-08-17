function test(a)
    system.out.println(a)
    if(a>0)
        return test(a-1)
    else
        return a
    end
end
system.out.println("Please enter a intger:")
var in=system.in.input()
test(in)
system.out.println(in)
