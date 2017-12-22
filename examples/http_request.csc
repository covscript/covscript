import network
using network
function http_request(host,port,page,data)
    var sock=new tcp.socket
    sock.connect(tcp.resolve(host,port))
   @begin
    var head=
    "GET "+page+" HTTP/1.0\r\n"+
    "Host: "+host+":"+port+"\r\n"+
    "Accept: */*\r\n"+
    "Content-Length: "+data.size()+"\r\n"+
    "Content-Type: application/x-www-form-urlencoded\r\n"+
    "Connection: close\r\n\r\n"+data
    @end
    sock.send(head)
    sock.receive(head,1024)
    system.out.println(head)
end
http_request("www.baidu.com","80","/","")