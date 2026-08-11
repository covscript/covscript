#include <covscript/covscript.hpp>
#include "test_helpers.hpp"
#include <limits>
#include <sstream>

// =============================================================================
// Branch regression tests (fix_import).
//
// Every behavior below was exercised with ad-hoc command-line scripts while
// fixing the audit findings; those scripts are promoted to these in-process
// tests so the fixes cannot silently regress. Each test either:
//   - reproduces a behavior that is broken on master and fixed on this branch
//     (so it fails on master, passes here), or
//   - guards a semantic the branch relies on.
// =============================================================================

namespace {

// Run a full script in a fresh context, capturing system.out output. Any
// exception (compile_error, cs::exception, ...) propagates to the caller.
std::string run_script(const std::string &src)
{
	cs::array args;
	args.push_back(cs::var::make<cs::string>("<UNIT_TEST>"));
	auto ctx = cs::create_context(args);
	std::ostringstream captured;
	auto *old = std::cout.rdbuf(captured.rdbuf());
	try {
		std::istringstream in(src);
		ctx->instance->compile(in);
		ctx->instance->interpret();
	}
	catch (...) {
		std::cout.rdbuf(old);
		throw;
	}
	std::cout.rdbuf(old);
	return captured.str();
}

// Run a script that is expected to throw and return the thrown error message.
// Fails the test (test_failure) if the script completes without throwing.
std::string run_script_expect_throw(const std::string &src)
{
	try {
		run_script(src);
	}
	catch (const cs::exception &e) {
		return e.what();
	}
	catch (const cs::compile_error &e) {
		return e.what();
	}
	catch (const std::exception &e) {
		return e.what();
	}
	throw cs_test::test_failure("expected the script to throw");
}

} // namespace

// =============================================================================
// break/continue are only valid inside a loop (C1, aa2b7cd).
// Master compiled them anywhere, letting a stray break/continue punch through
// a function boundary and corrupt the enclosing loop's control flow.
// =============================================================================

TEST(break_outside_loop_rejected)
{
	EXPECT_CONTAINS(run_script_expect_throw("using system\nbreak\n"), "not inside any loop");
}

TEST(continue_outside_loop_rejected)
{
	EXPECT_CONTAINS(run_script_expect_throw("using system\ncontinue\n"), "not inside any loop");
}

TEST(break_in_function_without_loop_rejected)
{
	const std::string src =
	    "using system\n"
	    "function f()\n"
	    "\tbreak\n"
	    "end\n"
	    "f()\n";
	EXPECT_CONTAINS(run_script_expect_throw(src), "not inside any loop");
}

TEST(continue_in_function_without_loop_rejected)
{
	const std::string src =
	    "using system\n"
	    "function f()\n"
	    "\tcontinue\n"
	    "end\n"
	    "f()\n";
	EXPECT_CONTAINS(run_script_expect_throw(src), "not inside any loop");
}

TEST(break_in_switch_without_loop_rejected)
{
	// A switch is not itself a break target in CovScript: its cases are
	// explicit end-delimited blocks, so a break with no enclosing loop is
	// meaningless and must be rejected.
	const std::string src =
	    "using system\n"
	    "switch 1\n"
	    "case 1\n"
	    "\tbreak\n"
	    "end\n"
	    "end\n";
	EXPECT_CONTAINS(run_script_expect_throw(src), "not inside any loop");
}

// =============================================================================
// loop_depth bookkeeping (464a0e3): a loop that closes while an enclosing
// block is still open must release its depth, otherwise the leaked depth is
// counted again during the deferred body translation and a break OUTSIDE the
// loop is wrongly accepted.
// =============================================================================

TEST(break_after_nested_loop_in_switch_rejected)
{
	const std::string src =
	    "using system\n"
	    "switch 1\n"
	    "case 1\n"
	    "\tloop\n"
	    "\t\tbreak\n"
	    "\tend\n"
	    "\tbreak\n"
	    "end\n"
	    "end\n";
	EXPECT_CONTAINS(run_script_expect_throw(src), "not inside any loop");
}

