import network
using network
var ep=tcp.endpoint("127.0.0.1",1024)
var a=tcp.acceptor(ep)
var sock=new tcp.socket
sock.accept(a)
loop
    var s=new string
    sock.receive(s,512)
    system.out.println(s)
    sock.send(s+"[RECEIVED]")
end