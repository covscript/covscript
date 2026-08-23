// =============================================================================
// Unit tests for type extensions (sources/instance/type_ext.cpp).
// Tests String, Array, Hash_map, Hash_set, Pair, and Numeric methods.
// =============================================================================
#include "test_helpers.hpp"

// =============================================================================
// String methods
// =============================================================================
TEST(string_size)
{
	EXPECT_EQ(run_script("var s = \"hello\"; system.out.println(s.size)"), "5\n");
}

TEST(string_empty_true)
{
	EXPECT_EQ(run_script("var s = \"\"; system.out.println(s.empty())"), "true\n");
}

TEST(string_empty_false)
{
	EXPECT_EQ(run_script("var s = \"x\"; system.out.println(s.empty())"), "false\n");
}

TEST(string_substr)
{
	EXPECT_EQ(run_script("var s = \"hello world\"; system.out.println(s.substr(6, 5))"), "world\n");
}

TEST(string_find)
{
	EXPECT_EQ(run_script("var s = \"hello world\"; system.out.println(s.find(\"world\", 0))"), "6\n");
}

TEST(string_find_not_found)
{
	EXPECT_EQ(run_script("var s = \"hello\"; system.out.println(s.find(\"xyz\", 0))"), "-1\n");
}

TEST(string_replace)
{
	EXPECT_EQ(run_script("var s = \"hello world\"; s.replace(5, 1, \",\"); system.out.println(s)"), "hello,world\n");
}

TEST(string_split)
{
	EXPECT_EQ(run_script(R"(
		var s = "a,b,c"
		var parts = s.split({','})
		foreach i in parts
			system.out.println(i)
		end
	)"),
	          "a\nb\nc\n");
}

TEST(string_to_upper)
{
	EXPECT_EQ(run_script("var s = \"Hello\"; system.out.println(s.to_upper())"), "HELLO\n");
}

TEST(string_to_lower)
{
	EXPECT_EQ(run_script("var s = \"Hello\"; system.out.println(s.to_lower())"), "hello\n");
}

TEST(string_trim)
{
	EXPECT_EQ(run_script("var s = \"  hello  \"; system.out.println(\"[\" + s.trim() + \"]\")"), "[hello]\n");
}

TEST(string_to_number)
{
	EXPECT_EQ(run_script("var s = \"42\"; system.out.println(s.to_number() + 1)"), "43\n");
}

TEST(string_append)
{
	EXPECT_EQ(run_script("var s = \"hel\"; s.append(\"lo\"); system.out.println(s)"), "hello\n");
}

TEST(string_insert)
{
	EXPECT_EQ(run_script("var s = \"helo\"; s.insert(2, \"l\"); system.out.println(s)"), "hello\n");
}

TEST(string_erase)
{
	EXPECT_EQ(run_script("var s = \"hello world\"; s.erase(5, 6); system.out.println(s)"), "hello\n");
}

TEST(string_assign)
{
	EXPECT_EQ(run_script("var s = \"hello\"; s.assign(0, 'H'); system.out.println(s)"), "Hello\n");
}

TEST(string_cut)
{
	EXPECT_EQ(run_script("var s = \"hello world\"; s.cut(6); system.out.println(s)"), "hello\n");
}

TEST(string_clear)
{
	EXPECT_EQ(run_script("var s = \"hello\"; s.clear(); system.out.println(s.empty())"), "true\n");
}

TEST(string_rfind)
{
	EXPECT_EQ(run_script("var s = \"abab\"; system.out.println(s.rfind(\"b\", -1))"), "3\n");
}

// =============================================================================
// Array methods
// =============================================================================
TEST(array_size)
{
	EXPECT_EQ(run_script("var a = {1, 2, 3}; system.out.println(a.size)"), "3\n");
}

TEST(array_empty)
{
	EXPECT_EQ(run_script("var a = {}; system.out.println(a.empty())"), "true\n");
}

TEST(array_at)
{
	EXPECT_EQ(run_script("var a = {10, 20, 30}; system.out.println(a.at(1))"), "20\n");
}

TEST(array_push_back)
{
	EXPECT_EQ(run_script("var a = {1}; a.push_back(2); system.out.println(a.size)"), "2\n");
}

