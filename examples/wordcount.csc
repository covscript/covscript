import "str_tools.csp"
var in=iostream.fstream("../LICENSE",iostream.openmode.in)
if !in.good()
    runtime.error("File is not exist")
end
var table=new hash_map
while !in.eof()
    var str_arr=str_tools.split(str_tools.tolower(in.getline()))
    for str iterate str_arr
        if(!str.empty())
            ++table[str]
        end
    end
end
for it iterate table
    system.out.println(it.first()+":"+it.second())
end
