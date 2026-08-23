#pragma once
/*
 * Covariant Test: CovScript Unit Test Framework
 *
 * Licensed under the Apache License, Version 2.0
 * Copyright (C) 2017-2026 Michael Lee(李登淳)
 *
 * A lightweight, dependency-free unit test framework providing:
 *   - TEST / TEST_F registration
 *   - EXPECT_* non-fatal assertions: record a failure and keep running, so a
 *     single test can report every failed check
 *   - ASSERT_* fatal assertions: record a failure and abort the current test
 *   - Value-printing comparisons (EXPECT_EQ shows both operands) with the
 *     exact file:line of each assertion
 *   - Per-test timing, filtering, repeating, shuffling
 *   - JUnit XML output for CI integration
 */

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <exception>
#include <fstream>
#include <functional>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <typeinfo>
#include <type_traits>
#include <utility>
#include <vector>

namespace cs_test
{
	// =========================================================================
	// Value formatting
	// =========================================================================
	template <typename T, typename = void>
	struct has_stream_op : std::false_type
	{
	};

	template <typename T>
	struct has_stream_op<T, std::void_t<decltype(std::declval<std::ostream &>() << std::declval<const T &>())>> : std::true_type
	{
	};

	template <typename T>
	inline std::string stringify(const T &val)
	{
		if constexpr (has_stream_op<T>::value)
		{
			std::ostringstream oss;
			oss << val;
			return oss.str();
		}
		else
		{
			return std::string("<") + typeid(T).name() + ">";
		}
	}

	inline std::string stringify(bool val)
	{
		return val ? "true" : "false";
	}

	inline std::string stringify(char val)
	{
		return std::string("'") + val + "'";
	}

	inline std::string stringify(const char *val)
	{
		return val ? std::string("\"") + val + "\"" : "nullptr";
	}

	inline std::string stringify(char *val)
	{
		return stringify(static_cast<const char *>(val));
	}

	inline std::string stringify(const std::string &val)
	{
		return "\"" + val + "\"";
	}

	inline std::string stringify(std::nullptr_t)
	{
		return "nullptr";
	}

	// =========================================================================
	// Exception types
	// =========================================================================
	// Thrown by helpers (e.g. run_script_expect_throw) to signal a fatal
	// failure directly; the runner records its message against the test.
	class test_failure : public std::runtime_error
	{
	   public:
		test_failure(const std::string &msg) : std::runtime_error(msg) {}
	};

	// Internal control-flow exception thrown by ASSERT_* to abort the current
	// test after the failure has already been recorded. Not an error by itself.
	class test_abort : public std::exception
	{
	   public:
		const char *what() const noexcept override
		{
			return "fatal assertion (test aborted)";
		}
	};

	// =========================================================================
	// Failure accumulation (non-fatal EXPECT support)
	// =========================================================================
	struct failure_record
	{
		std::string location; // file:line of the assertion
		std::string message;
	};

	struct test_context
	{
		std::vector<failure_record> failures;
		std::vector<std::string> traces;
	};

	// Points at the currently running test's context; nullptr between tests.
	inline test_context *&current_context()
	{
		static thread_local test_context *ctx = nullptr;
		return ctx;
	}

	inline std::string make_location(const char *file, int line)
	{
		return std::string(file) + ":" + std::to_string(line);
	}

	// Record a non-fatal failure (EXPECT_*). Execution continues.
	inline void record_failure(const char *file, int line, const std::string &message)
	{
		if (auto *ctx = current_context())
			ctx->failures.push_back({make_location(file, line), message});
		else
			throw test_failure(make_location(file, line) + ": " + message);
	}

	// Record a fatal failure (ASSERT_*) and abort the current test.
	[[noreturn]] inline void report_fatal(const char *file, int line, const std::string &message)
	{
		record_failure(file, line, message);
		throw test_abort();
	}

