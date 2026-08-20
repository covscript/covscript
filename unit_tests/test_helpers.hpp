#pragma once
#include <covscript/covscript.hpp>
#include "covariant_test.hpp"

// =============================================================================
// Helpers: create a shared compiler context for all tests.
// Uses static bootstrap to prevent premature garbage collection.
//
// IMPORTANT: This returns a SHARED context backed by a static bootstrap.
//   - Safe for read-only compiler operations (build_expr, build_ast, etc.)
//   - Do NOT define variables or modify global state; they persist across tests.
//   - For tests that need isolation, use cs::create_context() instead.
// =============================================================================
inline cs::context_t shared_compiler_context()
{
	static cs::bootstrap env;
	return env.context;
}

// Backward-compatible alias.
inline cs::context_t make_context()
{
	return shared_compiler_context();
}

// =============================================================================
// Helper: build an expression tree from source string.
// Uses context->compiler->build_expr() which runs the full pipeline:
//   lexer -> parser -> gen_tree -> optimize_expression (trim_expr + opt_expr)
// =============================================================================
inline cs::tree_type<cs::token_base *> build_expr_tree(const std::string &src)
{
	auto ctx = make_context();
	cs::tree_type<cs::token_base *> tree;
	std::deque<char> buff(src.begin(), src.end());
	ctx->compiler->build_expr(buff, tree);
	return tree;
}

// =============================================================================
// Script helpers: run scripts in fresh or existing contexts, capturing
// system.out output; any exception propagates to the caller.
// =============================================================================
inline std::string run_script(const std::string &src, const std::function<void(const cs::context_t &)> &setup = {})
{
	cs::array args;
	args.push_back(cs::var::make<cs::string>("<UNIT_TEST>"));
	auto ctx = cs::create_context(args);
	if (setup)
		setup(ctx);
	std::ostringstream captured;
	auto *old = std::cout.rdbuf(captured.rdbuf());
	try
	{
		std::istringstream in(src);
		ctx->instance->compile(in);
		ctx->instance->interpret();
	}
	catch (...)
	{
		std::cout.rdbuf(old);
		throw;
	}
	std::cout.rdbuf(old);
	return captured.str();
}

inline std::string run_script_on(const cs::context_t &ctx, const std::string &src)
{
	std::ostringstream captured;
	auto *old = std::cout.rdbuf(captured.rdbuf());
	try
	{
		std::istringstream in(src);
		ctx->instance->compile(in);
		ctx->instance->interpret();
	}
	catch (...)
	{
		std::cout.rdbuf(old);
		throw;
	}
	std::cout.rdbuf(old);
	return captured.str();
}

// Run a script expected to throw; returns the thrown error message. Fails the
// test (test_failure) if the script completes without throwing.
inline std::string run_script_expect_throw(const std::string &src)
{
	try
	{
		run_script(src);
	}
	catch (const cs::exception &e)
	{
		return e.what();
	}
	catch (const cs::compile_error &e)
	{
		return e.what();
	}
	catch (const std::exception &e)
	{
		return e.what();
	}
	throw cs_test::test_failure("expected the script to throw");
}

// =============================================================================
// Helper: build a full AST (lines of tokens) from source string
// =============================================================================
inline std::deque<std::deque<cs::token_base *>> build_ast_lines(const std::string &src)
{
	auto ctx = make_context();
	std::deque<std::deque<cs::token_base *>> ast;
	std::deque<char> buff(src.begin(), src.end());
	ctx->compiler->build_ast(buff, ast);
	return ast;
}

// =============================================================================
// Helper: build a single line (like REPL input)
// =============================================================================
inline std::deque<std::deque<cs::token_base *>> build_line(const std::string &src)
{
	auto ctx = make_context();
	std::deque<std::deque<cs::token_base *>> ast;
	std::deque<char> buff(src.begin(), src.end());
	ctx->compiler->build_line(buff, ast);
	return ast;
}

// =============================================================================
// Token type checking helpers
// =============================================================================
inline bool is_signal(const cs::token_base *t, cs::signal_types sig)
{
	if (!t || t->get_type() != cs::token_types::signal)
		return false;
	return static_cast<const cs::token_signal *>(t)->get_signal() == sig;
}

inline bool is_id(const cs::token_base *t, const std::string &name)
{
	if (!t || t->get_type() != cs::token_types::id)
		return false;
	return static_cast<const cs::token_id *>(t)->get_id().get_id() == name;
}
