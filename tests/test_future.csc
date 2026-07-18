var start_time = 0, end_time = 0, round_trip = 0
var async_future = null, fiber_future = null, co = null

function wait_for_worker(future, times, timeout)
    foreach i in range(times)
        future.wait_for(timeout)
    end
end

function wait_worker(future)
    future.wait()
end

function get_worker(future)
    future.get()
end

system.out.println("\n=== Test Within Fiber ===")

# Test for regular wait_for and get
system.out.println("\nTesting async future wait_for and get...")
round_trip = 0
async_future = future.create(runtime.sleep_for, 1000)
co = fiber.create(wait_for_worker, async_future, 10, 100)
start_time = runtime.time()
loop
    co.resume()
    ++round_trip
until co.is_finished()
end_time = runtime.time()
system.out.println("Async future wait_for took: " + (end_time - start_time) + " ms")
system.out.println("Round trips: " + round_trip)

start_time = runtime.time()
async_future.get()
end_time = runtime.time()
system.out.println("Async future get took: " + (end_time - start_time) + " ms")

# Test for frequent wait_for and get
system.out.println("\nTesting async future frequent wait_for and get...")
round_trip = 0
async_future = future.create(runtime.sleep_for, 1000)
co = fiber.create(wait_for_worker, async_future, 200, 5)
start_time = runtime.time()
loop
    co.resume()
    ++round_trip
until co.is_finished()
end_time = runtime.time()
system.out.println("Async future wait_for took: " + (end_time - start_time) + " ms")
system.out.println("Round trips: " + round_trip)

start_time = runtime.time()
async_future.get()
end_time = runtime.time()
system.out.println("Async future get took: " + (end_time - start_time) + " ms")

# Test for regular wait and get
system.out.println("\nTesting async future wait and get...")
round_trip = 0
async_future = future.create(runtime.sleep_for, 1000)
co = fiber.create(wait_worker, async_future)
start_time = runtime.time()
loop
    co.resume()
    ++round_trip
until co.is_finished()
end_time = runtime.time()
system.out.println("Async future wait took: " + (end_time - start_time) + " ms")
system.out.println("Round trips: " + round_trip)

start_time = runtime.time()
async_future.get()
end_time = runtime.time()
system.out.println("Async future get took: " + (end_time - start_time) + " ms")

# Test for directly get
system.out.println("\nTesting async future directly get...")
async_future = future.create(runtime.sleep_for, 1000)
co = fiber.create(get_worker, async_future)
start_time = runtime.time()
round_trip = 0
loop
    co.resume()
    ++round_trip
until co.is_finished()
end_time = runtime.time()
system.out.println("Async future get took: " + (end_time - start_time) + " ms")
system.out.println("Round trips: " + round_trip)

system.out.println("\n=== Test Without Fiber ===")

# Test for regular wait_for and get
system.out.println("\nTesting async future wait_for and get...")
async_future = future.create(runtime.sleep_for, 1000)
start_time = runtime.time()
foreach i in range(10)
    async_future.wait_for(100)
end
end_time = runtime.time()
system.out.println("Async future wait_for took: " + (end_time - start_time) + " ms")

start_time = runtime.time()
async_future.get()
end_time = runtime.time()
system.out.println("Async future get took: " + (end_time - start_time) + " ms")

# Test for frequent wait_for and get
system.out.println("\nTesting async future frequent wait_for and get...")
async_future = future.create(runtime.sleep_for, 1000)
start_time = runtime.time()
foreach i in range(200)
    async_future.wait_for(5)
end
end_time = runtime.time()
system.out.println("Async future wait_for took: " + (end_time - start_time) + " ms")

start_time = runtime.time()
async_future.get()
end_time = runtime.time()
system.out.println("Async future get took: " + (end_time - start_time) + " ms")

# Test for regular wait and get
system.out.println("\nTesting async future wait and get...")
async_future = future.create(runtime.sleep_for, 1000)
start_time = runtime.time()
async_future.wait()
end_time = runtime.time()
system.out.println("Async future wait took: " + (end_time - start_time) + " ms")

start_time = runtime.time()
async_future.get()
end_time = runtime.time()
system.out.println("Async future get took: " + (end_time - start_time) + " ms")

# Test for directly get
system.out.println("\nTesting async future directly get...")
async_future = future.create(runtime.sleep_for, 1000)
start_time = runtime.time()
async_future.get()
end_time = runtime.time()
system.out.println("Async future get took: " + (end_time - start_time) + " ms")

# Test for fiber future within fiber: wait_for and get
system.out.println("\n=== Test Fiber Future Within Fiber ===")

