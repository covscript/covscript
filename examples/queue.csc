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
var lst=new list
for i=1 to 100
    push(lst,runtime.randint(1,100))
end
for it iterate lst
    system.println(it)
end
