struct foo
end
function test(a)
	if(typeid a!=typeid foo)
		throw runtime.exception("oh")
	end
end
var a=new foo
system.out.println(typeid a)
test(a)
system.out.println("ok")
try
	block
		var b=10
		test(1)
	end
catch e
	system.out.println(e.what())
end
