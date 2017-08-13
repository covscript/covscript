function split(str)
	var arr=new array
	var tmp=new string
	for ch iterate str
		if(ch.isspace())
			if(tmp.size()!=0)
				arr.push_back(tmp)
				tmp.clear()
			end
			continue
		end
		tmp.append(ch)
	end
	if(tmp.size()!=0)
		arr.push_back(tmp)
		tmp.clear()
	end
	return arr
end
var r=split(system.getline())
for it iterate r
	system.print("#",it,"#")
end
system.println()
