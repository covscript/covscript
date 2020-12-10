var lst={0,1,1,2,3,3,3,5}.to_list()
lst.unique()
var it=lst.begin
while it!=lst.end
	if(it.data==3)
		it=lst.insert(it.next(),4)
	end
	if(it.data==5)
		it=lst.erase(it)
	end
	it.next()
end
lst.push_front("Hello")
lst.push_front(0)
lst.front=2
foreach it in lst
	system.out.println(it)
end
lst.pop_front()
lst.push_front("Hahaha")
foreach it in lst
	system.out.println(it)
end
lst.reverse()
foreach it in lst
	system.out.println(it)
end
