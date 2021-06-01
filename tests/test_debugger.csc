function test3()
    system.out.println("3S1")
    system.out.println("3S2")
end
function test2()
    system.out.println("2S1")
    foreach i in range(10)
        system.out.println("2S2")
        test3()
        system.out.println("2S3")
    end
    system.out.println("2S4")
end
function test1()
    system.out.println("1S1")
    foreach i in range(10)
        system.out.println("1S2")
        test2()
        system.out.println("1S3")
    end
    system.out.println("SS4")
end
system.out.println("S1")
test1()
system.out.println("S2")