TEST(break_after_closed_loop_in_function_rejected)
{
	const std::string src =
	    "using system\n"
	    "function f()\n"
	    "\tloop\n"
	    "\t\tbreak\n"
	    "\tend\n"
	    "\tbreak\n"
	    "end\n"
	    "f()\n";
	EXPECT_CONTAINS(run_script_expect_throw(src), "not inside any loop");
}

TEST(break_in_function_loop_allowed)
{
	// A function boundary resets loop depth, and a loop opened inside the
	// function body must re-enable break for its own scope only.
	const std::string src =
	    "using system\n"
	    "function f()\n"
	    "\tloop\n"
	    "\t\tbreak\n"
	    "\tend\n"
	    "end\n"
	    "f()\n";
	EXPECT_TRUE(run_script(src).empty());
}

TEST(break_in_nested_loop_allowed)
{
	const std::string src =
	    "using system\n"
	    "var i = 0\n"
	    "loop\n"
	    "\ti += 1\n"
	    "\tif i > 1\n"
	    "\t\tbreak\n"
	    "\tend\n"
	    "\tswitch i\n"
	    "\tcase 1\n"
	    "\t\tbreak\n"
	    "\tend\n"
	    "\tend\n"
	    "end\n"
	    "system.out.println(i)\n";
	EXPECT_TRUE(run_script(src) == "1\n");
}

// =============================================================================
// A break/continue inside a switch case controls the ENCLOSING loop: the
// switch block is transparent to these flags (cases are explicit end-delimited
// blocks, so no C-style "break out of switch" is needed).
// =============================================================================

TEST(switch_break_controls_outer_loop)
{
	const std::string src =
	    "using system\n"
	    "var i = 0\n"
	    "loop\n"
	    "\ti += 1\n"
	    "\tswitch i\n"
	    "\tcase 2\n"
	    "\t\tbreak\n"
	    "\tend\n"
	    "\tend\n"
	    "end\n"
	    "system.out.println(i)\n";
	EXPECT_TRUE(run_script(src) == "2\n");
}

TEST(switch_continue_controls_outer_loop)
{
	const std::string src =
	    "using system\n"
	    "var i = 0\n"
	    "loop\n"
	    "\ti += 1\n"
	    "\tif i > 3\n"
	    "\t\tbreak\n"
	    "\tend\n"
	    "\tswitch i\n"
	    "\tcase 1\n"
	    "\t\tcontinue\n"
	    "\tend\n"
	    "\tend\n"
	    "\tsystem.out.println(i)\n"
	    "end\n";
	EXPECT_TRUE(run_script(src) == "2\n3\n");
}

// =============================================================================
// async/fiber classification (A1, 324748a): async must reject ALL script code.
// Master's is_native_callable treated anything that was not a function_ptr as
// native, so a lambda (compiled to cs::function) ran the interpreter on a
// std::async worker thread.
// =============================================================================

TEST(future_create_lambda_rejected)
{
	EXPECT_CONTAINS(run_script_expect_throw("using system\nvar f = future.create([](x)->x*2, 21)\n"),
	                "only be created from native functions");
}

TEST(future_create_script_function_rejected)
{
	const std::string src =
	    "using system\n"
	    "function fib(n)\n"
	    "\treturn n\n"
	    "end\n"
	    "var f = future.create(fib, 10)\n";
	EXPECT_CONTAINS(run_script_expect_throw(src), "only be created from native functions");
}

// =============================================================================
// Constant immutability (D1, 5c9f61a): inc/dec must not be folded onto a
// constant. Master folded `c++` into a write-through of the shared constant,
// silently printing 6; this branch rejects the modification at runtime.
// =============================================================================

TEST(constant_write_through_rejected)
{
	const std::string src =
	    "using system\n"
	    "constant c = 5\n"
	    "c++\n"
	    "system.out.println(c)\n";
	EXPECT_CONTAINS(run_script_expect_throw(src), "not allowed to change the value of a constant");
}

// =============================================================================
// system.exit contract: the exit code is dispatched through on_process_exit.
// =============================================================================

TEST(system_exit_dispatches_code)
{
	static int captured = -1;
	cs::current_process->on_process_exit.add_listener([](void *code) -> bool {
		captured = *static_cast<int *>(code);
		return true; // swallow: never actually exit the unit-test process
	});
	captured = -1;
	run_script("using system\nsystem.exit(3)\n");
	EXPECT_TRUE(captured == 3);
}

