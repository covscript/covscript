struct base
    function initialize()
        system.out.println("Init base")
    end
    function duplicate()
        system.out.println("Copy base")
    end
    function finalize()
        system.out.println("Destroy base")
    end
    function type()
        system.out.println("base")
    end
end
struct child extends base
    function initialize() override
        system.out.println("Init child")
    end
    function type() override
        system.out.println("child")
    end
end
(new child).type()