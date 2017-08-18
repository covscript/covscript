var a={10:3.14,"Hello":10,2:6.28}
a["Hello"]=1
a.insert("World",20)
a.at("World")=2
system.out.println("Size="+to_string(a.size()))
a.erase(10)
var input=system.in.input()
if(a.exist(input))
    system.out.println(a[input])
else
    system.out.println("Key is not exist.")
end
system.out.println("Size="+to_string(a.size()))
