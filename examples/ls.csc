if system.args.size()!=2
    system.out.println("Usage: ls <dir>")
    system.exit(-1)
end
var info=system.path.scan(system.args.at(1))
for it iterate info
    switch it.type()
        case system.path.type.dir
            system.out.println(to_string(system.path.separator)+it.name())
        end
        default
            system.out.println(it.name())
        end
    end
end