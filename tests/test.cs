function main(start)
    var i=start
    while(i<100)
        system.println(i)
        if(i>=50)
            break
        else
            ++i
        end
    end
    return i
end
system.print("Please enter the start:")
system.println("Result:",main(system.input()))
