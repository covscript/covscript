import network
using network
var ep = tcp.endpoint("127.0.0.1", 1024)
var a = tcp.acceptor(ep)
var listen = false

struct co_type
    var co = null
    # 0 = spawn, 1 = running, 2 = died
    var state = 0
end

function worker(co, a)
    co->state = 1
    var sock = new tcp.socket
    context.await_s(sock.accept, {a})
    var id = sock.receive(512)
    system.out.println("[" + id + "]: Connected")
    listen = false
    loop
        var s = context.await_s(sock.receive, {512})
        system.out.println("[" + id + "]: " + s)
        sock.send(s + "[RECEIVED]")
    until s.toupper() == "END"
    system.out.println("[" + id + "]: Disconnected")
    co->state = 2
end

var worker_list = new array
loop
    if !listen
        listen = true
        var co = gcnew co_type
        co->co = context.create_co_s(worker, {co, a})
        context.resume(co->co)
        worker_list.push_back(co)
    else
        foreach co in worker_list
            if co->state != 2
                context.resume(co->co)
            else
                co->co = null
            end
        end
    end
end
