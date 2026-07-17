/*
 * CovScript Fiber API Unit Tests
 *
 * Licensed under the Apache License, Version 2.0
 * Copyright (C) 2017-2026 Michael Lee(李登淳)
 */

#include <covscript/covscript.hpp>
#include "test_harness.hpp"
#include <chrono>
#include <thread>

// =============================================================================
// Helper: run a fiber to completion
// =============================================================================
inline void run_fiber(cs::fiber_t f)
{
	while (f->get_state() != cs::fiber_state::finished)
		cs::fiber::resume(f);
}

// =============================================================================
// fiber::within
// =============================================================================
TEST(fiber_within_main_context)
{
	EXPECT_FALSE(cs::fiber::within());
}

TEST(fiber_within_inside_fiber)
{
	bool inside = false;
	auto f = cs::fiber::create_native([&]() -> cs::var {
		inside = cs::fiber::within();
		return {};
	});
	run_fiber(f);
	EXPECT_TRUE(inside);
}

// =============================================================================
// fiber::current
// =============================================================================
TEST(fiber_current_main_context)
{
	EXPECT_TRUE(cs::fiber::current() == nullptr);
}

TEST(fiber_current_inside_fiber)
{
	cs::fiber_type const *self = nullptr;
	auto f = cs::fiber::create_native([&]() -> cs::var {
		self = cs::fiber::current();
		return {};
	});
	run_fiber(f);
	EXPECT_FALSE(self == nullptr);
	EXPECT_TRUE(self->get_state() == cs::fiber_state::finished);
}

// =============================================================================
// fiber::sleep_for
// =============================================================================
TEST(fiber_sleep_for_throws_outside_fiber)
{
	EXPECT_THROW(cs::fiber::sleep_for(100), cs::lang_error);
}

TEST(fiber_sleep_for_basic_timing)
{
	auto start = std::chrono::steady_clock::now();
	auto f = cs::fiber::create_native([&]() -> cs::var {
		cs::fiber::sleep_for(50);
		return {};
	});
	run_fiber(f);
	auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
	                   std::chrono::steady_clock::now() - start).count();
	EXPECT_TRUE(elapsed >= 40);
}

TEST(fiber_sleep_for_zero)
{
	auto f = cs::fiber::create_native([&]() -> cs::var {
		cs::fiber::sleep_for(0);
		return {};
	});
	cs::fiber::resume(f);
	// After sleep_for(0), fiber yields — should be sleeping or suspended
	auto state = f->get_state();
	EXPECT_TRUE(state == cs::fiber_state::sleeping || state == cs::fiber_state::suspended);
	cs::fiber::resume(f);
	EXPECT_TRUE(f->get_state() == cs::fiber_state::finished);
}

// =============================================================================
// sleeping counts as suspended
// =============================================================================
TEST(fiber_sleeping_is_suspended)
{
	auto f = cs::fiber::create_native([&]() -> cs::var {
		cs::fiber::sleep_for(5000);
		return {};
	});
	cs::fiber::resume(f);
	EXPECT_TRUE(f->get_state() == cs::fiber_state::sleeping);
}

// =============================================================================
// create_native: C++ function as fiber
// =============================================================================
TEST(create_native_basic)
{
	static int call_count = 0;
	call_count = 0;
	auto f = cs::fiber::create_native([]() -> cs::var {
		call_count++;
		return cs::var(42);
	});
	run_fiber(f);
	EXPECT_TRUE(call_count == 1);
	EXPECT_TRUE(f->return_value().to_integer() == 42);
}

TEST(create_native_void_function)
{
	static int void_count = 0;
	void_count = 0;
	auto f = cs::fiber::create_native([]() -> cs::var {
		void_count++;
		return {};
	});
	run_fiber(f);
	EXPECT_TRUE(void_count == 1);
}

// =============================================================================
// create_native with bound arguments
// =============================================================================
static int test_add(int a, int b)
{
	return a + b;
}

TEST(create_native_with_bound_args)
{
	auto f = cs::fiber::create_native([]() -> cs::var {
		return cs::var(test_add(3, 4));
	});
	run_fiber(f);
	EXPECT_TRUE(f->return_value().to_integer() == 7);
}

TEST(create_native_void_lambda)
{
	static bool called = false;
	called = false;
	auto f = cs::fiber::create_native([&]() -> cs::var {
		called = true;
		return {};
	});
	run_fiber(f);
	EXPECT_TRUE(called);
}

// =============================================================================
// yield and sleep_for independence
// =============================================================================
TEST(fiber_yield_after_sleep)
{
	int step = 0;
	auto f = cs::fiber::create_native([&]() -> cs::var {
		cs::fiber::sleep_for(1);
		step = 1;
		cs::fiber::yield();
		step = 2;
		return {};
	});

	cs::fiber::resume(f);
	// Poll until sleep_for expires
	while (f->get_state() == cs::fiber_state::sleeping)
		cs::fiber::resume(f);
	EXPECT_TRUE(step == 1);
	EXPECT_TRUE(f->get_state() == cs::fiber_state::suspended);

	cs::fiber::resume(f);
	EXPECT_TRUE(step == 2);
	EXPECT_TRUE(f->get_state() == cs::fiber_state::finished);
}

// =============================================================================
// Exception propagation
// =============================================================================
TEST(fiber_exception_through_sleep)
{
	auto f = cs::fiber::create_native([]() -> cs::var {
		cs::fiber::sleep_for(10);
		throw std::runtime_error("test error");
		return {};
	});
	cs::fiber::resume(f);
	bool caught = false;
	while (!caught) {
		try {
			cs::fiber::resume(f);
		}
		catch (const std::runtime_error &) {
			caught = true;
		}
	}
	EXPECT_TRUE(caught);
	EXPECT_TRUE(f->get_state() == cs::fiber_state::finished);
}

// =============================================================================
// Nested fibers
// =============================================================================
TEST(fiber_nested_sleep)
{
	int outer_step = 0;
	int inner_step = 0;

	auto f_outer = cs::fiber::create_native([&]() -> cs::var {
		outer_step = 1;

		auto f_inner = cs::fiber::create_native([&]() -> cs::var {
			inner_step = 1;
			cs::fiber::sleep_for(10);
			inner_step = 2;
			return {};
		});

		cs::fiber::resume(f_inner);
		outer_step = 2;
		while (f_inner->get_state() != cs::fiber_state::finished)
			cs::fiber::resume(f_inner);
		outer_step = 3;
		return {};
	});

	run_fiber(f_outer);
	EXPECT_TRUE(outer_step == 3);
	EXPECT_TRUE(inner_step == 2);
}

// =============================================================================
// State transitions
// =============================================================================
TEST(fiber_state_ready_to_finished)
{
	auto f = cs::fiber::create_native([]() -> cs::var { return cs::var(1); });
	EXPECT_TRUE(f->get_state() == cs::fiber_state::ready);
	run_fiber(f);
	EXPECT_TRUE(f->get_state() == cs::fiber_state::finished);
}

// =============================================================================
// Stress: many fibers with short sleeps
// =============================================================================
TEST(fiber_many_short_sleeps)
{
	constexpr int N = 20;
	int completed = 0;

	for (int i = 0; i < N; i++) {
		auto f = cs::fiber::create_native([&]() -> cs::var {
			cs::fiber::sleep_for(1);
			return {};
		});
		while (f->get_state() != cs::fiber_state::finished)
			cs::fiber::resume(f);
		completed++;
	}
	EXPECT_TRUE(completed == N);
}
