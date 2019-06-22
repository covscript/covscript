function test(...args)
    return {args...}
end
var (a, (b, c), d)={1, {2, 3}, 4}
system.out.println(a)
system.out.println(b)
system.out.println(c)
system.out.println(d)
(a, b, (c, d))=test(5, 6, test(7, 8))
system.out.println(a)
system.out.println(b)
system.out.println(c)
system.out.println(d)
(a, b)={b, a}
system.out.println(a)
system.out.println(b)
system.out.println(c)
system.out.println(d)
for (i, j)={0, 1}, i+j<10, i=j++ do system.out.println(to_string(i)+":"+to_string(j))
