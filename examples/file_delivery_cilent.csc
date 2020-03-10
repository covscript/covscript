import network
using network
using system
using iostream
function read_file(path)
    var fs=fstream(path,openmode.in)
    var buff=new string
    while !fs.eof()
        buff+=fs.getline()+"\r\n"
    end
    return buff
end
function send_request(sock,length)
    var head=to_string(length)
    while head.size<32
        head+=" "
    end
    sock.send(head)
    if sock.receive(32)=="READY"
        return true
    else
        return false
    end
end
var sock=new tcp.socket
sock.connect(tcp.endpoint("127.0.0.1",1024))
var buff=read_file(in.getline())
if send_request(sock,buff.size)
    sock.send(buff)
    if sock.receive(32)!="FINISH"
        out.println("Error")
    end
end