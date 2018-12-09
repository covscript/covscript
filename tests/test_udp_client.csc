import network
using network
var ep=udp.endpoint("127.0.0.1",1024)
var rep=udp.endpoint_v4(0)
var sock=new udp.socket
sock.open_v4()
loop
    var s=system.in.getline()
    if !s.empty()
        sock.send_to(s,ep)
        system.out.println(sock.receive_from(512,rep))
    end
end