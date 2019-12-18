import network
import regex
import codec
using network
function http_request(method, host,port,page,data)
    var sock=new tcp.socket
    sock.connect(tcp.resolve(host,port))
    @begin
    var head=
    method+" "+page+" HTTP/1.1\r\n"+
    "Host: "+host+"\r\n"+
    "Connection: Close\r\n"+
    "Content-Length: "+data.size()+"\r\n"+
    "Accept: */*\r\n"+
    "Pragma: no-cache\r\n"+
    "Cache-Control: no-cache\r\n"+
    "\r\n"+
    data
    @end
    sock.send(head)
    var response = new string
    try
        loop
            response += sock.receive(32)
        end
    catch e
    end
    return response
end
function http_response(response)
    var response_header = regex.build("^HTTP/(\\S+) (\\S+) (\\S+)\r\n")
    var header = response_header.match(response)
    system.out.println("HTTP Version: " + header.str(1))
    system.out.println("Status Code : " + header.str(2))
    system.out.println("Description : " + header.str(3))
    var response_data = regex.build("\r\n\r\n")
    var data = response_data.search(response)
    system.out.println("Content Data:\n" + data.suffix())
end
@begin
var data = 
{
    "stdin" : codec.base64.standard.encode("
        {
            \"user\":\"mikelee\",
            \"password\":\"hello,world!\"
        }
    "),
    "code"  : codec.base64.standard.encode("
        import codec;
        var json = codec.json.to_var(codec.json.from_string(system.in.getline()));
        system.out.println(\"User = \" + json.user);
        system.out.println(\"Password = \" + json.password);
    ")
}
.to_hash_map();
@end
var response = http_request("POST", "dev.covariant.cn", "http", "/cgi/cs-online", codec.json.to_string(codec.json.from_var(data)))
http_response(response)
