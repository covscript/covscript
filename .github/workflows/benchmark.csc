@require:210507
iostream.setprecision(64)
function step1(n)
    var score=0
    foreach i in range(1,n+1)
        var ts=runtime.time()
        var fc=0
        loop
            ++fc
        until runtime.time()-ts>=1000
        score=score+fc
    end
    return to_integer(score/n)
end
function step2(n,k)
    var precision=10^(-n)
    function integral(func,begin,end)
        var sum=0
        for x=begin,x<=end,x=x+precision do sum+=func(x)*precision
        return sum
    end
    function f(x)
        return math.sqrt(1-x^2)
    end
    var score=0
    foreach i in range(1,k+1)
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
    var (i,j,v)={m-1,n,a[n]}
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
function step3(n,k)
    var score=0
    foreach i in range(1,k+1)
        var a={}
        for i=0,i<=n,++i do a.push_back(math.randint(0,n))
        var ts=runtime.time()
        quicksort(a,0,a.size-1)
        score=score+runtime.time()-ts
    end
    return to_integer(score/k)
end
function fibonacci(count)
    var fib={0,1}
    for n=2,n<=count,++n do fib.push_back(fib.at(n-1)+fib.at(n-2))
    var sum=new hash_map
    for i=2,i<fib.size,++i do ++sum[to_string(fib.at(i)/fib.at(i-1))]
    var max=0:0
    foreach it in sum do it.second>max.second?(max=it):null
    return max.first
end
function step4(n,k)
    var score=0
    foreach i in range(1,k+1)
        var ts=runtime.time()
        fibonacci(n)
        score=score+runtime.time()-ts
    end
    return to_integer(score/k)
end
function step5(n,k)
    var score=0,fib=[](n)->n>2?fib(n-1)+fib(n-2):1
    foreach i in range(1,k+1)
        var ts=runtime.time()
        fib(n)
        score+=runtime.time()-ts
    end
    return to_integer(score/k)
end
var co_swap = null
function send(val)
    co_swap = val
    runtime.yield()
end
function receive(co)
    runtime.resume(co)
    return co_swap
end
function worker()
    var i = 0
    loop
        send(++i)
    until co_swap == null
end
function step6(n)
    var score = 0
    foreach it in range(n)
        var co_list = new array
        foreach i in range(10) do co_list.push_back(runtime.create_co(worker))
        var i = 0, ts = runtime.time()
        while runtime.time() - ts < 1000
            foreach co in co_list do i += receive(co)
        end
        score += i
        co_swap = null
        foreach co in co_list do runtime.resume(co)
        co_list = null
    end
    return score/n
end
system.out.println("Covariant Script Performance Benchmark")
system.out.println("Test 1: Simple Statement Execution (Count/Second)")
system.out.println("Result: "+to_string(step1(5)))
system.out.println("Test 2: Coroutine Context Switch (Count/Second)")
system.out.println("Result: "+to_string(step6(5)))
system.out.println("Test 3: Integral (Millsecond)")
system.out.println("Result: "+to_string(step2(5,5)))
system.out.println("Test 4: Quick Sort (Millsecond, Data = 10k)")
system.out.println("Result: "+to_string(step3(10000,5)))
system.out.println("Test 5: Serial Fibonacci (Millsecond, Data = 10k)")
system.out.println("Result: "+to_string(step4(10000,5)))
system.out.println("Test 6: Recursive Fibonacci (Millsecond, Depth = 30)")
system.out.println("Result: "+to_string(step5(30,5)))
