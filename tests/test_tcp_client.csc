import network
using network
var ep=tcp.endpoint("127.0.0.1",1024)
var sock=new tcp.socket
sock.connect(ep)
loop
    var s=system.in.getline()
    if !s.empty()
        sock.send(s)
        sock.receive(s,512)
        system.out.println(s)
    end
end