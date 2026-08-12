#include <covscript/cni.hpp>
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
