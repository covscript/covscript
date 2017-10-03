struct foo
end
var types={type(context),type(0),type(true),type(null),type('\0'),type(""),type({}.to_list()),type({}),type(0:0),type({0:0}),type(number),type(system),type(type),type(new foo),type(runtime.exception("")),type(system.in),type(system.out)}
for it iterate types
    system.out.println(it)
end
