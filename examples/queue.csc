function push(lst,val)
    var it=lst.begin
    while it!=lst.end
        if val<=it.data
            break
        end
        it.next()
    end
    lst.insert(it,val)
end
var ts=runtime.time()
var lst=new list
for i=1,i<=100,++i
    push(lst,math.randint(1,100))
end
system.out.println("Time spend:"+to_string(runtime.time()-ts))
foreach it in lst
    system.out.println(it)
end