	// Record a trace message; printed on failure.
	inline void record_trace(const std::string &message)
	{
		if (auto *ctx = current_context())
			ctx->traces.push_back(message);
	}

	// =========================================================================
	// Core types
	// =========================================================================
	struct test_case
	{
		std::string name;
		std::string file;
		int line;
		std::function<void()> func;
	};

	struct test_result
	{
		std::string name;
		std::string file;
		int line = 0;
		bool passed = false;
		bool skipped = false;
		double elapsed_ms = 0;
		std::string failure_message;
	};

	struct run_options
	{
		std::string filter;
		std::string exclude;
		int repeat = 1;
		bool shuffle = false;
		std::string xml_path;
		bool list_only = false;
		int timeout_ms = 0; // 0 = disabled
	};

	// =========================================================================
	// Registry
	// =========================================================================
	inline std::vector<test_case> &registry()
	{
		static std::vector<test_case> reg;
		return reg;
	}

	struct test_registrar
	{
		test_registrar(const char *name, const char *file, int line, std::function<void()> func)
		{
			registry().push_back({name, file, line, std::move(func)});
		}
	};

	// =========================================================================
	// Fixture base class (optional). Derive your fixture from this to get
	// default no-op SetUp/TearDown; override only what you need. TEST_F works
	// with any type that exposes SetUp() and TearDown().
	// =========================================================================
	struct test_fixture
	{
		virtual ~test_fixture() = default;
		virtual void SetUp() {}
		virtual void TearDown() {}
	};

	// =========================================================================
	// Filtering
	// =========================================================================
	inline bool matches_filter(const std::string &name, const std::string &filter)
	{
		if (filter.empty())
			return true;
		return name.find(filter) != std::string::npos;
	}

	inline std::string resolve_filter(const run_options &opts)
	{
		if (!opts.filter.empty())
			return opts.filter;
		const char *env = std::getenv("CS_TEST_FILTER");
		return env ? std::string(env) : std::string();
	}

	// =========================================================================
	// XML helpers
	// =========================================================================
	inline std::string xml_escape(const std::string &s)
	{
		std::string out;
		out.reserve(s.size());
		for (char c : s)
		{
			switch (c)
			{
				case '&':
					out += "&amp;";
					break;
				case '<':
					out += "&lt;";
					break;
				case '>':
					out += "&gt;";
					break;
				case '"':
					out += "&quot;";
					break;
				case '\'':
					out += "&apos;";
					break;
				default:
					out += c;
			}
		}
		return out;
	}

	inline void write_junit_xml(const std::string &path, const std::vector<test_result> &results, double total_ms)
	{
		std::ofstream out(path);
		if (!out)
		{
			std::cerr << "covariant_test: cannot open XML output: " << path << std::endl;
			return;
		}
		int total = 0, failed = 0, skipped = 0;
		for (const auto &r : results)
		{
			++total;
			if (r.skipped)
				++skipped;
			else if (!r.passed)
				++failed;
		}
		out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
		out << "<testsuites tests=\"" << total << "\" failures=\"" << failed << "\" time=\"" << (total_ms / 1000.0) << "\">\n";
		out << "  <testsuite name=\"cs_unit_tests\" tests=\"" << total << "\" failures=\"" << failed
		    << "\" skipped=\"" << skipped << "\" time=\"" << (total_ms / 1000.0) << "\">\n";
		for (const auto &r : results)
		{
			out << "    <testcase name=\"" << xml_escape(r.name) << "\" file=\"" << xml_escape(r.file)
			    << "\" line=\"" << r.line << "\" time=\"" << (r.elapsed_ms / 1000.0) << "\"";
			if (r.skipped)
			{
				out << ">\n      <skipped/>\n    </testcase>\n";
			}
			else if (!r.passed)
			{
				out << ">\n      <failure message=\"" << xml_escape(r.failure_message) << "\"/>\n    </testcase>\n";
			}
			else
			{
				out << "/>\n";
			}
		}
		out << "  </testsuite>\n";
		out << "</testsuites>\n";
	}

