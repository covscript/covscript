struct foo
end
@begin
var types=
{
    type(context),
    type(0),
    type(true),
    type(null),
    type('\0'),
    type(""),
    type(new list),
    type({}),
    type(0:0),
    type({0:0}),
    type(number),
    type(system),
    type(type),
    type(new foo),
    type(runtime.exception("")),
    type(system.in),
    type(system.out),
    type({}.begin()),
    type((new list).begin()),
    type(iostream.seekdir.start),
    type(iostream.openmode.app),
    type(context.build("x"))
}
@end
for it iterate types
    system.out.println(it)
end
