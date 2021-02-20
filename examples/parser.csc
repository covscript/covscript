import regex
namespace grammar
    var _stat=regex.build("(.*?);")
    var _blck=regex.build("(.*)\\{(.*?)\\}")
    var _blst=regex.build("(.*);(.*)\\{(.*?)\\}")
    var _var=regex.build("^\\s*var\\s+([A-Za-z_]+[A-Za-z0-9_]*)=(.+)$")
    var _if=regex.build("^\\s*if\\s*\\((.+)\\)\\s*$")
    var _else=regex.build("^\\s*else\\s*$")
end
function run(str)
    var expr=context.build(str)
    return context.solve(expr)
end
var storage={new hash_map}
function parse(_line)
    var line=_line
    while !line.empty()
        var r=grammar._blck.search(line)
        var r0=grammar._blst.search(line)
        if r.empty()||!r0.empty()
            r=grammar._stat.search(line)
            if r.empty()
                throw runtime.exception("Unknow Stat Grammar")
            end
            var s=r.str(1)
            var _r=grammar._var.match(s)
            if _r.empty()
                run(s)
            else
                (storage.front).insert(_r.str(1),run(_r.str(2)))
            end
        else
            var s=r.str(1)
            var l=r.str(2)
            var _r=grammar._if.match(s)
            if _r.empty()
                throw runtime.exception("Unknow Blck Grammar")
            end
            if run(_r.str(1))
                parse(l)
            end
        end
        line=r.suffix()
    end
end
try
@begin
parse(
    "var a=10;"+
    "if(system.in.input()>10){"+
    "   system.out.println(\"OK\");"+
    "}"
)
@end
catch e
    system.out.println(e.what)
end