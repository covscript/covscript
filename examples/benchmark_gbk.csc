@charset:gbk
iostream.setprecision(64)
function 第一步(n)
    var 得分=0
    foreach i in range(1,n+1)
        var 用时=runtime.time()
        var fc=0
        loop
            ++fc
        until runtime.time()-用时>=1000
        得分=得分+fc
    end
    return to_integer(得分/n)
end
function 第二步(n,k)
    var precision=10^(-n)
    function integral(func,begin,end)
        var sum=0
        for x=begin,x<=end,x=x+precision do sum+=func(x)*precision
        return sum
    end
    function f(x)
        return math.sqrt(1-x^2)
    end
    var 得分=0
    foreach i in range(1,k+1)
        var 用时=runtime.time()
        integral(f,0,1)
        得分=得分+runtime.time()-用时
    end
    return to_integer(得分/k)
end
function 快速排序(a,m,n)
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
    快速排序(a,m,j)
    快速排序(a,i+1,n)
end
function 第三步(n,k)
    var 得分=0
    foreach i in range(1,k+1)
        var a={}
        for i=0,i<=n,++i do a.push_back(math.randint(0,n))
        var 用时=runtime.time()
        快速排序(a,0,a.size-1)
        得分=得分+runtime.time()-用时
    end
    return to_integer(得分/k)
end
function 斐波那契(count)
    var fib={0,1}
    for n=2,n<=count,++n do fib.push_back(fib.at(n-1)+fib.at(n-2))
    var sum=new hash_map
    for i=2,i<fib.size,++i do ++sum[to_string(fib.at(i)/fib.at(i-1))]
    var max=0:0
    foreach it in sum do it.second>max.second?(max=it):null
    return max.first
end
function 第四步(n,k)
    var 得分=0
    foreach i in range(1,k+1)
        var 用时=runtime.time()
        斐波那契(n)
        得分=得分+runtime.time()-用时
    end
    return to_integer(得分/k)
end
function 第五步(n,k)
    var 得分=0, fib=[](n)->n>2?fib(n-1)+fib(n-2):1
    foreach i in range(1,k+1)
        var 用时=runtime.time()
        fib(n)
        得分+=runtime.time()-用时
    end
    return to_integer(得分/k)
end
system.out.println("Covariant Script 性能测试套件")
system.out.println("第一步：顺序执行性能")
system.out.println("结果："+to_string(第一步(5)))
system.out.println("第二步：积分计算性能")
system.out.println("结果："+to_string(第二步(5,5)))
system.out.println("第三步：快速排序算法(10000 Data)")
system.out.println("结果："+to_string(第三步(10000,5)))
system.out.println("第四步：顺序斐波那契(10000 Data)")
system.out.println("结果："+to_string(第四步(10000,5)))
system.out.println("第五步：递归斐波那契(Depth 30)")
system.out.println("结果："+to_string(第五步(30,5)))