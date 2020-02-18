using system
constant args=context.cmd_args
if args.size!=2
    out.println("Usage: ls <dir>")
    exit(-1)
end
var info=path.scan(args.at(1))
foreach it in info
    switch it.type
        case path.type.dir
            out.println(to_string(path.separator)+it.name)
        end
        default
            out.println(it.name)
        end
    end
end