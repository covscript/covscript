var lst={0,1,1,2,3,3,3,5}.to_list()
lst.unique()
var it=lst.begin()
while it!=lst.term()
	if(it.data()==3)
		it=lst.insert(it.forward(),4)
	end
	if(it.data()==5)
		it=lst.erase(it)
	end
	it.forward()
end
lst.push_front("Hello")
lst.push_front(0)
lst.front()=2
for it iterate lst
	system.println(it)
end
lst.pop_front()
lst.push_front("Hahaha")
for it iterate lst
	system.println(it)
end
lst.reverse()
for it iterate lst
	system.println(it)
end
