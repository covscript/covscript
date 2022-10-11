@charset:gbk
iostream.setprecision(64)
function ��һ��(n)
    var �÷�=0
    foreach i in range(1,n+1)
        var ��ʱ=runtime.time()
        var fc=0
        loop
            ++fc
        until runtime.time()-��ʱ>=1000
        �÷�=�÷�+fc
    end
    return to_integer(�÷�/n)
end
function �ڶ���(n,k)
    var precision=10^(-n)
    function integral(func,begin,end)
        var sum=0
        for x=begin,x<=end,x=x+precision do sum+=func(x)*precision
        return sum
    end
    function f(x)
        return math.sqrt(1-x^2)
    end
    var �÷�=0
    foreach i in range(1,k+1)
        var ��ʱ=runtime.time()
        integral(f,0,1)
        �÷�=�÷�+runtime.time()-��ʱ
    end
    return to_integer(�÷�/k)
end
function ��������(a,m,n)
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
    ��������(a,m,j)
    ��������(a,i+1,n)
end
function ������(n,k)
    var �÷�=0
    foreach i in range(1,k+1)
        var a={}
        for i=0,i<=n,++i do a.push_back(math.randint(0,n))
        var ��ʱ=runtime.time()
        ��������(a,0,a.size-1)
        �÷�=�÷�+runtime.time()-��ʱ
    end
    return to_integer(�÷�/k)
end
function 쳲�����(count)
    var fib={0,1}
    for n=2,n<=count,++n do fib.push_back(fib.at(n-1)+fib.at(n-2))
    var sum=new hash_map
    for i=2,i<fib.size,++i do ++sum[to_string(fib.at(i)/fib.at(i-1))]
    var max=0:0
    foreach it in sum do it.second>max.second?(max=it):null
    return max.first
end
function ���Ĳ�(n,k)
    var �÷�=0
    foreach i in range(1,k+1)
        var ��ʱ=runtime.time()
        쳲�����(n)
        �÷�=�÷�+runtime.time()-��ʱ
    end
    return to_integer(�÷�/k)
end
function ���岽(n,k)
    var �÷�=0, fib=[](n)->n>2?fib(n-1)+fib(n-2):1
    foreach i in range(1,k+1)
        var ��ʱ=runtime.time()
        fib(n)
        �÷�+=runtime.time()-��ʱ
    end
    return to_integer(�÷�/k)
end
system.out.println("Covariant Script ���ܲ����׼�")
system.out.println("��һ����˳��ִ������")
system.out.println("�����"+to_string(��һ��(5)))
system.out.println("�ڶ��������ּ�������")
system.out.println("�����"+to_string(�ڶ���(5,5)))
system.out.println("�����������������㷨(10000 Data)")
system.out.println("�����"+to_string(������(10000,5)))
system.out.println("���Ĳ���˳��쳲�����(10000 Data)")
system.out.println("�����"+to_string(���Ĳ�(10000,5)))
system.out.println("���岽���ݹ�쳲�����(Depth 30)")
system.out.println("�����"+to_string(���岽(30,5)))