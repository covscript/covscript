import network, regex
using network
var personal_msg = regex.build("^@([0-9]+):(.+)$")
function escape_string(msg)
    var str = new string
    foreach ch in msg
        switch ch
            default
                str += ch
            end
            case '@'
                str += char.from_ascii(1)
            end
            case ':'
                str += char.from_ascii(2)
            end
            case ';'
                str += char.from_ascii(3)
            end
        end
    end
    return str
end
function unescape_string(msg)
    var str = new string
    foreach ch in msg
        switch ch
            default
                str += ch
            end
            case char.from_ascii(1)
                str += '@'
            end
            case char.from_ascii(2)
                str += ':'
            end
            case char.from_ascii(3)
                str += ';'
            end
        end
    end
    return str
end
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
    foreach it in cilent_profile
        it.second->add_msg(msg)
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
    var umsg=unescape_string(msg)
    var r=personal_msg.match(umsg)
    var m="["+id+"@"+sd->name+"]:"+msg
    if !r.empty()
        var tid=r.str(1).to_number()
        if cilent_profile.exist(tid)
            cilent_profile.at(id)->add_msg(m)
            cilent_profile.at(tid)->add_msg(m)
        else
            cilent_profile.at(id)->add_msg("[System Information]:Target User \"" + tid + "\" does not exist!")
        end
    else
        foreach it in cilent_profile
            it.second->add_msg(m)
        end
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
# Main
var sock=new udp.socket
var port=88088
if context.cmd_args.size==2
    port=context.cmd_args.at(2).to_number()
end
sock.open_v4()
sock.bind(udp.endpoint_v4(port))
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