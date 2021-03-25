var in = iostream.fstream("../LICENSE",iostream.openmode.in)
if !in.good()
    throw runtime.exception("File is not exist")
end
var table = new hash_map
while !in.eof()
    var str_arr = ((in.getline()).tolower()).split({' ', '\t', '\n', '\v', '\f', '\r'})
    foreach str in str_arr
        if !str.empty()
            ++table[str]
        end
    end
end
var lst = new list
foreach it in table do lst.push_back(it.first)
lst.sort([](lhs, rhs) -> lhs < rhs)
foreach it in lst
    system.out.println(it + ":" + table[it])
end
