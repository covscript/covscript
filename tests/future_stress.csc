using system

# Concurrency stress: native-function futures run on std::thread workers; this
# hammers the async path (thread_local current_process on worker threads, the
# thread_guard allocator pool, future machinery) and the fiber <-> future
# interleaving. Run under TSAN (setarch -R) for race detection.
var ok = true
foreach batch in range(200)
    var futures = new array
    foreach i in range(32)
        futures.push_back(future.create(math.sqrt, i))
    end
    var idx = 0
    foreach f in futures
        var r = f.get()
        if r != math.sqrt(idx)
            ok = false
        end
        ++idx
    end
end

# Same hammer from inside a fiber (fiber + async-worker interleaving).
function worker()
    foreach batch in range(50)
        var futures = new array
        foreach i in range(16)
            futures.push_back(future.create(to_integer, i + 1))
        end
        foreach f in futures
            if f.get() <= 0
                ok = false
            end
        end
    end
end

var co = fiber.create(worker)
loop
    co.resume()
until co.is_finished()

system.out.println(ok ? "stress ok" : "stress bad")