	// =========================================================================
	// Runner
	// =========================================================================
	inline int run_all(const run_options &opts = {})
	{
		auto &tests = registry();
		const std::string filter = resolve_filter(opts);

		if (opts.list_only)
		{
			for (const auto &tc : tests)
				if (matches_filter(tc.name, filter) &&
				    (opts.exclude.empty() || !matches_filter(tc.name, opts.exclude)))
					std::cout << tc.name << std::endl;
			std::cout << tests.size() << " tests registered." << std::endl;
			return 0;
		}

		std::vector<std::size_t> order(tests.size());
		for (std::size_t i = 0; i < tests.size(); ++i)
			order[i] = i;

		if (opts.shuffle)
		{
			std::random_device rd;
			std::mt19937 gen(rd());
			std::shuffle(order.begin(), order.end(), gen);
		}

		int total_passed = 0, total_failed = 0, total_skipped = 0;
		std::vector<test_result> all_results;
		const auto suite_start = std::chrono::steady_clock::now();

		for (int round = 0; round < opts.repeat; ++round)
		{
			if (opts.repeat > 1)
				std::cout << "\n[==========] Round " << (round + 1) << "/" << opts.repeat << std::endl;

			for (auto idx : order)
			{
				auto &tc = tests[idx];
				test_result res;
				// With --repeat, XML must contain unique testcase names.
				res.name = (opts.repeat > 1)
				               ? tc.name + " [round " + std::to_string(round + 1) + "]"
				               : tc.name;
				res.file = tc.file;
				res.line = tc.line;

				if (!matches_filter(tc.name, filter) ||
				    (!opts.exclude.empty() && matches_filter(tc.name, opts.exclude)))
				{
					res.skipped = true;
					++total_skipped;
					all_results.push_back(res);
					continue;
				}

				std::cout << "[ RUN      ] " << tc.name << std::endl;
				test_context ctx;
				current_context() = &ctx;
				const auto start = std::chrono::steady_clock::now();
				try
				{
					tc.func();
				}
				catch (const test_abort &)
				{
					// Fatal assertion: failure already recorded in ctx.
				}
				catch (const test_failure &e)
				{
					record_failure(tc.file.c_str(), tc.line, e.what());
				}
				catch (const std::exception &e)
				{
					record_failure(tc.file.c_str(), tc.line, std::string("unexpected exception: ") + e.what());
				}
				catch (...)
				{
					record_failure(tc.file.c_str(), tc.line, "unknown exception");
				}
				const auto end = std::chrono::steady_clock::now();
				current_context() = nullptr;
				res.elapsed_ms = std::chrono::duration<double, std::milli>(end - start).count();
				res.passed = ctx.failures.empty();

				if (opts.timeout_ms > 0 && res.elapsed_ms > opts.timeout_ms)
					std::cout << "[ TIMEOUT  ] " << tc.name << " exceeded " << opts.timeout_ms << " ms (" << res.elapsed_ms << " ms)" << std::endl;

				if (res.passed)
				{
					++total_passed;
					std::cout << "[       OK ] " << tc.name << " (" << res.elapsed_ms << " ms)" << std::endl;
				}
				else
				{
					++total_failed;
					std::string joined;
					for (const auto &f : ctx.failures)
					{
						std::cout << "    " << f.location << ": " << f.message << std::endl;
						if (!joined.empty())
							joined += "\n";
						joined += f.location + ": " + f.message;
					}
					if (!ctx.traces.empty())
					{
						std::cout << "    Trace:" << std::endl;
						for (const auto &t : ctx.traces)
							std::cout << "      " << t << std::endl;
					}
					res.failure_message = joined;
					std::cout << "[  FAILED  ] " << tc.name << " (" << res.elapsed_ms << " ms)" << std::endl;
				}
				all_results.push_back(res);
			}
		}

		const auto suite_end = std::chrono::steady_clock::now();
		const double total_ms = std::chrono::duration<double, std::milli>(suite_end - suite_start).count();

		std::cout << "\n[==========] " << (total_passed + total_failed) << " tests ran. (" << total_ms << " ms total)" << std::endl;
		std::cout << "[  PASSED  ] " << total_passed << " tests." << std::endl;
		if (total_skipped > 0)
			std::cout << "[  SKIPPED ] " << total_skipped << " tests." << std::endl;
		if (total_failed > 0)
			std::cout << "[  FAILED  ] " << total_failed << " tests." << std::endl;

		if (!opts.xml_path.empty())
			write_junit_xml(opts.xml_path, all_results, total_ms);

		return total_failed > 0 ? 1 : 0;
	}
} // namespace cs_test