// =============================================================================
// system.exit from inside a fiber must still reach the on_process_exit handler
// (the exit code is forwarded through the fiber's forked process up to the
// main process). The CNI boundary must not convert the located cs::exception
// sentinel into a forward_exception that embeds the "File ..., line ..."
// wrapper, or CS_EXIT/CS_SIGINT can never be recognized again.
// =============================================================================

TEST(system_exit_from_fiber_dispatches_code)
{
	// Mirror the interpreter's exit listener: record the code on the main
	// process (main() reads current_process->exit_code after covscript_main
	// returns) and throw the CS_EXIT sentinel. The exit code must survive a
	// fiber boundary; writing it to the fiber's forked process would lose it.
	static cs::process_context *main_process = cs::current_process;
	static int captured = -1;
	cs::current_process->on_process_exit.add_listener([](void *code) -> bool {
		main_process->exit_code = *static_cast<int *>(code);
		captured = *static_cast<int *>(code);
		throw cs::fatal_error("CS_EXIT");
		return true;
	});
	main_process->exit_code = -1;
	captured = -1;
	try {
		run_script("using system\n"
		           "function f()\n"
		           "\tsystem.exit(7)\n"
		           "end\n"
		           "fiber.create(f).resume()\n");
	}
	catch (const cs::exception &) {
		// The CS_EXIT sentinel escapes the fiber (checked by the next test).
	}
	EXPECT_TRUE(captured == 7);
	EXPECT_TRUE(main_process->exit_code == 7);
}

TEST(fiber_exit_sentinel_keeps_bare_message)
{
	// Mirror the interpreter: the exit listener throws the CS_EXIT sentinel.
	// The fiber resumes the exception across the CNI boundary; it must come
	// back as a located cs::exception whose bare message is exactly CS_EXIT
	// (not a nested "File ..., line ...: CS_EXIT" text).
	cs::current_process->on_process_exit.add_listener([](void *code) -> bool {
		throw cs::fatal_error("CS_EXIT");
		return true;
	});
	try {
		run_script("using system\n"
		           "function f()\n"
		           "\tsystem.exit(7)\n"
		           "end\n"
		           "fiber.create(f).resume()\n");
	}
	catch (const cs::exception &e) {
		EXPECT_TRUE(e.message() == "CS_EXIT");
		return;
	}
	catch (const cs::fatal_error &e) {
		EXPECT_TRUE(e.message() == "CS_EXIT");
		return;
	}
	throw cs_test::test_failure("expected the CS_EXIT sentinel to escape the fiber");
}

TEST(grandchild_fiber_exit_after_parent_destroyed)
{
	// A fiber A creates fiber B, then A is destroyed (its forked process dies).
	// B must still be able to dispatch exit through the root process: the fork
	// forwarding listener must not capture A's now-dangling process pointer.
	static int captured = -1;
	cs::current_process->on_process_exit.add_listener([](void *code) -> bool {
		captured = *static_cast<int *>(code);
		return true;
	});

	cs::array args;
	args.push_back(cs::var::make<cs::string>("<GRANDCHILD>"));
	auto ctx = cs::create_context(args);
	std::istringstream src(
	    "using system\n"
	    "function inner()\n"
	    "\tfiber.yield()\n"
	    "\tsystem.exit(42)\n"
	    "end\n"
	    "function outer()\n"
	    "\tvar b = fiber.create(inner)\n"
	    "\treturn b\n"
	    "end\n"
	    "var a = fiber.create(outer)\n");
	ctx->instance->compile(src);
	ctx->instance->interpret();

	cs::fiber_t a = ctx->instance->storage.get_var("a").const_val<cs::fiber_t>();
	cs::fiber::resume(a, cs::fiber::schedule_policy::normal);
	cs::fiber_t b = a->return_value().const_val<cs::fiber_t>();
	ctx->instance->storage.get_var("a") = cs::null_pointer; // drop the script reference
	a.reset(); // destroy A; its forked process is kept alive by B's generation chain

	captured = -1;
	cs::fiber::resume(b, cs::fiber::schedule_policy::normal);
	cs::fiber::resume(b, cs::fiber::schedule_policy::normal); // inner calls exit(42)
	EXPECT_TRUE(captured == 42);
}

