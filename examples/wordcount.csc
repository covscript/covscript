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
var in=iostream.fstream("../LICENSE",iostream.openmode.in)
if !in.good()
    throw runtime.exception("File is not exist")
end
var table=new hash_map
while !in.eof()
    var str_arr=((in.getline()).tolower()).split({' ','\t','\n','\v','\f','\r'})
    foreach str in str_arr
        if(!str.empty())
            ++table[str]
        end
    end
end
var arr = {}
foreach it in table do arr.push_back(it.first)
quicksort(arr, 0, arr.size-1)
foreach it in arr
    system.out.println(it+":"+table[it])
end
