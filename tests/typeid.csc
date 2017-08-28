struct foo
end
function test(a)
	if(typeid a!=typeid foo)
		throw runtime.exception("oh")
	end
end
try
	var a=new foo
	system.out.println(typeid a)
	test(a)
	system.out.println("ok")
	test(1)
catch e
	system.out.println(e.what())
end