// ===========================================================================
// Test registration
// ===========================================================================
#define TEST(name)                                                         \
	static void test_##name();                                             \
	static ::cs_test::test_registrar reg_##name(#name, __FILE__, __LINE__, \
	                                            test_##name);              \
	static void test_##name()

#define TEST_F(fixture, name)                                \
	class fixture##_##name : public fixture                  \
	{                                                        \
	   public:                                               \
		void test_body();                                    \
	};                                                       \
	static ::cs_test::test_registrar reg_##fixture##_##name( \
	    #fixture "." #name, __FILE__, __LINE__, []() {       \
		fixture##_##name instance;                           \
		try                                                  \
		{                                                    \
			instance.SetUp();                                \
		}                                                    \
		catch (...)                                          \
		{                                                    \
			instance.TearDown();                             \
			throw;                                           \
		}                                                    \
		try                                                  \
		{                                                    \
			instance.test_body();                            \
		}                                                    \
		catch (...)                                          \
		{                                                    \
			instance.TearDown();                             \
			throw;                                           \
		}                                                    \
		instance.TearDown();                                 \
	});                                                      \
	void fixture##_##name::test_body()

// ===========================================================================
// Comparison core (shared by EXPECT_* and ASSERT_*). `fatal` selects whether
// a failure aborts the test (ASSERT) or just records and continues (EXPECT).
// ===========================================================================
#define CS_TEST_CMP_IMPL(fatal, prefix, a, b, op, op_name)                 \
	do                                                                     \
	{                                                                      \
		const auto &cs_test_a_ = (a);                                      \
		const auto &cs_test_b_ = (b);                                      \
		if (!(cs_test_a_ op cs_test_b_))                                   \
		{                                                                  \
			std::ostringstream oss;                                        \
			oss << prefix " failed: " << #a << " " << op_name << " " << #b \
			    << "\n    actual: " << ::cs_test::stringify(cs_test_a_)    \
			    << " vs " << ::cs_test::stringify(cs_test_b_);             \
			if constexpr (fatal)                                           \
				::cs_test::report_fatal(__FILE__, __LINE__, oss.str());    \
			else                                                           \
				::cs_test::record_failure(__FILE__, __LINE__, oss.str());  \
		}                                                                  \
	} while (0)

#define CS_TEST_BOOL_IMPL(fatal, prefix, cond, expect_true)               \
	do                                                                    \
	{                                                                     \
		const bool cs_test_c_ = static_cast<bool>(cond);                  \
		if (cs_test_c_ != (expect_true))                                  \
		{                                                                 \
			std::ostringstream oss;                                       \
			oss << prefix " failed: " << #cond;                           \
			if constexpr (fatal)                                          \
				::cs_test::report_fatal(__FILE__, __LINE__, oss.str());   \
			else                                                          \
				::cs_test::record_failure(__FILE__, __LINE__, oss.str()); \
		}                                                                 \
	} while (0)

