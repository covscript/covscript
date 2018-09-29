function test()
    var a=new array
    loop
        var num=math.randint(math.constant.min,math.constant.max)
        foreach it:a
            if math.abs(num-it)<10000
                return a.size()
            end
        end
        a.push_back(num)
    end
end
loop
    var min=0
    var avg=0
    for it=0,it<=100,++it
        var res=test()
        if res<min||min==0
            min=res
        end
        avg=to_integer((avg+res)/2)
    end
    system.out.println("Min:"+min)
    system.out.println("Avg:"+avg)
end
