#include "test_helpers.hpp"
#include <covscript/impl/compiler.hpp>

// =============================================================================
// Friend accessor for testing private compiler methods
// Must be in namespace cs to match the friend declaration in compiler.hpp
// =============================================================================
namespace cs {
	struct compiler_test_accessor {
		static void trim_expr(compiler_type &compiler,
		                      tree_type<token_base *> &tree,
		                      tree_type<token_base *>::iterator it,
		                      compiler_type::trim_type do_trim)
		{
			compiler.trim_expr(tree, it, do_trim);
		}

		static void optimize_expression(compiler_type &compiler,
		                                tree_type<token_base *> &tree)
		{
			compiler.optimize_expression(tree);
		}

		// Convenience: run trim_expr on tree root with normal trim type
		static bool trim_throws(compiler_type &compiler, tree_type<token_base *> &tree)
		{
			try {
				trim_expr(compiler, tree, tree.root(), compiler_type::trim_type::normal);
				return false;
			}
			catch (const compile_error &) {
				return true;
			}
		}

		// Convenience: run optimize_expression and return whether it threw
		static bool opt_throws(compiler_type &compiler, tree_type<token_base *> &tree)
		{
			try {
				optimize_expression(compiler, tree);
				return false;
			}
			catch (const compile_error &) {
				return true;
			}
		}
	};
}

// =============================================================================
// trim_expr / opt_expr Validation Tests
// =============================================================================

// ---------------------------------------------------------------------------
// Helpers: construct manual trees and run private compiler methods
// ---------------------------------------------------------------------------

// These helpers allocate tokens with plain `new` for hand-built trees. The
// trees do not own their token data, and trim_expr/opt_expr may swap nodes for
// arena-owned tokens, so track the manual allocations here and free exactly
// those (via flush_tokens) at the end of each test instead of walking the tree.
static std::vector<cs::token_base *> g_manual_tokens;

static cs::token_id *T_id(const std::string &name)
{
	auto *t = new cs::token_id(name);
	g_manual_tokens.push_back(t);
	return t;
}
static cs::token_signal *T_sig(cs::signal_types s)
{
	auto *t = new cs::token_signal(s);
	g_manual_tokens.push_back(t);
	return t;
}

static void flush_tokens()
{
	for (auto *t : g_manual_tokens)
		delete t;
	g_manual_tokens.clear();
}

// Following the same pattern as compiler_type::build_tree:
//   1. emplace_root_left  → create root signal node
//   2. emplace_left_left  → create left child (even if nullptr — tree node exists but data is null)
//   3. emplace_right_right → create right child
// This ensures tree iterators are always "usable" even when the operand is null.

static cs::tree_type<cs::token_base *> make_binary(cs::signal_types op,
        cs::token_base *left,
        cs::token_base *right)
{
	cs::tree_type<cs::token_base *> tree;
	tree.emplace_root_left(tree.root(), T_sig(op));
	tree.emplace_left_left(tree.root(), left);
	tree.emplace_right_right(tree.root(), right);
	return tree;
}

static cs::tree_type<cs::token_base *> make_unary(cs::signal_types op,
        cs::token_base *operand)
{
	cs::tree_type<cs::token_base *> tree;
	tree.emplace_root_left(tree.root(), T_sig(op));
	tree.emplace_left_left(tree.root(), nullptr);  // left is always null for unary prefix
	tree.emplace_right_right(tree.root(), operand);
	return tree;
}

static bool trim_throws(cs::tree_type<cs::token_base *> &tree)
{
	auto ctx = make_context();
	return cs::compiler_test_accessor::trim_throws(*ctx->compiler, tree);
}

static bool opt_throws(cs::tree_type<cs::token_base *> &tree)
{
	auto ctx = make_context();
	return cs::compiler_test_accessor::opt_throws(*ctx->compiler, tree);
}

// Helper to avoid rvalue binding issue: evaluate on a local lvalue
#define TRIM_THROWS(expr) ([&]{ auto _t = (expr); return trim_throws(_t); }())
#define OPT_THROWS(expr)  ([&]{ auto _t = (expr); return opt_throws(_t); }())

// =============================================================================
// SECTION 1: Integration tests — full pipeline via build_expr
// =============================================================================

