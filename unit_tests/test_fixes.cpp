#include <covscript/covscript.hpp>
#include "test_helpers.hpp"
#include <sstream>
#include <climits>
#include <cstdio>
#include <fstream>
#include <filesystem>
#include <thread>
#include <atomic>

// =============================================================================
// Regression tests guarding the fixes from the full-project audit (2026-08).
//
// Each test reproduces a real defect that was confirmed with a minimal script,
// then fixed; it exists so the fix cannot silently regress. See
// audit/report.md for the full numbered list (F01-F38).
// =============================================================================

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
		try
		{
			auto ctx = cs::create_context(args);
			{
				std::istringstream in(
				    "using system\n"
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
		catch (...)
		{
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
	try
	{
		std::istringstream in("using system\nfunction f()\n\tvar x = 1\n\tbreak\nend\n");
		ctx->instance->compile(in);
	}
	catch (...)
	{
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
	try
	{
		std::istringstream in("var g = []()->2\nbreak\n");
		ctx->instance->compile(in);
	}
	catch (...)
	{
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
	try
	{
		s = v.to_string().extract();
	}
	catch (...)
	{
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
	                               cs::var::make<cs::callable>(cs::callable([weak_repl](cs::vector &) -> cs::var
	{
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
	try
	{
		cs::vector b;
		b.push_back(cs::var::make<cs::string>("0xZZ"));
		ext.get_var("hex_literal").val<cs::callable>().call(b);
	}
	catch (const cs::exception &)
	{
		threw = true;
	}
	catch (const cs::lang_error &)
	{
		threw = true;
	}
	catch (const std::exception &)
	{
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
	local.push_back('A'); // ASCII passthrough
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
	EXPECT_TRUE(cs::extension::truncate(260901, 4) == 2609);
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
	try
	{
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
	catch (...)
	{
		std::remove(module_path.c_str());
		throw;
	}
	std::remove(module_path.c_str());
}

TEST(case_label_folds_imported_namespace_constant_without_using)
{
	const std::string module_path = "_audit_ns_const.csp";
	std::ofstream modf(module_path);
	modf << "package _audit_ns_const\n"
	        "namespace ns\n"
	        "    constant val = 1\n"
	        "end\n";
	modf.close();
	try
	{
		EXPECT_CONTAINS(run_script(
		                    "import _audit_ns_const\n"
		                    "var x = 1\n"
		                    "switch x\n"
		                    "\tcase _audit_ns_const.ns.val\n"
		                    "\t\tsystem.out.println(\"matched\")\n"
		                    "\tend\n"
		                    "end\n"),
		                "matched");
	}
	catch (...)
	{
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

TEST(constant_containing_callable_rejected)
{
	// Constants holding script functions can't live in the token arena
	// (arena<->function cycle); the declaration itself is rejected.
	EXPECT_CONTAINS(run_script_expect_throw(
	                    "function f()\n"
	                    "end\n"
	                    "constant arr = {f}\n"),
	                "must be initialized with a constant value");
}

TEST(case_label_containing_callable_rejected)
{
	// A case label folding to a value containing a script function is rejected,
	// independent of any constant declaration.
	EXPECT_CONTAINS(run_script_expect_throw(
	                    "using system\n"
	                    "function f()\n"
	                    "end\n"
	                    "switch 1\n"
	                    "\tcase {f}[0]\n"
	                    "\t\tsystem.out.println(\"hit\")\n"
	                    "\tend\n"
	                    "end\n"),
	                "must be a constant value");
}

// =============================================================================
// F05b: a failed REPL line must roll back every lambda the statement
// registered - including the ones its build_line compiled before the line was
// matched/translated. The watermark is taken before build_line, not at
// interpret() entry.
// =============================================================================
TEST(repl_failed_line_rolls_back_function_store)
{
	cs::array args;
	args.push_back(cs::var::make<cs::string>("<AUDIT_TEST>"));
	auto ctx = cs::create_context(args);
	auto repl = std::make_shared<cs::repl>(ctx);
	repl->exec("var f = []()->1\n");
	const std::size_t before = ctx->instance->functions.size();
	EXPECT_TRUE(before >= 1);
	try
	{
		// The lambda compiles (registered into the store) but the malformed
		// parallel definition `x` fails translation.
		repl->exec("var g = []()->2, x\n");
	}
	catch (...)
	{
	}
	EXPECT_TRUE(ctx->instance->functions.size() == before);
	// The repl must stay usable and its committed lambda callable.
	EXPECT_TRUE(run_script_on(ctx, "system.out.println(f())\n") == "1\n");
}

// =============================================================================
// F05c: lambdas registered on earlier lines of a multi-line block must also be
// rolled back when a later line fails (previously each line had its own local
// snapshot, so a failed block leaked every earlier line's lambdas).
// =============================================================================
TEST(repl_multiline_block_failure_rolls_back_lambdas)
{
	cs::array args;
	args.push_back(cs::var::make<cs::string>("<AUDIT_TEST>"));
	auto ctx = cs::create_context(args);
	auto repl = std::make_shared<cs::repl>(ctx);
	repl->exec("var f = []()->1\n");
	const std::size_t before = ctx->instance->functions.size();
	repl->exec("function foo()\n");
	// Registered while the function block is open.
	repl->exec("var g = []()->2\n");
	try
	{
		// The malformed parallel definition `a` fails translation; every lambda
		// registered since the block opened (g and h) must be rolled back.
		repl->exec("var h = []()->3, a\n");
	}
	catch (...)
	{
	}
	EXPECT_TRUE(ctx->instance->functions.size() == before);
	// The repl must stay usable afterwards.
	repl->exec("var k = []()->4\n");
	EXPECT_TRUE(ctx->instance->functions.size() == before + 1);
	EXPECT_TRUE(run_script_on(ctx, "system.out.println(f() + k())\n") == "5\n");
}

// =============================================================================
// F05d: a re-entrant exec() commits a nested statement's lambda; if the outer
// statement fails afterwards, the nested statement's committed lambda survives
// through the variable's own copy even though its store slot is rolled back
// with the outer statement.
// =============================================================================
TEST(repl_reentrant_nested_lambda_survives_outer_failure)
{
	cs::array args;
	args.push_back(cs::var::make<cs::string>("<AUDIT_TEST>"));
	auto ctx = cs::create_context(args);
	auto repl = std::make_shared<cs::repl>(ctx);
	std::weak_ptr<cs::repl> weak_repl = repl;
	ctx->instance->storage.add_var("run_repl",
	                               cs::var::make<cs::callable>(cs::callable([weak_repl](cs::vector &) -> cs::var
	{
		// The nested statement commits `inner`; then the outer statement
		// fails via this exception.
		weak_repl.lock()->exec("var inner = []()->([]()->42)\n");
		throw std::runtime_error("boom");
	})));
	const std::size_t before = ctx->instance->functions.size();
	try
	{
		repl->exec("var outer_lambda = []()->1, outer = run_repl()\n");
	}
	catch (...)
	{
	}
	// The outer rollback removes only its own slots. The nested committed slot
	// retains its stable index.
	EXPECT_TRUE(ctx->instance->functions.active_size() == before + 2);
	EXPECT_TRUE(run_script_on(ctx, "var nested = inner()\nsystem.out.println(nested())\n") == "42\n");
}

// =============================================================================
// F05e: a re-entrant exec() during an outer REPL statement must not free the
// outer statement's token arena. Previously the nested exec replaced the sole
// arena, so resuming the outer expression (the `+ 1` after run_repl()) read
// freed tokens.
// =============================================================================
TEST(repl_reentrant_nested_exec_preserves_outer_tokens)
{
	cs::array args;
	args.push_back(cs::var::make<cs::string>("<AUDIT_TEST>"));
	auto ctx = cs::create_context(args);
	auto repl = std::make_shared<cs::repl>(ctx);
	std::weak_ptr<cs::repl> weak_repl = repl;
	ctx->instance->storage.add_var("run_repl",
	                               cs::var::make<cs::callable>(cs::callable([weak_repl](cs::vector &) -> cs::var
	{
		weak_repl.lock()->exec("var inner = []()->42\n");
		return cs::var::make<cs::numeric>(7);
	})));
	repl->exec("var outer = run_repl() + 1\n");
	EXPECT_TRUE(run_script_on(ctx, "system.out.println(outer)\n") == "8\n");
}

// =============================================================================
// F05f: @begin accepts one logical multi-line statement, not multiple top-level
// statements whose partial commit could leave storage and lambda indices out
// of sync.
// =============================================================================
TEST(repl_buffer_failure_preserves_committed_lambda_indices)
{
	cs::array args;
	args.push_back(cs::var::make<cs::string>("<AUDIT_TEST>"));
	auto ctx = cs::create_context(args);
	auto repl = std::make_shared<cs::repl>(ctx);
	repl->exec("@begin");
	repl->exec("function make_lambda()");
	repl->exec("return []()->42");
	repl->exec("end");
	repl->exec("var bad = []()->1, x");
	bool threw = false;
	try
	{
		repl->exec("@end");
	}
	catch (...)
	{
		threw = true;
	}
	EXPECT_TRUE(threw);
	EXPECT_TRUE(ctx->instance->functions.active_size() == 0);
	repl->exec("var ok = 42");
	EXPECT_TRUE(run_script_on(ctx, "system.out.println(ok)\n") == "42\n");
}

// =============================================================================
// F05g: a re-entrant exec() may not leave a block open. Reject it before the
// shared method/storage stacks are modified, then prove the outer statement
// and subsequent REPL input remain usable.
// =============================================================================
TEST(repl_reentrant_open_block_rejected_cleanly)
{
	cs::array args;
	args.push_back(cs::var::make<cs::string>("<AUDIT_TEST>"));
	auto ctx = cs::create_context(args);
	auto repl = std::make_shared<cs::repl>(ctx);
	std::weak_ptr<cs::repl> weak_repl = repl;
	ctx->instance->storage.add_var("run_repl",
	                               cs::var::make<cs::callable>(cs::callable([weak_repl](cs::vector &) -> cs::var
	{
		try
		{
			weak_repl.lock()->exec("function bad()");
		}
		catch (...)
		{
		}
		return cs::var::make<cs::numeric>(7);
	})));
	repl->exec("var outer = run_repl() + 1");
	EXPECT_TRUE(repl->get_level() == 0);
	EXPECT_TRUE(run_script_on(ctx, "system.out.println(outer)\n") == "8\n");
}

TEST(repl_restores_external_compile_unit)
{
	cs::array args;
	args.push_back(cs::var::make<cs::string>("<AUDIT_TEST>"));
	auto ctx = cs::create_context(args);
	auto repl = std::make_shared<cs::repl>(ctx);
	cs::compile_unit_guard guard(ctx.get());
	auto outer_unit = ctx->current_unit;
	repl->exec("var nested = 1");
	EXPECT_TRUE(ctx->current_unit == outer_unit);
	cs::expression_t tree;
	std::deque<char> buff{'1', '+', '1'};
	ctx->compiler->build_expr(buff, tree);
	EXPECT_TRUE(ctx->instance->parse_expr(tree.root()).const_val<cs::numeric>() == 2);
}

TEST(repl_empty_begin_buffer_rejected)
{
	cs::array args;
	args.push_back(cs::var::make<cs::string>("<AUDIT_TEST>"));
	auto ctx = cs::create_context(args);
	auto repl = std::make_shared<cs::repl>(ctx);
	repl->exec("@begin");
	bool threw = false;
	try
	{
		repl->exec("@end");
	}
	catch (...)
	{
		threw = true;
	}
	EXPECT_TRUE(threw);
	repl->exec("var ok = 42");
	EXPECT_TRUE(run_script_on(ctx, "system.out.println(ok)\n") == "42\n");
}

TEST(repl_reentrant_preprocessor_rejected_cleanly)
{
	cs::array args;
	args.push_back(cs::var::make<cs::string>("<AUDIT_TEST>"));
	auto ctx = cs::create_context(args);
	auto repl = std::make_shared<cs::repl>(ctx);
	std::weak_ptr<cs::repl> weak_repl = repl;
	ctx->instance->storage.add_var("run_repl",
	                               cs::var::make<cs::callable>(cs::callable([weak_repl](cs::vector &) -> cs::var
	{
		try
		{
			weak_repl.lock()->exec("@begin");
		}
		catch (...)
		{
		}
		return cs::var::make<cs::numeric>(7);
	})));
	repl->exec("var outer = run_repl() + 1");
	EXPECT_TRUE(run_script_on(ctx, "system.out.println(outer)\n") == "8\n");
}

TEST(repl_destructor_cleans_open_block)
{
	cs::array args;
	args.push_back(cs::var::make<cs::string>("<AUDIT_TEST>"));
	auto ctx = cs::create_context(args);
	const std::size_t before = ctx->instance->functions.active_size();
	{
		auto repl = std::make_shared<cs::repl>(ctx);
		repl->exec("function abandoned()");
		repl->exec("var leaked = []()->1");
		EXPECT_TRUE(ctx->instance->functions.active_size() > before);
	}
	EXPECT_TRUE(ctx->instance->functions.active_size() == before);
}

TEST(compile_exception_enabled_stream_accepts_eof)
{
	cs::array args;
	args.push_back(cs::var::make<cs::string>("<AUDIT_TEST>"));
	auto ctx = cs::create_context(args);
	std::istringstream in("var answer = 42\n");
	in.exceptions(std::ios::failbit | std::ios::badbit);
	ctx->instance->compile(in);
	ctx->instance->interpret();
	EXPECT_TRUE(ctx->instance->storage.get_var("answer").const_val<cs::numeric>() == 42);
}

TEST(repl_destructor_preserves_external_compile_unit)
{
	cs::array args;
	args.push_back(cs::var::make<cs::string>("<AUDIT_TEST>"));
	auto ctx = cs::create_context(args);
	cs::compile_unit_guard guard(ctx.get());
	auto outer_unit = ctx->current_unit;
	{
		auto repl = std::make_shared<cs::repl>(ctx);
		repl->exec("var nested = 1");
	}
	EXPECT_TRUE(ctx->current_unit == outer_unit);
	cs::expression_t tree;
	std::deque<char> buff{'2', '+', '3'};
	ctx->compiler->build_expr(buff, tree);
	EXPECT_TRUE(ctx->instance->parse_expr(tree.root()).const_val<cs::numeric>() == 5);
}

TEST(repl_reentrant_preprocessor_clears_command_buffer)
{
	cs::array args;
	args.push_back(cs::var::make<cs::string>("<AUDIT_TEST>"));
	auto ctx = cs::create_context(args);
	auto repl = std::make_shared<cs::repl>(ctx);
	std::weak_ptr<cs::repl> weak_repl = repl;
	ctx->instance->storage.add_var("run_repl",
	                               cs::var::make<cs::callable>(cs::callable([weak_repl](cs::vector &) -> cs::var
	{
		try
		{
			weak_repl.lock()->exec("@begin");
		}
		catch (...)
		{
		}
		return cs::var::make<cs::numeric>(7);
	})));
	repl->exec("var outer = run_repl()");
	EXPECT_NO_THROW(repl->exec("@charset:utf8"));
	repl->exec("var ok = 42");
	EXPECT_TRUE(run_script_on(ctx, "system.out.println(ok)\n") == "42\n");
}

// =============================================================================
// F05h: a failing re-entrant exec() must restore the outer statement's token
// arena so the outer expression keeps evaluating correctly.
// =============================================================================
TEST(repl_reentrant_inner_failure_preserves_outer_tokens)
{
	cs::array args;
	args.push_back(cs::var::make<cs::string>("<AUDIT_TEST>"));
	auto ctx = cs::create_context(args);
	cs::compile_unit_guard guard(ctx.get());
	auto outer_unit = ctx->current_unit;
	auto repl = std::make_shared<cs::repl>(ctx);
	std::weak_ptr<cs::repl> weak_repl = repl;
	ctx->instance->storage.add_var("run_repl",
	                               cs::var::make<cs::callable>(cs::callable([weak_repl](cs::vector &) -> cs::var
	{
		try
		{
			// Malformed parallel definition: translation fails, popping the
			// inner unit.
			weak_repl.lock()->exec("var inner = []()->1, x\n");
		}
		catch (...)
		{
		}
		return cs::var::make<cs::numeric>(7);
	})));
	repl->exec("var outer = run_repl() + 1\n");
	EXPECT_TRUE(ctx->current_unit == outer_unit);
	EXPECT_TRUE(run_script_on(ctx, "system.out.println(outer)\n") == "8\n");
}

// =============================================================================
// F05i: an inner failing exec() plus a later outer failure must pop the unit
// stack twice and restore the pre-statement arena with no store slots left.
// =============================================================================
TEST(repl_reentrant_double_failure_balances_unit_stack)
{
	cs::array args;
	args.push_back(cs::var::make<cs::string>("<AUDIT_TEST>"));
	auto ctx = cs::create_context(args);
	cs::compile_unit_guard guard(ctx.get());
	auto outer_unit = ctx->current_unit;
	auto repl = std::make_shared<cs::repl>(ctx);
	std::weak_ptr<cs::repl> weak_repl = repl;
	ctx->instance->storage.add_var("run_repl",
	                               cs::var::make<cs::callable>(cs::callable([weak_repl](cs::vector &) -> cs::var
	{
		try
		{
			weak_repl.lock()->exec("var inner = []()->1, x\n");
		}
		catch (...)
		{
		}
		return cs::var::make<cs::numeric>(7);
	})));
	// Fails at run time after run_repl() already ran, so the outer statement
	// gets its own pop after the inner one.
	ctx->instance->storage.add_var("boom",
	                               cs::var::make<cs::callable>(cs::callable([](cs::vector &) -> cs::var
	{
		throw cs::runtime_error("boom");
	})));
	const std::size_t before = ctx->instance->functions.size();
	bool threw = false;
	try
	{
		repl->exec("var outer = run_repl() + boom()\n");
	}
	catch (...)
	{
		threw = true;
	}
	EXPECT_TRUE(threw);
	EXPECT_TRUE(ctx->current_unit == outer_unit);
	EXPECT_TRUE(ctx->instance->functions.size() == before);
	repl->exec("var ok = []()->42\n");
	EXPECT_TRUE(run_script_on(ctx, "system.out.println(ok())\n") == "42\n");
}

// =============================================================================
// F05j: three nesting levels (inner fails, middle succeeds, outer succeeds)
// must restore each level's enclosing unit.
// =============================================================================
TEST(repl_reentrant_three_level_units_balanced)
{
	cs::array args;
	args.push_back(cs::var::make<cs::string>("<AUDIT_TEST>"));
	auto ctx = cs::create_context(args);
	cs::compile_unit_guard guard(ctx.get());
	auto outer_unit = ctx->current_unit;
	auto repl = std::make_shared<cs::repl>(ctx);
	std::weak_ptr<cs::repl> weak_repl = repl;
	ctx->instance->storage.add_var("run_inner",
	                               cs::var::make<cs::callable>(cs::callable([weak_repl](cs::vector &) -> cs::var
	{
		try
		{
			weak_repl.lock()->exec("var x = []()->1, boom\n");
		}
		catch (...)
		{
		}
		return cs::var::make<cs::numeric>(7);
	})));
	ctx->instance->storage.add_var("run_mid",
	                               cs::var::make<cs::callable>(cs::callable([weak_repl](cs::vector &) -> cs::var
	{
		weak_repl.lock()->exec("var mid = run_inner() + 1\n");
		return cs::var::make<cs::numeric>(7);
	})));
	repl->exec("var outer = run_mid() + 1\n");
	EXPECT_TRUE(ctx->current_unit == outer_unit);
	EXPECT_TRUE(run_script_on(ctx, "system.out.println(mid)\nsystem.out.println(outer)\n") == "8\n8\n");
}

// =============================================================================
// F39: a switch body whose later statement fails to translate leaked the
// already-translated case/default statements (no body_guard around the
// recursive translate). The context must stay usable after repeated failures.
// =============================================================================

TEST(switch_body_translate_failure_is_cleaned_up)
{
	cs::array args;
	args.push_back(cs::var::make<cs::string>("<AUDIT_TEST>"));
	auto ctx = cs::create_context(args);
	for (int i = 0; i < 20; ++i)
	{
		bool threw = false;
		try
		{
			// "break" at switch-body level fails translation after the case
			// statement has already been created.
			std::istringstream in("switch 1\ncase 1\nvar a = 1\nend\nbreak\nend\n");
			ctx->instance->compile(in);
		}
		catch (const std::exception &)
		{
			threw = true;
		}
		EXPECT_TRUE(threw);
	}
	// A valid switch program still compiles and runs afterwards.
	EXPECT_TRUE(run_script_on(ctx, "using system\nswitch 1\ncase 1\n\tsystem.out.println(\"ok\")\nend\nend\n") == "ok\n");
}

TEST(struct_builder_copy_assignment_releases_old_methods)
{
	class counted_statement final : public cs::statement_base
	{
		int &destroyed;

	   public:
		counted_statement(cs::context_type *context, cs::token_base *endline, int &count)
		    : statement_base(context, endline), destroyed(count) {}

		~counted_statement() override
		{
			++destroyed;
		}

		cs::statement_types get_type() const noexcept override
		{
			return cs::statement_types::expression_;
		}

		void run_impl() override {}

		void dump(std::ostream &) const override {}
	};

	cs::array args;
	args.push_back(cs::var::make<cs::string>("<STRUCT_BUILDER_ASSIGN>"));
	auto ctx = cs::create_context(args);
	cs::token_endline endline(1);
	int destroyed = 0;
	{
		std::deque<cs::statement_base *> first_methods{
		    new counted_statement(ctx.get(), &endline, destroyed)};
		std::deque<cs::statement_base *> second_methods{
		    new counted_statement(ctx.get(), &endline, destroyed)};
		cs::struct_builder first(ctx.get(), "first", {}, std::move(first_methods));
		cs::struct_builder second(ctx.get(), "second", {}, std::move(second_methods));
		first = second;
		EXPECT_TRUE(destroyed == 1);
	}
	EXPECT_TRUE(destroyed == 2);
}

// =============================================================================
// F40: deep-copying a container that holds a recursive lambda must rebind the
// clone's `self` borrow to its own proxy (the container detach path goes
// through copy_no_return, which now shares the rebind logic of cs::copy).
// =============================================================================

TEST(clone_container_with_recursive_lambda_rebinds_self)
{
	cs::array args;
	args.push_back(cs::var::make<cs::string>("<CLONE_SELF_ARR>"));
	auto ctx = cs::create_context(args);
	run_script_on(ctx, "var arr = {[](n)->n>1?self(n-1)*n:1}\n");
	cs::var arr = ctx->instance->storage.get_var("arr");
	cs::var arr2 = cs::copy(arr);
	// Drop the original: the clone's lambda must not borrow the freed proxy.
	arr = cs::var();
	cs::var fn = arr2.const_val<cs::array>().front();
	cs::process_run_scope scope(ctx);
	cs::var r = cs::invoke(fn, cs::var::make<cs::numeric>(5));
	EXPECT_TRUE(r.const_val<cs::numeric>() == 120);
}

// =============================================================================
// F41: fiber.create on a recursive lambda used to hand the fiber a live alias
// of `self`; reassigning the source variable before resume broke the fiber.
// The self-referential wrapper is now snapshotted (deep copy + rebind).
// =============================================================================

TEST(fiber_snapshots_recursive_lambda_self)
{
	cs::array args;
	args.push_back(cs::var::make<cs::string>("<FIBER_SELF>"));
	auto ctx = cs::create_context(args);
	std::ostringstream captured;
	auto *old = std::cout.rdbuf(captured.rdbuf());
	try
	{
		std::istringstream in(
		    "using system\n"
		    "var f = [](n)->n>1?self(n-1)*n:1\n"
		    "var co = fiber.create(f, 5)\n"
		    "f = 0\n"
		    "co.resume()\n"
		    "system.out.println(co.return_value())\n");
		ctx->instance->compile(in);
		ctx->instance->interpret();
	}
	catch (...)
	{
		std::cout.rdbuf(old);
		throw;
	}
	std::cout.rdbuf(old);
	EXPECT_TRUE(captured.str() == "120\n");
}

// =============================================================================
// F41b: async arguments are detached through the same copy_no_return path, so
// a recursive lambda passed to future.create must have its `self` rebound to
// the clone before the worker thread materializes it.
// =============================================================================

TEST(async_future_snapshots_recursive_lambda_argument)
{
	cs::array args;
	args.push_back(cs::var::make<cs::string>("<ASYNC_SELF>"));
	auto ctx = cs::create_context(args);
	ctx->instance->storage.add_var("invoke_it",
	                               cs::var::make<cs::callable>([](cs::vector &data) -> cs::var
	{
		return cs::invoke(data[0], cs::var::make<cs::numeric>(5));
	}));
	std::ostringstream captured;
	auto *old = std::cout.rdbuf(captured.rdbuf());
	try
	{
		std::istringstream in(
		    "using system\n"
		    "var f = [](n)->n>1?self(n-1)*n:1\n"
		    "var fu = future.create(invoke_it, f)\n"
		    "f = 0\n"
		    "system.out.println(fu.get())\n");
		ctx->instance->compile(in);
		ctx->instance->interpret();
	}
	catch (...)
	{
		std::cout.rdbuf(old);
		throw;
	}
	std::cout.rdbuf(old);
	EXPECT_TRUE(captured.str() == "120\n");
}

// =============================================================================
// Copilot review: any::clone() used to copy a recursive lambda's non-owning
// `self` borrower verbatim; the rebind lived in the cs::copy wrappers, so a
// bare clone() produced a clone that dangled (or recursed into the original)
// once the source proxy was released. clone() now rebinds via the dispatched
// rebind operator, so the raw primitive is self-contained.
// =============================================================================

TEST(clone_of_recursive_lambda_is_self_contained)
{
	cs::array args;
	args.push_back(cs::var::make<cs::string>("<CLONE_SELF>"));
	auto ctx = cs::create_context(args);
	std::istringstream in("var f = [](n) -> n > 1 ? self(n - 1) * n : 1\n");
	ctx->instance->compile(in);
	ctx->instance->interpret();
	cs::var original = ctx->instance->storage.get_var("f");
	// Bare clone without the cs::copy pipeline: the clone must rebind its own
	// `self` borrower to its own proxy.
	cs::var clone = original;
	clone.clone();
	// Drop the original; the clone must remain usable.
	original = cs::var();
	cs::process_run_scope scope(ctx);
	EXPECT_TRUE(cs::invoke(clone, cs::var::make<cs::numeric>(5)).const_val<cs::numeric>().as_integer() == 120);
}

// =============================================================================
// F42: a failed module import used to wipe every module cached on the shared
// compiler (the temporary subcontext's destructor cleared it). Only the
// compiler-owning context may clear the cache.
// =============================================================================

TEST(failed_import_preserves_loaded_modules)
{
	cs::array args;
	args.push_back(cs::var::make<cs::string>("<MODULE_FAIL>"));
	auto ctx = cs::create_context(args);
	auto dir = std::filesystem::temp_directory_path() / "cs_module_fail";
	std::filesystem::create_directories(dir);
	{
		std::ofstream f(dir / "goodmod.csp");
		f << "package goodmod\nfunction triple(x)\n    return x * 3\nend\n";
	}
	ctx->process->import_path += cs::path_delimiter + dir.string();
	// Import the good module, then trigger a failed import via context.import
	// (which swallows the error and lets the script continue).
	EXPECT_TRUE(run_script_on(ctx, "import goodmod\nvar bad = context.import(\".\", \"nosuchpkg\")\n") == "");
	// The previously imported module must still be intact.
	EXPECT_TRUE(run_script_on(ctx, "using system\nsystem.out.println(goodmod.triple(14))\n") == "42\n");
	std::filesystem::remove_all(dir);
}

// =============================================================================
// F43: clear_global released the symbol table before destroying the values, so
// a structure finalizer referencing another global failed silently. Finalizers
// now run first, while names still resolve (and exactly once).
// =============================================================================

TEST(global_finalizer_resolves_other_globals)
{
	cs::array args;
	args.push_back(cs::var::make<cs::string>("<FINALIZE_GLOBALS>"));
	auto ctx = cs::create_context(args);
	std::ostringstream captured;
	auto *old = std::cout.rdbuf(captured.rdbuf());
	try
	{
		std::istringstream in(
		    "using system\n"
		    "var marker = 21\n"
		    "struct watcher\n"
		    "    function finalize()\n"
		    "        system.out.println(marker * 2)\n"
		    "    end\n"
		    "end\n"
		    "var w = new watcher\n");
		ctx->instance->compile(in);
		ctx->instance->interpret();
		// Releasing the globals runs the finalizer; `marker` must still resolve.
		ctx->instance->storage.clear_global();
	}
	catch (...)
	{
		std::cout.rdbuf(old);
		throw;
	}
	std::cout.rdbuf(old);
	EXPECT_TRUE(captured.str() == "42\n");
	// The destructor must not run the finalizer a second time.
	ctx.reset();
	EXPECT_TRUE(captured.str() == "42\n");
}

// =============================================================================
// F44: two contexts running on separate threads concurrently. The var proxy
// pool is per-thread and init_extensions is call_once, so this must not race.
// =============================================================================

TEST(concurrent_contexts_on_separate_threads)
{
	std::atomic<bool> failed{false};
	auto worker = [&failed](int id)
	{
		try
		{
			for (int i = 0; i < 20; ++i)
			{
				cs::array args;
				args.push_back(cs::var::make<cs::string>(id == 0 ? "<THREAD_A>" : "<THREAD_B>"));
				auto ctx = cs::create_context(args);
				// Heavy var churn exercises the proxy pool and heap stores.
				std::istringstream in(
				    "var acc = 0\n"
				    "var i = 0\n"
				    "while i < 200\n"
				    "\tacc = acc + i\n"
				    "\ti = i + 1\n"
				    "end\n"
				    "var check = acc == 19900\n");
				ctx->instance->compile(in);
				ctx->instance->interpret();
				if (!ctx->instance->storage.get_var("check").const_val<bool>())
					throw cs_test::test_failure("bad accumulation");
			}
		}
		catch (...)
		{
			failed.store(true);
		}
	};
	std::thread t1(worker, 0);
	std::thread t2(worker, 1);
	t1.join();
	t2.join();
	EXPECT_TRUE(!failed.load());
}

// =============================================================================
// B1: recompiling on the same instance used to leave record-name views into
// the previous program's (freed) token arena, a heap-use-after-free under
// ASan. The record set owns its keys, so a second compile stays clean and the
// previously declared variables remain visible.
// =============================================================================

TEST(recompile_on_same_instance_is_clean)
{
	cs::array args;
	args.push_back(cs::var::make<cs::string>("<RECOMPILE>"));
	auto ctx = cs::create_context(args);
	EXPECT_TRUE(run_script_on(ctx, "var a = 1\nvar b = 2\n") == "");
	EXPECT_TRUE(run_script_on(ctx, "var c = a + b\n") == "");
	EXPECT_TRUE(run_script_on(ctx, "system.out.println(c * 2)\n") == "6\n");
}
// =============================================================================
// Constructing a struct from a teardown finalizer used to fail the weak
// context lock and get silently swallowed; it now borrows the dying context.
// =============================================================================

TEST(struct_constructed_in_finalizer)
{
	cs::array args;
	args.push_back(cs::var::make<cs::string>("<AUDIT_TEST>"));
	std::string out;
	{
		std::ostringstream captured;
		auto *old = std::cout.rdbuf(captured.rdbuf());
		try
		{
			auto ctx = cs::create_context(args);
			{
				std::istringstream in(
				    "using system\n"
				    "class B\n"
				    "    function initialize()\n"
				    "        system.out.println(\"init-b\")\n"
				    "    end\n"
				    "end\n"
				    "class A\n"
				    "    function finalize()\n"
				    "        var b = new B\n"
				    "        system.out.println(\"finalize-new-b\")\n"
				    "    end\n"
				    "end\n"
				    "var g = new A\n");
				ctx->instance->compile(in);
				ctx->instance->interpret();
			}
			ctx.reset();
		}
		catch (...)
		{
			std::cout.rdbuf(old);
			throw;
		}
		std::cout.rdbuf(old);
		out = captured.str();
	}
	EXPECT_CONTAINS(out, "init-b");
	EXPECT_CONTAINS(out, "finalize-new-b");
}

// =============================================================================
// Constructing an escaped struct type from native code requires the caller to
// hold a process scope; the constructor runs the initializer under it.
// =============================================================================

TEST(escaped_type_constructor_via_native_call)
{
	cs::array args;
	args.push_back(cs::var::make<cs::string>("<AUDIT_TEST>"));
	auto ctx = cs::create_context(args);
	run_script_on(ctx,
	              "class T\n"
	              "    function initialize()\n"
	              "        iostream.setprecision(6)\n"
	              "        system.out.println(\"init-t-no-process\")\n"
	              "    end\n"
	              "end\n");
	cs::var type = ctx->instance->storage.get_var("T");
	EXPECT_TRUE(type.is_type_of<cs::type_t>());
	cs::process_run_scope scope(ctx);
	std::ostringstream captured;
	auto *old = std::cout.rdbuf(captured.rdbuf());
	cs::var obj;
	try
	{
		obj = type.const_val<cs::type_t>().constructor();
	}
	catch (...)
	{
		std::cout.rdbuf(old);
		throw;
	}
	std::cout.rdbuf(old);
	EXPECT_TRUE(obj.is_type_of<cs::structure>());
	EXPECT_CONTAINS(captured.str(), "init-t-no-process");
}

// =============================================================================
// F44: invoking a script callable without an active process (native callers
// must hold process_run_scope) must raise a clean error, not dereference null.
// =============================================================================

TEST(native_invoke_without_process_raises_clean_error)
{
	cs::array args;
	args.push_back(cs::var::make<cs::string>("<NO_PROCESS>"));
	auto ctx = cs::create_context(args);
	cs::var fn;
	{
		cs::process_run_scope scope(ctx);
		run_script_on(ctx, "var f = []() -> 1\n");
		fn = ctx->instance->storage.get_var("f");
	}
	// Scope exited: invoking without one must throw, not crash.
	std::exception_ptr eptr;
	try
	{
		cs::var r = cs::invoke(fn);
		EXPECT_TRUE(r.const_val<cs::numeric>() == 1);
	}
	catch (const cs::runtime_error &)
	{
		eptr = std::current_exception();
	}
	EXPECT_TRUE(eptr != nullptr);
}

// =============================================================================
// F45: array auto-growth must reject a difference equal to max_auto_extend
// (growing to index 16777216 from an empty array would insert 16777217
// elements; previously only a greater difference was rejected).
// =============================================================================
TEST(array_auto_extend_boundary)
{
	cs::array args;
	args.push_back(cs::var::make<cs::string>("<EXTEND_BOUNDARY>"));
	auto ctx = cs::create_context(args);
	EXPECT_THROW(run_script_on(ctx, "var b = new array\nb[16777216] = 1\n"), cs::exception);
}

// =============================================================================
// F46: main-thread detection must report true on the test (main) thread and
// false on worker threads; the old Windows implementation compared thread ID
// to process ID, which never match, silently disabling the pooled allocator
// on Windows.
// =============================================================================
TEST(main_thread_detection_on_test_thread)
{
	EXPECT_TRUE(cs_system_impl::is_main_thread());
}

TEST(main_thread_detection_on_worker_thread)
{
	std::atomic<bool> is_main{true};
	std::thread worker([&is_main]
	{ is_main = cs_system_impl::is_main_thread(); });
	worker.join();
	EXPECT_FALSE(is_main);
}

// =============================================================================
// F47: expanding a constant array during compile-time folding used to bind a
// reference into a temporary var (parse_expr(...).const_val<array>()); the
// temporary died before the elements were read. Keep the var alive.
// =============================================================================
TEST(expand_folding_keeps_temporary_arrays_alive)
{
	const std::string out = run_script(
	    "using system\n"
	    "function add(a, b)\n"
	    "    return a + b\n"
	    "end\n"
	    "var arr = {1, 2}\n"
	    "var x = {arr...}\n"
	    "var y = add(arr...)\n"
	    "system.out.println(x)\n"
	    "system.out.println(y)\n");
	EXPECT_CONTAINS(out, "{1, 2}");
	EXPECT_CONTAINS(out, "3");
}
