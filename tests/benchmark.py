# ============================================================
# Python Performance Benchmark Suite
# ============================================================

import math
import random
import time
import asyncio


# Utility: get current time in microseconds
def runtime_time():
    return time.time_ns() // 1000000  # microseconds


# ---------------------- Sequential Execution ----------------------
def test_sequential_exec(num_tests):
    """Benchmark simple sequential loop performance"""
    total_ops = 0
    for _ in range(num_tests):
        counter = 0
        start_time = runtime_time()
        # Count how many iterations can be done within 1000 microseconds
        while runtime_time() - start_time < 1000:
            counter += 1
        total_ops += counter
    return int(total_ops / num_tests)


# ---------------------- Function Call Performance ----------------------
def test_sequential_fcall(num_tests):
    """Benchmark function call overhead in tight loops"""
    total_ops = 0

    def increment(counter):
        return counter + 1

    for _ in range(num_tests):
        counter = 0
        start_time = runtime_time()
        while runtime_time() - start_time < 1000:
            counter = increment(counter)
        total_ops += counter
    return int(total_ops / num_tests)


# ---------------------- Recursion Performance ----------------------
def recursive(depth):
    """Simple recursive counting function"""
    if depth <= 0:
        return 0
    else:
        return 1 + recursive(depth - 1)


def test_recursion(num_tests, depth):
    total_ops = 0
    for _ in range(num_tests):
        counter = 0
        start_time = runtime_time()
        while runtime_time() - start_time < 1000:
            recursive(depth)
            counter += 1
        total_ops += counter
    return int(total_ops / num_tests)


# ---------------------- Fiber Context Switching ----------------------
swap_count = None


async def worker(stop_event: asyncio.Event):
    """Async worker that yields repeatedly until stopped"""
    global swap_count
    while not stop_event.is_set():
        swap_count += 1
        await asyncio.sleep(0)  # simulate fiber.yield()


async def test_context_switch_async(num_tests, num_fibers):
    """Benchmark async context switching"""
    global swap_count
    total_ops = 0

    for _ in range(num_tests):
        swap_count = 0
        stop_event = asyncio.Event()
        fibers = [asyncio.create_task(worker(stop_event)) for _ in range(num_fibers)]

        start_time = runtime_time()
        while (runtime_time() - start_time) < 1000:
            await asyncio.sleep(0)  # give control to other tasks

        total_ops += swap_count
        swap_count = None
        stop_event.set()
        await asyncio.gather(*fibers)

    return int(total_ops / num_tests)


# ---------------------- Math Operations ----------------------
def do_integral(f, begin, end, precision_power):
    """Approximate integral using Riemann sum"""
    precision = 10 ** (-precision_power)
    x = begin
    total = 0.0
    while x < end:
        total += f(x) * precision
        x += precision
    return total


def test_math_ops(num_tests, precision_power):
    total_time = 0
    for _ in range(num_tests):
        start_time = runtime_time()
        do_integral(
            lambda x: math.sin(x) * math.cos(x) + math.sqrt(x), 0, 1, precision_power
        )
        total_time += runtime_time() - start_time
    # Higher score = more operations per second
    return int(num_tests * (10**precision_power) * 1000 / total_time)


# ---------------------- String Operations ----------------------
char_pool = (
    "QWERTYUIOPASDFGHJKLZXCVBNMqwertyuiopasdfghjklzxcvbnm1234567890-=!@#$%^&*()_+"
)


def rand_bytes(length):
    """Generate a random string of specified length"""
    return "".join(random.choice(char_pool) for _ in range(length))


def test_string_ops(num_tests, length):
    total_ops = 0
    pool = [rand_bytes(length) for _ in range(100)]  # pre-generated pool
    for _ in range(num_tests):
        counter = 0
        start_time = runtime_time()
        while runtime_time() - start_time < 1000:
            s = pool[random.randint(0, 99)] + pool[random.randint(0, 99)]
            counter += 1
        total_ops += counter
    return int(total_ops / num_tests)


# ---------------------- Array Random Access ----------------------
def test_array_index(num_tests, count):
    total_time = 0
    for _ in range(num_tests):
        arr = [random.randint(0, 9999) for _ in range(10000)]
        start_time = runtime_time()
        s = 0
        for _ in range(count):
            s += arr[random.randint(0, 9999)]
        total_time += runtime_time() - start_time
    return int(num_tests * count * 1000 / total_time)


# ---------------------- Hash Map Random Access ----------------------
def test_hash_index(num_tests, count):
    total_time = 0
    for _ in range(num_tests):
        hmap = {f"TAG{t}": random.randint(0, 9999) for t in range(10000)}
        start_time = runtime_time()
        s = 0
        for _ in range(count):
            s += hmap[f"TAG{random.randint(0, 9999)}"]
        total_time += runtime_time() - start_time
    return int(num_tests * count * 1000 / total_time)


# ---------------------- Quicksort Benchmark ----------------------
def quicksort(a, left, right):
    if right <= left:
        return
    i, j, pivot = left - 1, right, a[right]
    while True:
        while True:
            i += 1
            if i >= len(a) or a[i] >= pivot:
                break
        while True:
            j -= 1
            if j < 0 or a[j] <= pivot:
                break
        if i >= j:
            break
        a[i], a[j] = a[j], a[i]
    a[i], a[right] = a[right], a[i]
    quicksort(a, left, j)
    quicksort(a, i + 1, right)


def test_quicksort(num_tests, length):
    total_time = 0
    for _ in range(num_tests):
        arr = [random.randint(0, length) for _ in range(length)]
        start_time = runtime_time()
        quicksort(arr, 0, len(arr) - 1)
        total_time += runtime_time() - start_time
    return int(num_tests * length * 1000 / total_time)


# ---------------------- Benchmark Report ----------------------
def main():
    print("Python Performance Benchmark")
    print("Measure Unit: Operations Per Second (OPS)\n")

    print(f"Sequential Simple Execution: {test_sequential_exec(5)}")
    print(f"Sequential Function Calling: {test_sequential_fcall(5)}")
    print(f"Recursive Function Calling:  {test_recursion(5, 10)}")
    print(
        f"Fiber Context Switching:     {asyncio.run(test_context_switch_async(5, 5))}"
    )
    print(f"Complex Math Operations:     {test_math_ops(5, 5)}")
    print(f"String Concatenation:        {test_string_ops(5, 5)}")
    print(f"Random Array Access:         {test_array_index(5, 100000)}")
    print(f"Random Hash Access:          {test_hash_index(5, 100000)}")
    print(f"Array Quicksort:             {test_quicksort(5, 10000)}")

    print("\nBenchmark Finished\n")


if __name__ == "__main__":
    main()
