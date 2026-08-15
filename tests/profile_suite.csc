using system

function seq_loop(iter)
    var x = 0
    foreach _ in range(iter)
        ++x
    end
    return x
end

function rec(n)
    if n <= 0
        return 0
    end
    return 1 + rec(n - 1)
end

function fcall_loop(iter)
    var f = [](x) -> x + 1
    var y = 0
    foreach _ in range(iter)
        y = f(y)
    end
    return y
end

function math_loop(iter)
    var s = 0
    foreach i in range(iter)
        s += math.sin(i) + math.cos(i) + math.sqrt(i + 1)
    end
    return s
end

function str_loop(iter)
    var s = ""
    var pool = {"abc", "def", "ghi", "jkl"}
    foreach i in range(iter)
        s = pool[i % 4] + pool[(i + 1) % 4]
    end
    return s.size
end

function arr_loop(iter, size)
    var arr = new array
    foreach i in range(size)
        arr.push_back(i)
    end
    var s = 0
    foreach _ in range(iter)
        s += arr[math.randint(0, size - 1)]
    end
    return s
end

function hash_loop(iter, size)
    var m = new hash_map
    foreach i in range(size)
        m.insert("KEY" + i, i)
    end
    var s = 0
    foreach _ in range(iter)
        s += m["KEY" + math.randint(0, size - 1)]
    end
    return s
end

function set_loop(iter)
    var hs = {1, 2, 3}.to_hash_set()
    var s = 0
    foreach i in range(iter)
        hs.insert(i)
        if hs.exist(i)
            s += 1
        end
        hs.erase(i)
    end
    return s
end

function quicksort(a, left, right)
    if right <= left
        return 0
    end
    var (i, j, pivot) = {left - 1, right, a[right]}
    loop
        loop
            ++i
            if i >= a.size
                break
            end
        until a[i] >= pivot
        loop
            --j
            if j < 0
                break
            end
        until a[j] <= pivot
        if i >= j
            break
        end
        swap(a[i], a[j])
    end
    swap(a[i], a[right])
    quicksort(a, left, j)
    quicksort(a, i + 1, right)
end

var cnt = 0
function fib_worker()
    foreach i in range(400000)
        ++cnt
        fiber.yield()
    end
end

function main_work()
    seq_loop(600000)
    fcall_loop(600000)
    rec(20)
    math_loop(100000)
    str_loop(400000)
    arr_loop(500000, 1000)
    hash_loop(400000, 1000)
    set_loop(200000)
    var a = new array
    foreach i in range(3000)
        a.push_back(math.randint(0, 2000))
    end
    quicksort(a, 0, a.size - 1)
    var co = fiber.create(fib_worker)
    loop
        co.resume()
    until co.is_finished()
end

main_work()
system.out.println("profile_suite done")
