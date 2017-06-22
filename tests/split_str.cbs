function split(str)
	define arr as array
	define tmp as string
	foreach ch iterate str
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
define r=split(system.getline())
foreach it iterate r
	system.print("#",it,"#")
end
system.println()