TEST(integration_valid_expressions)
{
	// Unary
	EXPECT_NO_THROW(build_expr_tree("new Test"));
	EXPECT_NO_THROW(build_expr_tree("not flag"));
	EXPECT_NO_THROW(build_expr_tree("-x"));
	EXPECT_NO_THROW(build_expr_tree("*p"));

	// Binary arithmetic
	EXPECT_NO_THROW(build_expr_tree("a + b"));
	EXPECT_NO_THROW(build_expr_tree("a - b"));
	EXPECT_NO_THROW(build_expr_tree("a * b"));
	EXPECT_NO_THROW(build_expr_tree("a / b"));
	EXPECT_NO_THROW(build_expr_tree("a ^ b"));

	// Comparison, logical
	EXPECT_NO_THROW(build_expr_tree("a < b"));
	EXPECT_NO_THROW(build_expr_tree("a == b"));
	EXPECT_NO_THROW(build_expr_tree("a and b"));

	// Assignment
	EXPECT_NO_THROW(build_expr_tree("a = b"));
	EXPECT_NO_THROW(build_expr_tree("a += b"));

	// Ternary, member access, subscript
	EXPECT_NO_THROW(build_expr_tree("cond ? x : y"));
	EXPECT_NO_THROW(build_expr_tree("obj.member"));
	EXPECT_NO_THROW(build_expr_tree("obj->member"));
	EXPECT_NO_THROW(build_expr_tree("arr[i]"));

	// Inc/dec
	EXPECT_NO_THROW(build_expr_tree("++i"));
	EXPECT_NO_THROW(build_expr_tree("i++"));
}

TEST(integration_complex_expressions)
{
	EXPECT_NO_THROW(build_expr_tree("(a + b) * (c - d) / (e % f)"));
	EXPECT_NO_THROW(build_expr_tree("a < b and c > d or e == f"));
	EXPECT_NO_THROW(build_expr_tree("a.b.c"));
}

// =============================================================================
// SECTION 2: Integration tests — malformed expressions via build_expr
// =============================================================================

TEST(malformed_unary_without_operand)
{
	// "new" without operand: gen_tree's single-token path produces a tree
	// without children, causing a tree-level runtime_error before trim_expr
	// can fire its compile_error. Both indicate a malformed expression.
	EXPECT_THROW(build_expr_tree("new"), std::exception);
	EXPECT_THROW(build_expr_tree("not"), std::exception);
	EXPECT_THROW(build_expr_tree("typeid"), std::exception);
	EXPECT_THROW(build_expr_tree("&"), std::exception);
}

TEST(malformed_incomplete_binary)
{
	EXPECT_THROW(build_expr_tree("a +"), cs::compile_error);
	EXPECT_THROW(build_expr_tree("a -"), cs::compile_error);
	EXPECT_THROW(build_expr_tree("a *"), cs::compile_error);
	EXPECT_THROW(build_expr_tree("a /"), cs::compile_error);
	EXPECT_THROW(build_expr_tree("a <"), cs::compile_error);
	EXPECT_THROW(build_expr_tree("a =="), cs::compile_error);
	EXPECT_THROW(build_expr_tree("a and"), cs::compile_error);
	EXPECT_THROW(build_expr_tree("a ="), cs::compile_error);
}

TEST(malformed_member_access)
{
	EXPECT_THROW(build_expr_tree("a."), cs::compile_error);
	EXPECT_THROW(build_expr_tree("a->"), cs::compile_error);
}

TEST(malformed_ternary)
{
	EXPECT_THROW(build_expr_tree("cond ?"), cs::compile_error);
}

TEST(malformed_subscript)
{
	EXPECT_THROW(build_expr_tree("a["), cs::compile_error);
}

// =============================================================================
// SECTION 3: Low-level tests — manually constructed edge-case trees
// =============================================================================

// --- Unary prefix: reject non-null left operand ---

TEST(low_unary_rejects_non_null_left)
{
	EXPECT_TRUE(TRIM_THROWS(make_binary(cs::signal_types::new_, T_id("x"), T_id("Test"))));
	EXPECT_TRUE(TRIM_THROWS(make_binary(cs::signal_types::typeid_, T_id("x"), T_id("y"))));
	EXPECT_TRUE(TRIM_THROWS(make_binary(cs::signal_types::not_, T_id("x"), T_id("flag"))));
	EXPECT_TRUE(TRIM_THROWS(make_binary(cs::signal_types::addr_, T_id("x"), T_id("var"))));
	flush_tokens();
}

// --- Unary prefix: reject null right operand ---

TEST(low_unary_rejects_null_right)
{
	EXPECT_TRUE(TRIM_THROWS(make_unary(cs::signal_types::new_, nullptr)));
	EXPECT_TRUE(TRIM_THROWS(make_unary(cs::signal_types::not_, nullptr)));
	EXPECT_TRUE(TRIM_THROWS(make_unary(cs::signal_types::typeid_, nullptr)));
	flush_tokens();
}

