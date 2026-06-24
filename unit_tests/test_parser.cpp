#include "test_helpers.hpp"

// =============================================================================
// Parser / Expression Tree Tests
// =============================================================================

// ---------------------------------------------------------------------------
// Operator precedence: arithmetic
// ---------------------------------------------------------------------------

TEST(parser_precedence_mul_over_add)
{
	// a + b * c  →  add_(a, mul_(b, c))
	auto tree = build_expr_tree("a + b * c");
	auto root = tree.root();
	EXPECT_TRUE(is_signal(root.data(), cs::signal_types::add_));
	EXPECT_TRUE(is_id(root.left().data(), "a"));
	EXPECT_TRUE(is_signal(root.right().data(), cs::signal_types::mul_));
	EXPECT_TRUE(is_id(root.right().left().data(), "b"));
	EXPECT_TRUE(is_id(root.right().right().data(), "c"));
}

TEST(parser_precedence_add_over_comma)
{
	// a, b + c  →  after trim_expr, comma is flattened to token_parallel
	// The tree root data becomes a token_parallel (not a signal)
	auto tree = build_expr_tree("a, b + c");
	auto root = tree.root();
	// After optimization, comma is converted to parallel list
	EXPECT_TRUE(root.usable());
	// Each element in the parallel list is a separate expression tree
}

TEST(parser_precedence_pow_over_mul)
{
	// a * b ^ c  →  mul_(a, pow_(b, c))
	auto tree = build_expr_tree("a * b ^ c");
	auto root = tree.root();
	EXPECT_TRUE(is_signal(root.data(), cs::signal_types::mul_));
	EXPECT_TRUE(is_id(root.left().data(), "a"));
	EXPECT_TRUE(is_signal(root.right().data(), cs::signal_types::pow_));
}

TEST(parser_precedence_compare_over_logical)
{
	// a < b and c > d  →  and_(abo_(a, b), abo_(c, d))
	auto tree = build_expr_tree("a < b and c > d");
	auto root = tree.root();
	EXPECT_TRUE(is_signal(root.data(), cs::signal_types::and_));
}

// ---------------------------------------------------------------------------
// Operator precedence: unary
// ---------------------------------------------------------------------------

TEST(parser_unary_minus)
{
	// -a  →  sub_(null, a)  →  trim_expr converts to minus_(a)
	// build_expr runs the full pipeline, so we see the post-trim form.
	auto tree = build_expr_tree("-a");
	auto root = tree.root();
	EXPECT_TRUE(is_signal(root.data(), cs::signal_types::minus_));
}

TEST(parser_unary_dereference)
{
	// *p  →  mul_(null, p)  →  trim_expr converts to escape_(p)
	auto tree = build_expr_tree("*p");
	auto root = tree.root();
	EXPECT_TRUE(is_signal(root.data(), cs::signal_types::escape_));
}

TEST(parser_unary_not)
{
	// not a  →  not_(null, a)
	auto tree = build_expr_tree("not a");
	auto root = tree.root();
	EXPECT_TRUE(is_signal(root.data(), cs::signal_types::not_));
}

// ---------------------------------------------------------------------------
// Operator associativity: left-associative
// ---------------------------------------------------------------------------

TEST(parser_left_associative_sub)
{
	// a - b - c  →  sub_(sub_(a, b), c)
	auto tree = build_expr_tree("a - b - c");
	auto root = tree.root();
	EXPECT_TRUE(is_signal(root.data(), cs::signal_types::sub_));
	EXPECT_TRUE(is_signal(root.left().data(), cs::signal_types::sub_));
	EXPECT_TRUE(is_id(root.left().left().data(), "a"));
	EXPECT_TRUE(is_id(root.left().right().data(), "b"));
	EXPECT_TRUE(is_id(root.right().data(), "c"));
}

TEST(parser_left_associative_add)
{
	// a + b + c  →  add_(add_(a, b), c)
	auto tree = build_expr_tree("a + b + c");
	auto root = tree.root();
	EXPECT_TRUE(is_signal(root.data(), cs::signal_types::add_));
	EXPECT_TRUE(is_signal(root.left().data(), cs::signal_types::add_));
}

// ---------------------------------------------------------------------------
// Operator associativity: right-associative
// ---------------------------------------------------------------------------

TEST(parser_right_associative_assign)
{
	// a = b = c  →  asi_(a, asi_(b, c))
	auto tree = build_expr_tree("a = b = c");
	auto root = tree.root();
	EXPECT_TRUE(is_signal(root.data(), cs::signal_types::asi_));
	EXPECT_TRUE(is_id(root.left().data(), "a"));
	EXPECT_TRUE(is_signal(root.right().data(), cs::signal_types::asi_));
	EXPECT_TRUE(is_id(root.right().left().data(), "b"));
	EXPECT_TRUE(is_id(root.right().right().data(), "c"));
}