TEST(fiber_exit_forwards_through_live_parent)
{
	// The generation chain must keep the parent fiber's process alive and
	// forward exit through it, so a listener registered on the parent process
	// fires when a child fiber exits (not flattened to the root).
	static int captured = -1;
	static bool parent_listener_fired = false;
	cs::current_process->on_process_exit.add_listener([](void *code) -> bool {
		captured = *static_cast<int *>(code);
		return true; // swallow
	});

	cs::array args;
	args.push_back(cs::var::make<cs::string>("<CHAIN>"));
	auto ctx = cs::create_context(args);
	std::istringstream src(
	    "using system\n"
	    "function inner()\n"
	    "\tfiber.yield()\n"
	    "\tsystem.exit(42)\n"
	    "end\n"
	    "function outer()\n"
	    "\tvar b = fiber.create(inner)\n"
	    "\tfiber.yield()\n"
	    "\treturn b\n"
	    "end\n"
	    "var a = fiber.create(outer)\n");
	ctx->instance->compile(src);
	ctx->instance->interpret();

	cs::fiber_t a = ctx->instance->storage.get_var("a").const_val<cs::fiber_t>();
	EXPECT_TRUE(a->get_process() != nullptr);
	a->get_process()->on_process_exit.add_listener([](void *) -> bool {
		parent_listener_fired = true;
		return false; // do not swallow: keep forwarding up the chain
	});

	cs::fiber::resume(a, cs::fiber::schedule_policy::normal); // outer creates b, yields
	cs::fiber::resume(a, cs::fiber::schedule_policy::normal); // outer returns b
	cs::fiber_t b = a->return_value().const_val<cs::fiber_t>();
	// Keep `a` (and hence its process) alive while `b` runs.

	captured = -1;
	parent_listener_fired = false;
	cs::fiber::resume(b, cs::fiber::schedule_policy::normal); // inner yields
	cs::fiber::resume(b, cs::fiber::schedule_policy::normal); // inner calls exit(42)
	EXPECT_TRUE(parent_listener_fired);
	EXPECT_TRUE(captured == 42);
}

// =============================================================================
// C1: negative array index keeps auto-growth semantics but must not loop forever
// or read out of bounds (the old unsigned-mixed comparison did).
// a[-4] on a 3-element array prepends one zero and writes index 0.
// =============================================================================

TEST(negative_array_index_auto_grow)
{
	EXPECT_CONTAINS(
	    run_script("using system\n"
	               "var a = {}\n"
	               "a.push_back(1)\n"
	               "a.push_back(2)\n"
	               "a.push_back(3)\n"
	               "a[-4] = 5\n"
	               "foreach x in a\n"
	               "\tsystem.out.print(to_string(x))\n"
	               "end\n"),
	    "5123");
}

TEST(negative_array_index_in_range_unchanged)
{
	EXPECT_CONTAINS(
	    run_script("using system\n"
	               "var a = {}\n"
	               "a.push_back(1)\n"
	               "a.push_back(2)\n"
	               "a.push_back(3)\n"
	               "a[-1] = 9\n"
	               "foreach x in a\n"
	               "\tsystem.out.print(to_string(x))\n"
	               "end\n"),
	    "129");
}

// =============================================================================
// C2: integer division/modulo by zero must throw a catchable lang_error instead
// of crashing the process with SIGFPE.
// =============================================================================

TEST(integer_division_by_zero_catchable)
{
	EXPECT_CONTAINS(
	    run_script("using system\n"
	               "try\n"
	               "\tsystem.out.println(1 / 0)\n"
	               "catch (e)\n"
	               "\tsystem.out.println(\"caught\")\n"
	               "end\n"),
	    "caught");
}

TEST(integer_modulo_by_zero_catchable)
{
	EXPECT_CONTAINS(
	    run_script("using system\n"
	               "try\n"
	               "\tsystem.out.println(1 % 0)\n"
	               "catch (e)\n"
	               "\tsystem.out.println(\"caught\")\n"
	               "end\n"),
	    "caught");
}