system.out.println("\nTesting fiber future wait_for within fiber...")
round_trip = 0
fiber_future = fiber.create([]()->fiber.sleep_for(1000)).get_future()
co = fiber.create(wait_for_worker, fiber_future, 10, 100)
start_time = runtime.time()
loop
    co.resume()
    ++round_trip
until co.is_finished()
end_time = runtime.time()
system.out.println("Fiber future wait_for took: " + (end_time - start_time) + " ms")
system.out.println("Round trips: " + round_trip)

start_time = runtime.time()
fiber_future.get()
end_time = runtime.time()
system.out.println("Fiber future get took: " + (end_time - start_time) + " ms")

# Test for fiber future within fiber: frequent wait_for
system.out.println("\nTesting fiber future frequent wait_for within fiber...")
round_trip = 0
fiber_future = fiber.create([]()->fiber.sleep_for(1000)).get_future()
co = fiber.create(wait_for_worker, fiber_future, 200, 5)
start_time = runtime.time()
loop
    co.resume()
    ++round_trip
until co.is_finished()
end_time = runtime.time()
system.out.println("Fiber future wait_for took: " + (end_time - start_time) + " ms")
system.out.println("Round trips: " + round_trip)

start_time = runtime.time()
fiber_future.get()
end_time = runtime.time()
system.out.println("Fiber future get took: " + (end_time - start_time) + " ms")

# Test for fiber future within fiber: wait and get
system.out.println("\nTesting fiber future wait within fiber...")
round_trip = 0
fiber_future = fiber.create([]()->fiber.sleep_for(1000)).get_future()
co = fiber.create(wait_worker, fiber_future)
start_time = runtime.time()
loop
    co.resume()
    ++round_trip
until co.is_finished()
end_time = runtime.time()
system.out.println("Fiber future wait took: " + (end_time - start_time) + " ms")
system.out.println("Round trips: " + round_trip)

start_time = runtime.time()
fiber_future.get()
end_time = runtime.time()
system.out.println("Fiber future get took: " + (end_time - start_time) + " ms")

# Test for fiber future within fiber: directly get
system.out.println("\nTesting fiber future directly get within fiber...")
fiber_future = fiber.create([]()->fiber.sleep_for(1000)).get_future()
co = fiber.create(get_worker, fiber_future)
start_time = runtime.time()
round_trip = 0
loop
    co.resume()
    ++round_trip
until co.is_finished()
end_time = runtime.time()
system.out.println("Fiber future get took: " + (end_time - start_time) + " ms")
system.out.println("Round trips: " + round_trip)

system.out.println("\n=== Test Fiber Future Without Fiber ===")

# Test for regular fiber wait_for and get
system.out.println("\nTesting fiber future wait_for and get...")
fiber_future = fiber.create([]()->fiber.sleep_for(1000)).get_future()
start_time = runtime.time()
foreach i in range(10)
    fiber_future.wait_for(100)
end
end_time = runtime.time()
system.out.println("Fiber future wait_for took: " + (end_time - start_time) + " ms")

start_time = runtime.time()
fiber_future.get()
end_time = runtime.time()
system.out.println("Fiber future get took: " + (end_time - start_time) + " ms")

# Test for frequent fiber wait_for and get
system.out.println("\nTesting fiber future frequent wait_for and get...")
fiber_future = fiber.create([]()->fiber.sleep_for(1000)).get_future()
start_time = runtime.time()
foreach i in range(200)
    fiber_future.wait_for(5)
end
end_time = runtime.time()
system.out.println("Fiber future wait_for took: " + (end_time - start_time) + " ms")

start_time = runtime.time()
fiber_future.get()
end_time = runtime.time()
system.out.println("Fiber future get took: " + (end_time - start_time) + " ms")

# Test for regular fiber wait and get
system.out.println("\nTesting fiber future wait and get...")
fiber_future = fiber.create([]()->fiber.sleep_for(1000)).get_future()
start_time = runtime.time()
fiber_future.wait()
end_time = runtime.time()
system.out.println("Fiber future wait took: " + (end_time - start_time) + " ms")

start_time = runtime.time()
fiber_future.get()
end_time = runtime.time()
system.out.println("Fiber future get took: " + (end_time - start_time) + " ms")

# Test for directly fiber get
system.out.println("\nTesting fiber future directly get...")
fiber_future = fiber.create([]()->fiber.sleep_for(1000)).get_future()
start_time = runtime.time()
fiber_future.get()
end_time = runtime.time()
system.out.println("Fiber future get took: " + (end_time - start_time) + " ms")