// --- Ambiguous sub_/mul_: unary→minus_/escape_ conversion ---

TEST(low_sub_unary_becomes_minus)
{
	auto tree = make_unary(cs::signal_types::sub_, T_id("x"));
	EXPECT_FALSE(trim_throws(tree));
	EXPECT_TRUE(is_signal(tree.root().data(), cs::signal_types::minus_));
	flush_tokens();
}

TEST(low_mul_unary_becomes_escape)
{
	auto tree = make_unary(cs::signal_types::mul_, T_id("p"));
	EXPECT_FALSE(trim_throws(tree));
	EXPECT_TRUE(is_signal(tree.root().data(), cs::signal_types::escape_));
	flush_tokens();
}

TEST(low_ambiguous_unary_rejects_null)
{
	EXPECT_TRUE(TRIM_THROWS(make_unary(cs::signal_types::sub_, nullptr)));
	EXPECT_TRUE(TRIM_THROWS(make_unary(cs::signal_types::mul_, nullptr)));
	flush_tokens();
}

TEST(low_ambiguous_binary_rejects_null)
{
	EXPECT_TRUE(TRIM_THROWS(make_binary(cs::signal_types::sub_, T_id("a"), nullptr)));
	EXPECT_TRUE(TRIM_THROWS(make_binary(cs::signal_types::mul_, T_id("a"), nullptr)));
	flush_tokens();
}

// --- minus_/escape_ reject null operand ---

TEST(low_minus_escape_rejects_null)
{
	EXPECT_TRUE(TRIM_THROWS(make_unary(cs::signal_types::minus_, nullptr)));
	EXPECT_TRUE(TRIM_THROWS(make_unary(cs::signal_types::escape_, nullptr)));
	flush_tokens();
}

// --- inc_/dec_: exactly one non-null operand ---

TEST(low_inc_dec_validation)
{
	EXPECT_TRUE(TRIM_THROWS(make_binary(cs::signal_types::inc_, nullptr, nullptr)));
	EXPECT_TRUE(TRIM_THROWS(make_binary(cs::signal_types::dec_, nullptr, nullptr)));
	EXPECT_TRUE(TRIM_THROWS(make_binary(cs::signal_types::inc_, T_id("a"), T_id("b"))));
	EXPECT_TRUE(TRIM_THROWS(make_binary(cs::signal_types::dec_, T_id("a"), T_id("b"))));
	EXPECT_FALSE(TRIM_THROWS(make_unary(cs::signal_types::inc_, T_id("a"))));
	EXPECT_FALSE(TRIM_THROWS(make_unary(cs::signal_types::dec_, T_id("a"))));
	flush_tokens();
}

// --- Binary operators: null operand rejection ---

TEST(low_binary_null_operand_throws)
{
	EXPECT_TRUE(TRIM_THROWS(make_binary(cs::signal_types::add_, nullptr, T_id("b"))));
	EXPECT_TRUE(TRIM_THROWS(make_binary(cs::signal_types::add_, T_id("a"), nullptr)));
	EXPECT_TRUE(TRIM_THROWS(make_binary(cs::signal_types::div_, nullptr, nullptr)));
	EXPECT_TRUE(TRIM_THROWS(make_binary(cs::signal_types::mod_, nullptr, T_id("b"))));
	EXPECT_TRUE(TRIM_THROWS(make_binary(cs::signal_types::pow_, T_id("a"), nullptr)));
	EXPECT_TRUE(TRIM_THROWS(make_binary(cs::signal_types::equ_, nullptr, nullptr)));
	EXPECT_TRUE(TRIM_THROWS(make_binary(cs::signal_types::neq_, T_id("a"), nullptr)));
	EXPECT_TRUE(TRIM_THROWS(make_binary(cs::signal_types::abo_, nullptr, nullptr)));
	EXPECT_TRUE(TRIM_THROWS(make_binary(cs::signal_types::and_, nullptr, T_id("b"))));
	EXPECT_TRUE(TRIM_THROWS(make_binary(cs::signal_types::or_, nullptr, nullptr)));
	flush_tokens();
}

// --- Compound assignment ---

