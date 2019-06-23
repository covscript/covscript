var id_count=0
class test
    var id=null
    function initialize()
        id=++id_count
        system.out.println("Init "+this.id)
    end
    function duplicate(orig)
        id=++id_count
        system.out.println("Copy "+orig.id)
    end
    function finalize()
        system.out.println("Destroy "+this.id)
    end
end
block
    var a=new test
    var b=a
    var c=move(a)
end