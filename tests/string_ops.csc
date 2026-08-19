using system
# string case conversion, trim, split
var s = "Hello World"
system.out.println(s.to_lower())
system.out.println(s.to_upper())
system.out.println(s.tolower())
system.out.println(s.toupper())
system.out.println("  pad  ".trim())
var parts = "a,b,c".split({','})
system.out.println(parts.size)
system.out.println(parts[0] + parts[1] + parts[2])
system.out.println("x  y  z".split({' '}).size)
