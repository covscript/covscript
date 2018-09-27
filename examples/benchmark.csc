iostream.setprecision(64)
function step1(n)
    var score=0
    for i=1,i<=n,++i
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
    for i=1,i<=k,++i
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
        for x=begin,x<=end_,x=x+precision
            sum=sum+func(x)*precision
        end
        return sum
    end
    function f(x)
        return math.sqrt(1-x^2)
    end
    var score=0
    for i=1,i<=k,++i
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
    for i=1,i<=k,++i
        var a={}
        for i=0,i<=n,++i
            a.push_back(math.randint(0,n))
        end
        var ts=runtime.time()
        quicksort(a,0,a.size()-1)
        score=score+runtime.time()-ts
    end
    return to_integer(score/k)
end
function fibonacci(count)
    var fib={0,1}
    for n=2,n<=count,++n
        fib.push_back(fib.at(n-1)+fib.at(n-2))
    end
    var sum=new hash_map
    for i=2,i<fib.size(),++i
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
    for i=1,i<=k,++i
        var ts=runtime.time()
        fibonacci(n)
        score=score+runtime.time()-ts
    end
    return to_integer(score/k)
end
function step6(n,k)
    var score=0
    var fib=[](n)->n>2?fib(n-1)+fib(n-2):1
    for i=1,i<=k,++i
        var ts=runtime.time()
        fib(n)
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
system.out.println("Step 6:Speed of fibonacci(Depth 30)")
system.out.println("Result:"+to_string(step6(30,5)))