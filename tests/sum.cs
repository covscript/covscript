system.setprecision(10)
define s=0
define n=0
loop
    define r=1/(2*n+1)
    if(n%2==0)
        s=s+r
    else
        s=s-r
    end
    ++n
until r<10^(-6)
end
system.println(s)
system.println(math.pi/4)