// =============================================================================
// C3: hash_set.clear must actually clear (was registered as `empty`).
// =============================================================================

TEST(hash_set_clear_works)
{
	EXPECT_CONTAINS(
	    run_script("using system\n"
	               "var s = new hash_set\n"
	               "s.insert(1)\n"
	               "s.clear()\n"
	               "system.out.println(s.size)\n"),
	    "0");
}

// =============================================================================
// M3: int_pow must not overflow-UB or hang on large/negative exponents; it
// falls back to a float result.
// =============================================================================

TEST(int_pow_large_exponent_no_overflow)
{
	// 2^63 overflows long long; must fall back to float, not wrap to LLONG_MIN.
	EXPECT_CONTAINS(run_script("using system\nsystem.out.println(2 ^ 63)\n"), "9.22337");
}

TEST(int_pow_negative_exponent_no_hang)
{
	// Negative exponent previously looped ~|exp| times; now computed via std::pow.
	EXPECT_CONTAINS(
	    run_script("using system\n"
	               "var e = 0 - 1000000000\n"
	               "system.out.println(2 ^ e)\n"),
	    "0");
}

// =============================================================================
// M4: range with a negative step must iterate downward; step 0 must be rejected.
// =============================================================================

TEST(range_negative_step_descends)
{
	EXPECT_CONTAINS(
	    run_script("using system\n"
	               "foreach i in range(5, 0, -1)\n"
	               "\tsystem.out.print(to_string(i))\n"
	               "end\n"),
	    "54321");
}

TEST(range_zero_step_rejected)
{
	EXPECT_CONTAINS(run_script_expect_throw("using system\nforeach i in range(0, 5, 0)\nend\n"),
	                "Range step cannot be zero");
}

// =============================================================================
// M5: numeric hash must be consistent for equal int/float values (1 == 1.0), so
// a hash_map keyed with 1 can be found via 1.0.
// =============================================================================

TEST(numeric_hash_int_float_consistent)
{
	EXPECT_CONTAINS(
	    run_script("using system\n"
	               "var m = new hash_map\n"
	               "m[1] = \"x\"\n"
	               "system.out.println(m[1.0])\n"),
	    "x");
}

// =============================================================================
// M7: link assignment (:=) must not silently replace a protected/constant slot.
// =============================================================================

TEST(link_assignment_to_constant_rejected)
{
	EXPECT_CONTAINS(
	    run_script_expect_throw("using system\nconstant c = 5\nc := 10\nsystem.out.println(c)\n"),
	    "The variable has been protected");
}

// =============================================================================
// Numeric: INT_MIN % -1 is UB in C++; the remainder is 0.
// =============================================================================

TEST(int_min_mod_minus_one_zero)
{
	EXPECT_CONTAINS(
	    run_script("using system\n"
	               "var x = -9223372036854775807 - 1\n"
	               "system.out.println(x % (-1))\n"),
	    "0");
}

// =============================================================================
// Numeric: INT_MIN / -1 overflows int64 (result 2^63 not representable); must
// return the mathematically correct value as a float, not wrap to INT_MIN.
// =============================================================================

TEST(int_min_div_minus_one_returns_float)
{
	EXPECT_CONTAINS(
	    run_script("using system\n"
	               "var x = -9223372036854775807 - 1\n"
	               "system.out.println(x / (-1))\n"),
	    "9.22337");
}

// =============================================================================
// Numeric: large integer hash_map keys must not collapse. Distinct int64 keys
// (2^53+1 vs 2^53) compare unequal, so they must land in distinct buckets even
// though converting them through a double would round them together.
// =============================================================================

TEST(large_integer_hash_keys_stay_distinct)
{
	EXPECT_CONTAINS(
	    run_script("using system\n"
	               "var m = new hash_map\n"
	               "m[9007199254740993] = \"big\"\n"
	               "m[9007199254740992] = \"small\"\n"
	               "system.out.println(m[9007199254740993])\n"),
	    "big");
}

