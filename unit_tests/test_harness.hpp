#pragma once
/*
 * CovScript Compiler Unit Test Harness
 *
 * Licensed under the Apache License, Version 2.0
 * Copyright (C) 2017-2026 Michael Lee(李登淳)
 */

#include <iostream>
#include <string>
#include <vector>
#include <functional>
#include <exception>
#include <sstream>

namespace cs_test {

	struct test_case {
		std::string name;
		std::string file;
		int line;
		std::function<void()> func;
	};

	inline std::vector<test_case> &registry()
	{
		static std::vector<test_case> reg;
		return reg;
	}

	struct test_registrar {
		test_registrar(const char *name, const char *file, int line, std::function<void()> func)
		{
			registry().push_back({name, file, line, std::move(func)});
		}
	};

	class test_failure : public std::runtime_error {
	public:
		test_failure(const std::string &msg) : std::runtime_error(msg) {}
	};

	inline int run_all()
	{
		int passed = 0;
		int failed = 0;

		for (auto &tc : registry()) {
			std::cout << "[ RUN      ] " << tc.name << std::endl;
			try {
				tc.func();
				std::cout << "[       OK ] " << tc.name << std::endl;
				++passed;
			}
			catch (const test_failure &e) {
				std::cout << "[  FAILED  ] " << tc.name << std::endl;
				std::cout << "    " << tc.file << ":" << tc.line << ": " << e.what() << std::endl;
				++failed;
			}
			catch (const std::exception &e) {
				std::cout << "[  FAILED  ] " << tc.name << std::endl;
				std::cout << "    " << tc.file << ":" << tc.line
				          << ": unexpected exception: " << e.what() << std::endl;
				++failed;
			}
			catch (...) {
				std::cout << "[  FAILED  ] " << tc.name << std::endl;
				std::cout << "    " << tc.file << ":" << tc.line << ": unknown exception" << std::endl;
				++failed;
			}
		}

		std::cout << "\n[==========] " << (passed + failed) << " tests ran." << std::endl;
		std::cout << "[  PASSED  ] " << passed << " tests." << std::endl;
		if (failed > 0)
			std::cout << "[  FAILED  ] " << failed << " tests." << std::endl;
		return failed > 0 ? 1 : 0;
	}

} // namespace cs_test

#define TEST(name)                                                             \
	static void test_##name();                                                 \
	static ::cs_test::test_registrar reg_##name(#name, __FILE__, __LINE__,     \
	                                           test_##name);                   \
	static void test_##name()

#define EXPECT_TRUE(cond)                                                      \
	do {                                                                       \
		if (!(cond)) {                                                         \
			std::ostringstream oss;                                            \
			oss << "EXPECT_TRUE failed: " << #cond;                            \
			throw ::cs_test::test_failure(oss.str());                          \
		}                                                                      \
	} while (0)

#define EXPECT_FALSE(cond)                                                     \
	do {                                                                       \
		if (cond) {                                                            \
			std::ostringstream oss;                                            \
			oss << "EXPECT_FALSE failed: " << #cond;                           \
			throw ::cs_test::test_failure(oss.str());                          \
		}                                                                      \
	} while (0)

#define EXPECT_THROW(expr, ex_type)                                            \
	do {                                                                       \
		bool thrown = false;                                                   \
		try {                                                                  \
			expr;                                                              \
		}                                                                      \
		catch (const ex_type &) {                                              \
			thrown = true;                                                     \
		}                                                                      \
		if (!thrown) {                                                         \
			std::ostringstream oss;                                            \
			oss << "EXPECT_THROW failed: " << #expr                            \
			    << " did not throw " << #ex_type;                              \
			throw ::cs_test::test_failure(oss.str());                          \
		}                                                                      \
	} while (0)

#define EXPECT_NO_THROW(expr)                                                  \
	do {                                                                       \
		try {                                                                  \
			expr;                                                              \
		}                                                                      \
		catch (const std::exception &e) {                                      \
			std::ostringstream oss;                                            \
			oss << "EXPECT_NO_THROW failed: " << #expr                         \
			    << " threw: " << e.what();                                     \
			throw ::cs_test::test_failure(oss.str());                          \
		}                                                                      \
		catch (...) {                                                          \
			std::ostringstream oss;                                            \
			oss << "EXPECT_NO_THROW failed: " << #expr                         \
			    << " threw unknown exception";                                 \
			throw ::cs_test::test_failure(oss.str());                          \
		}                                                                      \
	} while (0)

// String contains check
#define EXPECT_CONTAINS(str, substr)                                           \
	do {                                                                       \
		std::string _s(str);                                                   \
		std::string _sub(substr);                                              \
		if (_s.find(_sub) == std::string::npos) {                              \
			std::ostringstream oss;                                            \
			oss << "EXPECT_CONTAINS failed: \"" << _s << "\""                  \
			    << " does not contain \"" << _sub << "\"";                     \
			throw ::cs_test::test_failure(oss.str());                          \
		}                                                                      \
	} while (0)
