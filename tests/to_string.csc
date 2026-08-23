class test
    var str = "Hello"
    function to_string()
        return str
    end
end
system.out.println({{}, null, 3,14 : 'A', type(system.out), typeid test, range(10), new test, {gcnew test, 3,14, 'A'}.to_list()})
var m = {"abc" : "def", "ghi" : null}.to_hash_map()
var keys = m.keys()
keys.sort([](lhs, rhs)-> lhs < rhs)
foreach k in keys
    system.out.println(k + " = " + to_string(m[k]))
end
var hs = {"abc", "def", "ghi"}.to_hash_set()
var items = {}
foreach it in hs do items.push_back(it)
items.sort([](lhs, rhs)-> lhs < rhs)
system.out.println(items)
