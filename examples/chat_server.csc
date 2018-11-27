import network
using network
struct cilent_data
    var name=null
    var ep=null
    var msg_buff=null
    function add_msg(msg)
        if msg_buff==null
            msg_buff=msg
        else
            msg_buff+=";"+msg
        end
    end
end
var cilent_profile=new hash_map
var id_count=10000
function broadcast(msg)
    system.out.println(msg)
    foreach it:cilent_profile
        it.second()->add_msg(msg)
    end
end
function join(name,ep)
    ++id_count
    var sd=gcnew cilent_data
    sd->name=name
    sd->ep=ep
    cilent_profile.insert(id_count,sd)
    broadcast("["+id_count+"@"+name+"] Joined CovScript Chat Room.")
    return id_count
end
function recv_msg(id,msg)
    var sd=cilent_profile.at(id)
    var m="["+id+"@"+sd->name+"]:"+msg
    foreach it:cilent_profile
        it.second()->add_msg(m)
    end
end
function sync(sock,id)
    var sd=cilent_profile.at(id)
    if sd->msg_buff==null
        sock.send_to("NULL",sd->ep)
    else
        sock.send_to(sd->msg_buff,sd->ep)
        sd->msg_buff=null
    end
end
function exit(id)
    broadcast("["+id+"@"+cilent_profile.at(id)->name+"] Left CovScript Chat Room.")
    cilent_profile.erase(id)
end
if context.cmd_args().size()!=3
    system.out.println("Argument syntax error.Usage: <method> <port>")
    system.exit(0)
end
var sock=new udp.socket
sock.open_v4()
switch context.cmd_args().at(1)
    case "local"
        sock.bind(udp.endpoint("127.0.0.1",context.cmd_args().at(2).to_number()))
    end
    case "online"
        sock.bind(udp.endpoint_v4(context.cmd_args().at(2).to_number()))
    end
    default
        system.out.println("Argument syntax error.Usage: <method> <port>")
    end
end
loop
    var ep=udp.endpoint_v4(0)
    var dat=sock.receive_from(500,ep).split({'@'})
    switch dat.at(0)
        case "JOIN"
            sock.send_to(to_string(join(dat.at(1),ep)),ep)
        end
        case "MSG"
            var msg=dat.at(1).split({':'})
            recv_msg(msg.at(0).to_number(),msg.at(1))
        end
        case "SYNC"
            sync(sock,dat.at(1).to_number())
        end
        case "EXIT"
            exit(dat.at(1).to_number())
        end
        default
            system.out.println("Unsupported Message.")
            system.exit(0)
        end
    end
end