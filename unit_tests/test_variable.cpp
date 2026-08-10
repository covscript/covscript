#include <exception>
#include <stdexcept>
#include <string>
#include "test_helpers.hpp"

// =============================================================================
// Tests for basic_var::copy_store exception safety (F5 remainder).
//
// copy_store must give the strong exception guarantee: if copying the value
// throws, the destination keeps its previous value instead of being left
// empty. The real trigger is any::assign(obj, raw=true) (the `:=` raw-assign
// path that reuses the proxy storage).
// =============================================================================

namespace {

// Small type stored via the SVO path (sizeof <= CS_VAR_SVO_ALIGN - 16).
struct ThrowOnCopy {
	int value = 0;
	ThrowOnCopy() = default;
	explicit ThrowOnCopy(int v) : value(v) {}
	ThrowOnCopy(const ThrowOnCopy &) { throw std::runtime_error("copy boom"); }
	ThrowOnCopy(ThrowOnCopy &&) noexcept = default;
	ThrowOnCopy &operator=(const ThrowOnCopy &) = default;
	ThrowOnCopy &operator=(ThrowOnCopy &&) noexcept = default;
};

static_assert(sizeof(ThrowOnCopy) <= CS_VAR_SVO_ALIGN - 16,
              "ThrowOnCopy must fit in the SVO buffer");

// Large type stored on the heap (sizeof > CS_VAR_SVO_ALIGN - 16).
struct BigThrowOnCopy {
	char pad[64];
	int value = 0;
	BigThrowOnCopy() = default;
	explicit BigThrowOnCopy(int v) : value(v) {}
	BigThrowOnCopy(const BigThrowOnCopy &) { throw std::runtime_error("copy boom"); }
	BigThrowOnCopy(BigThrowOnCopy &&) noexcept = default;
	BigThrowOnCopy &operator=(const BigThrowOnCopy &) = default;
	BigThrowOnCopy &operator=(BigThrowOnCopy &&) noexcept = default;
};

static_assert(sizeof(BigThrowOnCopy) > CS_VAR_SVO_ALIGN - 16,
              "BigThrowOnCopy must use the heap storage path");

// Non-throwing heap type used to verify the success path still works.
struct BigCopy {
	char pad[64];
	std::string value;
	BigCopy() = default;
	explicit BigCopy(std::string v) : value(std::move(v)) {}
};

static_assert(sizeof(BigCopy) > CS_VAR_SVO_ALIGN - 16,
              "BigCopy must use the heap storage path");

} // namespace

TEST(variable_copy_store_normal_svo)
{
	cs::var target = cs::var::make<int>(1);
	cs::var source = cs::var::make<int>(2);
	target.assign(source, true);
	EXPECT_TRUE(target.const_val<int>() == 2);
}

TEST(variable_copy_store_normal_heap)
{
	cs::var target = cs::var::make<BigCopy>(std::string("old"));
	cs::var source = cs::var::make<BigCopy>(std::string("new"));
	target.assign(source, true);
	EXPECT_TRUE(target.const_val<BigCopy>().value == "new");
}

TEST(variable_copy_store_sso_string)
{
	// std::string is small enough for the SVO buffer; its short-string
	// optimization stores a self-referential pointer, so the commit must
	// move-construct rather than bitwise-copy the buffer.
	cs::var target = cs::var::make<std::string>("old");
	cs::var source = cs::var::make<std::string>("new");
	target.assign(source, true);
	EXPECT_TRUE(target.const_val<std::string>() == "new");
	EXPECT_TRUE(source.const_val<std::string>() == "new");
	// A long payload heap-allocates internally; bitwise transfer would
	// alias two strings to the same buffer and double-free on destroy.
	cs::var long_target = cs::var::make<std::string>("a");
	cs::var long_source = cs::var::make<std::string>(
	    "0123456789012345678901234567890123456789");
	long_target.assign(long_source, true);
	EXPECT_TRUE(long_target.const_val<std::string>() ==
	            "0123456789012345678901234567890123456789");
	EXPECT_TRUE(long_source.const_val<std::string>() ==
	            "0123456789012345678901234567890123456789");
}

TEST(variable_copy_store_svo_strong_guarantee)
{
	cs::var target = cs::var::make<ThrowOnCopy>(1);
	cs::var source = cs::var::make<ThrowOnCopy>(2);
	EXPECT_THROW(target.assign(source, true), std::exception);
	EXPECT_TRUE(target.const_val<ThrowOnCopy>().value == 1);
}

TEST(variable_copy_store_heap_strong_guarantee)
{
	cs::var target = cs::var::make<BigThrowOnCopy>(1);
	cs::var source = cs::var::make<BigThrowOnCopy>(2);
	EXPECT_THROW(target.assign(source, true), std::exception);
	EXPECT_TRUE(target.const_val<BigThrowOnCopy>().value == 1);
}
