function test(...args)
    if(args.size>0)
        system.out.print(to_string(args.front)+"\t")
        args.pop_front()
        test(args...)
    else
        system.out.println("")
    end
end
constant print=[](...args)->test(args...)
system.out.println("Test 1")
print(1,2,3,4,5,6,7)
system.out.println("Test 2")
print(1,2,{3,4,5}...,6,7)
system.out.println("Test 3")
constant a={1,{2,{3,{4}...,5}...,6}...,7}
print(a...)
system.out.println("Test 4")
var b=5
print({1,{2,{3,4,{b++}...}...,b}...,7}...)
system.out.println("Test 5")
var c={1,{2,{3,4,{--b}...}...,b}...,7}
print(c...)
system.out.println("Test 6")
print({c...}...)