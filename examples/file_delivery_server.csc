import network
using network
using system
using iostream
var sock=new tcp.socket
var a=tcp.acceptor(tcp.endpoint("127.0.0.1",1024))
sock.accept(a)
var length=sock.receive(32).to_number()
sock.send("READY")
var buff=sock.receive(length)
sock.send("FINISH")
out.println(buff)