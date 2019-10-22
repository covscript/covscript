function hmove(list, n, from, to, via)
    if n == 1
        list.push_back({from, to})
        return
    end

    hmove(list, n - 1, from, via, to)
    list.push_back({from, to})
    hmove(list, n - 1, via, to, from)
end

function hanoi(n)
    var r = new list
    hmove(r, n, 1, 2, 3)
    return r
end

var result = hanoi(3)
foreach a in result
    system.out.println("" + a[0] + " -> " + a[1])
end

