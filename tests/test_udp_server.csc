import network
using network
var ep=udp.endpoint("127.0.0.1",1024)
var sock=new udp.socket
sock.open_v4()
sock.bind(ep)
loop
    var rep=udp.endpoint_v4(0)
    var s=sock.receive_from(512,rep)
    system.out.println(s)
    sock.send_to(s+"[RECEIVED]",rep)
end