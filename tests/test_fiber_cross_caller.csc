# Regression test for fiber prev_ctx rebinding.
#
# A worker fiber is started by one caller (itself a fiber) and then resumed
# again from a different caller after the original caller has already finished.
# Before the fix, the worker's return context (prev_ctx) was bound only once on
# creation, so yielding/finishing would jump back to a stale - possibly already
# destroyed - context, causing wrong control flow or a crash.

var worker = null

function worker_body()
    system.out.println("worker: step 1")
    fiber.yield()
    system.out.println("worker: step 2")
    fiber.yield()
    system.out.println("worker: step 3")
    return 42
end

# starter is a fiber that starts the worker and then finishes, so the worker's
# original caller frame is gone afterwards.
function starter_body()
    system.out.println("starter: begin")
    worker.resume()
    system.out.println("starter: end")
end

function main()
    worker = fiber.create(worker_body)
    var starter = fiber.create(starter_body)
    # starter starts the worker (worker runs step 1, yields back to starter),
    # then starter finishes.
    starter.resume()
    system.out.println("main: starter finished = " + to_string(starter.is_finished()))
    # Resume the worker from main - a different caller than starter, whose
    # frame is already gone. This must safely return control to main.
    worker.resume()
    system.out.println("main: after first resume from main")
    # Resume again - worker runs step 3, returns 42 and finishes.
    worker.resume()
    system.out.println("main: worker finished = " + to_string(worker.is_finished()))
    system.out.println("main: worker return = " + to_string(worker.return_value()))
end

main()
