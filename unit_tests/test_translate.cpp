#include "test_helpers.hpp"

// =============================================================================
// translate() Validation Tests
//
// Tests for the statement line validation added in the fix_compiler branch:
//   1. Empty statement lines are rejected
//   2. Lines without endline token are rejected
//   3. Valid lines are accepted
// =============================================================================

// ---------------------------------------------------------------------------
// Helper: run translate() on manually constructed lines
// ---------------------------------------------------------------------------

static bool translate_throws(cs::context_t ctx,
                             std::deque<std::deque<cs::token_base *>> &lines)
{
	std::deque<cs::statement_base *> statements;
	try {
		ctx->compiler->translate(lines, statements);
		return false;
	}
	catch (const cs::compile_error &) {
		return true;
	}
}

// ---------------------------------------------------------------------------
// Valid line structure tests
// ---------------------------------------------------------------------------

TEST(translate_valid_simple_expression)
{
	auto ast = build_line("a + b");
	EXPECT_TRUE(ast.size() > 0);
	for (auto &line : ast) {
		EXPECT_FALSE(line.empty());
		auto *tail = line.back();
		EXPECT_TRUE(tail != nullptr);
		EXPECT_TRUE(tail->get_type() == cs::token_types::endline);
	}
}

TEST(translate_valid_assignment)
{
	auto ast = build_line("x = 42");
	EXPECT_TRUE(ast.size() > 0);
	for (auto &line : ast) {
		EXPECT_FALSE(line.empty());
		auto *tail = line.back();
		EXPECT_TRUE(tail != nullptr);
		EXPECT_TRUE(tail->get_type() == cs::token_types::endline);
	}
}

TEST(translate_multiple_lines_have_endline)
{
	auto ast = build_ast_lines("var x = 1\nvar y = 2\nx + y");
	for (auto &line : ast) {
		EXPECT_FALSE(line.empty());
		auto *tail = line.back();
		EXPECT_TRUE(tail != nullptr);
		EXPECT_TRUE(tail->get_type() == cs::token_types::endline);
	}
}

TEST(translate_empty_ast_no_crash)
{
	auto ctx = make_context();
	std::deque<std::deque<cs::token_base *>> ast;
	EXPECT_NO_THROW({
		std::deque<cs::statement_base *> statements;
		ctx->compiler->translate(ast, statements);
	});
}

// ---------------------------------------------------------------------------
// Malformed line tests
// ---------------------------------------------------------------------------

TEST(translate_rejects_line_without_endline)
{
	auto ctx = make_context();
	std::deque<std::deque<cs::token_base *>> lines;
	std::deque<cs::token_base *> bad_line;
	bad_line.push_back(new cs::token_id("x"));
	bad_line.push_back(new cs::token_signal(cs::signal_types::asi_));
	bad_line.push_back(new cs::token_id("y"));
	// Missing: token_endline
	lines.push_back(bad_line);
	EXPECT_TRUE(translate_throws(ctx, lines));
}

TEST(translate_rejects_empty_line)
{
	auto ctx = make_context();
	std::deque<std::deque<cs::token_base *>> lines;
	std::deque<cs::token_base *> empty_line;
	lines.push_back(empty_line);
	EXPECT_TRUE(translate_throws(ctx, lines));
}

TEST(translate_rejects_null_endline)
{
	auto ctx = make_context();
	std::deque<std::deque<cs::token_base *>> lines;
	std::deque<cs::token_base *> bad_line;
	bad_line.push_back(new cs::token_id("x"));
	bad_line.push_back(nullptr); // null instead of endline
	lines.push_back(bad_line);
	EXPECT_TRUE(translate_throws(ctx, lines));
}
