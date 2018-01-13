iostream.setprecision(64)
function fibonacci(count)
    var fib={0,1}
    for n=2 to count
        fib.push_back(fib.at(n-1)+fib.at(n-2))
    end
    var sum=new hash_map
    for i=2 to fib.size()-1
        ++sum[to_string(fib.at(i)/fib.at(i-1))]
    end
    var max=0:0
    for it iterate sum
        if it.second()>max.second()
            max=it
        end
    end
    return max.first()
end
system.out.println(fibonacci(10000))