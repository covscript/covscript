var lambda=[](x)->x^2-2*x+1
for i=-5,i<=5,++i
    system.out.println(lambda(i))
end
var func=[](n)->n>1?self(n-1)*n:1
foreach i in range(10)
    system.out.println(func(i))
end
var print=[](...args)->args.size>1?(system.out.print(args.front), args.pop_front(), self(args...)):system.out.println(args.front)
print("Hello", ',', "World", '!')
