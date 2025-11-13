# ============================================================
# Covariant Script Performance Benchmark Suite
# ============================================================

# ---------------------- Sequential Execution ----------------------
function test_sequential_exec(num_tests)
    # Benchmark simple sequential loop performance
    var total_ops = 0
    foreach _ in range(num_tests)
        var counter = 0
        var start_time = runtime.time()
        # Count how many iterations can be done within 1000 microseconds
        loop
            ++counter
        until runtime.time() - start_time >= 1000
        total_ops += counter
    end
    return to_integer(total_ops / num_tests)
end

# ---------------------- Function Call Performance ----------------------
function test_sequential_fcall(num_tests)
    # Benchmark function call overhead in tight loops
    var total_ops = 0
    var increment = [](counter)->++counter
    foreach _ in range(num_tests)
        var counter = 0
        var start_time = runtime.time()
        loop
            increment(counter)
        until runtime.time() - start_time >= 1000
        total_ops += counter
    end
    return to_integer(total_ops / num_tests)
end

# ---------------------- Recursion Performance ----------------------
function recursive(depth)
    # Simple recursive counting function
    if depth <= 0
        return 0
    else
        return 1 + recursive(depth - 1)
    end
end

function test_recursion(num_tests, depth)
    var total_ops = 0
    foreach _ in range(num_tests)
        var counter = 0
        var start_time = runtime.time()
        loop
            recursive(depth)
            ++counter
        until runtime.time() - start_time >= 1000
        total_ops += counter
    end
    return to_integer(total_ops / num_tests)
end

# ---------------------- Fiber Context Switching ----------------------
var swap_count = null

function worker()
    # Fiber worker that yields repeatedly until stopped
    loop
        ++swap_count
        fiber.yield()
    until swap_count == null
end

function test_context_switch(num_tests, num_fibers)
    var total_ops = 0
    foreach _ in range(num_tests)
        swap_count = 0
        var fibers = new array
        foreach _ in range(num_fibers)
            fibers.push_back(fiber.create(worker))
        end

        var start_time = runtime.time()
        while runtime.time() - start_time < 1000
            foreach f in fibers do f.resume()
        end

        total_ops += swap_count
        swap_count = null

        # Ensure all fibers exit gracefully
        foreach f in fibers do f.resume()
    end
    return to_integer(total_ops / num_tests)
end

# ---------------------- Math Operations ----------------------
function do_integral(f, begin, end, precision_power)
    # Approximate integral using Riemann sum
    var precision = 10^(-precision_power)
    var sum = 0
    foreach x in range(begin, end, precision)
        sum += f(x) * precision
    end
    return sum
end

function test_math_ops(num_tests, precision_power)
    var total_time = 0
    foreach _ in range(num_tests)
        var start_time = runtime.time()
        do_integral([](x) -> math.sin(x) * math.cos(x) + math.sqrt(x), 0, 1, precision_power)
        total_time += runtime.time() - start_time
    end
    # Higher score = more operations per second
    return to_integer(num_tests * 10^precision_power * 1000 / total_time)
end

# ---------------------- String Operations ----------------------
var char_pool = "QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm1234567890-=!@#$%^&*()_+"

function rand_bytes(length)
    # Generate a random string of specified length
    var str = new string
    foreach _ in range(length)
        str += char_pool.substr(math.randint(0, char_pool.size - 1), 1)
    end
    return str
end

function test_string_ops(num_tests, length)
    var total_ops = 0
    var pool = new array
    # Pre-generate a string pool for faster reuse
    foreach _ in range(100)
        pool.push_back(rand_bytes(length))
    end
    foreach _ in range(num_tests)
        var counter = 0
        var start_time = runtime.time()
        loop
            var s = pool[math.randint(0, 99)] + pool[math.randint(0, 99)]
            ++counter
        until runtime.time() - start_time >= 1000
        total_ops += counter
    end
    return to_integer(total_ops / num_tests)
end

# ---------------------- Array Random Access ----------------------
function test_array_index(num_tests, count)
    var total_time = 0
    foreach _ in range(num_tests)
        var arr = new array
        foreach _ in range(10000)
            arr.push_back(math.randint(0, 9999))
        end
        var start_time = runtime.time()
        var sum = 0
        foreach _ in range(count)
            sum += arr[math.randint(0, 9999)]
        end
        total_time += runtime.time() - start_time
    end
    return to_integer(num_tests * count * 1000 / total_time)
end

# ---------------------- Hash Map Random Access ----------------------
function test_hash_index(num_tests, count)
    var total_time = 0
    foreach _ in range(num_tests)
        var map = new hash_map
        foreach t in range(10000)
            map.insert(to_string("TAG" + t), math.randint(0, 9999))
        end
        var start_time = runtime.time()
        var sum = 0
        foreach _ in range(count)
            sum += map[to_string("TAG" + math.randint(0, 9999))]
        end
        total_time += runtime.time() - start_time
    end
    return to_integer(num_tests * count * 1000 / total_time)
end

# ---------------------- Quicksort Benchmark ----------------------
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

function test_quicksort(num_tests, length)
    var total_time = 0
    foreach _ in range(num_tests)
        var arr = new array
        foreach _ in range(length)
            arr.push_back(math.randint(0, length))
        end
        var start_time = runtime.time()
        quicksort(arr, 0, arr.size - 1)
        total_time += runtime.time() - start_time
    end
    return to_integer(num_tests * length * 1000 / total_time)
end

# ---------------------- Benchmark Report ----------------------
system.out.println("Covariant Script Performance Benchmark")
system.out.println("Measure Unit: Operations Per Second (OPS)\n")

system.out.println("Sequential Simple Execution: " + test_sequential_exec(5))
system.out.println("Sequential Function Calling: " + test_sequential_fcall(5))
system.out.println("Recursive Function Calling:  " + test_recursion(5, 10))
system.out.println("Fiber Context Switching:     " + test_context_switch(5, 5))
system.out.println("Complex Math Operations:     " + test_math_ops(5, 5))
system.out.println("String Concatenation:        " + test_string_ops(5, 5))
system.out.println("Random Array Access:         " + test_array_index(5, 100000))
system.out.println("Random Hash Access:          " + test_hash_index(5, 100000))
system.out.println("Array Quicksort:             " + test_quicksort(5, 10000))

system.out.println("\nBenchmark Finished\n")
