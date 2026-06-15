# Regression test for container/string index bounds checking.
#
# Out-of-range element access through .at(), and out-of-range positions for
# string operations, must throw a catchable CovScript exception instead of
# crashing the interpreter or triggering undefined behaviour (e.g. cut() with
# a count larger than the string length used to call pop_back() past begin()).

# Valid accesses still work.
var arr = {10, 20, 30}
system.out.println("arr.at(1) = " + to_string(arr.at(1)))

var str = "Hello"
system.out.println("str.substr(1, 3) = " + str.substr(1, 3))
system.out.println("str.cut(2) = " + str.cut(2))

var map = new hash_map
map.insert("key", 123)
system.out.println("map.at(key) = " + to_string(map.at("key")))

# Out-of-range / invalid accesses must throw catchable errors.
var bad = {10, 20, 30}
try
    bad.at(5)
    system.out.println("array.at out of range: FAIL")
catch e
    system.out.println("array.at out of range: OK (" + e.what + ")")
end

try
    bad.at(-1)
    system.out.println("array.at negative: FAIL")
catch e
    system.out.println("array.at negative: OK (" + e.what + ")")
end

var s = "Hi"
try
    s.substr(10, 1)
    system.out.println("string.substr bad position: FAIL")
catch e
    system.out.println("string.substr bad position: OK (" + e.what + ")")
end

try
    s.cut(10)
    system.out.println("string.cut too large: FAIL")
catch e
    system.out.println("string.cut too large: OK (" + e.what + ")")
end

try
    map.at("missing")
    system.out.println("hash_map.at missing key: FAIL")
catch e
    system.out.println("hash_map.at missing key: OK (" + e.what + ")")
end

system.out.println("bounds check test done")
