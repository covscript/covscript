iostream.setprecision(10)
const var precision=10^(-6)
function integral(func,begin,end_)
    var sum=0
    var x=begin
    while(x<=end_)
        sum=sum+func(x)*precision
        x=x+precision
    end
    return sum
end
function f(x)
    return math.sqrt(1-x^2)
end
var ts=runtime.time()
system.out.println(4*integral(f,0,1))
system.out.println(runtime.time()-ts)
system.in.input()
