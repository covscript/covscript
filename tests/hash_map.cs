var a={10:3.14,"Hello":10,2:6.28}
a["Hello"]=1
a.insert("World",20)
a.at("World")=2
system.println("Size=",a.size())
a.erase(10)
var input=system.input()
if(a.exist(input))
    system.println(a[input])
else
    system.println("Key is not exist.")
end
system.println("Size=",a.size())
