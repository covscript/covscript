function check(condition, message)
    if !condition
        throw runtime.exception(message)
    end
end

function expect_get_exception(future, message)
    var caught = false
    try
        future.get()
    catch e
        caught = true
    end
    check(caught, message)
end

function fail_fiber()
    throw runtime.exception("fiber boom")
end

function self_wait_worker()
    var future = fiber.current().get_future()
    check(!future.wait_for(10), "A running fiber must not become ready while waiting for itself")
    future.wait()
    expect_get_exception(future, "A self-wait resume error must be reported by get")
end

# Exceptions must stay with the async future that produced them.
var empty_values = {}
var failed_async = future.create(empty_values.pop_back)
runtime.sleep_for(50)
var successful_async = future.create(runtime.sleep_for, 10)
successful_async.get()
expect_get_exception(failed_async, "An async exception must not be consumed by another future")

# wait_for and wait only observe readiness; get propagates the exception.
failed_async = future.create(empty_values.pop_back)
runtime.sleep_for(50)
check(failed_async.wait_for(100), "A failed async future must become ready")
failed_async.wait()
expect_get_exception(failed_async, "Only get should propagate an async exception")

# Fiber futures use the same get-only exception propagation contract.
var failed_fiber = fiber.create(fail_fiber).get_future()
check(failed_fiber.wait_for(100), "A failed fiber future must become ready")
failed_fiber.wait()
expect_get_exception(failed_fiber, "Only get should propagate a fiber exception")

# A short timeout may overshoot the wall clock, but it must not report ready.
var sleeping_fiber = fiber.create([]()->fiber.sleep_for(100)).get_future()
check(!sleeping_fiber.wait_for(5), "A sleeping fiber must not be ready before its wake time")
sleeping_fiber.get()

# Waiting on the current running fiber must terminate instead of spinning forever.
var self_wait = fiber.create(self_wait_worker)
loop
    fiber.resume(self_wait)
until fiber.is_finished(self_wait)

system.out.println("Future regression tests passed.")