// ===========================================================================
// Boolean assertions
// ===========================================================================
#define EXPECT_TRUE(cond) CS_TEST_BOOL_IMPL(false, "EXPECT_TRUE", cond, true)
#define EXPECT_FALSE(cond) CS_TEST_BOOL_IMPL(false, "EXPECT_FALSE", cond, false)
#define ASSERT_TRUE(cond) CS_TEST_BOOL_IMPL(true, "ASSERT_TRUE", cond, true)
#define ASSERT_FALSE(cond) CS_TEST_BOOL_IMPL(true, "ASSERT_FALSE", cond, false)

// ===========================================================================
// Comparison assertions (value-printing)
// ===========================================================================
#define EXPECT_EQ(a, b) CS_TEST_CMP_IMPL(false, "EXPECT_EQ", a, b, ==, "==")
#define EXPECT_NE(a, b) CS_TEST_CMP_IMPL(false, "EXPECT_NE", a, b, !=, "!=")
#define EXPECT_LT(a, b) CS_TEST_CMP_IMPL(false, "EXPECT_LT", a, b, <, "<")
#define EXPECT_GT(a, b) CS_TEST_CMP_IMPL(false, "EXPECT_GT", a, b, >, ">")
#define EXPECT_LE(a, b) CS_TEST_CMP_IMPL(false, "EXPECT_LE", a, b, <=, "<=")
#define EXPECT_GE(a, b) CS_TEST_CMP_IMPL(false, "EXPECT_GE", a, b, >=, ">=")

#define ASSERT_EQ(a, b) CS_TEST_CMP_IMPL(true, "ASSERT_EQ", a, b, ==, "==")
#define ASSERT_NE(a, b) CS_TEST_CMP_IMPL(true, "ASSERT_NE", a, b, !=, "!=")
#define ASSERT_LT(a, b) CS_TEST_CMP_IMPL(true, "ASSERT_LT", a, b, <, "<")
#define ASSERT_GT(a, b) CS_TEST_CMP_IMPL(true, "ASSERT_GT", a, b, >, ">")
#define ASSERT_LE(a, b) CS_TEST_CMP_IMPL(true, "ASSERT_LE", a, b, <=, "<=")
#define ASSERT_GE(a, b) CS_TEST_CMP_IMPL(true, "ASSERT_GE", a, b, >=, ">=")

// ===========================================================================
// Floating-point comparison
// ===========================================================================
#define CS_TEST_NEAR_IMPL(fatal, prefix, a, b, eps)                         \
	do                                                                      \
	{                                                                       \
		const auto cs_test_a_ = (a);                                        \
		const auto cs_test_b_ = (b);                                        \
		const auto cs_test_eps_ = (eps);                                    \
		if (!(std::abs(cs_test_a_ - cs_test_b_) <= cs_test_eps_))           \
		{                                                                   \
			std::ostringstream oss;                                         \
			oss << prefix " failed: |" << #a << " - " << #b                 \
			    << "| > " << #eps                                           \
			    << "\n    actual: |" << ::cs_test::stringify(cs_test_a_)    \
			    << " - " << ::cs_test::stringify(cs_test_b_) << "| = "      \
			    << ::cs_test::stringify(std::abs(cs_test_a_ - cs_test_b_)); \
			if constexpr (fatal)                                            \
				::cs_test::report_fatal(__FILE__, __LINE__, oss.str());     \
			else                                                            \
				::cs_test::record_failure(__FILE__, __LINE__, oss.str());   \
		}                                                                   \
	} while (0)

#define EXPECT_NEAR(a, b, eps) CS_TEST_NEAR_IMPL(false, "EXPECT_NEAR", a, b, eps)
#define ASSERT_NEAR(a, b, eps) CS_TEST_NEAR_IMPL(true, "ASSERT_NEAR", a, b, eps)

