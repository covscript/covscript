function show_numeric(num)
    system.out.println("Value = " + num + ", Type = " + (num.is_integer() ? "Integer" : "Float"))
end
var a = 0
show_numeric(a)
a += 1
show_numeric(a)
a += 0.1
show_numeric(a)
a += 1
show_numeric(a)
a.ntoi()
show_numeric(a)
a.ntof()
show_numeric(a)
system.out.println("Max  = " + math.constants.max)
system.out.println("Min  = " + math.constants.min)
system.out.println("IMax = " + math.constants.integer_max)
system.out.println("IMin = " + math.constants.integer_min)
system.out.println("FMax = " + math.constants.float_max)
system.out.println("FMin = " + math.constants.float_min)
