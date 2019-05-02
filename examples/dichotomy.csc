iostream.setprecision(20)
var f=[](x)->math.sin(x)
var a=3
var b=4
loop
    var c=(a+b)/2
    if f(a)*f(c)<0
        b=c
    else
        a=c
    end
until b-a<10^(-8)
system.out.println(b)
loop
    var c=(a+b)/2
    if f(a)*f(c)<0
        b=c
    else
        a=c
    end
until b-a<10^(-18)
system.out.println(b)