// ===========================================================================
// String assertions
// ===========================================================================
#define CS_TEST_STREQ_IMPL(fatal, prefix, a, b)                           \
	do                                                                    \
	{                                                                     \
		const std::string cs_test_a_ = (a);                               \
		const std::string cs_test_b_ = (b);                               \
		if (cs_test_a_ != cs_test_b_)                                     \
		{                                                                 \
			std::ostringstream oss;                                       \
			oss << prefix " failed: " << #a << " != " << #b               \
			    << "\n    actual: \"" << cs_test_a_ << "\" vs \""         \
			    << cs_test_b_ << "\"";                                    \
			if constexpr (fatal)                                          \
				::cs_test::report_fatal(__FILE__, __LINE__, oss.str());   \
			else                                                          \
				::cs_test::record_failure(__FILE__, __LINE__, oss.str()); \
		}                                                                 \
	} while (0)

#define CS_TEST_STRNE_IMPL(fatal, prefix, a, b)                           \
	do                                                                    \
	{                                                                     \
		const std::string cs_test_a_ = (a);                               \
		const std::string cs_test_b_ = (b);                               \
		if (cs_test_a_ == cs_test_b_)                                     \
		{                                                                 \
			std::ostringstream oss;                                       \
			oss << prefix " failed: " << #a << " == " << #b               \
			    << "\n    actual: both are \"" << cs_test_a_ << "\"";     \
			if constexpr (fatal)                                          \
				::cs_test::report_fatal(__FILE__, __LINE__, oss.str());   \
			else                                                          \
				::cs_test::record_failure(__FILE__, __LINE__, oss.str()); \
		}                                                                 \
	} while (0)

#define EXPECT_STREQ(a, b) CS_TEST_STREQ_IMPL(false, "EXPECT_STREQ", a, b)
#define EXPECT_STRNE(a, b) CS_TEST_STRNE_IMPL(false, "EXPECT_STRNE", a, b)
#define ASSERT_STREQ(a, b) CS_TEST_STREQ_IMPL(true, "ASSERT_STREQ", a, b)
#define ASSERT_STRNE(a, b) CS_TEST_STRNE_IMPL(true, "ASSERT_STRNE", a, b)

// ===========================================================================
// Exception assertions
// ===========================================================================
#define CS_TEST_THROW_IMPL(fatal, prefix, expr, ex_type)                  \
	do                                                                    \
	{                                                                     \
		bool cs_test_thrown_ = false;                                     \
		try                                                               \
		{                                                                 \
			expr;                                                         \
		}                                                                 \
		catch (const ex_type &)                                           \
		{                                                                 \
			cs_test_thrown_ = true;                                       \
		}                                                                 \
		if (!cs_test_thrown_)                                             \
		{                                                                 \
			std::ostringstream oss;                                       \
			oss << prefix " failed: " << #expr                            \
			    << " did not throw " << #ex_type;                         \
			if constexpr (fatal)                                          \
				::cs_test::report_fatal(__FILE__, __LINE__, oss.str());   \
			else                                                          \
				::cs_test::record_failure(__FILE__, __LINE__, oss.str()); \
		}                                                                 \
	} while (0)

#define CS_TEST_NO_THROW_IMPL(fatal, prefix, expr)                        \
	do                                                                    \
	{                                                                     \
		try                                                               \
		{                                                                 \
			expr;                                                         \
		}                                                                 \
		catch (const std::exception &e)                                   \
		{                                                                 \
			std::ostringstream oss;                                       \
			oss << prefix " failed: " << #expr                            \
			    << " threw: " << e.what();                                \
			if constexpr (fatal)                                          \
				::cs_test::report_fatal(__FILE__, __LINE__, oss.str());   \
			else                                                          \
				::cs_test::record_failure(__FILE__, __LINE__, oss.str()); \
		}                                                                 \
		catch (...)                                                       \
		{                                                                 \
			std::ostringstream oss;                                       \
			oss << prefix " failed: " << #expr                            \
			    << " threw unknown exception";                            \
			if constexpr (fatal)                                          \
				::cs_test::report_fatal(__FILE__, __LINE__, oss.str());   \
			else                                                          \
				::cs_test::record_failure(__FILE__, __LINE__, oss.str()); \
		}                                                                 \
	} while (0)

