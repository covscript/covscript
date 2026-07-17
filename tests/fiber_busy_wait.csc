function worker()
    foreach i in range(10)
        runtime.delay(1000)
        fiber.yield()
    end
end

function do_test()
    var co = fiber.create(worker)
    var latencies = {}
    var warmup = 5

    loop
        var start = runtime.time()
        fiber.resume(co)
        var elapsed = runtime.time() - start

        if warmup > 0
            warmup = warmup - 1
        else
            latencies.push_back(elapsed)
        end
    until co.is_finished()

    latencies.sort([](lhs, rhs)-> lhs < rhs)
    var n = latencies.size
    var total = 0
    foreach x in latencies
        total = total + x
    end
    system.out.println("Samples: " + n)
    system.out.println("Average: " + (total / n) + "ms")
    system.out.println("Max:     " + latencies.back + "ms")
    system.out.println("P50:     " + latencies[n * 50 / 100] + "ms")
    system.out.println("P90:     " + latencies[n * 90 / 100] + "ms")
    system.out.println("P99:     " + latencies[n * 99 / 100] + "ms")
end

system.out.println("=== balanced ===")
do_test()
system.out.println("=== responsive ===")
fiber.set_schedule_policy("responsive")
do_test()
system.out.println("=== efficient ===")
fiber.set_schedule_policy("efficient")
do_test()
system.out.println("=== throughput ===")
fiber.set_schedule_policy("throughput")
do_test()
