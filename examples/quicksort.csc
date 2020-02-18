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
			if i>=a.size
				break
			end
		until a[i]>=v
		loop
			--j
			if j<0
				break
			end
		until a[j]<=v
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
for i=0,i<=100,++i
	a.push_back(math.randint(0,100))
end
var ts=runtime.time()
quicksort(a,0,a.size-1)
system.out.println("Time spend:"+to_string(runtime.time()-ts))
foreach it in a
	system.out.println(it)
end
