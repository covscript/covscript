function worker()
    foreach i in range(10)
        system.out.println("Working: " + i)
        runtime.delay(1000)
        fiber.yield()
    end
end

var co = fiber.create(worker)
var start_time = runtime.time()
var max_scheduling_time = 0
var scheduling_count = 0
loop
    var start = runtime.time()
    fiber.resume(co)
    max_scheduling_time = math.max(max_scheduling_time, runtime.time() - start)
    ++scheduling_count
until co.is_finished()

system.out.println("Average Scheduling Time: " + ((runtime.time() - start_time) / scheduling_count))
system.out.println("Max Scheduling Time: " + max_scheduling_time)
