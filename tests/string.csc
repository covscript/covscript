var str="abcdHelloefgh"
system.out.println(str.append(3.14))
system.out.println(str.cut(4))
system.out.println(str.substr(str.find("Hello",0),5))
system.out.println(str.substr(str.rfind("Hello",-1),5))
system.out.println(str.replace(str.find("Hello",0),5,"FUCK"))
system.out.println(str.erase(str.find("FUCK",0),4))
system.out.println(str.insert(str.find("d",0)+1,"Hello"))
