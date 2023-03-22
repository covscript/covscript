import network
using network
var ep = tcp.endpoint("127.0.0.1", 1024)
var sock = new tcp.socket
var exit = false

function worker(sock)
    loop
        var s = context.await(system.in.getline)
        if !s.empty()
            sock.send(s)
            system.out.println(sock.receive(512))
        end
    until s.toupper() == "END"
    exit = true
end

var id = null
if context.cmd_args.size == 1
    system.out.print("Please enter an ID: ")
    id = system.in.getline()
else
    id = context.cmd_args[1]
end
sock.connect(ep)
sock.send(id)
var co = context.create_co_s(worker, {sock})
var count = 0
loop
    context.resume(co)
    if count == 50
        sock.send("ECHO from " + id)
        system.out.println(sock.receive(512))
        count = 0
    else
        runtime.delay(100)
        ++count
    end
until exit
