function dummy_func(i)
    return i + 1
end

var co = null

foreach i in range(10)
    co = fiber.create(dummy_func, i)
    loop
        co.resume()
    until co.is_finished()
    system.out.println(co.return_value())
end
