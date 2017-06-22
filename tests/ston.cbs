function cton(ch)
	switch ch
		case '0'
			return 0
		end
		case '1'
			return 1
		end
		case '2'
			return 2
		end
		case '3'
			return 3
		end
		case '4'
			return 4
		end
		case '5'
			return 5
		end
		case '6'
			return 6
		end
		case '7'
			return 7
		end
		case '8'
			return 8
		end
		case '9'
			return 9
		end
	end
end
function reverse_str(str)
	define s as string
	define i=str.size()-1
	while i>=0
		s.append(str[i])
		--i
	end
	return s
end
function ston(str)
	if str.size()==0
		runtime.error("Receive empty string")
	end
	if str[0]=='-'
		define tmp as string
		for i=1 to size_of(str)-1
			tmp.append(str[i])
		end
		return -ston(tmp)
	end
	define ipart as string
	define fpart as string
	define dot_count=0
	foreach ch iterate str
		if ch!='.'
			if !ch.isdigit()
				runtime.error("Illegal string")
			end
			if dot_count==0
				ipart.append(ch)
			else
				fpart.append(ch)
			end
		else
			if ++dot_count>1
				runtime.error("Illegal string")
			end
		end
	end
	define result=0
	ipart=reverse_str(ipart)
	for i=0 to ipart.size()-1
		result=result+cton(ipart[i])*10^i
	end
	for i=0 to fpart.size()-1
		result=result+cton(fpart[i])*10^(-i-1)
	end
	return result
end
define result=ston(system.getline())
system.println(result)
system.println(result*2)
