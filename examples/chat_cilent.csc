import network
using network
var id=0
if system.args.size()!=4
    system.out.println("Argument syntax error.Usage: <method> <port/server> <name>")
    system.exit(0)
end
var sock=new udp.socket
sock.open_v4()
var server=null
switch system.args.at(1)
    case "local"
        server=udp.endpoint("127.0.0.1",system.args.at(2).to_number())
    end
    default
        server=udp.resolve(system.args.at(1),system.args.at(2))
    end
end
system.out.println("Connect to server...")
sock.send_to("JOIN@"+system.args.at(3),server)
id=sock.receive_from(32,server)
system.out.println("Welcome to CovScript Chat Room!")
system.out.println("Do not exit directly!Type \"@EXIT\" to exit.")
loop
    if system.console.kbhit()
        system.out.print("Me:")
        var msg=system.in.getline()
        if msg.empty()
            continue
        else
            if msg=="@EXIT"
                sock.send_to("EXIT@"+id,server)
                system.exit(0)
            else
                sock.send_to("MSG@"+id+":"+msg,server)
            end
        end
    else
        sock.send_to("SYNC@"+id,server)
        var buff=sock.receive_from(1000,server)
        if buff!="NULL"
            for it iterate buff.split({';'})
                system.out.println(it)
            end
        end
    end
end