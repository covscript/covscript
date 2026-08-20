// =============================================================================
// Unit tests for code generation (sources/compiler/codegen.cpp).
// Tests constant folding and operator simplification.
// =============================================================================
#include "test_helpers.hpp"

// =============================================================================
// Constant folding
// =============================================================================
TEST(const_fold_integer_add)
{
	EXPECT_EQ(run_script("system.out.println(1 + 2)"), "3\n");
}

TEST(const_fold_integer_sub)
{
	EXPECT_EQ(run_script("system.out.println(10 - 3)"), "7\n");
}

TEST(const_fold_integer_mul)
{
	EXPECT_EQ(run_script("system.out.println(4 * 5)"), "20\n");
}

TEST(const_fold_integer_div)
{
	EXPECT_EQ(run_script("system.out.println(10 / 2)"), "5\n");
}

TEST(const_fold_string_concat)
{
	EXPECT_EQ(run_script("system.out.println(\"hello\" + \" \" + \"world\")"), "hello world\n");
}

TEST(const_fold_boolean_not)
{
	EXPECT_EQ(run_script("system.out.println(not false)"), "true\n");
}

TEST(const_fold_comparison)
{
	EXPECT_EQ(run_script("system.out.println(1 < 2)"), "true\n");
}

TEST(const_fold_negation)
{
	EXPECT_EQ(run_script("system.out.println(-42)"), "-42\n");
}

// =============================================================================
// Arithmetic with mixed types
// =============================================================================
TEST(arithmetic_integer_float)
{
	EXPECT_EQ(run_script("system.out.println(1 + 2.5)"), "3.5\n");
}

TEST(arithmetic_precedence)
{
	EXPECT_EQ(run_script("system.out.println(2 + 3 * 4)"), "14\n");
}

TEST(arithmetic_parens)
{
	EXPECT_EQ(run_script("system.out.println((2 + 3) * 4)"), "20\n");
}

// =============================================================================
// String operations
// =============================================================================
TEST(string_repeat)
{
	EXPECT_EQ(run_script("system.out.println(\"ab\" * 3)"), "ababab\n");
}

TEST(string_comparison)
{
	EXPECT_EQ(run_script("system.out.println(\"abc\" < \"abd\")"), "true\n");
}

// =============================================================================
// Boolean logic
// =============================================================================
TEST(boolean_and)
{
	EXPECT_EQ(run_script("system.out.println(true and true)"), "true\n");
	EXPECT_EQ(run_script("system.out.println(true and false)"), "false\n");
}

TEST(boolean_or)
{
	EXPECT_EQ(run_script("system.out.println(false or true)"), "true\n");
	EXPECT_EQ(run_script("system.out.println(false or false)"), "false\n");
}

TEST(variable_arithmetic)
{
	EXPECT_EQ(run_script("var a = 10; var b = 20; system.out.println(a + b)"), "30\n");
}

TEST(variable_string_concat)
{
	EXPECT_EQ(run_script("var a = \"hello\"; var b = \" world\"; system.out.println(a + b)"), "hello world\n");
}

// =============================================================================
// Conditional expressions
// =============================================================================
TEST(ternary_true)
{
	EXPECT_EQ(run_script("var x = true ? 1 : 2; system.out.println(x)"), "1\n");
}

TEST(ternary_false)
{
	EXPECT_EQ(run_script("var x = false ? 1 : 2; system.out.println(x)"), "2\n");
}

// =============================================================================
// Edge cases
// =============================================================================
TEST(single_value_expression)
{
	EXPECT_EQ(run_script("system.out.println(42)"), "42\n");
}

TEST(nested_expressions)
{
	EXPECT_EQ(run_script("system.out.println((1 + 2) * (3 + 4))"), "21\n");
}

TEST(string_escape_sequences)
{
	EXPECT_EQ(run_script("system.out.println(\"hello\\nworld\")"), "hello\nworld\n");
}

TEST(divide_by_zero)
{
	EXPECT_ANY_THROW(run_script("system.out.println(1 / 0)"));
}

// =============================================================================
// Operator simplification
// =============================================================================
TEST(double_negation)
{
	EXPECT_EQ(run_script("var x = true; system.out.println(not (not x))"), "true\n");
}

TEST(deep_nesting)
{
	EXPECT_EQ(run_script("system.out.println((1 + (2 + (3 + (4 + 5)))) )"), "15\n");
}

TEST(subtract_negation)
{
	EXPECT_EQ(run_script("system.out.println(5 - (-3))"), "8\n");
}

TEST(mod_operation)
{
	EXPECT_EQ(run_script("system.out.println(10 % 3)"), "1\n");
}
