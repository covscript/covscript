import test_fd_a
function worker()
    test_fd_a.from_a()
end
var co = fiber.create(worker)
co.resume()
system.out.println("main resumed")
co.resume()
system.out.println("done")