TEST(array_pop_back)
{
	EXPECT_EQ(run_script("var a = {1, 2}; system.out.println(a.pop_back())"), "2\n");
}

TEST(array_front)
{
	EXPECT_EQ(run_script("var a = {42, 99}; system.out.println(a.front)"), "42\n");
}

TEST(array_back)
{
	EXPECT_EQ(run_script("var a = {42, 99}; system.out.println(a.back)"), "99\n");
}

TEST(array_clear)
{
	EXPECT_EQ(run_script("var a = {1, 2, 3}; a.clear(); system.out.println(a.empty())"), "true\n");
}

TEST(array_sort)
{
	EXPECT_EQ(run_script(R"(
		var a = {3, 1, 2}
		a.sort([](l, r)-> l < r)
		foreach i in a
			system.out.println(i)
		end
	)"),
	          "1\n2\n3\n");
}

TEST(array_join)
{
	EXPECT_EQ(run_script(R"(
		var a = {"hello", "world"}
		system.out.println(a.join(" "))
	)"),
	          "hello world\n");
}

TEST(array_push_front)
{
	EXPECT_EQ(run_script(R"(
		var a = {2, 3}
		a.push_front(1)
		system.out.println(a.at(0))
	)"),
	          "1\n");
}

TEST(array_pop_front)
{
	EXPECT_EQ(run_script(R"(
		var a = {1, 2, 3}
		system.out.println(a.pop_front())
		system.out.println(a.size)
	)"),
	          "1\n2\n");
}

TEST(array_insert)
{
	EXPECT_EQ(run_script(R"(
		var a = {1, 3}
		a.insert(a.begin, 2)
		system.out.println(a.at(0))
		system.out.println(a.at(1))
		system.out.println(a.at(2))
		system.out.println(a.size)
	)"),
	          "2\n1\n3\n3\n");
}

TEST(array_erase)
{
	EXPECT_EQ(run_script(R"(
		var a = {1, 2, 3}
		a.erase(a.begin)
		system.out.println(a.size)
		system.out.println(a.at(0))
	)"),
	          "2\n2\n");
}

TEST(array_to_hash_set)
{
	EXPECT_EQ(run_script(R"(
		var a = {1, 2, 2, 3}
		var s = a.to_hash_set()
		system.out.println(s.size)
	)"),
	          "3\n");
}

TEST(array_to_hash_map)
{
	EXPECT_EQ(run_script(R"(
		var a = {1:"a", 2:"b"}
		var m = a.to_hash_map()
		system.out.println(m.size)
		system.out.println(m.at(1))
	)"),
	          "2\na\n");
}

TEST(array_enumerate)
{
	EXPECT_EQ(run_script(R"(
		var a = {"x", "y", "z"}
		var m = a.enumerate()
		system.out.println(m.size)
		system.out.println(m.at(0))
		system.out.println(m.at(2))
	)"),
	          "3\nx\nz\n");
}

TEST(array_to_list)
{
	EXPECT_EQ(run_script(R"(
		var a = {1, 2, 3}
		var l = a.to_list()
		system.out.println(l.size)
	)"),
	          "3\n");
}

TEST(array_iterator_next)
{
	EXPECT_EQ(run_script(R"(
		var a = {10, 20, 30}
		var it = a.begin
		system.out.println(it.data)
		it.next()
		system.out.println(it.data)
		it.next()
		system.out.println(it.data)
	)"),
	          "10\n20\n30\n");
}

TEST(array_iterator_prev)
{
	EXPECT_EQ(run_script(R"(
		var a = {10, 20, 30}
		var it = a.end
		it.prev()
		system.out.println(it.data)
		it.prev()
		system.out.println(it.data)
	)"),
	          "30\n20\n");
}

TEST(array_iterator_next_n)
{
	EXPECT_EQ(run_script(R"(
		var a = {10, 20, 30, 40, 50}
		var it = a.begin
		it.next_n(3)
		system.out.println(it.data)
	)"),
	          "40\n");
}

