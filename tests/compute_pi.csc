function compute_pi(accuracy)
    var pi = 0, i = 1, neg = 1, op = 0
    loop
        op = 1/i
        pi += neg*op
        neg = -neg
        i += 2
    until op < accuracy
    return pi*4
end

iostream.setprecision(10)
system.out.println(compute_pi(0.0000001))
