#include <exception>
#include <stdexcept>
#include <string>
#include "test_helpers.hpp"

TEST(context_pointer_var_accesses_context_extension)
{
	auto ctx = make_context();
	cs::process_run_scope scope(ctx);
	ctx->cmd_args = cs::var::make<cs::array>();
	cs::var value = cs::var::make<cs::context_type *>(ctx.get());
	EXPECT_TRUE(&value.get_ext() == &cs_impl::context_ext);
	cs::vector args{value};
	cs::var result = value.get_ext()->get_var("cmd_args").const_val<cs::callable>().call(args);
	EXPECT_TRUE(result.is_type_of<cs::array>());
}

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

// A type whose construction throws: make/make_protect must release the
// allocated proxy instead of leaking it when construct_store throws.
struct ThrowOnConstruct {
	ThrowOnConstruct()
	{
		throw std::runtime_error("ctor boom");
	}
};

// SVO type that counts live instances, to verify the moved-from object of a
// move (whose source dispatcher is nulled by move_store) is still destroyed.
struct CountedSVO {
	static inline int live = 0;
	CountedSVO()
	{
		++live;
	}
	CountedSVO(const CountedSVO &)
	{
		++live;
	}
	CountedSVO(CountedSVO &&) noexcept
	{
		++live;
	}
	CountedSVO &operator=(const CountedSVO &) = default;
	CountedSVO &operator=(CountedSVO &&) noexcept = default;
	~CountedSVO()
	{
		--live;
	}
};

// Heap-stored type whose construction throws: the block allocated before the
// placement construction must be deallocated, not leaked.
struct BigThrowOnConstruct {
	char pad[64];
	BigThrowOnConstruct()
	{
		throw std::runtime_error("ctor boom");
	}
};

static_assert(sizeof(BigThrowOnConstruct) > CS_VAR_SVO_ALIGN - 16,
              "BigThrowOnConstruct must use the heap storage path");

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

TEST(variable_move_assign_sso_string)
{
	// The rvalue assignment must move-construct into the SVO buffer, not
	// bitwise-copy it, or SSO strings keep a self-pointer into the source.
	using bv_t = cs_impl::basic_var<CS_VAR_SVO_ALIGN>;
	bv_t target = bv_t::make<std::string>("old");
	bv_t source = bv_t::make<std::string>("new");
	target = std::move(source);
	EXPECT_TRUE(target.is_type_of<std::string>());
	EXPECT_TRUE(std::string(target.to_string()) == "new");
	bv_t long_target = bv_t::make<std::string>("a");
	bv_t long_source = bv_t::make<std::string>(
	    "0123456789012345678901234567890123456789");
	long_target = std::move(long_source);
	EXPECT_TRUE(std::string(long_target.to_string()) ==
	            "0123456789012345678901234567890123456789");
}

TEST(variable_make_ctor_throw_no_leak)
{
	// construct_store throws inside any::make; the proxy must be returned to
	// the pool rather than leaked, and the exception must propagate cleanly.
	EXPECT_THROW(cs::var::make<ThrowOnConstruct>(), std::exception);
	EXPECT_THROW(cs::var::make_protect<ThrowOnConstruct>(), std::exception);
	EXPECT_THROW(cs::var::make_constant<ThrowOnConstruct>(), std::exception);
	EXPECT_THROW(cs::var::make_single<ThrowOnConstruct>(), std::exception);
	EXPECT_THROW(cs::var(ThrowOnConstruct{}), std::exception);
}

TEST(variable_svo_move_destroys_moved_from)
{
	using bv_t = cs_impl::basic_var<CS_VAR_SVO_ALIGN>;
	bv_t target = bv_t::make<CountedSVO>();
	bv_t source = bv_t::make<CountedSVO>();
	EXPECT_TRUE(CountedSVO::live == 2);
	target = std::move(source);
	// The destination was move-constructed (live+1) and the source's moved-from
	// object must have been destroyed (live-1); the old destination value was
	// destroyed too, so exactly one live instance remains.
	EXPECT_TRUE(CountedSVO::live == 1);
	bv_t moved = std::move(target);
	EXPECT_TRUE(CountedSVO::live == 1);
}

TEST(variable_heap_make_ctor_throw_no_leak)
{
	// A throwing constructor for a heap-stored type must release the block that
	// was allocated before placement construction, and the proxy must be freed.
	EXPECT_THROW(cs::var::make<BigThrowOnConstruct>(), std::exception);
	EXPECT_THROW(cs::var::make_protect<BigThrowOnConstruct>(), std::exception);
	using bv_t = cs_impl::basic_var<CS_VAR_SVO_ALIGN>;
	EXPECT_THROW(bv_t::make<BigThrowOnConstruct>(), std::exception);
}
