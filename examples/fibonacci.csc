iostream.setprecision(64)
function fibonacci(count)
    var fib={0,1}
    for n=2,n<=count,++n
        fib.push_back(fib.at(n-1)+fib.at(n-2))
    end
    var sum=new hash_map
    for i=2,i<fib.size,++i
        ++sum[to_string(fib.at(i)/fib.at(i-1))]
    end
    var max=0:0
    foreach it in sum
        if it.second>max.second
            max=it
        end
    end
    return max.first
end
system.out.println(fibonacci(10000))