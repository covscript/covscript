#include <covscript/cni.hpp>
#include <type_traits>
#include "test_helpers.hpp"

// =============================================================================
// CNI reference-parameter conversion tests (F6).
//
// A CNI taking a non-const reference (T&) whose script-side value needs a
// value conversion materializes the converted value into the temporary
// argument slot. It must not mark the slot constant afterwards, or the
// non-const access needed to hand the reference out would be rejected.
// =============================================================================

TEST(cni_nonconst_ref_conversion)
{
	cs::var fn = cs::make_cni([](double &d) { d = d * 2.0; });
	cs::vector args;
	args.push_back(cs::var::make<cs::numeric>(21));
	EXPECT_NO_THROW(fn.const_val<cs::callable>().call(args));
	EXPECT_TRUE(args[0].is_type_of<double>());
	EXPECT_TRUE(args[0].const_val<double>() == 42.0);
}

TEST(cni_const_ref_conversion)
{
	double seen = 0;
	cs::var fn = cs::make_cni([&](const double &d) { seen = d; });
	cs::vector args;
	args.push_back(cs::var::make<cs::numeric>(7));
	EXPECT_NO_THROW(fn.const_val<cs::callable>().call(args));
	EXPECT_TRUE(seen == 7.0);
}

// =============================================================================
// cni must satisfy the Rule of Five: it owns a heap holder via a raw pointer,
// so the implicit shallow copy/move assignment would double-free on assignment
// or swap. Copying, moving and swapping two native functions must keep both
// holders distinct and functional.
// =============================================================================

TEST(cni_rule_of_five_copy_move_swap)
{
	static_assert(std::is_nothrow_move_constructible<cs::cni>::value,
	              "cni must remain nothrow-movable for CovScript's inline storage");
	static_assert(std::is_nothrow_move_assignable<cs::cni>::value,
	              "cni must remain nothrow-move-assignable for CovScript's inline storage");

	cs::cni add1([](double x) -> double { return x + 1; });
	cs::cni mul2([](double x) -> double { return x * 2; });

	auto call = [](const cs::cni &fn, double x) -> cs::numeric
	{
		cs::vector args{cs::var::make<cs::numeric>(x)};
		return fn(args).const_val<cs::numeric>();
	};

	// Copy assignment deep-clones (must not alias the source holder).
	cs::cni c = add1;
	c = mul2;
	EXPECT_TRUE(call(c, 10) == 20); // c: mul2 (copy-assigned)

	// Move construction and move assignment.
	cs::cni d(std::move(c));
	EXPECT_TRUE(c.argument_count() == 0);
	cs::vector moved_args{cs::var::make<cs::numeric>(10)};
	EXPECT_THROW(c(moved_args), cs::runtime_error);
	cs::cni e([](double x) -> double { return x - 1; });
	e = std::move(d);
	EXPECT_TRUE(d.argument_count() == 0);
	EXPECT_TRUE(call(e, 10) == 20); // e: mul2 (moved through d)

	// Swap exercises move construction + move assignment on the holders.
	cs::cni f = add1;
	cs::cni g = mul2;
	std::swap(f, g);
	EXPECT_TRUE(call(f, 10) == 20); // f: mul2 after swap
	EXPECT_TRUE(call(g, 10) == 11); // g: add1 after swap
}
