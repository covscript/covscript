function async_worker(queue)
    var counter = 0
    loop
        system.out.println("Resumed")
        queue.push_back(++counter)
        system.out.println("Produced: " + counter)
        system.out.println("Yielding...")
        fiber.yield()
    until queue.pop_back()
    system.out.println("Finished")
end

struct async_iterator
    var queue = new array
    var co = null
    function bind(func)
        co = fiber.create(func, queue)
        return this
    end
    function next()
        co.resume()
        var val = queue.pop_back()
        if val > 10
            queue.push_back(true)
            co.resume()
            return {val, true}
        else
            queue.push_back(false)
            return {val, false}
        end
    end
end

function create_async_iterator(func)
    return (new async_iterator).bind(func)
end

foreach it in create_async_iterator(async_worker)
    system.out.println("Iteration: " + it)
end

system.out.println("Done")