#include <covscript/covscript.hpp>
#include "test_helpers.hpp"
#include <sstream>
#include <climits>
#include <cstdio>
#include <fstream>

// =============================================================================
// Regression tests guarding the fixes from the full-project audit (2026-08).
//
// Each test reproduces a real defect that was confirmed with a minimal script,
// then fixed; it exists so the fix cannot silently regress. See
// audit/report.md for the full numbered list (F01-F38).
// =============================================================================

namespace {

// Run a full script in a fresh context, capturing system.out output. Any
// exception propagates to the caller.
std::string run_script(const std::string &src)
{
	cs::array args;
	args.push_back(cs::var::make<cs::string>("<AUDIT_TEST>"));
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

// Run a script on an existing context, capturing system.out output.
std::string run_script_on(const cs::context_t &ctx, const std::string &src)
{
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
// F01: runtime.argument_count crashed (0xC0000005) on callables whose target is
// a plain function pointer (future.create, fiber.create, ...) because it
// assumed every non-function_ptr target was a cni.
// =============================================================================
TEST(argument_count_native_function_pointer)
{
	EXPECT_CONTAINS(run_script_expect_throw(
	                    "using system\n"
	                    "system.out.println(runtime.argument_count(future.create))\n"),
	                "not a function");
}

TEST(argument_count_cni_ok)
{
	EXPECT_TRUE(run_script("using system\nsystem.out.println(runtime.argument_count(system.exit))\n") == "1\n");
}

// =============================================================================
// F03: && / || short-circuiting (verified correct; guarded here).
// A false left operand must not evaluate the right side.
// =============================================================================
TEST(and_short_circuits_side_effect)
{
	const std::string out = run_script(
	    "using system\n"
	    "function f()\n"
	    "\tsystem.out.println(\"side\")\n"
	    "\treturn 1\n"
	    "end\n"
	    "var a = false\n"
	    "var x = a && f()\n"
	    "var y = true || f()\n"
	    "system.out.println(\"done\")\n");
	EXPECT_TRUE(out.find("side") == std::string::npos);
	EXPECT_CONTAINS(out, "done");
}

TEST(or_short_circuits_division_by_zero)
{
	// 1/0 raises "Integer division by zero"; short-circuiting must skip it.
	const std::string out = run_script(
	    "using system\n"
	    "var x = true || (1/0 == 0)\n"
	    "var y = false && (1/0 == 0)\n"
	    "system.out.println(\"survived\")\n");
	EXPECT_CONTAINS(out, "survived");
}

// =============================================================================
// F02: a SIGINT with no registered listener dereferenced a null int* through
// the default (exit) handler. It now has a dedicated default that ignores it.
// =============================================================================
TEST(sigint_without_listener_ignored)
{
	cs::array args;
	args.push_back(cs::var::make<cs::string>("<AUDIT_TEST>"));
	auto ctx = cs::create_context(args);
	cs::global_signals.raise_sigint();
	// The loop runs poll_event(), which consumes the pending SIGINT.
	const std::string out = run_script_on(
	    ctx,
	    "var i = 0\n"
	    "while i < 100\n"
	    "\ti = i + 1\n"
	    "end\n"
	    "system.out.println(i)\n");
	EXPECT_CONTAINS(out, "100");
}

// =============================================================================
// F21: a structure's finalizer used to run with a null current_process when the
// instance was being torn down (its body referenced system.out, which needs a
// live process). structure now pins its owning process and activates it while
// running finalize. Guard the normal (runtime) path here.
// =============================================================================
TEST(finalize_runs_with_live_process)
{
	const std::string out = run_script(
	    "using system\n"
	    "class foo\n"
	    "    function finalize()\n"
	    "        system.out.println(\"bye\")\n"
	    "    end\n"
	    "end\n"
	    "block\n"
	    "    var g = new foo\n"
	    "end\n");
	EXPECT_CONTAINS(out, "bye");
}

TEST(finalize_runs_on_context_release)
{
	// Releasing a create_context instance directly (no bootstrap/repl) must run
	// structure finalizers while the runtime is still usable.
	cs::array args;
	args.push_back(cs::var::make<cs::string>("<AUDIT_TEST>"));
	std::string out;
	{
		std::ostringstream captured;
		auto *old = std::cout.rdbuf(captured.rdbuf());
		try {
			auto ctx = cs::create_context(args);
			{
				std::istringstream in("using system\n"
				                      "class foo\n"
				                      "    function finalize()\n"
				                      "        system.out.println(\"bye-on-release\")\n"
				                      "    end\n"
				                      "end\n"
				                      "var g = new foo\n");
				ctx->instance->compile(in);
				ctx->instance->interpret();
			}
			ctx.reset();
		}
		catch (...) {
			std::cout.rdbuf(old);
			throw;
		}
		std::cout.rdbuf(old);
		out = captured.str();
	}
	EXPECT_CONTAINS(out, "bye-on-release");
}

// =============================================================================
// F04: a failed nested block leaked the statements already generated into its
// local deque (compile-level cleanup only handles the top level). Block methods
// now clean their partial body on failure; the context must stay fully usable.
// =============================================================================
TEST(nested_block_compile_failure_then_recover)
{
	cs::array args;
	args.push_back(cs::var::make<cs::string>("<AUDIT_TEST>"));
	auto ctx = cs::create_context(args);
	// function body: a valid var statement then an invalid break -> compile fails
	// partway through the block translate.
	try {
		std::istringstream in("using system\nfunction f()\n\tvar x = 1\n\tbreak\nend\n");
		ctx->instance->compile(in);
	}
	catch (...) {
	}
	// The same context must still compile and run cleanly afterwards.
	run_script_on(ctx, "var z = 6 * 7\n");
	EXPECT_TRUE(ctx->instance->storage.get_var("z").const_val<cs::numeric>() == 42);
}

// =============================================================================
// F05: failed compilations leaked lambdas (and their token arena) into the
// runtime function_store forever. compile() now rolls the store back on error.
// =============================================================================
TEST(compile_failure_rolls_back_function_store)
{
	cs::array args;
	args.push_back(cs::var::make<cs::string>("<AUDIT_TEST>"));
	auto ctx = cs::create_context(args);
	{
		std::istringstream in("using system\nvar f = []()->1\n");
		ctx->instance->compile(in);
		ctx->instance->interpret();
	}
	const std::size_t after_ok = ctx->instance->functions.size();
	EXPECT_TRUE(after_ok >= 1);
	try {
		std::istringstream in("var g = []()->2\nbreak\n");
		ctx->instance->compile(in);
	}
	catch (...) {
	}
	EXPECT_TRUE(ctx->instance->functions.size() == after_ok);
}

// =============================================================================
// F06: numeric::as_integer() was an undefined float->int cast for out-of-range
// values (to_integer(10^300) returned a garbage LLONG_MIN). Now it saturates.
// =============================================================================
TEST(to_integer_saturates_huge_float)
{
	EXPECT_TRUE(run_script("using system\nsystem.out.println(to_integer(10^300))\n") ==
	            "9223372036854775807\n");
}

TEST(to_integer_saturates_huge_negative_float)
{
	EXPECT_TRUE(run_script("using system\nsystem.out.println(to_integer(0-(10^300)))\n") ==
	            "-9223372036854775808\n");
}

TEST(to_integer_fraction_truncates)
{
	EXPECT_TRUE(run_script("using system\nsystem.out.println(to_integer(3.7))\n") == "3\n");
}

// =============================================================================
// F10: to_string<cs::numeric> dereferenced a null current_process (crashed
// 0xC0000005) when formatting outside any process scope. It now falls back to
// the default precision.
// =============================================================================
TEST(numeric_to_string_without_process)
{
	cs::process_context *saved = cs::current_process;
	cs::current_process = nullptr;
	cs::numeric n(3.14);
	cs::var v = cs::var::make<cs::numeric>(n);
	std::string s;
	try {
		s = v.to_string().extract();
	}
	catch (...) {
	}
	cs::current_process = saved;
	EXPECT_TRUE(!s.empty());
}

// =============================================================================
// F15: a re-entrant repl::exec() (from a CNI function while a script function
// is on the stack) used to clear the value stack, corrupting the outer call
// frames (0xC0000005). The clear now only runs when the stack is idle.
// =============================================================================
TEST(reentrant_repl_preserves_outer_frames)
{
	cs::array args;
	args.push_back(cs::var::make<cs::string>("<AUDIT_TEST>"));
	auto ctx = cs::create_context(args);
	auto repl = std::make_shared<cs::repl>(ctx);
	// Capture the repl weakly: storing a callable that strongly captures the
	// repl (which strongly holds ctx) inside ctx's own storage would form a
	// ctx -> callable -> repl -> ctx cycle that never tears down.
	std::weak_ptr<cs::repl> weak_repl = repl;
	ctx->instance->storage.add_var("run_repl",
	    cs::var::make<cs::callable>(cs::callable([weak_repl](cs::vector &) -> cs::var {
		    weak_repl.lock()->exec("var nested = 1\n");
		    return cs::var::make<cs::numeric>(7);
	    })));
	EXPECT_CONTAINS(run_script_on(ctx,
	                    "using system\n"
	                    "function outer()\n"
	                    "\tvar before = run_repl()\n"
	                    "\treturn before + 1\n"
	                    "end\n"
	                    "system.out.println(outer())\n"),
	                "8");
}

// =============================================================================
// F25: an extension DLL binds its own thread-local current_process only while
// it initializes, then restores it. The extension must still load, call, and
// raise catchable errors across the module boundary. (my_ext.cse is copied next
// to the test binary by unit_tests/CMakeLists.txt.)
// =============================================================================
TEST(extension_load_call_and_error)
{
	cs::array args;
	args.push_back(cs::var::make<cs::string>("<AUDIT_TEST>"));
	auto ctx = cs::create_context(args);
	cs::process_run_scope scope(ctx);
	cs::extension ext(CS_MY_EXT_PATH);
	// Cross-module call of a plain CNI function.
	cs::vector a;
	a.push_back(cs::var::make<cs::numeric>(1));
	cs::var r = ext.get_var("test").val<cs::callable>().call(a);
	EXPECT_TRUE(r.const_val<cs::numeric>() == 2);
	// While a process is active, the extension's current_process must follow the
	// host's current-thread process (via the injected accessor), not be null.
	cs::vector s;
	s.push_back(cs::var::make<cs::numeric>(0));
	cs::var st = ext.get_var("proc_state").val<cs::callable>().call(s);
	EXPECT_TRUE(st.const_val<cs::numeric>() == 1);
	// A CNI function that throws a lang_error must surface as a catchable error
	// (the DLL's current_process is null after init, so its exception handling
	// falls back to the default handler instead of dereferencing a dangling one).
	bool threw = false;
	try {
		cs::vector b;
		b.push_back(cs::var::make<cs::string>("0xZZ"));
		ext.get_var("hex_literal").val<cs::callable>().call(b);
	}
	catch (const cs::exception &) {
		threw = true;
	}
	catch (const cs::lang_error &) {
		threw = true;
	}
	catch (const std::exception &) {
		threw = true;
	}
	EXPECT_TRUE(threw);
}

// =============================================================================
// F14: a UTF-8 BOM at the start of a script used to fail lexing with
// "Unknown character". The lexer now skips U+FEFF at token boundaries.
// =============================================================================
TEST(utf8_bom_skipped)
{
	EXPECT_CONTAINS(run_script("\xEF\xBB\xBFusing system\nsystem.out.println(1)\n"), "1");
}

// =============================================================================
// F06b: array auto-growth with a huge index used to spin forever (bounded
// allocation). Now it rejects the out-of-range index.
// =============================================================================
TEST(array_huge_index_rejected)
{
	EXPECT_CONTAINS(run_script_expect_throw(
	                    "using system\n"
	                    "var arr = {}\n"
	                    "arr[1] = 10\n"
	                    "var idx = 10^300\n"
	                    "system.out.println(arr[idx])\n"),
	                "Index out of range");
}

TEST(array_huge_negative_index_rejected)
{
	EXPECT_CONTAINS(run_script_expect_throw(
	                    "using system\n"
	                    "var arr = {}\n"
	                    "var idx = 0-(10^300)\n"
	                    "arr[idx] = 1\n"),
	                "Index out of range");
}

TEST(array_auto_growth_within_cap_ok)
{
	// Small auto-growth is still supported.
	EXPECT_TRUE(run_script(
	                "using system\n"
	                "var arr = {}\n"
	                "arr[5] = 42\n"
	                "system.out.println(arr.size)\n"
	                "system.out.println(arr[5])\n") ==
	            "6\n42\n");
}

// =============================================================================
// F09: range_iterator::operator++ overflowed on huge integer ranges and the
// wrapped index never satisfied the end condition (infinite loop). Now the
// iterator clamps instead of wrapping.
// =============================================================================
TEST(range_ascending_overflow_terminates)
{
	EXPECT_TRUE(run_script(
	                "using system\n"
	                "var count = 0\n"
	                "foreach i in range(9223372036854775806, 9223372036854775807, 2)\n"
	                "\tcount = count + 1\n"
	                "end\n"
	                "system.out.println(count)\n") ==
	            "1\n");
}

TEST(range_descending_overflow_terminates)
{
	EXPECT_TRUE(run_script(
	                "using system\n"
	                "var count = 0\n"
	                "foreach i in range(0-(9223372036854775807), 0-(9223372036854775806), 2)\n"
	                "\tcount = count + 1\n"
	                "end\n"
	                "system.out.println(count)\n") ==
	            "1\n");
}

// =============================================================================
// F18: gbk::wide2local must treat GBK/1 code points (0xA1A1-0xA9FE, the symbol
// region holding all full-width Chinese punctuation) as double-byte. The first
// fix covered only the identifier ranges (GBK/2-5) and dropped the high byte of
// GBK/1 code points, corrupting string literals such as "," (0xA3AC). The
// local->wide->local round trip must be the identity.
// =============================================================================
TEST(gbk_wide2local_symbol_roundtrip)
{
	cs::codecvt::gbk cvt;
	std::string local;
	local.push_back(static_cast<char>(0xA3)); // full-width comma (GBK/1)
	local.push_back(static_cast<char>(0xAC));
	local.push_back(static_cast<char>(0xA1)); // full-width period (GBK/1)
	local.push_back(static_cast<char>(0xA3));
	local.push_back(static_cast<char>(0xB0)); // first GBK/2 hanzi
	local.push_back(static_cast<char>(0xA1));
	local.push_back('A');                     // ASCII passthrough
	auto wide = cvt.local2wide(std::string_view(local));
	EXPECT_TRUE(cvt.wide2local(wide) == local);
}

// =============================================================================
// F27: extension::truncate computed the magnitude with std::abs(n), which is UB
// for INT_MIN (|INT_MIN| is not representable as int). The magnitude is now
// computed via a widened long long. Guard: INT_MIN keeps its sign and is
// actually truncated (the UB path yielded the untouched/garbage value).
// =============================================================================
TEST(truncate_int_min_no_ub)
{
	int r = cs::extension::truncate(INT_MIN, 4);
	EXPECT_TRUE(r < 0);       // sign preserved
	EXPECT_TRUE(r > INT_MIN); // magnitude reduced, not the raw/garbage INT_MIN
	// Normal (ABI-style) truncation is unaffected: keep the first 4 digits.
	EXPECT_TRUE(cs::extension::truncate(260805, 4) == 2608);
}

// =============================================================================
// Constant folding: `case a[0]` where a is a constant array must fold. The
// optimizer refuses to fold a value holding a script function into a token_value
// (it would recreate an arena<->function cycle), so a module namespace that
// defines a function stays an id at 'using' time; method_involve must still
// involve it from the storage so its constants reach compile-time folding.
// =============================================================================
TEST(case_label_folds_local_constant_array_index)
{
	EXPECT_CONTAINS(run_script(
	                    "using system\n"
	                    "constant a = {1, 2, 3}\n"
	                    "switch 1\n"
	                    "\tcase a[0]\n"
	                    "\t\tsystem.out.println(\"hit\")\n"
	                    "\tend\n"
	                    "end\n"),
	                "hit");
}

TEST(case_label_folds_imported_constant_array_from_module_with_function)
{
	const std::string module_path = "_audit_fold_mod.csp";
	std::ofstream modf(module_path);
	modf << "package _audit_fold_mod\n"
	        "constant a = {1, 2, 3}\n"
	        "function f()\n"
	        "end\n";
	modf.close();
	try {
		EXPECT_CONTAINS(run_script(
		                    "import _audit_fold_mod\n"
		                    "using _audit_fold_mod\n"
		                    "switch 1\n"
		                    "\tcase a[0]\n"
		                    "\t\tsystem.out.println(\"hit\")\n"
		                    "\tend\n"
		                    "end\n"),
		                "hit");
	}
	catch (...) {
		std::remove(module_path.c_str());
		throw;
	}
	std::remove(module_path.c_str());
}

TEST(case_label_rejects_runtime_array_index)
{
	// A runtime variable must not be folded into a case label constant.
	EXPECT_TRUE(run_script_expect_throw(
	                "using system\n"
	                "var b = {1, 2, 3}\n"
	                "switch 1\n"
	                "\tcase b[0]\n"
	                "\t\tsystem.out.println(\"hit\")\n"
	                "\tend\n"
	                "end\n") != "");
}

TEST(case_label_rejects_constant_containing_callable)
{
	// Folding a constant that holds a script function would recreate the
	// arena<->function cycle, so it must still be rejected.
	EXPECT_TRUE(run_script_expect_throw(
	                "using system\n"
	                "function f()\n"
	                "end\n"
	                "constant arr = {f}\n"
	                "switch 1\n"
	                "\tcase arr[0]\n"
	                "\t\tsystem.out.println(\"hit\")\n"
	                "\tend\n"
	                "end\n") != "");
}
