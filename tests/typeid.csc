struct foo
end
function test(a)
	if(typeid a!=typeid foo)
		runtime.error("oh")
	end
end
var a=new foo
system.out.println(typeid a)
test(a)
system.out.println("ok")
test(1)