// =============================================================================
// Numeric: exact int-float ordering. 2^53+1 is representable as an integer but
// not as a double; it must still compare unequal/greater than the double 2^53.
// =============================================================================

TEST(exact_int_float_ordering)
{
	EXPECT_CONTAINS(
	    run_script("using system\n"
	               "var big = 9007199254740993\n"
	               "system.out.println(big == 9007199254740992.0)\n"
	               "system.out.println(big > 9007199254740992.0)\n"),
	    "false\ntrue");
}

// =============================================================================
// Numeric: NaN is unordered. int-vs-NaN must behave like the IEEE float-vs-float
// path (all ordering/equality false, inequality true), not report `>`/`>=`.
// =============================================================================

TEST(numeric_nan_comparisons_unordered)
{
	using cs::numeric;
	numeric nan(std::numeric_limits<cs::numeric_float>::quiet_NaN());
	numeric one(1);
	// int <-> NaN, NaN on either side
	EXPECT_TRUE(!(one > nan));
	EXPECT_TRUE(!(one >= nan));
	EXPECT_TRUE(!(one < nan));
	EXPECT_TRUE(!(one <= nan));
	EXPECT_TRUE(!(one == nan));
	EXPECT_TRUE(one != nan);
	EXPECT_TRUE(!(nan > one));
	EXPECT_TRUE(!(nan >= one));
	EXPECT_TRUE(!(nan < one));
	EXPECT_TRUE(!(nan <= one));
	EXPECT_TRUE(!(nan == one));
	EXPECT_TRUE(nan != one);
	// float-vs-float reference (IEEE)
	EXPECT_TRUE(!(nan > numeric(1.0)));
	EXPECT_TRUE(!(nan >= numeric(1.0)));
	EXPECT_TRUE(nan != numeric(1.0));
}

// =============================================================================
// Numeric: -0.0 and +0.0 (and integer 0) compare equal and must hash alike, so
// a hash_map key stored under one form is found under the others.
// =============================================================================

TEST(negative_zero_hash_consistent)
{
	EXPECT_CONTAINS(
	    run_script("using system\n"
	               "var m = new hash_map\n"
	               "m[-0.0] = \"zero\"\n"
	               "system.out.println(m[0.0])\n"
	               "system.out.println(m[0])\n"),
	    "zero\nzero");
}

// =============================================================================
// C6: script fibers must have isolated value stacks. Two fibers that yield
// mid-function-call and then return must not cross-contaminate return slots.
// =============================================================================

TEST(script_fiber_value_stack_isolation)
{
	EXPECT_CONTAINS(
	    run_script("using system\n"
	               "function fa()\n"
	               "\tfiber.yield()\n"
	               "\treturn 111\n"
	               "end\n"
	               "function fb()\n"
	               "\tfiber.yield()\n"
	               "\treturn 222\n"
	               "end\n"
	               "var f1 = fiber.create(fa)\n"
	               "var f2 = fiber.create(fb)\n"
	               "f1.resume()\n"
	               "f2.resume()\n"
	               "f1.resume()\n"
	               "f2.resume()\n"
	               "system.out.println(f1.return_value() + f2.return_value())\n"),
	    "333");
}

// =============================================================================
// M2: two import/using statements in one block must not clobber each other's
// preprocessing results (shared-method mResult bug).
// =============================================================================

TEST(multi_using_in_one_block)
{
	EXPECT_CONTAINS(
	    run_script("using system\n"
	               "block\n"
	               "\tusing system\n"
	               "\tusing runtime\n"
	               "\tsystem.out.println(\"m2-ok\")\n"
	               "end\n"),
	    "m2-ok");
}

// =============================================================================
// M14: local_time/utc_time with an out-of-range timestamp must throw instead of
// dereferencing the nullptr returned by localtime/gmtime.
// =============================================================================

TEST(time_conversion_out_of_range_throws)
{
	EXPECT_CONTAINS(run_script_expect_throw("using system\nruntime.local_time(9223372036854775807)\n"),
	                "localtime failed");
	EXPECT_CONTAINS(run_script_expect_throw("using system\nruntime.utc_time(9223372036854775807)\n"),
	                "gmtime failed");
}