TEST(array_iterator_prev_n)
{
	EXPECT_EQ(run_script(R"(
		var a = {10, 20, 30, 40, 50}
		var it = a.end
		it.prev_n(2)
		system.out.println(it.data)
	)"),
	          "40\n");
}

// =============================================================================
// Hash_map methods
// =============================================================================
TEST(hash_map_size)
{
	EXPECT_EQ(run_script(R"(
		var m = {{1, "a"}, {2, "b"}}
		system.out.println(m.size)
	)"),
	          "2\n");
}

TEST(hash_map_empty)
{
	EXPECT_EQ(run_script("var m = {}; system.out.println(m.empty())"), "true\n");
}

TEST(hash_map_insert_at)
{
	EXPECT_EQ(run_script(R"(
		var m = {}.to_hash_map()
		m.insert(1, "hello")
		system.out.println(m.at(1))
	)"),
	          "hello\n");
}

TEST(hash_map_exist)
{
	EXPECT_EQ(run_script(R"(
		var m = {1:"a"}.to_hash_map()
		system.out.println(m.exist(1))
		system.out.println(m.exist(2))
	)"),
	          "true\nfalse\n");
}

TEST(hash_map_clear)
{
	EXPECT_EQ(run_script(R"(
		var m = {{1, "a"}}
		m.clear()
		system.out.println(m.empty())
	)"),
	          "true\n");
}

TEST(hash_map_erase)
{
	EXPECT_EQ(run_script(R"(
		var m = {1:"a", 2:"b", 3:"c"}.to_hash_map()
		m.erase(2)
		system.out.println(m.size)
		system.out.println(m.exist(2))
	)"),
	          "2\nfalse\n");
}

TEST(hash_map_keys)
{
	EXPECT_EQ(run_script(R"(
		var m = {1:"a", 2:"b"}.to_hash_map()
		var k = m.keys()
		system.out.println(k.size)
	)"),
	          "2\n");
}

TEST(hash_map_values)
{
	EXPECT_EQ(run_script(R"(
		var m = {1:"a", 2:"b"}.to_hash_map()
		var v = m.values()
		system.out.println(v.size)
	)"),
	          "2\n");
}

TEST(hash_map_iterate)
{
	EXPECT_EQ(run_script(R"(
		var m = {1:"a", 2:"b"}.to_hash_map()
		var count = 0
		foreach it in m
			count = count + 1
		end
		system.out.println(count)
	)"),
	          "2\n");
}

// =============================================================================
// Hash_set methods
// =============================================================================
TEST(hash_set_size)
{
	EXPECT_EQ(run_script(R"(
		var s = {1, 2, 3}
		system.out.println(s.size)
	)"),
	          "3\n");
}

TEST(hash_set_empty)
{
	EXPECT_EQ(run_script("var s = {}; system.out.println(s.empty())"), "true\n");
}

TEST(hash_set_insert_exist)
{
	EXPECT_EQ(run_script(R"(
		var s = {1, 2, 3}.to_hash_set()
		s.insert(42)
		system.out.println(s.exist(42))
		system.out.println(s.exist(99))
	)"),
	          "true\nfalse\n");
}

TEST(hash_set_clear)
{
	EXPECT_EQ(run_script(R"(
		var s = {1, 2}
		s.clear()
		system.out.println(s.empty())
	)"),
	          "true\n");
}

TEST(hash_set_erase)
{
	EXPECT_EQ(run_script(R"(
		var s = {1, 2, 3}.to_hash_set()
		s.erase(2)
		system.out.println(s.size)
		system.out.println(s.exist(2))
	)"),
	          "2\nfalse\n");
}

TEST(hash_set_iterate)
{
	EXPECT_EQ(run_script(R"(
		var s = {10, 20, 30}.to_hash_set()
		var count = 0
		foreach it in s
			count = count + 1
		end
		system.out.println(count)
	)"),
	          "3\n");
}

TEST(hash_set_intersect)
{
	EXPECT_EQ(run_script(R"(
		var a = {1, 2, 3}.to_hash_set()
		var b = {2, 3, 4}.to_hash_set()
		var c = hash_set.intersect(a, b)
		system.out.println(c.size)
		system.out.println(c.exist(1))
		system.out.println(c.exist(2))
		system.out.println(c.exist(4))
	)"),
	          "2\nfalse\ntrue\nfalse\n");
}

