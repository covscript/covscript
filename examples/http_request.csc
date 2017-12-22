import network
using network
function http_request(host,port,page)
    var sock=new tcp.socket
    sock.connect(tcp.resolve(host,port))
   @begin
    var head=
    "GET "+page+" HTTP/1.1\r\n"+
    "Host: "+host+"\r\n"+
    "Accept: */*\r\n"+
    "Pragma: no-cache\r\n"+
    "Cache-Control: no-cache\r\n"+
    "Connection: close\r\n"+
    "\r\n"
    @end
    sock.send(head)
    try
        loop
            system.out.print(sock.receive(32))
        end
    catch e
    end
end
http_request("covscript.org","http","/")