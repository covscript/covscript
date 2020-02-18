var arr={}
arr[6]=6
arr[8]=8
arr[10]=10
var it=arr.begin
while it!=arr.end
	if(it.data==6)
		it=arr.insert(it.next,7)
	end
	if(it.data==8)
		it=arr.erase(it)
	end
	it.next
end
foreach it in arr
	system.out.print(to_string(it)+" ")
end
system.out.println("")
it=arr.begin
while it!=arr.end
	system.out.print(to_string(it.data)+" ")
	it.next
end
system.out.println("")
it=(arr.end).prev
while it!=(arr.begin).prev
	system.out.print(to_string(it.data)+" ")
	it.prev
end
system.out.println("")
