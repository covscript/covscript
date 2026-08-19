#include <covscript/covscript.hpp>
#include "test_helpers.hpp"
#include <limits>
#include <sstream>
#include <fstream>
#include <filesystem>

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
	captured = -1;
	run_script("using system\nsystem.exit(3)\n", [](const cs::context_t &ctx)
	{
		ctx->process->on_process_exit.add_listener([](void *code) -> bool
		{
			captured = *static_cast<int *>(code);
			return true; // swallow: never actually exit the unit-test process
		});
	});
	EXPECT_TRUE(captured == 3);
}

// =============================================================================
// system.exit from a fiber must reach the main on_process_exit handler.
// =============================================================================

TEST(system_exit_from_fiber_dispatches_code)
{
	// Mirror the interpreter: record the code on the context's process, throw CS_EXIT.
	static int captured = -1;
	static int main_exit_code = -1;
	captured = -1;
	main_exit_code = -1;
	try
	{
		run_script(
		    "using system\n"
		    "function f()\n"
		    "\tsystem.exit(7)\n"
		    "end\n"
		    "fiber.create(f).resume()\n",
		    [](const cs::context_t &ctx)
		{
			ctx->process->exit_code = -1;
			ctx->process->on_process_exit.add_listener([](void *code) -> bool
			{
				main_exit_code = *static_cast<int *>(code);
				captured = *static_cast<int *>(code);
				throw cs::fatal_error("CS_EXIT");
				return true;
			});
		});
	}
	catch (const cs::exception &)
	{
		// CS_EXIT sentinel escaped the fiber (checked by the next test).
	}
	EXPECT_TRUE(captured == 7);
	EXPECT_TRUE(main_exit_code == 7);
}

TEST(fiber_exit_sentinel_keeps_bare_message)
{
	// The CS_EXIT sentinel must escape the fiber as a located cs::exception
	// whose bare message is exactly "CS_EXIT".
	try
	{
		run_script(
		    "using system\n"
		    "function f()\n"
		    "\tsystem.exit(7)\n"
		    "end\n"
		    "fiber.create(f).resume()\n",
		    [](const cs::context_t &ctx)
		{
			ctx->process->on_process_exit.add_listener([](void *code) -> bool
			{
				throw cs::fatal_error("CS_EXIT");
				return true;
			});
		});
	}
	catch (const cs::exception &e)
	{
		EXPECT_TRUE(e.message() == "CS_EXIT");
		return;
	}
	catch (const cs::fatal_error &e)
	{
		EXPECT_TRUE(e.message() == "CS_EXIT");
		return;
	}
	throw cs_test::test_failure("expected the CS_EXIT sentinel to escape the fiber");
}

TEST(grandchild_fiber_exit_after_parent_destroyed)
{
	// B must still reach the root handler after A's fiber is destroyed.
	static int captured = -1;

	cs::array args;
	args.push_back(cs::var::make<cs::string>("<GRANDCHILD>"));
	auto ctx = cs::create_context(args);
	// Manipulating script fibers from native code requires an active session so
	// current_process is restored (and never dangles after the context dies).
	cs::process_run_scope scope(ctx);
	ctx->process->on_process_exit.add_listener([](void *code) -> bool
	{
		captured = *static_cast<int *>(code);
		return true;
	});
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
	a.reset();                                              // destroy A; its forked process is kept alive by B's generation chain

	captured = -1;
	cs::fiber::resume(b, cs::fiber::schedule_policy::normal);
	cs::fiber::resume(b, cs::fiber::schedule_policy::normal); // inner calls exit(42)
	EXPECT_TRUE(captured == 42);
}

