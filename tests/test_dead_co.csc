function dummy_func()
    return
end

var co = null

foreach i in range(10)
    co = fiber.create(dummy_func)
    loop
        var state = co.resume()
    until state == -2
end
