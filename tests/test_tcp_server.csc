import network
using network
var ep = tcp.endpoint_v4(1024)
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
    runtime.await(sock.accept, a)
    while sock.available() == 0
        fiber.yield()
    end
    var id = sock.receive(64); sock.send(id)
    system.out.println("[" + id + "]: Connected")
    listen = false
    var count = 0, million = 1, timestamp = runtime.time()
    loop
        while sock.available() == 0
            fiber.yield()
        end
        var s = sock.receive(64)
        if s == "end"
            break
        end
        sock.send(to_string(s.to_number() + 1))
        if ++count == 100000
            system.out.println("[" + id + "]: " + to_string(million) + " Million" + (million > 1 ? "s" : "") + " Request, Time = " + to_string((runtime.time() - timestamp)/1000) + "s")
            timestamp = runtime.time()
            ++million
            count = 0
        end
        fiber.yield()
    end
    system.out.println("[" + id + "]: Disconnected")
    co->state = 2
end

var worker_list = new array
loop
    if !listen
        listen = true
        var co = gcnew co_type
        co->co = fiber.create(worker, co, a)
        co->co.resume()
        worker_list.push_back(co)
    else
        foreach co in worker_list
            if co->state != 2
                co->co.resume()
            else
                co->co = null
            end
        end
    end
end
