import network
using network
var ep = tcp.endpoint("127.0.0.1", 1024)
var sock = new tcp.socket
var exit = false

function worker(sock)
    loop
        var s = runtime.await(system.in.getline)
        if !s.empty()
            sock.send(s)
            if s == "end"
                break
            end
            system.out.println(sock.receive(512))
        end
    end
    exit = true
end

var id = null, pure = false
if context.cmd_args.size < 2
    system.out.print("Usage: test_tcp_client <ID> [--pure]")
    system.exit(0)
else
    id = context.cmd_args[1]
    if context.cmd_args.size > 2 && context.cmd_args[2] == "--pure"
        pure = true
    end
end

# Establish connection
sock.connect(ep)
sock.send(id)
if sock.receive(64) != id
    system.out.println("Protocol error!!!")
    system.exit(0)
end

# Create coroutine
var co = null
if !pure
    co = runtime.create_co_s(worker, {sock})
end

var count = 0, million = 1
loop
    if !pure
        runtime.resume(co)
    end
    if exit
        break
    end
    var num = math.randint(0, 100000)
    sock.send(to_string(num))
    if sock.receive(64) != to_string(num + 1)
        system.out.println("Transmission error!!!")
        system.exit(0)
    end
    if ++count == 100000
        system.out.println("[" + id + "]: " + to_string(million) + " Million" + (million > 1 ? "s" : "") + " Request")
        ++million
        count = 0
    end
end
