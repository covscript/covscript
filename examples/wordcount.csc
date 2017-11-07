var in=iostream.fstream("../LICENSE",iostream.openmode.in)
if !in.good()
    throw runtime.exception("File is not exist")
end
var table=new hash_map
while !in.eof()
    var str_arr=((in.getline()).tolower()).split({' ','\t','\n','\v','\f','\r'})
    for str iterate str_arr
        if(!str.empty())
            ++table[str]
        end
    end
end
for it iterate table
    system.out.println(it.first()+":"+it.second())
end