// ---------------------------------------------------------------------------
// Ternary / Conditional
// ---------------------------------------------------------------------------

TEST(parser_ternary)
{
	// a ? b : c  →  choice_(a, pair_(b, c))
	auto tree = build_expr_tree("a ? b : c");
	auto root = tree.root();
	EXPECT_TRUE(is_signal(root.data(), cs::signal_types::choice_));
	EXPECT_TRUE(is_id(root.left().data(), "a"));
	EXPECT_TRUE(is_signal(root.right().data(), cs::signal_types::pair_));
	EXPECT_TRUE(is_id(root.right().left().data(), "b"));
	EXPECT_TRUE(is_id(root.right().right().data(), "c"));
}

// ---------------------------------------------------------------------------
// Member access
// ---------------------------------------------------------------------------

TEST(parser_member_access_dot)
{
	// a.b  →  dot_(a, b)
	auto tree = build_expr_tree("a.b");
	auto root = tree.root();
	EXPECT_TRUE(is_signal(root.data(), cs::signal_types::dot_));
	EXPECT_TRUE(is_id(root.left().data(), "a"));
	EXPECT_TRUE(is_id(root.right().data(), "b"));
}

TEST(parser_member_access_arrow)
{
	// a->b  →  arrow_(a, b)
	auto tree = build_expr_tree("a->b");
	auto root = tree.root();
	EXPECT_TRUE(is_signal(root.data(), cs::signal_types::arrow_));
	EXPECT_TRUE(is_id(root.left().data(), "a"));
	EXPECT_TRUE(is_id(root.right().data(), "b"));
}

// ---------------------------------------------------------------------------
// Function call
// ---------------------------------------------------------------------------

TEST(parser_function_call_no_args)
{
	// f()  →  fcall_(f, arglist)
	auto tree = build_expr_tree("f()");
	auto root = tree.root();
	EXPECT_TRUE(is_signal(root.data(), cs::signal_types::fcall_));
	EXPECT_TRUE(is_id(root.left().data(), "f"));
}

TEST(parser_function_call_with_args)
{
	// f(a, b)  →  fcall_(f, arglist(a, b))
	auto tree = build_expr_tree("f(a, b)");
	auto root = tree.root();
	EXPECT_TRUE(is_signal(root.data(), cs::signal_types::fcall_));
	EXPECT_TRUE(is_id(root.left().data(), "f"));
}

// ---------------------------------------------------------------------------
// Subscript / array access
// ---------------------------------------------------------------------------

TEST(parser_subscript)
{
	// a[i]  →  access_(a, i)
	auto tree = build_expr_tree("a[i]");
	auto root = tree.root();
	EXPECT_TRUE(is_signal(root.data(), cs::signal_types::access_));
	EXPECT_TRUE(is_id(root.left().data(), "a"));
}

// ---------------------------------------------------------------------------
// Increment / Decrement
// ---------------------------------------------------------------------------

TEST(parser_prefix_inc)
{
	// ++a → inc_(null, a) after tree building
	auto tree = build_expr_tree("++a");
	auto root = tree.root();
	EXPECT_TRUE(is_signal(root.data(), cs::signal_types::inc_));
}

TEST(parser_postfix_inc)
{
	// a++ → inc_(a, null) after tree building
	auto tree = build_expr_tree("a++");
	auto root = tree.root();
	EXPECT_TRUE(is_signal(root.data(), cs::signal_types::inc_));
}

// ---------------------------------------------------------------------------
// Complex nested expressions
// ---------------------------------------------------------------------------

TEST(parser_complex_nested)
{
	// (a + b) * (c - d)  →  mul_(add_(a,b), sub_(c,d))
	auto tree = build_expr_tree("(a + b) * (c - d)");
	auto root = tree.root();
	EXPECT_TRUE(is_signal(root.data(), cs::signal_types::mul_));
	EXPECT_TRUE(is_signal(root.left().data(), cs::signal_types::add_));
	EXPECT_TRUE(is_signal(root.right().data(), cs::signal_types::sub_));
}

TEST(parser_deeply_nested)
{
	// a + b + c + d + e → left-associative nesting
	auto tree = build_expr_tree("a + b + c + d + e");
	auto root = tree.root();
	EXPECT_TRUE(is_signal(root.data(), cs::signal_types::add_));
	// Traverse: should be add_(add_(add_(add_(a,b),c),d),e)
	int depth = 0;
	for (auto it = root; it.usable() && is_signal(it.data(), cs::signal_types::add_);
	     it = it.left()) {
		++depth;
	}
	EXPECT_TRUE(depth == 4);
}
