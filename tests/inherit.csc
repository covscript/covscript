struct base
    function type()
        system.out.println("base")
    end
end
struct child extends base
    function type() override
        system.out.println("child")
    end
end
(new child).type()