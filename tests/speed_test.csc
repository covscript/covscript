loop
    var fc=0, ts=runtime.time()
    loop; ++fc; until runtime.time()-ts>=1000
    system.out.println(fc)
end