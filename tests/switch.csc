loop
    var in=system.in.input()
    switch typeid in
    	default
    		system.out.println("Types!")
        end
        case typeid number
            system.out.println("Number!")
        end
        case typeid string
            system.out.println("String!")
        end
    end
    switch in
        case 10
            system.out.println("10!")
        end
        case "Hello"
            system.out.println("Hello!")
        end
        case "World"
            system.out.println("World!")
        end
        default
            system.out.println("default!")
        end
    end
end
