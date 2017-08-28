loop
    switch(system.in.input())
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