#define CS_TEST_ANY_THROW_IMPL(fatal, prefix, expr)                       \
	do                                                                    \
	{                                                                     \
		bool cs_test_thrown_ = false;                                     \
		try                                                               \
		{                                                                 \
			expr;                                                         \
		}                                                                 \
		catch (...)                                                       \
		{                                                                 \
			cs_test_thrown_ = true;                                       \
		}                                                                 \
		if (!cs_test_thrown_)                                             \
		{                                                                 \
			std::ostringstream oss;                                       \
			oss << prefix " failed: " << #expr << " did not throw";       \
			if constexpr (fatal)                                          \
				::cs_test::report_fatal(__FILE__, __LINE__, oss.str());   \
			else                                                          \
				::cs_test::record_failure(__FILE__, __LINE__, oss.str()); \
		}                                                                 \
	} while (0)

#define EXPECT_THROW(expr, ex_type) CS_TEST_THROW_IMPL(false, "EXPECT_THROW", expr, ex_type)
#define EXPECT_NO_THROW(expr) CS_TEST_NO_THROW_IMPL(false, "EXPECT_NO_THROW", expr)
#define EXPECT_ANY_THROW(expr) CS_TEST_ANY_THROW_IMPL(false, "EXPECT_ANY_THROW", expr)
#define ASSERT_THROW(expr, ex_type) CS_TEST_THROW_IMPL(true, "ASSERT_THROW", expr, ex_type)
#define ASSERT_NO_THROW(expr) CS_TEST_NO_THROW_IMPL(true, "ASSERT_NO_THROW", expr)
#define ASSERT_ANY_THROW(expr) CS_TEST_ANY_THROW_IMPL(true, "ASSERT_ANY_THROW", expr)

#define CS_TEST_THROW_MSG_IMPL(fatal, prefix, expr, ex_type, substring)       \
	do                                                                        \
	{                                                                         \
		bool cs_test_thrown_ = false;                                         \
		try                                                                   \
		{                                                                     \
			expr;                                                             \
		}                                                                     \
		catch (const ex_type &e)                                              \
		{                                                                     \
			cs_test_thrown_ = true;                                           \
			std::string cs_test_msg_ = e.what();                              \
			if (cs_test_msg_.find(substring) == std::string::npos)            \
			{                                                                 \
				std::ostringstream oss;                                       \
				oss << prefix " failed: " << #expr << " threw " << #ex_type   \
				    << " with message \"" << cs_test_msg_                     \
				    << "\" which does not contain \"" << substring << "\"";   \
				if constexpr (fatal)                                          \
					::cs_test::report_fatal(__FILE__, __LINE__, oss.str());   \
				else                                                          \
					::cs_test::record_failure(__FILE__, __LINE__, oss.str()); \
			}                                                                 \
		}                                                                     \
		if (!cs_test_thrown_)                                                 \
		{                                                                     \
			std::ostringstream oss;                                           \
			oss << prefix " failed: " << #expr                                \
			    << " did not throw " << #ex_type;                             \
			if constexpr (fatal)                                              \
				::cs_test::report_fatal(__FILE__, __LINE__, oss.str());       \
			else                                                              \
				::cs_test::record_failure(__FILE__, __LINE__, oss.str());     \
		}                                                                     \
	} while (0)

#define EXPECT_THROW_MSG(expr, ex_type, substring) CS_TEST_THROW_MSG_IMPL(false, "EXPECT_THROW_MSG", expr, ex_type, substring)
#define ASSERT_THROW_MSG(expr, ex_type, substring) CS_TEST_THROW_MSG_IMPL(true, "ASSERT_THROW_MSG", expr, ex_type, substring)

