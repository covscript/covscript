function char2num(ch)
    if !ch.isdigit()
        throw runtime.exception("Bad format: non-digit char")
    end
    return to_integer(ch) - to_integer('0')
end
function check(id)
    var sum = 0, examine = 0
    if id.size != 18
        throw runtime.exception("Bad format: size")
    end
    foreach i in range(1, 18)
        sum += 2 ^ (18 - i) % 11 * char2num(id[i - 1])
    end
    if char2num(id[17]) == (examine = (12 - (sum % 11)) % 11)
        system.out.println("Good")
    else
        system.out.println("Bad, last number should be " + examine)
    end
end
loop
    try
        check(system.in.getline())
    catch e
        system.out.println(e.what)
    end
end
