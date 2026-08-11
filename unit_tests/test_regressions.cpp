#include <covscript/covscript.hpp>
#include "test_helpers.hpp"
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
