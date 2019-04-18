function test(...args)
    if(args.size()>0)
        system.out.println(args.front())
        args.pop_front()
        test(args...)
    end
end
test(1,2,3,4,5,6,7)
test(1,2,{3,4,5}...,6,7)

