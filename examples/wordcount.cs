function tolower(str)
    var s=str
    for i=0 to s.size()-1
        s.replace(i,1,(s[i]).tolower())
    end
    return s
end
var in=file.open("./wordcount.cs",file.read_method)
if !file.is_open(in)
    runtime.error("File is not exist")
end
var table as hash_map
while !file.eof(in)
    var str=tolower(file.read(in))
    if(str!="")
        ++table[str]
    end
end
for it iterate table
    system.println(it.first(),":",it.second())
end
