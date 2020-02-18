import network
using network
using system
using iostream
var sock=new tcp.socket
var a=tcp.acceptor(tcp.endpoint("127.0.0.1",1024))
runtime.wait_until(10000, []()->sock.accept(a), {})
var size=sock.receive(32)
size.cut(size.size-size.find(" ", 0))
var length=size.to_number()
sock.send("READY")
var buff=sock.receive(length)
sock.send("FINISH")
out.println(buff)