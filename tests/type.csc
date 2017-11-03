import regex
import darwin
import sqlite
struct foo
end
var reg=regex.build("REGEX")
var reg_result=reg.search("COVSCRIPTREGEX")
var sql=sqlite.open(":memory:")
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
    type(context.build("x")),
    type(reg),
    type(reg_result),
    type(darwin.red),
    type(darwin.pixel(' ',darwin.white,darwin.white)),
    type(darwin.get_drawable()),
    type(sql),
    type(sqlite.integer),
    type(sql.prepare("create table test(a integer)"))
}
@end
for it iterate types
    system.out.println(it)
end
