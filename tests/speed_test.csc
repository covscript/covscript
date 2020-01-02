var sum = new array
function push(val)
    sum.push_back(val)
    if sum.size() > 100
        sum.pop_front()
    end
end
loop
    var fc=0, ts=runtime.time()
    loop; fc++; until runtime.time()-ts>=1000
    push(fc)
    var s = 0
    foreach i in sum do s += i
    system.out.println(s/sum.size())
end