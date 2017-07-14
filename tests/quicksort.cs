function quicksort(a,m,n)
	if n<=m
		return 0
	end
	var i=m-1
	var j=n
	var v=a[n]
	loop
		loop
			++i
			if i>=a.size()
				break
			end
		until a[i]>=v
		end
		loop
			--j
			if j<0
				break
			end
		until a[j]<=v
		end
		if i>=j
			break
		end
		swap(a[i],a[j])
	end
	swap(a[i],a[n])
	quicksort(a,m,j)
	quicksort(a,i+1,n)
end
var a={}
for i=0 to 100
	a.push_back(runtime.randint(0,100))
end
var ts=runtime.time()
quicksort(a,0,a.size()-1)
system.println("Time spend:",runtime.time()-ts)
for it iterate a
	system.println(it)
end
