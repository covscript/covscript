loop
    var fc=0
    var ts=runtime.time()
    loop
        fc++
    until runtime.time()-ts>=1000
    end
    system.out.println(fc)
end
