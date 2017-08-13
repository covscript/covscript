var arr={}
arr[6]=6
arr[8]=8
arr[10]=10
var it=arr.begin()
while it!=arr.term()
	if(it.data()==6)
		it=arr.insert(it.forward(),7)
	end
	if(it.data()==8)
		it=arr.erase(it)
	end
	it.forward()
end
for it iterate arr
	system.print(it," ")
end
system.println()
it=arr.begin()
while it!=arr.term()
	system.print(it.data()," ")
	it.forward()
end
system.println()
it=(arr.term()).backward()
while it!=(arr.begin()).backward()
	system.print(it.data()," ")
	it.backward()
end
system.println()
