import network
using network
var ep=udp.endpoint("127.0.0.1",1024)
var sock=new udp.socket
sock.open()
sock.bind(ep)
loop
    var s=new string
    var rep=udp.endpoint("0.0.0.0",0)
    sock.receive_from(s,512,rep)
    system.out.println(s)
    sock.send_to(s+"[RECEIVED]",rep)
end