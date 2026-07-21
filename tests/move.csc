var test_uid = 0
var duplicate_count = 0

class B
    var uid = null
    function initialize()
        uid = ++test_uid
        system.out.println("[B] Call initializer: " + uid)
    end
    function duplicate(orig)
        uid = ++test_uid
        ++duplicate_count
        system.out.println("[B] Call duplicator: " + orig.uid + " -> " + uid)
    end
    function finalize()
        system.out.println("[B] Call destroyer: " + uid)
    end
end

class A
    var uid = null
    var member_b = null
    function initialize()
        uid = ++test_uid
        member_b = new B
        system.out.println("[A] Call initializer: " + uid)
    end
    function duplicate(orig)
        uid = ++test_uid
        ++duplicate_count
        system.out.println("[A] Call duplicator: " + orig.uid + " -> " + uid)
    end
    function finalize()
        system.out.println("[A] Call destroyer: " + uid)
    end

    function make_b()
        system.out.println("[A] make_b called")
        var b = new B
        system.out.println("[A] returning move(b)")
        return move(b)
    end

    function make_b_no_move()
        system.out.println("[A] make_b_no_move called")
        var b = new B
        system.out.println("[A] returning b (no move)")
        return b
    end

    function get_b()
        system.out.println("[A] get_b called")
        system.out.println("[A] returning member_b")
        return member_b
    end
end

function trivial_move()
    system.out.println("[free] trivial_move called")
    var b = new B
    system.out.println("[free] returning move(b)")
    return move(b)
end

function trivial_no_move()
    system.out.println("[free] trivial_no_move called")
    var b = new B
    system.out.println("[free] returning b (no move)")
    return b
end

function assert(cond, msg)
    if !cond
        system.out.println("FAIL: " + msg)
    else
        system.out.println("PASS: " + msg)
    end
end

# --- Test 0: free function, return move(local) ---
system.out.println("")
system.out.println("=== Test 0: free function return move(b) ===")
duplicate_count = 0
var t0 = trivial_move()
assert(duplicate_count == 0, "Test 0: duplicate_count should be 0, got " + duplicate_count)
system.out.println("=== End Test 0 ===")

# --- Test 0b: free function, return local without move ---
system.out.println("")
system.out.println("=== Test 0b: free function return b (no move) ===")
duplicate_count = 0
var t1 = trivial_no_move()
assert(duplicate_count == 0, "Test 0b: duplicate_count should be 0, got " + duplicate_count)
system.out.println("=== End Test 0b ===")

# --- Test 1: member function, return move(local) ---
system.out.println("")
system.out.println("=== Test 1: return move(b) ===")
var a = new A
duplicate_count = 0
var b1 = a.make_b()
assert(duplicate_count == 0, "Test 1: duplicate_count should be 0, got " + duplicate_count)
system.out.println("=== End Test 1 ===")

# --- Test 2: member function, return local without move ---
system.out.println("")
system.out.println("=== Test 2: return b (no move) ===")
duplicate_count = 0
var b2 = a.make_b_no_move()
assert(duplicate_count == 0, "Test 2: duplicate_count should be 0, got " + duplicate_count)
system.out.println("=== End Test 2 ===")

# --- Test 3: member function, return member variable ---
system.out.println("")
system.out.println("=== Test 3: return member variable ===")
duplicate_count = 0
var b3 = a.get_b()
assert(duplicate_count == 1, "Test 3: duplicate_count should be 1, got " + duplicate_count)
system.out.println("=== End Test 3 ===")
