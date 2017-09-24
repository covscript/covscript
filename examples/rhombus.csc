system.out.print("width=")
var w=system.in.input()
system.out.print("height=")
var h=system.in.input()
var t=h/w
var half=to_integer(0.5*h)
var y=0
while y<half
	var x=to_integer(2*(y/t))
	var space=to_integer(0.5*(w-x))
	var c=0
	while c<=space
		system.out.print(" ")
		c=c+1
	end
	c=0
	while c<=x
		system.out.print("*")
		c=c+1
	end
	system.out.println("")
	y=y+1
end
y=half
while y>=0
	var x=to_integer(2*(y/t))
	var space=to_integer(0.5*(w-x))
	var c=0
	while c<=space
		system.out.print(" ")
		c=c+1
	end
	c=0
	while c<=x
		system.out.print("*")
		c=c+1
	end
	system.out.println("")
	y=y-1
end
