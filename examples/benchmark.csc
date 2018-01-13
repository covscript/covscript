iostream.setprecision(64)
function step1(n)
    var score=0
    for i=1 to n
        var ts=runtime.time()
        var fc=0
        loop
            fc++
        until runtime.time()-ts>=1000
        end
        score=score+fc
    end
    return to_integer(score/n)
end
function step2(n,k)
    function recursion(n)
        if n>0
            return recursion(n-1)
        else
            return
        end
    end
    var score=0
    for i=1 to k
        var ts=runtime.time()
        recursion(n)
        score=score+runtime.time()-ts
    end
    return to_integer(score/k)
end
function step3(n,k)
    var precision=10^(-n)
    function integral(func,begin,end_)
        var sum=0
        var x=begin
        while x<=end_
            sum=sum+func(x)*precision
            x=x+precision
        end
        return sum
    end
    function f(x)
        return math.sqrt(1-x^2)
    end
    var score=0
    for i=1 to k
        var ts=runtime.time()
        integral(f,0,1)
        score=score+runtime.time()-ts
    end
    return to_integer(score/k)
end
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
function step4(n,k)
    var score=0
    for i=1 to k
        var a={}
        for i=0 to n
            a.push_back(runtime.randint(0,n))
        end
        var ts=runtime.time()
        quicksort(a,0,a.size()-1)
        score=score+runtime.time()-ts
    end
    return to_integer(score/k)
end
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
function step5(n,k)
    var score=0
    for i=1 to k
        var ts=runtime.time()
        fibonacci(n)
        score=score+runtime.time()-ts
    end
    return to_integer(score/k)
end
system.out.println("Covariant Script Performance Benchmark")
system.out.println("Step 1:Speed of simple statement execution")
system.out.println("Result:"+to_string(step1(5)))
system.out.println("Step 2:Speed of function recursion")
system.out.println("Result:"+to_string(step2(1000,5)))
system.out.println("Step 3:Speed of integral")
system.out.println("Result:"+to_string(step3(5,5)))
system.out.println("Step 4:Speed of quick sort(10000 Data)")
system.out.println("Result:"+to_string(step4(10000,5)))
system.out.println("Step 5:Speed of fibonacci(10000 Data)")
system.out.println("Result:"+to_string(step5(10000,5)))