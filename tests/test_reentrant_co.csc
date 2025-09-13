class tracer_t
    var id = null
    function finalize()
        system.out.println(this.id + " destroy")
    end
end

var test_exception = false

function body0()
    var tracer = new tracer_t
    tracer.id = "Body 0"
    system.out.println("Body 0 start")
    if test_exception
        throw runtime.exception("Test Exception")
    end
    system.out.println("Body 0 yield")
    runtime.yield()
    system.out.println("Body 0 exit")
end

function body1()
    var tracer = new tracer_t
    tracer.id = "Body 1"
    system.out.println("Body 1 start")
    var c0 = runtime.create_co(body0)
    system.out.println("Body 1 yield")
    runtime.yield()
    system.out.println("Body 1 resume 0")
    runtime.resume(c0)
    system.out.println("Body 1 resume 1")
    runtime.resume(c0)
    system.out.println("Body 1 exit")
end

system.out.println("Main start")
var c1 = runtime.create_co(body1)
runtime.resume(c1)
system.out.println("Main resume")
try
    runtime.resume(c1)
catch e
    system.out.println(e.what)
end
system.out.println("Main exit")
