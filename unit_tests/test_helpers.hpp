#pragma once
#include <covscript/covscript.hpp>
#include "test_harness.hpp"

// =============================================================================
// Helpers: create a shared compiler context for all tests.
// Uses static bootstrap to prevent premature garbage collection.
// =============================================================================
inline cs::context_t make_context()
{
	static cs::bootstrap env;
	return env.context;
}

// =============================================================================
// Helper: build an expression tree from source string.
// Uses context->compiler->build_expr() which runs the full pipeline:
//   lexer → parser → gen_tree → optimize_expression (trim_expr + opt_expr)
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
