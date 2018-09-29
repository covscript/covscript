var fib=[](n)->n>1?fib(n-1)+fib(n-2):n
for i=0,i<=20,++i
    system.out.println(fib(i))
end