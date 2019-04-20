function test(...args)
    if(args.size()>0)
        system.out.print(to_string(args.front())+"\t")
        args.pop_front()
        test(args...)
    end
end
system.out.println("Test1")
test(1,2,3,4,5,6,7)
system.out.println("")
system.out.println("Test2")
test(1,2,{3,4,5}...,6,7)
system.out.println("")
system.out.println("Test3")
constant c={1,{2,{3,{4}...,5}...,6}...,7}
test(c...)
system.out.println("")
system.out.println("Test4")
var b=5
test({1,{2,{3,4,b++}...,b}...,7}...)
system.out.println("")