TEST(hash_set_merge)
{
	EXPECT_EQ(run_script(R"(
		var a = {1, 2}.to_hash_set()
		var b = {3, 4}.to_hash_set()
		var c = hash_set.merge(a, b)
		system.out.println(c.size)
		system.out.println(c.exist(1))
		system.out.println(c.exist(4))
	)"),
	          "4\ntrue\ntrue\n");
}

TEST(hash_set_subtract)
{
	EXPECT_EQ(run_script(R"(
		var a = {1, 2, 3}.to_hash_set()
		var b = {2, 3, 4}.to_hash_set()
		var c = hash_set.subtract(a, b)
		system.out.println(c.size)
		system.out.println(c.exist(1))
		system.out.println(c.exist(2))
	)"),
	          "1\ntrue\nfalse\n");
}

// =============================================================================
// File operations
// =============================================================================
TEST(file_copy_rename_remove)
{
	// Clean up stale files from previous runs (in case of mid-script failure)
	run_script(R"(
		system.file.remove("cs_unit_test_fcr.txt")
		system.file.remove("cs_unit_test_fcr_copy.txt")
		system.file.remove("cs_unit_test_fcr_renamed.txt")
	)");
	EXPECT_EQ(run_script(R"(
		var outfs = iostream.fstream("cs_unit_test_fcr.txt", iostream.openmode.out)
		outfs.println("hello")
		outfs.flush()
		outfs = 0
		system.out.println(system.file.exist("cs_unit_test_fcr.txt"))
		system.file.copy("cs_unit_test_fcr.txt", "cs_unit_test_fcr_copy.txt")
		system.out.println(system.file.exist("cs_unit_test_fcr_copy.txt"))
		system.file.rename("cs_unit_test_fcr_copy.txt", "cs_unit_test_fcr_renamed.txt")
		system.out.println(system.file.exist("cs_unit_test_fcr_renamed.txt"))
		system.out.println(system.file.exist("cs_unit_test_fcr_copy.txt"))
		system.file.remove("cs_unit_test_fcr.txt")
		system.file.remove("cs_unit_test_fcr_renamed.txt")
	)"),
	          "true\ntrue\ntrue\nfalse\n");
}

// =============================================================================
// Path operations
// =============================================================================
TEST(path_mkdir_remove)
{
	// Clean up stale directory from previous runs
	run_script("system.path.remove(\"cs_unit_test_tmpdir\")");
	EXPECT_EQ(run_script(R"(
		system.path.mkdir("cs_unit_test_tmpdir")
		system.out.println(system.path.is_directory("cs_unit_test_tmpdir"))
		system.path.remove("cs_unit_test_tmpdir")
		system.out.println(system.path.exist("cs_unit_test_tmpdir"))
	)"),
	          "true\nfalse\n");
}

// =============================================================================
// Pair methods
// =============================================================================
TEST(pair_first_second)
{
	EXPECT_EQ(run_script(R"(
		var p = new pair
		p.first = 42
		p.second = "hello"
		system.out.println(p.first)
		system.out.println(p.second)
	)"),
	          "42\nhello\n");
}

TEST(pair_key_value)
{
	EXPECT_EQ(run_script(R"(
		var p = new pair
		p.key = 99
		p.value = "world"
		system.out.println(p.key)
		system.out.println(p.value)
	)"),
	          "99\nworld\n");
}

// =============================================================================
// Numeric methods
// =============================================================================
TEST(numeric_is_integer)
{
	EXPECT_EQ(run_script("var n = 42; system.out.println(n.is_integer())"), "true\n");
}

TEST(numeric_is_float)
{
	EXPECT_EQ(run_script("var n = 3.14; system.out.println(n.is_float())"), "true\n");
}

TEST(numeric_to_integer)
{
	EXPECT_EQ(run_script("var n = 3.14; n.to_integer(); system.out.println(n)"), "3\n");
}

TEST(numeric_to_float)
{
	EXPECT_EQ(run_script("var n = 42; n.to_float(); system.out.println(n.is_float())"), "true\n");
}