// ===========================================================================
// String containment
// ===========================================================================
#define CS_TEST_CONTAINS_IMPL(fatal, prefix, text, substring)             \
	do                                                                    \
	{                                                                     \
		const std::string cs_test_s_ = (text);                            \
		const std::string cs_test_sub_ = (substring);                     \
		if (cs_test_s_.find(cs_test_sub_) == std::string::npos)           \
		{                                                                 \
			std::ostringstream oss;                                       \
			oss << prefix " failed: \"" << cs_test_s_ << "\""             \
			    << " does not contain \"" << cs_test_sub_ << "\"";        \
			if constexpr (fatal)                                          \
				::cs_test::report_fatal(__FILE__, __LINE__, oss.str());   \
			else                                                          \
				::cs_test::record_failure(__FILE__, __LINE__, oss.str()); \
		}                                                                 \
	} while (0)

#define CS_TEST_NOT_CONTAINS_IMPL(fatal, prefix, text, substring)         \
	do                                                                    \
	{                                                                     \
		const std::string cs_test_s_ = (text);                            \
		const std::string cs_test_sub_ = (substring);                     \
		if (cs_test_s_.find(cs_test_sub_) != std::string::npos)           \
		{                                                                 \
			std::ostringstream oss;                                       \
			oss << prefix " failed: \"" << cs_test_s_ << "\""             \
			    << " contains \"" << cs_test_sub_ << "\"";                \
			if constexpr (fatal)                                          \
				::cs_test::report_fatal(__FILE__, __LINE__, oss.str());   \
			else                                                          \
				::cs_test::record_failure(__FILE__, __LINE__, oss.str()); \
		}                                                                 \
	} while (0)

#define EXPECT_CONTAINS(text, substring) CS_TEST_CONTAINS_IMPL(false, "EXPECT_CONTAINS", text, substring)
#define EXPECT_NOT_CONTAINS(text, substring) CS_TEST_NOT_CONTAINS_IMPL(false, "EXPECT_NOT_CONTAINS", text, substring)
#define ASSERT_CONTAINS(text, substring) CS_TEST_CONTAINS_IMPL(true, "ASSERT_CONTAINS", text, substring)
#define ASSERT_NOT_CONTAINS(text, substring) CS_TEST_NOT_CONTAINS_IMPL(true, "ASSERT_NOT_CONTAINS", text, substring)

// ===========================================================================
// Pointer assertions
// ===========================================================================
#define CS_TEST_NULL_IMPL(fatal, prefix, ptr, expect_null)                \
	do                                                                    \
	{                                                                     \
		const bool cs_test_is_null_ = ((ptr) == nullptr);                 \
		if (cs_test_is_null_ != (expect_null))                            \
		{                                                                 \
			std::ostringstream oss;                                       \
			oss << prefix " failed: " << #ptr                             \
			    << (expect_null ? " is not null" : " is null");           \
			if constexpr (fatal)                                          \
				::cs_test::report_fatal(__FILE__, __LINE__, oss.str());   \
			else                                                          \
				::cs_test::record_failure(__FILE__, __LINE__, oss.str()); \
		}                                                                 \
	} while (0)

#define EXPECT_NULL(ptr) CS_TEST_NULL_IMPL(false, "EXPECT_NULL", ptr, true)
#define EXPECT_NOT_NULL(ptr) CS_TEST_NULL_IMPL(false, "EXPECT_NOT_NULL", ptr, false)
#define ASSERT_NULL(ptr) CS_TEST_NULL_IMPL(true, "ASSERT_NULL", ptr, true)
#define ASSERT_NOT_NULL(ptr) CS_TEST_NULL_IMPL(true, "ASSERT_NOT_NULL", ptr, false)

// ===========================================================================
// Trace
// ===========================================================================
#define TRACE(msg) ::cs_test::record_trace(msg)