TEST(fiber_exit_forwards_through_live_parent)
{
	// A listener on the live parent's process must fire when the child exits.
	static int captured = -1;
	static bool parent_listener_fired = false;

	cs::array args;
	args.push_back(cs::var::make<cs::string>("<CHAIN>"));
	auto ctx = cs::create_context(args);
	// Session scope: see grandchild_fiber_exit_after_parent_destroyed.
	cs::process_run_scope scope(ctx);
	ctx->process->on_process_exit.add_listener([](void *code) -> bool
	{
		captured = *static_cast<int *>(code);
		return true; // swallow
	});
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
	a->get_process()->on_process_exit.add_listener([](void *) -> bool
	{
		parent_listener_fired = true;
		return false; // keep forwarding
	});

	cs::fiber::resume(a, cs::fiber::schedule_policy::normal); // outer creates b, yields
	cs::fiber::resume(a, cs::fiber::schedule_policy::normal); // outer returns b
	cs::fiber_t b = a->return_value().const_val<cs::fiber_t>();
	// Keep `a` alive so its process outlives `b`'s run.

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
// NaN is unordered: int-vs-NaN must match the IEEE float-vs-float path.
// =============================================================================

TEST(numeric_nan_comparisons_unordered)
{
	using cs::numeric;
	numeric nan(std::numeric_limits<cs::numeric_float>::quiet_NaN());
	numeric one(1);
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
	EXPECT_TRUE(!(nan > numeric(1.0))); // float-vs-float reference
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

// =============================================================================
// GC: compiling a program allocates tokens into the instance's arena; releasing
// the program (next compile or instance teardown) frees them.
// =============================================================================

TEST(gc_program_arena_reclaimed_on_release)
{
	auto ctx = make_context();
	// First compile allocates tokens into the instance's arena.
	{
		std::istringstream in("var a = 1\nvar b = 2\na + b\n");
		ctx->instance->compile(in);
	}
	auto first = ctx->instance->get_current_unit();
	std::weak_ptr<cs::compile_unit> wfirst = first;
	EXPECT_TRUE(first != nullptr);
	EXPECT_TRUE(first->token_count() > 0);
	// Releasing the program drops the instance's reference; once the test's own
	// reference goes away the arena (and its tokens) is freed.
	ctx->instance->release_statements();
	first.reset();
	EXPECT_TRUE(wfirst.expired());
	// A fresh compile allocates a fresh arena.
	std::istringstream in("var c = 3\nc\n");
	ctx->instance->compile(in);
	auto second = ctx->instance->get_current_unit();
	EXPECT_TRUE(second != nullptr);
	EXPECT_TRUE(second != wfirst.lock());
	EXPECT_TRUE(second->token_count() > 0);
}

// =============================================================================
// GC: a function stored in a variable must remain callable after the compiled
// program is released (the function owns its body statements and token arena).
// =============================================================================

TEST(gc_escaped_function_survives_program_release)
{
	// Use a fresh context so the function does not pollute the shared test context.
	cs::array args;
	args.push_back(cs::var::make<cs::string>("<UNIT_TEST>"));
	auto ctx = cs::create_context(args);
	cs::process_run_scope scope(ctx);
	{
		std::istringstream in("function f()\n    return 42\nend\n");
		ctx->instance->compile(in);
		ctx->instance->interpret();
	}
	// Grab the callable into an external var (simulates a retained callable).
	cs::var escaped = ctx->instance->storage.get_var("f");
	EXPECT_TRUE(escaped.usable());
	// Release the compiled program: the statement_function node is freed, but
	// the function (and its body/arena) survive via the callable's owner.
	ctx->instance->release_statements();
	cs::vector args2;
	cs::var ret = escaped.val<cs::callable>().call(args2);
	EXPECT_TRUE(ret.const_val<cs::numeric>() == 42);
}

// =============================================================================
// GC: a recursive lambda's implicit `self` is a non-owning borrow.
// =============================================================================

TEST(self_referencing_lambda_does_not_own_itself)
{
	cs::array args;
	args.push_back(cs::var::make<cs::string>("<UNIT_TEST>"));
	auto ctx = cs::create_context(args);
	run_script_on(ctx, "var f = [](n)->n>1?self(n-1)*n:1\n");
	// `f`'s proxy has one owning reference (the domain); `self` is a borrow, so
	// no reference cycle forms.
	EXPECT_TRUE(ctx->instance->storage.get_var("f").debug_refcount() == 1);
	// Recursion resolves `self` correctly.
	EXPECT_CONTAINS(run_script("using system\nvar g = [](n)->n>1?self(n-1)*n:1\nsystem.out.println(g(5))\n"), "120");
}

// =============================================================================
// GC: a lambda's value lives in the runtime's store, not in a token, so its
// arena is reclaimed when the context (and with it the store) is gone.
// =============================================================================

TEST(lambda_arena_reclaimed_after_context_release)
{
	std::weak_ptr<cs::compile_unit> wunit;
	{
		cs::array args;
		args.push_back(cs::var::make<cs::string>("<LAMBDA_ARENA>"));
		auto ctx = cs::create_context(args);
		{
			std::istringstream in("var f = [](x)->x+1\nf(2)\n");
			ctx->instance->compile(in);
			ctx->instance->interpret();
		}
		wunit = ctx->instance->get_current_unit();
	}
	EXPECT_TRUE(wunit.expired());
}

// =============================================================================
// GC: a lambda nested in a function body is likewise store-owned; its arena is
// reclaimed at teardown.
// =============================================================================

TEST(nested_lambda_arena_reclaimed_after_context_release)
{
	std::weak_ptr<cs::compile_unit> wunit;
	{
		cs::array args;
		args.push_back(cs::var::make<cs::string>("<NESTED_LAMBDA_ARENA>"));
		auto ctx = cs::create_context(args);
		{
			std::istringstream in("function outer()\n    return [](x)->x+1\nend\nouter()\n");
			ctx->instance->compile(in);
			ctx->instance->interpret();
		}
		wunit = ctx->instance->get_current_unit();
	}
	EXPECT_TRUE(wunit.expired());
}

// =============================================================================
// A nested lambda survives release_statements(): its store-owned function keeps
// the escaped outer function's body readable at call time.
// =============================================================================

TEST(nested_lambda_survives_program_release)
{
	cs::array args;
	args.push_back(cs::var::make<cs::string>("<NESTED_LAMBDA_ESCAPE>"));
	auto ctx = cs::create_context(args);
	cs::process_run_scope scope(ctx);
	{
		std::istringstream in("function make()\n    return [](x)->x+1\nend\nvar f = make\n");
		ctx->instance->compile(in);
		ctx->instance->interpret();
	}
	// Grab the outer function before releasing the program (it escapes).
	cs::var make_var = ctx->instance->storage.get_var("f");
	ctx->instance->release_statements();
	cs::vector no_args;
	cs::var inner = make_var.const_val<cs::callable>().call(no_args);
	cs::vector one;
	one.push_back(cs::var::make<cs::numeric>(41));
	cs::var ret = inner.const_val<cs::callable>().call(one);
	EXPECT_TRUE(ret.const_val<cs::numeric>() == 42);
}

// =============================================================================
// A recompile retires the old arena, but a store-owned domain function's m_unit
// keeps it alive until teardown releases the store.
// =============================================================================

TEST(retired_nested_lambda_arena_reclaimed_at_teardown)
{
	std::weak_ptr<cs::compile_unit> wold;
	{
		cs::array args;
		args.push_back(cs::var::make<cs::string>("<RETIRED_NESTED_LAMBDA>"));
		auto ctx = cs::create_context(args);
		{
			std::istringstream in("function make()\n    return [](x)->x+1\nend\nvar f = make\n");
			ctx->instance->compile(in);
			ctx->instance->interpret();
		}
		wold = ctx->instance->get_current_unit();
		// Recompile: the old arena is no longer the current unit but is still
		// pinned by the domain function's m_unit.
		{
			std::istringstream in("var x = 1\n");
			ctx->instance->compile(in);
		}
		EXPECT_TRUE(!wold.expired());
	}
	EXPECT_TRUE(wold.expired());
}

// =============================================================================
// A plain program's arena is reclaimed immediately on release_statements().
// =============================================================================

TEST(plain_arena_reclaimed_on_release)
{
	cs::array args;
	args.push_back(cs::var::make<cs::string>("<PLAIN_ARENA>"));
	auto ctx = cs::create_context(args);
	std::weak_ptr<cs::compile_unit> wunit;
	{
		std::istringstream in("var p = 1\nvar q = 2\np + q\n");
		ctx->instance->compile(in);
		wunit = ctx->instance->get_current_unit();
	}
	ctx->instance->release_statements();
	EXPECT_TRUE(wunit.expired());
}

// =============================================================================
// REPL: a nested lambda defined in one statement keeps working in later
// statements (its store-owned function pins the statement arena).
// =============================================================================

TEST(repl_nested_lambda_survives_across_statements)
{
	cs::array args;
	args.push_back(cs::var::make<cs::string>("<REPL_NESTED>"));
	auto ctx = cs::create_context(args);
	std::ostringstream captured;
	auto *old = std::cout.rdbuf(captured.rdbuf());
	try
	{
		cs::repl r(ctx);
		r.exec("function make()");
		r.exec("    return [](x)->x+1");
		r.exec("end");
		r.exec("var f = make");
		r.exec("var g = f()");
		r.exec("system.out.println(g(41))");
	}
	catch (...)
	{
		std::cout.rdbuf(old);
		throw;
	}
	std::cout.rdbuf(old);
	EXPECT_CONTAINS(captured.str(), "42");
}

// =============================================================================
// A lambda's value lives in the runtime's function store and stays callable
// through both the store and the domain variable.
// =============================================================================

TEST(lambda_function_owned_by_store)
{
	cs::array args;
	args.push_back(cs::var::make<cs::string>("<STORE>"));
	auto ctx = cs::create_context(args);
	cs::process_run_scope scope(ctx);
	run_script_on(ctx, "var f = [](x)->x+1\n");
	cs::var f = ctx->instance->storage.get_var("f");
	cs::vector a;
	a.push_back(cs::var::make<cs::numeric>(41));
	cs::var r = f.const_val<cs::callable>().call(a);
	EXPECT_TRUE(r.const_val<cs::numeric>() == 42);
	cs::vector b;
	b.push_back(cs::var::make<cs::numeric>(1));
	cs::var r2 = ctx->instance->functions.get(0).const_val<cs::callable>().call(b);
	EXPECT_TRUE(r2.const_val<cs::numeric>() == 2);
}

// =============================================================================
// A lambda built via cs::eval's temporary arena must stay callable.
// =============================================================================

TEST(eval_lambda_returns_callable)
{
	cs::array args;
	args.push_back(cs::var::make<cs::string>("<EVAL_NESTED>"));
	auto ctx = cs::create_context(args);
	cs::process_run_scope scope(ctx);
	cs::var f = cs::eval(ctx, "[](x)->x+1");
	cs::vector a;
	a.push_back(cs::var::make<cs::numeric>(41));
	cs::var r = f.const_val<cs::callable>().call(a);
	EXPECT_TRUE(r.const_val<cs::numeric>() == 42);
}

// =============================================================================
// A deep copy of a recursive lambda rebinds `self` to the copy's own proxy, so
// it stays callable after the original is released.
// =============================================================================

TEST(clone_recursive_lambda_rebinds_self)
{
	cs::array args;
	args.push_back(cs::var::make<cs::string>("<CLONE_SELF>"));
	auto ctx = cs::create_context(args);
	cs::process_run_scope scope(ctx);
	run_script_on(ctx, "var f = [](n)->n>1?self(n-1)*n:1\n");
	cs::var f = ctx->instance->storage.get_var("f");
	cs::var f2 = cs::copy(f);
	f = cs::var();
	cs::var r = cs::invoke(f2, cs::var::make<cs::numeric>(5));
	EXPECT_TRUE(r.const_val<cs::numeric>() == 120);
}

// =============================================================================
// Resuming an escaped script fiber after its context is destroyed is rejected
// (weak context handle) instead of dereferencing a dangling pointer.
// =============================================================================

TEST(fiber_resume_rejected_after_context_release)
{
	std::exception_ptr eptr;
	cs::fiber_t f;
	{
		cs::array args;
		args.push_back(cs::var::make<cs::string>("<FIBER_CTX>"));
		auto ctx = cs::create_context(args);
		cs::process_run_scope scope(ctx);
		f = cs::fiber::create(ctx.get(), []() -> cs::var
		{ return cs::var(); });
	}
	try
	{
		cs::fiber::resume(f);
	}
	catch (const cs::lang_error &)
	{
		eptr = std::current_exception();
	}
	EXPECT_TRUE(eptr != nullptr);
}

// =============================================================================
// Calling an escaped function after its defining context is destroyed must be a
// clear error, not a dangling-context use-after-free.
// =============================================================================

TEST(escaped_function_call_rejected_after_context_release)
{
	std::exception_ptr eptr;
	cs::var escaped;
	std::shared_ptr<cs::process_context> proc;
	{
		cs::array args;
		args.push_back(cs::var::make<cs::string>("<ESC_FN>"));
		auto ctx = cs::create_context(args);
		proc = ctx->process;
		cs::process_run_scope scope(ctx);
		run_script_on(ctx, "var f = [](x)->x+1\n");
		escaped = ctx->instance->storage.get_var("f");
	}
	// ctx destroyed here; process stays alive via callable's captured context ref.
	cs::process_run_scope scope(proc.get());
	try
	{
		cs::vector a;
		a.push_back(cs::var::make<cs::numeric>(1));
		escaped.const_val<cs::callable>().call(a);
	}
	catch (const cs::runtime_error &)
	{
		eptr = std::current_exception();
	}
	EXPECT_TRUE(eptr != nullptr);
}

TEST(escaped_function_fiber_create_rejected_after_context_release)
{
	std::exception_ptr eptr;
	cs::var escaped;
	{
		cs::array args;
		args.push_back(cs::var::make<cs::string>("<ESC_FIBER_FN>"));
		auto ctx = cs::create_context(args);
		run_script_on(ctx, "function f()\n\treturn 1\nend\n");
		escaped = ctx->instance->storage.get_var("f");
	}
	try
	{
		cs::array args;
		args.push_back(cs::var::make<cs::string>("<ESC_FIBER_CALL>"));
		auto ctx = cs::create_context(args);
		ctx->instance->storage.add_var("f", escaped);
		run_script_on(ctx, "var fiber_obj = fiber.create(f)\n");
	}
	catch (...)
	{
		eptr = std::current_exception();
	}
	EXPECT_TRUE(eptr != nullptr);
}

// =============================================================================
// Escaped objects require their defining context to stay alive (the type_node
// lives in the process, member names live in the token arena). A structure
// escaped from its scope is fully usable while the context is held.
// =============================================================================

TEST(escaped_structure_usable_while_context_alive)
{
	cs::var escaped;
	cs::context_t ctx;
	{
		cs::array args;
		args.push_back(cs::var::make<cs::string>("<UNIT_TEST>"));
		ctx = cs::create_context(args);
		run_script_on(ctx, "class foo\n    var x = 42\nend\nvar a = new foo\n");
		escaped = ctx->instance->storage.get_var("a");
	}
	// The context is still held, so the type identity node and members are valid.
	EXPECT_TRUE(escaped.val<cs::structure>().get_id().node != nullptr);
	EXPECT_TRUE(escaped.val<cs::structure>().get_id().node->name == "foo");
	EXPECT_TRUE(escaped.val<cs::structure>().get_var("x").const_val<cs::numeric>() == 42);
}

// =============================================================================
// The structure pins its owning process, so its type identity node and member
// data stay valid even after the defining context is destroyed. Only invoking
// a method (a script function with a raw context back-ref) would throw.
// =============================================================================

TEST(escaped_structure_data_usable_after_context_death)
{
	cs::var escaped;
	{
		cs::array args;
		args.push_back(cs::var::make<cs::string>("<UNIT_TEST>"));
		auto ctx = cs::create_context(args);
		run_script_on(ctx, "class foo\n    var x = 42\nend\nvar a = new foo\n");
		escaped = ctx->instance->storage.get_var("a");
	}
	// The context is gone; the pinned process still owns the type node and the
	// member domain is self-contained, so data access remains valid.
	EXPECT_TRUE(escaped.val<cs::structure>().get_id().node != nullptr);
	EXPECT_TRUE(escaped.val<cs::structure>().get_id().node->name == "foo");
	EXPECT_TRUE(escaped.val<cs::structure>().get_var("x").const_val<cs::numeric>() == 42);
}

// =============================================================================
// Process ownership: every context owns its own process; a second independent
// instance cannot start while one is active on the same thread.
// =============================================================================

TEST(second_independent_instance_rejected_while_one_active)
{
	cs::array args;
	args.push_back(cs::var::make<cs::string>("<EXCLUSIVE>"));
	auto ctx = cs::create_context(args);
	cs::process_run_scope scope(ctx);
	bool threw = false;
	try
	{
		cs::array args2;
		args2.push_back(cs::var::make<cs::string>("<EXCLUSIVE2>"));
		auto ctx2 = cs::create_context(args2);
		(void) ctx2;
	}
	catch (const cs::fatal_error &e)
	{
		threw = std::string(e.what()).find("already running") != std::string::npos;
	}
	EXPECT_TRUE(threw);
}

// =============================================================================
// Ownership refactor: releasing the last external reference to a context must
// reclaim the context, instance, process and compiler (no back-reference cycle
// keeps them alive).
// =============================================================================

TEST(context_reclaimed_on_release)
{
	std::weak_ptr<cs::context_type> wctx;
	std::weak_ptr<cs::instance_type> winst;
	std::weak_ptr<cs::process_context> wproc;
	std::weak_ptr<cs::compiler_type> wcomp;
	{
		cs::array args;
		args.push_back(cs::var::make<cs::string>("<RECLAIM>"));
		auto ctx = cs::create_context(args);
		wctx = ctx;
		winst = ctx->instance;
		wproc = ctx->process;
		wcomp = ctx->compiler;
	}
	EXPECT_TRUE(wctx.expired());
	EXPECT_TRUE(winst.expired());
	EXPECT_TRUE(wproc.expired());
	EXPECT_TRUE(wcomp.expired());
}

// =============================================================================
// A pending script fiber stored in the context's own storage must not prevent
// the context from being reclaimed (the fiber holds a weak context reference).
// =============================================================================

TEST(global_pending_fiber_does_not_prevent_context_release)
{
	std::weak_ptr<cs::context_type> wctx;
	{
		cs::array args;
		args.push_back(cs::var::make<cs::string>("<FIBER_CYCLE>"));
		auto ctx = cs::create_context(args);
		wctx = ctx;
		cs::process_run_scope scope(ctx);
		run_script_on(ctx, "function noop()\nend\nvar f = fiber.create(noop)\n");
	}
	EXPECT_TRUE(wctx.expired());
}

TEST(module_import_nests_on_same_process)
{
	cs::array args;
	args.push_back(cs::var::make<cs::string>("<IMPORT_NEST>"));
	auto ctx = cs::create_context(args);
	cs::process_run_scope scope(ctx);
	// Write a small module into a temp dir and import it; the nested compile/
	// interpret (subcontext shares the parent's process) must be transparent.
	auto dir = std::filesystem::temp_directory_path() / "cs_import_nest";
	std::filesystem::create_directories(dir);
	{
		std::ofstream f(dir / "nestmod.csp");
		f << "package nestmod\n";
	}
	ctx->process->import_path += cs::path_delimiter + dir.string();
	std::istringstream in("using system\nimport nestmod\n");
	ctx->instance->compile(in);
	ctx->instance->interpret();
	std::filesystem::remove_all(dir);
}

// =============================================================================
// Module ownership: a module's subcontext is owned by the importing context's
// subcontext pool, and module functions reference it non-owningly, so the
// module namespace does not form an ownership cycle back to the subcontext.
// =============================================================================

TEST(module_subcontext_owned_by_pool)
{
	cs::array args;
	args.push_back(cs::var::make<cs::string>("<MODULE_POOL>"));
	auto ctx = cs::create_context(args);
	cs::process_run_scope scope(ctx);
	auto dir = std::filesystem::temp_directory_path() / "cs_module_pool";
	std::filesystem::create_directories(dir);
	{
		std::ofstream f(dir / "poolmod.csp");
		f << "package poolmod\nfunction add(a, b)\n    return a + b\nend\n";
	}
	ctx->process->import_path += cs::path_delimiter + dir.string();
	std::istringstream in("using system\nimport poolmod\n");
	ctx->instance->compile(in);
	ctx->instance->interpret();
	// The imported module's subcontext is pinned in the pool (its sole external
	// owner; module functions reference it non-owningly). Note the subcontext's
	// own "context" builtin still self-references it — that C3 cycle is part of
	// the broader ownership refactor, not the module cycle fixed here.
	EXPECT_TRUE(ctx->subcontexts.size() == 1);
	std::filesystem::remove_all(dir);
}

// =============================================================================
// Multiple independent contexts can coexist: each owns a distinct process and
// its own storage; they run sequentially without interfering.
// =============================================================================

TEST(multiple_contexts_coexist)
{
	cs::array args_a, args_b, args_c;
	args_a.push_back(cs::var::make<cs::string>("<COEXIST_A>"));
	args_b.push_back(cs::var::make<cs::string>("<COEXIST_B>"));
	args_c.push_back(cs::var::make<cs::string>("<COEXIST_C>"));
	auto ctx_a = cs::create_context(args_a);
	auto ctx_b = cs::create_context(args_b);
	// Distinct processes per context.
	EXPECT_TRUE(ctx_a->process != nullptr);
	EXPECT_TRUE(ctx_b->process != nullptr);
	EXPECT_TRUE(ctx_a->process != ctx_b->process);
	// Both run sequentially, each on its own process.
	EXPECT_TRUE(run_script_on(ctx_a, "using system\nvar shared = 100\nsystem.out.println(1)\n") == "1\n");
	EXPECT_TRUE(run_script_on(ctx_b, "using system\nsystem.out.println(2)\n") == "2\n");
	// Storage is per context: a fresh context must not see a variable defined in A.
	auto ctx_c = cs::create_context(args_c);
	bool c_sees_a = true;
	try
	{
		run_script_on(ctx_c, "using system\nsystem.out.println(shared)\n");
	}
	catch (const std::exception &)
	{
		c_sees_a = false;
	}
	EXPECT_TRUE(!c_sees_a);
}

// =============================================================================
// Destroying one context must not corrupt others that coexist with it.
// =============================================================================

TEST(context_cross_destruction_isolation)
{
	cs::array args_a, args_b, args_c, args_d;
	args_a.push_back(cs::var::make<cs::string>("<DESTROY_A>"));
	args_b.push_back(cs::var::make<cs::string>("<DESTROY_B>"));
	args_c.push_back(cs::var::make<cs::string>("<DESTROY_C>"));
	args_d.push_back(cs::var::make<cs::string>("<DESTROY_D>"));
	auto ctx_a = cs::create_context(args_a);
	auto ctx_b = cs::create_context(args_b);
	// A defines a function and a variable; B stays untouched.
	run_script_on(ctx_a, "using system\nfunction f()\n\treturn 42\nend\nvar a = 1\n");
	// Destroy A: its process and program are freed.
	ctx_a.reset();
	// B is unaffected and still runs.
	EXPECT_TRUE(run_script_on(ctx_b, "using system\nsystem.out.println(7)\n") == "7\n");
	// A new context created after A's destruction also works.
	auto ctx_c = cs::create_context(args_c);
	EXPECT_TRUE(run_script_on(ctx_c, "using system\nsystem.out.println(9)\n") == "9\n");
	// Destroy B as well; a fresh context is still fine.
	ctx_b.reset();
	auto ctx_d = cs::create_context(args_d);
	EXPECT_TRUE(run_script_on(ctx_d, "using system\nsystem.out.println(5)\n") == "5\n");
}

// =============================================================================
// An async future runs on a std::thread with no thread_local current_process;
// it must carry the process active at future.create so script code there sees
// the owning context's process (not a null dereference).
// =============================================================================

TEST(async_future_carries_the_owning_process)
{
	cs::array args;
	args.push_back(cs::var::make<cs::string>("<ASYNC_PROC>"));
	auto ctx = cs::create_context(args);
	// A distinctive marker on the context's own process; runtime.get_import_path
	// reads it via current_process on the async thread.
	ctx->process->import_path = "<ASYNC_PROC_MARK>";
	std::ostringstream captured;
	auto *old = std::cout.rdbuf(captured.rdbuf());
	try
	{
		std::istringstream in(
		    "using system\n"
		    "var p = future.create(runtime.get_import_path)\n"
		    "system.out.println(p.get())\n");
		ctx->instance->compile(in);
		ctx->instance->interpret();
	}
	catch (...)
	{
		std::cout.rdbuf(old);
		throw;
	}
	std::cout.rdbuf(old);
	EXPECT_TRUE(captured.str() == "<ASYNC_PROC_MARK>\n");
}

// =============================================================================
// A subcontext shares the parent's process but owns its own storage — the
// pattern extension DLLs use to run independent script code while the host
// is active.
// =============================================================================
TEST(subcontext_isolation_from_extension)
{
	cs::array args;
	args.push_back(cs::var::make<cs::string>("<HOST>"));
	auto host = cs::create_context(args);
	cs::process_run_scope scope(host);

	// Host defines a variable.
	run_script_on(host, "var host_var = 100\n");

	// Extension-like code: create subcontext, compile and run independent code.
	auto ext = cs::create_subcontext(host.get());
	run_script_on(ext, "var ext_var = 42\nfunction ext_fn()\n    return ext_var\nend\n");

	// Subcontext sees its own definitions.
	cs::var ext_val = ext->instance->storage.get_var("ext_var");
	EXPECT_TRUE(ext_val.const_val<cs::numeric>() == 42);

	// Host does NOT see subcontext's definitions.
	bool host_has_ext_var = false;
	try
	{
		host->instance->storage.get_var("ext_var");
	}
	catch (...)
	{
		host_has_ext_var = true;
	}
	EXPECT_TRUE(host_has_ext_var);

	// Subcontext does NOT see host's definitions.
	bool ext_has_host_var = false;
	try
	{
		ext->instance->storage.get_var("host_var");
	}
	catch (...)
	{
		ext_has_host_var = true;
	}
	EXPECT_TRUE(ext_has_host_var);

	// Call a function defined in the subcontext.
	cs::vector fn_args;
	cs::var ret = ext->instance->storage.get_var("ext_fn").const_val<cs::callable>().call(fn_args);
	EXPECT_TRUE(ret.const_val<cs::numeric>() == 42);

	// Destroy subcontext; host remains functional.
	ext.reset();
	run_script_on(host, "host_var = host_var + 1\n");
	cs::var updated = host->instance->storage.get_var("host_var");
	EXPECT_TRUE(updated.const_val<cs::numeric>() == 101);
}
