var sum = 0, pass = 0
loop
    var fc=0, ts=runtime.time()
    loop; ++fc; until runtime.time()-ts>=1000
    sum += fc;
    ++pass
    system.out.println(to_string(fc) + ", avg: " + to_integer(sum/pass) + ", pass: " + pass)
end