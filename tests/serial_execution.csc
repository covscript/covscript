function test1(n)
    if n >= 2
        return test1(n - 2) + test1(n - 1)
    else
        return n
    end
end

function test2(lim, prec)
    var sum = 0
	for (i = 0, i < lim, i = i + prec)
		sum = sum + i/2*prec;
    end
	return sum;
end

function test3(lim)
    var sum = 0
	for (i = 0, i < lim, i = i + 1)
		sum = sum + i;
    end
	return sum;
end

var t1 = runtime.time()
foreach i in range(10000)
    test1(10)
end
t1 = runtime.time() - t1
system.out.println("Test1: " + t1)

var t2 = runtime.time()
foreach i in range(1000)
    test2(1, 0.0001)
end
t2 = runtime.time() - t2
system.out.println("Test2: " + t2)

var t3 = runtime.time()
foreach i in range(10000)
    test3(1000)
end
t3 = runtime.time() - t3
system.out.println("Test3: " + t3)