var fib=[](n)->n>1?fib(n-1)+fib(n-2):n
for i=0 to 20
    system.out.println(fib(i))
end