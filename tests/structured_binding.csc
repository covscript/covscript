function test(...args)
    return {args...}
end
var a=0, b=1
(a, b)=test(2, 3)
system.out.println(a)
system.out.println(b)
(a, b)={b, a}
system.out.println(a)
system.out.println(b)