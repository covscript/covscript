function tolower(str)
    var s=str
    for i=0 to s.size()-1
        s.replace(i,1,(s[i]).tolower())
    end
    return s
end
var in=file.open("./wordcount.csc",file.read_method)
if !in.is_open()
    runtime.error("File is not exist")
end
var table=new hash_map
while !in.eof()
    var str=tolower(in.read())
    if(str!="")
        ++table[str]
    end
end
for it iterate table
    system.println(it.first(),":",it.second())
end