// =============================================================================
// M10: invalid permission mode strings must throw instead of silently mapping
// to mode 0 (which would strip all permissions).
// =============================================================================

TEST(chmod_invalid_mode_rejected)
{
	EXPECT_CONTAINS(
	    run_script_expect_throw("using system\nsystem.path.chmod(\"no_such_file\", \"garbage\")\n"),
	    "Invalid permission mode");
	EXPECT_CONTAINS(
	    run_script_expect_throw("using system\nsystem.path.chmod(\"no_such_file\", \"888\")\n"),
	    "Invalid permission mode");
}

// =============================================================================
// M12: ctype functions must accept non-ASCII bytes (negative char on signed-char
// platforms) without UB; ASCII case conversion still applies.
// =============================================================================

TEST(ctype_non_ascii_case_conversion)
{
	EXPECT_CONTAINS(
	    run_script("using system\n"
	               "system.out.println(\"Hello中文WORLD\".to_lower())\n"),
	    "hello");
}

// =============================================================================
// L2: parse_number must reject trailing garbage and keep 1e3 as a float 1000.
// =============================================================================

TEST(parse_number_rejects_partial_and_exponent)
{
	EXPECT_CONTAINS(run_script_expect_throw("using system\n\"123abc\".to_number()\n"),
	                "Invalid numeric literal");
	EXPECT_CONTAINS(
	    run_script("using system\n"
	               "system.out.println(to_integer(\"1e3\".to_number()))\n"),
	    "1000");
}

// =============================================================================
// L14: string multiplication overflow and zero must be handled cleanly.
// =============================================================================

TEST(string_mult_zero_and_overflow)
{
	EXPECT_CONTAINS(run_script("using system\nvar s = \"a\" * 0\nsystem.out.println(s.size)\n"), "0");
	EXPECT_CONTAINS(
	    run_script_expect_throw("using system\nvar s = \"aaa\" * 9223372036854775807\n"),
	    "too large");
}

// =============================================================================
// M1: expression optimization must handle function-call argument lists (non-signal
// nodes) without UB, and fold constants correctly.
// =============================================================================

TEST(expression_optimization_non_signal_nodes)
{
	EXPECT_CONTAINS(
	    run_script("using system\n"
	               "function g(a, b)\n"
	               "\treturn a + b\n"
	               "end\n"
	               "system.out.println(g(1, 2) + 3)\n"),
	    "6");
	EXPECT_CONTAINS(run_script("using system\nsystem.out.println({1, 2, 3}[1])\n"), "2");
}

// =============================================================================
// C4: loading a nonexistent/broken extension must fail cleanly, not crash.
// =============================================================================

TEST(extension_load_failure_clean)
{
	EXPECT_THROW(cs::extension("zzz_no_such_extension_file.dll"), cs::runtime_error);
}

// =============================================================================
// M6: csym line relocation must reject line 0 (was an underflow) and accept the
// last line (was rejected by the old >= check).
// =============================================================================

TEST(csym_relocation_bounds)
{
	cs::csym_info info;
	info.file = "test.csp";
	info.map = {3};
	info.codes = {"c1", "c2", "c3", "c4"};
	EXPECT_THROW(cs::exception(0, "f", "c", "m").relocate_to_csym(info), cs::fatal_error);
	cs::exception ok(1, "f", "c", "m");
	ok.relocate_to_csym(info);
}

// =============================================================================
// M15: inserting above a tree root must reparent the old root's parent pointer;
// inserting into an empty tree must not dereference a null old root.
// =============================================================================

TEST(tree_type_root_reparenting)
{
	cs::tree_type<int> t;
	auto a = t.insert_root_left(t.root(), 10);
	EXPECT_TRUE(a.usable());
	EXPECT_TRUE(t.root().usable());
	EXPECT_TRUE(t.root().data() == 10);
	auto b = t.insert_root_left(t.root(), 20);
	EXPECT_TRUE(t.root().data() == 20);
	EXPECT_TRUE(t.root().left().usable());
	EXPECT_TRUE(t.root().left().data() == 10);
	EXPECT_TRUE(t.root().left().root().usable());
	EXPECT_TRUE(t.root().left().root().data() == 20);
}
