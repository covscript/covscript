function cs()
    var map=new hash_map
    var test=[](i)->i
    function func(name,value,consumer)
        map.insert(name, value)
        consumer(map)
        map.erase(name)
        return null
    end
    func("reimu",100,[](it)->test(233+it.at("reimu")))
end
function test(f, count)
    for i=1,i<=count,++i
        f(i)
    end
end

var sum = 0, pass = 0
loop
    var start=runtime.time()
    test([](i)->cs(),200000)
    var time=runtime.time()-start
    sum += time;
    ++pass
    system.out.println(to_string(time) + ", avg: " + to_integer(sum/pass) + ", pass: " + pass)
end