function tolower(str)
    define s=str
    for i=0 to s.size()-1
        s.replace(i,1,(s[i]).tolower())
    end
    return s
end
define in=file.open("./wordcount.cs",file.read_method)
if !file.is_open(in)
    runtime.error("File is not exist")
end
define table as hash_map
while !file.eof(in)
    define str=tolower(file.read(in))
    if(str!="")
        ++table[str]
    end
end
foreach it iterate table
    system.println(it.first(),":",it.second())
end
