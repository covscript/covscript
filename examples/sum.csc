iostream.setprecision(10)
var s=0
var n=0
loop
    var r=1/(2*n+1)
    if(n%2==0)
        s=s+r
    else
        s=s-r
    end
    ++n
until r<10^(-6)
system.out.println(s)
system.out.println(math.constants.pi/4)
