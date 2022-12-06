loop
    var ts = runtime.time()
    var i = 0
    var lam = []()->++i
    foreach count in range(100000)
        lam()
    end
    system.out.println(runtime.time() - ts)
end