import network
using network
function post(host,port,page,data)
    var sock=new tcp.socket
    sock.resolve(host,port)
    var head=new string
    head+="POST "+page+" HTTP/1.0\r\n"
    head+="Host: "+host+":"+port+"\r\n"
    head+="Accept: */*\r\n"
    head+="Content-Length: "+data.size()+"\r\n"
    head+="Content-Type: application/x-www-form-urlencoded\r\n"
    head+="Connection: close\r\n\r\n"
    head+=data
    sock.send(head)
    sock.receive(head,1024)
    system.out.println(head)
end
post("www.baidu.com","80","/index.html","")