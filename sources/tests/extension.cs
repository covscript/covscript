constant my=runtime.load_extension("./my.ext")
constant a=10
constant b=my.test(a)
system.println(a,b)
