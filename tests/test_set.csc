function show_set(s)
    var str = "{"
    foreach it in s do str += to_string(it) + ","
    str.cut(1)
    str += "}"
    system.out.println(str)
end

var s1 = {1,2,3,4,5}.to_hash_set()
var s2 = {3,4,5,6,7}.to_hash_set()

var s3 = hash_set.intersect(s1, s2)
show_set(s3)
var s4 = hash_set.merge(s1, s2)
show_set(s4)
var s5 = hash_set.subtract(s1, s3)
show_set(s5)