using system

function worker()
    fiber.yield()
end

var f = fiber.create(worker)
f.resume()
f = null
system.out.println("done")
