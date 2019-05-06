import str_tools
system.out.println("Str to number")
var result=str_tools.ston(system.in.getline())
system.out.println(result)
system.out.println(result*2)
system.out.println("Split str")
var r=str_tools.split(system.in.getline())
foreach it in r
	system.out.print("#"+it+"#")
end
system.out.println("")
