#include "test_helpers.hpp"

// =============================================================================
// Lexer Tests
// =============================================================================

TEST(lexer_string_literal_adjacent_identifier)
{
	// Test the bug fix: `"str"identifier` should not cause out-of-bounds read.
	// After a closing `"`, if the next char is an identifier start (not a signal),
	// the lexer should push a token_literal with empty literal type for string
	// concatenation. Previously, `*(it + 1)` was accessed without bounds check.
	//
	// We test this by parsing expressions that contain string literals with
	// various adjacent characters.

	// String at end of input — no next char
	auto tree = build_expr_tree("\"hello\"");
	EXPECT_TRUE(tree.root().usable());
	// Should parse as a string literal, not crash

	// String followed by space — next char is space (signal)
	tree = build_expr_tree("\"hello\"   ");
	EXPECT_TRUE(tree.root().usable());

	// String followed by identifier — tests the exact fix
	tree = build_expr_tree("\"hello\"world");
	EXPECT_TRUE(tree.root().usable());
}

TEST(lexer_string_literal_escape_sequences)
{
	// String with escape sequences
	auto tree = build_expr_tree("\"hello\\nworld\"");
	EXPECT_TRUE(tree.root().usable());

	tree = build_expr_tree("\"tab\\there\"");
	EXPECT_TRUE(tree.root().usable());

	tree = build_expr_tree("\"quote\\\"inside\"");
	EXPECT_TRUE(tree.root().usable());

	tree = build_expr_tree("\"backslash\\\\end\"");
	EXPECT_TRUE(tree.root().usable());
}

TEST(lexer_string_literal_empty)
{
	auto tree = build_expr_tree("\"\"");
	EXPECT_TRUE(tree.root().usable());
}

TEST(lexer_numeric_literals)
{
	// Integer literal
	auto tree = build_expr_tree("12345");
	EXPECT_TRUE(tree.root().usable());

	// Negative number: lexed as unary minus + integer.
	// May be constant-folded by opt_expr to a token_value, or remain as minus_.
	tree = build_expr_tree("-12345");
	EXPECT_TRUE(tree.root().usable());
	// Accept either signal or value (constant folding may have run)
	auto *data = tree.root().data();
	EXPECT_TRUE(data != nullptr);

	// Floating point
	tree = build_expr_tree("3.14159");
	EXPECT_TRUE(tree.root().usable());
}

TEST(lexer_identifiers)
{
	auto tree = build_expr_tree("hello");
	EXPECT_TRUE(tree.root().usable());

	// Underscore
	tree = build_expr_tree("_private");
	EXPECT_TRUE(tree.root().usable());

	// Alphanumeric
	tree = build_expr_tree("var123");
	EXPECT_TRUE(tree.root().usable());
}

TEST(lexer_string_adjacent_identifier_bound_check)
{
	// Tests the out-of-bounds fix: after a closing `"`, the lexer checks
	// whether the next char is an identifier start. When the string is at
	// end-of-buffer, `next == buff.end()` short-circuits safely.
	// Case 1: string at end of buffer — bound check prevents OOB read
	auto tree = build_expr_tree("\"hello\"");
	EXPECT_TRUE(tree.root().usable());

	// Case 2: string immediately followed by identifier — valid lexer path
	// The null literal_type tells the runtime to concatenate strings.
	tree = build_expr_tree("\"hello\"world");
	EXPECT_TRUE(tree.root().usable());
}
