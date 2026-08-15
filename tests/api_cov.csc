using system
constant File = system.file
constant Path = system.path

# math functions
system.out.println("math.abs: " + (to_integer(math.abs(-5)) == 5))
system.out.println("math.ln: " + (math.ln(2.7182818) > 0.9 && math.ln(2.7182818) < 1.1))
system.out.println("math.trig: " + (math.tan(0.0) == 0.0 && math.asin(0.0) == 0.0 && math.acos(1.0) == 0.0 && math.atan(0.0) == 0.0))
var rnd = math.rand(0.0, 1.0)
system.out.println("math.rand: " + (rnd >= 0 && rnd < 1))

# number methods
var flt = 3.14
system.out.println("number.to_float: " + (to_integer(flt.to_float() * 100) == 314))
system.out.println("number.is_float: " + flt.is_float())

# array methods
var arr = {1, 2, 3}
system.out.println("array.front/back: " + (arr.front == 1 && arr.back == 3))
system.out.println("array.empty/size: " + (arr.empty() == false && arr.size == 3))
system.out.println("array.enumerate: " + (arr.enumerate().size == 3))
system.out.println("array.to_list: " + (arr.to_list().size == 3))
arr.clear()
system.out.println("array.clear: " + arr.empty())

# hash_map methods
var hm = {1:10, 2:20}.to_hash_map()
system.out.println("hash_map.keys/values: " + (hm.keys().size == 2 && hm.values().size == 2))
hm.clear()
system.out.println("hash_map.clear: " + hm.empty())

# hash_set methods
var hs = {1,2,3}.to_hash_set()
system.out.println("hash_set.size/empty: " + (hs.size == 3 && hs.empty() == false))
hs.insert(4)
system.out.println("hash_set.insert/exist: " + (hs.exist(4) && hs.size == 4))
hs.erase(4)
system.out.println("hash_set.erase: " + (hs.exist(4) == false && hs.size == 3))
var hs2 = {2,3}.to_hash_set()
system.out.println("hash_set.subtract: " + (hash_set.subtract(hs, hs2).size == 1))
hs.clear()
system.out.println("hash_set.clear: " + hs.empty())

# runtime introspection
system.out.println("runtime.sdk_path: " + (runtime.get_sdk_path() != ""))
system.out.println("runtime.import_path: " + (runtime.get_import_path() != ""))
system.out.println("runtime.cwd: " + (runtime.get_current_dir() != ""))

# file / path operations (with cleanup)
Path.mkdir("./_api_dir")
system.out.println("path.mkdir/exist/is_file: " + (Path.exist("./_api_dir") && !Path.is_file("./_api_dir")))
function write_test_file()
    # local stream: the handle must be closed before the files are deleted
    var fs = iostream.ofstream("./_api_dir/a.txt")
    fs.println("data")
    fs.flush()
end
write_test_file()
system.out.println("file.copy: " + File.copy("./_api_dir/a.txt", "./_api_dir/b.txt"))
system.out.println("file.rename: " + File.rename("./_api_dir/b.txt", "./_api_dir/c.txt"))
system.out.println("file.ctime/mtime: " + (File.ctime("./_api_dir/a.txt") > 0 && File.mtime("./_api_dir/a.txt") > 0))
system.out.println("path.scan: " + (Path.scan("./_api_dir").size >= 2))
File.remove("./_api_dir/a.txt")
File.remove("./_api_dir/c.txt")
Path.remove("./_api_dir")
