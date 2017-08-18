function push(lst,val)
    var it=lst.begin()
    while it!=lst.term()
        if val<=it.data()
            break
        end
        it.forward()
    end
    lst.insert(it,val)
end
var ts=runtime.time()
var lst=new list
for i=1 to 100
    push(lst,runtime.randint(1,100))
end
system.out.println("Time spend:"+to_string(runtime.time()-ts))
for it iterate lst
    system.out.println(it)
end
