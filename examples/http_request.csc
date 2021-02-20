import network, regex
import codec.base64.standard as base64
import codec.json as json

using network

function http_request(method, host, page, data)
    var sock = new tcp.socket
    runtime.wait_for(2000, []()->sock.connect(tcp.resolve(host, "http")), {})
    @begin
    var head=
        method + " " + page + " HTTP/1.1\r\n" +
        "Host: " + host + "\r\n" +
        "Connection: Close\r\n" +
        "Content-Length: " + data.size + "\r\n" +
        "Accept: */*\r\n" +
        "Pragma: no-cache\r\n" +
        "Cache-Control: no-cache\r\n" +
        "\r\n" + data
    @end
    sock.send(head)
    var response = new string
    try
        loop
            response += sock.receive(32)
        end
    catch e
        return response
    end
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
    # STDIN, base64 encode
    "stdin" : base64.encode(
        "{" +
        "    \"username\":\"mikecovlee\","+
        "    \"password\":\"helloworld\"" +
        "}"
    ),
    # CovScript Code, base64 encode
    "code"  : base64.encode(
        "import codec;" +
        "var json = codec.json.to_var(codec.json.from_string(system.in.getline()));" +
        "system.out.println(\"UserName = \" + json.username);" +
        "system.out.println(\"Password = \" + json.password);"
    )
}
.to_hash_map();

http_response(
    http_request(
        # GET/POST
        "POST",
        # Host Name
        "dev.covariant.cn",
        # Host Url
        "/cgi/cs-online",
        # Data in string
        json.to_string(json.from_var(data))
    )
);

@end
