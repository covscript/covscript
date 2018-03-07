var id_count=0
struct base
    var id=++id_count
    function initialize()
        system.out.println("Init base"+id)
    end
    function duplicate(orig)
        id=++id_count
        system.out.println("Copy base"+orig.id)
    end
    function finalize()
        system.out.println("Destroy base"+id)
    end
    function type()
        system.out.println("base"+id)
    end
end
struct child extends base
    function initialize() override
        system.out.println("Init child"+this.id)
    end
    function duplicate(orig) override
        system.out.println("Copy child"+orig.id)
    end
    function finalize() override
        system.out.println("Destroy child"+this.id)
    end
    function type() override
        system.out.println("child"+this.id)
    end
end
var a=new child
a.type()
var b=a
b.type()
var c=b.parent
c.type()