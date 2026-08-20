// =============================================================================
// Unit tests for system module (sources/system/*.cpp).
// Tests file, path, environment, and runtime operations.
// =============================================================================
#include "test_helpers.hpp"
#include <cstdio>

// =============================================================================
// System I/O
// =============================================================================
TEST(system_out_println)
{
	EXPECT_EQ(run_script("system.out.println(\"test\")"), "test\n");
}

TEST(system_out_print)
{
	std::string out = run_script("system.out.print(\"hello\")");
	EXPECT_EQ(out, "hello");
}

TEST(system_out_println_number)
{
	EXPECT_EQ(run_script("system.out.println(42)"), "42\n");
}

TEST(system_out_println_bool)
{
	EXPECT_EQ(run_script("system.out.println(true)"), "true\n");
}

TEST(system_out_println_empty)
{
	EXPECT_EQ(run_script("system.out.println(\"\")"), "\n");
}

// =============================================================================
// File operations (system.file)
// =============================================================================
TEST(file_exist_false)
{
	EXPECT_EQ(run_script(R"(
		system.out.println(system.file.exist("nonexistent_cs_unit_test.txt"))
	)"),
	          "false\n");
}

TEST(file_exist_true)
{
	const char *path = "cs_unit_test_exist.txt";
	struct Guard
	{
		const char *p;
		~Guard() { std::remove(p); }
	} guard{path};
	FILE *f = std::fopen(path, "w");
	if (f) std::fclose(f);
	EXPECT_EQ(run_script(R"(
		system.out.println(system.file.exist("cs_unit_test_exist.txt"))
	)"),
	          "true\n");
}

TEST(file_can_read)
{
	EXPECT_EQ(run_script(R"(
		system.out.println(system.file.can_read("."))
	)"),
	          "true\n");
}

TEST(file_can_write)
{
	EXPECT_EQ(run_script(R"(
		system.out.println(system.file.can_write("."))
	)"),
	          "true\n");
}

// =============================================================================
// Path operations (system.path)
// =============================================================================
TEST(path_is_directory)
{
	EXPECT_EQ(run_script(R"(
		system.out.println(system.path.is_directory("."))
	)"),
	          "true\n");
}

TEST(path_is_file)
{
	EXPECT_EQ(run_script(R"(
		system.out.println(system.path.is_file("."))
	)"),
	          "false\n");
}

TEST(path_separator)
{
	std::string sep = run_script(R"(
		system.out.println(system.path.separator)
	)");
#ifdef COVSCRIPT_PLATFORM_WIN32
	EXPECT_EQ(sep, "\\\n");
#else
	EXPECT_EQ(sep, "/\n");
#endif
}

// =============================================================================
// Environment variables (system.getenv)
// =============================================================================
TEST(env_get_path)
{
	std::string out = run_script(R"(
		var p = system.getenv("PATH")
		system.out.println(p.empty())
	)");
	EXPECT_EQ(out, "false\n");
}

TEST(env_get_nonexistent)
{
	EXPECT_ANY_THROW(run_script(R"(
		system.getenv("CS_UNIT_TEST_NONEXISTENT_VAR_xyz")
	)"));
}

// =============================================================================
// OS info
// =============================================================================
TEST(system_os_name)
{
	std::string out = run_script("system.out.println(system.os_name)");
	EXPECT_FALSE(out.empty());
}

TEST(system_arch_name)
{
	std::string out = run_script("system.out.println(system.arch_name)");
	EXPECT_FALSE(out.empty());
}

// =============================================================================
// Runtime operations
// =============================================================================
TEST(runtime_time)
{
	std::string out = run_script("system.out.println(runtime.time())");
	EXPECT_FALSE(out.empty());
	// Should start with a digit
	EXPECT_TRUE(out[0] >= '0' && out[0] <= '9');
}

TEST(runtime_local_time)
{
	std::string out = run_script(R"(
		var t = runtime.local_time()
		system.out.println(t.hour >= 0)
		system.out.println(t.min >= 0)
		system.out.println(t.sec >= 0)
	)");
	EXPECT_EQ(out, "true\ntrue\ntrue\n");
}

// =============================================================================
// System commands
// =============================================================================
TEST(system_run)
{
	// system.run returns exit code; echo output goes to real stdout (not captured)
	std::string out = run_script(R"(
		system.out.println(system.run("echo hello"))
	)");
	EXPECT_EQ(out, "0\n");
}
