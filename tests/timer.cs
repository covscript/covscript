var ts=runtime.time()
var fc=0
loop
    if(runtime.time()-ts>=1000)
        system.println(fc)
        fc=0
        ts=runtime.time()
    else
        fc++
    end
end