TEST(low_compound_assign_null_throws)
{
	EXPECT_TRUE(TRIM_THROWS(make_binary(cs::signal_types::addasi_, nullptr, nullptr)));
	EXPECT_TRUE(TRIM_THROWS(make_binary(cs::signal_types::mulasi_, T_id("a"), nullptr)));
	EXPECT_TRUE(TRIM_THROWS(make_binary(cs::signal_types::subasi_, nullptr, T_id("b"))));
	EXPECT_TRUE(TRIM_THROWS(make_binary(cs::signal_types::divasi_, nullptr, nullptr)));
	EXPECT_TRUE(TRIM_THROWS(make_binary(cs::signal_types::modasi_, nullptr, T_id("b"))));
	EXPECT_TRUE(TRIM_THROWS(make_binary(cs::signal_types::powasi_, T_id("a"), nullptr)));
	EXPECT_TRUE(TRIM_THROWS(make_binary(cs::signal_types::lnkasi_, nullptr, nullptr)));
	flush_tokens();
}

// --- Subscript and pair ---

TEST(low_subscript_pair_null_throws)
{
	EXPECT_TRUE(TRIM_THROWS(make_binary(cs::signal_types::access_, nullptr, T_id("i"))));
	EXPECT_TRUE(TRIM_THROWS(make_binary(cs::signal_types::access_, T_id("a"), nullptr)));
	EXPECT_TRUE(TRIM_THROWS(make_binary(cs::signal_types::pair_, nullptr, nullptr)));
	flush_tokens();
}

// --- dot_/arrow_ ---

TEST(low_dot_null_throws)
{
	EXPECT_TRUE(TRIM_THROWS(make_binary(cs::signal_types::dot_, nullptr, T_id("m"))));
	EXPECT_TRUE(TRIM_THROWS(make_binary(cs::signal_types::dot_, T_id("obj"), nullptr)));
	EXPECT_TRUE(TRIM_THROWS(make_binary(cs::signal_types::dot_, T_id("obj"), T_sig(cs::signal_types::add_))));
	flush_tokens();
}

TEST(low_arrow_null_throws)
{
	EXPECT_TRUE(TRIM_THROWS(make_binary(cs::signal_types::arrow_, nullptr, T_id("m"))));
	EXPECT_TRUE(TRIM_THROWS(make_binary(cs::signal_types::arrow_, T_id("obj"), nullptr)));
	flush_tokens();
}

// --- asi_ ---

TEST(low_assignment_null_throws)
{
	EXPECT_TRUE(TRIM_THROWS(make_binary(cs::signal_types::asi_, nullptr, T_id("v"))));
	EXPECT_TRUE(TRIM_THROWS(make_binary(cs::signal_types::asi_, T_id("a"), nullptr)));
	flush_tokens();
}

// --- choice_ null condition ---

TEST(low_ternary_null_condition_throws)
{
	// choice_(null, pair_(a, b)) — missing condition
	// Follow build_tree pattern: create root, then left child (null), then right child
	cs::tree_type<cs::token_base *> t;
	t.emplace_root_left(t.root(), T_sig(cs::signal_types::choice_));
	t.emplace_left_left(t.root(), nullptr);  // null condition
	t.emplace_right_right(t.root(), T_sig(cs::signal_types::pair_));

	// Fill in the pair_ node's children
	auto pair_node = t.root().right();
	t.emplace_left_left(pair_node, T_id("a"));
	t.emplace_right_right(pair_node, T_id("b"));

	EXPECT_TRUE(trim_throws(t));
	flush_tokens();
}

// =============================================================================
// SECTION 4: opt_expr validation
// =============================================================================

TEST(opt_binary_null_throws)
{
	EXPECT_TRUE(OPT_THROWS(make_binary(cs::signal_types::add_, nullptr, nullptr)));
	EXPECT_TRUE(OPT_THROWS(make_binary(cs::signal_types::and_, nullptr, T_id("b"))));
	EXPECT_TRUE(OPT_THROWS(make_binary(cs::signal_types::inc_, nullptr, nullptr)));
	EXPECT_TRUE(OPT_THROWS(make_binary(cs::signal_types::dec_, T_id("a"), T_id("b"))));
	flush_tokens();
}

TEST(opt_unary_null_throws)
{
	EXPECT_TRUE(OPT_THROWS(make_unary(cs::signal_types::new_, nullptr)));
	EXPECT_TRUE(OPT_THROWS(make_unary(cs::signal_types::minus_, nullptr)));
	EXPECT_TRUE(OPT_THROWS(make_unary(cs::signal_types::escape_, nullptr)));
	flush_tokens();
}

TEST(opt_member_null_throws)
{
	EXPECT_TRUE(OPT_THROWS(make_binary(cs::signal_types::dot_, nullptr, T_id("m"))));
	EXPECT_TRUE(OPT_THROWS(make_binary(cs::signal_types::asi_, nullptr, T_id("v"))));
	flush_tokens();
}
