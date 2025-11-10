/*
 * Covariant Script Type Support
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Copyright (C) 2017-2025 Michael Lee(李登淳)
 *
 * This software is registered with the National Copyright Administration
 * of the People's Republic of China(Registration Number: 2020SR0408026)
 * and is protected by the Copyright Law of the People's Republic of China.
 *
 * Email:   mikecovlee@163.com
 * Github:  https://github.com/mikecovlee
 * Website: http://covscript.org.cn
 */
#include <covscript_impl/dirent/dirent.hpp>
#include <covscript_impl/mozart/random.hpp>
#include <covscript_impl/mozart/timer.hpp>
#include <covscript_impl/system.hpp>
#include <covscript/impl/impl.hpp>
#include <sys/types.h>
#include <sys/stat.h>
#include <algorithm>
#include <iostream>
#include <future>

#ifdef COVSCRIPT_PLATFORM_WIN32
#define cs_sys_stat _stat
#else
#define cs_sys_stat stat
#endif

namespace cs_impl {
	namespace member_visitor_cs_ext {
		using namespace cs;

		void init()
		{
			(*member_visitor_ext)
			.add_var("get", make_cni(&member_visitor::get, true))
			.add_var("set", make_cni(&member_visitor::set, true));
		}
	} // namespace member_visitor_cs_ext

	inline void insert_or_assign(cs::hash_map &map, const cs::var &key, const cs::var &val)
	{
		auto it = map.find(key);
		if (it == map.end())
			map.emplace(cs::copy(key), cs::copy(val));
		else
			it->second.swap(cs::copy(val), true);
	}

	namespace array_cs_ext {
		using namespace cs;

// Element access
		var at(array &arr, const numeric &posit)
		{
			return arr.at(posit.as_integer());
		}

		var front(const array &arr)
		{
			if (arr.empty())
				throw lang_error("Call front() on empty array.");
			return arr.front();
		}

		var back(const array &arr)
		{
			if (arr.empty())
				throw lang_error("Call back() on empty array.");
			return arr.back();
		}

// Iterators
		array::iterator begin(array &arr)
		{
			return arr.begin();
		}

		array::iterator end(array &arr)
		{
			return arr.end();
		}

		array::iterator next(array::iterator &it)
		{
			return it++;
		}

		array::iterator next_n(array::iterator &it, const numeric &offset)
		{
			return it += offset.as_integer();
		}

		array::iterator prev(array::iterator &it)
		{
			return it--;
		}

		array::iterator prev_n(array::iterator &it, const numeric &offset)
		{
			return it -= offset.as_integer();
		}

		var data(array::iterator &it)
		{
			return *it;
		}

// Capacity
		bool empty(const array &arr)
		{
			return arr.empty();
		}

		numeric size(const array &arr)
		{
			return arr.size();
		}

// Modifiers
		void clear(array &arr)
		{
			arr.clear();
		}

		array::iterator insert(array &arr, array::iterator &pos, const var &val)
		{
			return arr.insert(pos, copy(val));
		}

		array::iterator erase(array &arr, array::iterator &pos)
		{
			return arr.erase(pos);
		}

		void push_front(array &arr, const var &val)
		{
			arr.push_front(copy(val));
		}

		var pop_front(array &arr)
		{
			var fval = front(arr);
			arr.pop_front();
			return fval;
		}

		void push_back(array &arr, const var &val)
		{
			arr.push_back(copy(val));
		}

		var pop_back(array &arr)
		{
			var bval = back(arr);
			arr.pop_back();
			return bval;
		}

// Operations
		void sort(array &arr, const var &func)
		{
			std::sort(arr.begin(), arr.end(), [&](const var &lhs, const var &rhs) -> bool
			{ return invoke(func, lhs, rhs).const_val<boolean>(); });
		}

		var to_hash_set(const array &arr)
		{
			hash_set set;
			for (auto &it : arr) {
				if (set.count(it) == 0)
					set.insert(copy(it));
			}
			return var::make<hash_set>(std::move(set));
		}

		var to_hash_map(const array &arr)
		{
			hash_map map;
			for (auto &it : arr) {
				if (it.is_type_of<pair>()) {
					const auto &p = it.const_val<pair>();
					insert_or_assign(map, p.first, p.second);
				}
				else
					throw lang_error("Wrong syntax for hash map.");
			}
			return var::make<hash_map>(std::move(map));
		}

		var enumerate(const array &arr)
		{
			hash_map map;
			std::size_t idx = 0;
			for (auto &it : arr) {
				if (it.is_type_of<pair>()) {
					const auto &p = it.const_val<pair>();
					insert_or_assign(map, p.first, p.second);
				}
				else
					insert_or_assign(map, var::make<numeric>(idx), it);
				++idx;
			}
			return var::make<hash_map>(std::move(map));
		}

		var to_list(const array &arr)
		{
			var lst = var::make<list>(arr.begin(), arr.end());
			lst.detach();
			return std::move(lst);
		}

		string join(const array &arr, const string &sep)
		{
			string str;
			bool insert_sep = false;
			for (auto &it : arr) {
				if (insert_sep)
					str.append(sep);
				else
					insert_sep = true;
				if (it.is_type_of<string>())
					str.append(it.const_val<string>());
				else
					str.append(it.to_string());
			}
			return str;
		}

		void init()
		{
			(*array_iterator_ext)
			.add_var("next", make_cni(next, true))
			.add_var("next_n", make_cni(next_n, true))
			.add_var("prev", make_cni(prev, true))
			.add_var("prev_n", make_cni(prev_n, true))
			.add_var("data", make_cni(data, callable::types::member_visitor));
			(*array_ext)
			.add_var("iterator", make_namespace(array_iterator_ext))
			.add_var("at", make_cni(at, true))
			.add_var("front", make_cni(front, callable::types::member_visitor))
			.add_var("back", make_cni(back, callable::types::member_visitor))
			.add_var("begin", make_cni(begin, callable::types::member_visitor))
			.add_var("end", make_cni(end, callable::types::member_visitor))
			.add_var("empty", make_cni(empty, true))
			.add_var("size", make_cni(size, callable::types::member_visitor))
			.add_var("clear", make_cni(clear, true))
			.add_var("insert", make_cni(insert, true))
			.add_var("erase", make_cni(erase, true))
			.add_var("push_front", make_cni(push_front, true))
			.add_var("pop_front", make_cni(pop_front, true))
			.add_var("push_back", make_cni(push_back, true))
			.add_var("pop_back", make_cni(pop_back, true))
			.add_var("sort", make_cni(sort, true))
			.add_var("to_hash_set", make_cni(to_hash_set, true))
			.add_var("to_hash_map", make_cni(to_hash_map, true))
			.add_var("enumerate", make_cni(enumerate, true))
			.add_var("to_list", make_cni(to_list, true))
			.add_var("join", make_cni(join, true));
		}
	} // namespace array_cs_ext

	namespace number_cs_ext {
		using namespace cs;

		bool is_integer(const numeric &n)
		{
			return n.is_integer();
		}

		bool is_float(const numeric &n)
		{
			return n.is_float();
		}

		numeric &ntoi(numeric &n)
		{
			return n = n.as_integer();
		}

		numeric &ntof(numeric &n)
		{
			return n = n.as_float();
		}

		void init()
		{
			(*number_ext)
			.add_var("is_integer", make_cni(is_integer))
			.add_var("is_float", make_cni(is_float))
			.add_var("ntoi", make_cni(ntoi))
			.add_var("to_integer", make_cni(ntoi))
			.add_var("ntof", make_cni(ntof))
			.add_var("to_float", make_cni(ntof));
		}
	} // namespace number_cs_ext
	namespace char_cs_ext {
		using namespace cs;

		bool isalnum(char c)
		{
			return std::isalnum(c);
		}

		bool isalpha(char c)
		{
			return std::isalpha(c);
		}

		bool islower(char c)
		{
			return std::islower(c);
		}

		bool isupper(char c)
		{
			return std::isupper(c);
		}

		bool isdigit(char c)
		{
			return std::isdigit(c);
		}

		bool iscntrl(char c)
		{
			return std::iscntrl(c);
		}

		bool isgraph(char c)
		{
			return std::isgraph(c);
		}

		bool isspace(char c)
		{
			return std::isspace(c);
		}

		bool isblank(char c)
		{
			return std::isblank(c);
		}

		bool isprint(char c)
		{
			return std::isprint(c);
		}

		bool ispunct(char c)
		{
			return std::ispunct(c);
		}

		char tolower(char c)
		{
			return std::tolower(c);
		}

		char toupper(char c)
		{
			return std::toupper(c);
		}

		numeric to_ascii(char c)
		{
			return static_cast<numeric_integer>(static_cast<unsigned char>(c));
		}

		char from_ascii(const numeric &ascii)
		{
			if (ascii.as_integer() < 0 || ascii.as_integer() > 255)
				throw lang_error("Out of range.");
			return static_cast<char>(static_cast<unsigned char>(ascii.as_integer()));
		}

		void init()
		{
			(*char_ext)
			.add_var("isalnum", make_cni(isalnum, true))
			.add_var("isalpha", make_cni(isalpha, true))
			.add_var("islower", make_cni(islower, true))
			.add_var("isupper", make_cni(isupper, true))
			.add_var("isdigit", make_cni(isdigit, true))
			.add_var("iscntrl", make_cni(iscntrl, true))
			.add_var("isgraph", make_cni(isgraph, true))
			.add_var("isspace", make_cni(isspace, true))
			.add_var("isblank", make_cni(isblank, true))
			.add_var("isprint", make_cni(isprint, true))
			.add_var("ispunct", make_cni(ispunct, true))
			.add_var("tolower", make_cni(tolower, true))
			.add_var("toupper", make_cni(toupper, true))
			.add_var("to_ascii", make_cni(to_ascii, true))
			.add_var("from_ascii", make_cni(from_ascii, true));
		}
	} // namespace char_cs_ext
	namespace except_cs_ext {
		using namespace cs;

		string what(const lang_error &le)
		{
			return le.what();
		}

		void init()
		{
			except_ext->add_var("what", make_cni(what, callable::types::member_visitor));
		}
	} // namespace except_cs_ext
	namespace hash_set_cs_ext {
		using namespace cs;

// Capacity
		bool empty(const hash_set &set)
		{
			return set.empty();
		}

		numeric size(const hash_set &set)
		{
			return set.size();
		}

// Modifiers
		void clear(hash_set &set)
		{
			set.clear();
		}

		void insert(hash_set &set, const var &val)
		{
			set.insert(copy(val));
		}

		void erase(hash_set &set, const var &val)
		{
			set.erase(val);
		}

// Lookup
		bool exist(hash_set &set, const var &val)
		{
			return set.count(val) > 0;
		}

// Set Operations
		var intersect(const hash_set &lhs, const hash_set &rhs)
		{
			var ret = var::make<hash_set>();
			hash_set &s = ret.val<hash_set>();
			for (auto &it : lhs) {
				if (rhs.count(it) > 0)
					s.emplace(it);
			}
			return ret;
		}

		var merge(const hash_set &lhs, const hash_set &rhs)
		{
			var ret = var::make<hash_set>(lhs);
			hash_set &s = ret.val<hash_set>();
			for (auto &it : rhs) {
				if (s.count(it) == 0)
					s.emplace(it);
			}
			return ret;
		}

		var subtract(const hash_set &lhs, const hash_set &rhs)
		{
			var ret = var::make<hash_set>(lhs);
			hash_set &s = ret.val<hash_set>();
			for (auto &it : rhs) {
				if (s.count(it) > 0)
					s.erase(it);
			}
			return ret;
		}

		void init()
		{
			(*hash_set_ext)
			.add_var("empty", make_cni(empty, true))
			.add_var("size", make_cni(size, callable::types::member_visitor))
			.add_var("clear", make_cni(empty, true))
			.add_var("insert", make_cni(insert, true))
			.add_var("erase", make_cni(erase, true))
			.add_var("exist", make_cni(exist, true))
			.add_var("intersect", make_cni(intersect, callable::types::force_regular))
			.add_var("merge", make_cni(merge, callable::types::force_regular))
			.add_var("subtract", make_cni(subtract, callable::types::force_regular));
		}
	} // namespace hash_set_cs_ext
	namespace hash_map_cs_ext {
		using namespace cs;

// Capacity
		bool empty(const hash_map &map)
		{
			return map.empty();
		}

		numeric size(const hash_map &map)
		{
			return map.size();
		}

// Modifiers
		void clear(hash_map &map)
		{
			map.clear();
		}

		void insert(hash_map &map, const var &key, const var &val)
		{
			insert_or_assign(map, key, val);
		}

		void erase(hash_map &map, const var &key)
		{
			map.erase(key);
		}

// Lookup
		var at(hash_map &map, const var &key)
		{
			return map.at(key);
		}

		bool exist(hash_map &map, const var &key)
		{
			return map.count(key) > 0;
		}

		var keys(const hash_map &map)
		{
			var val = var::make<array>();
			array &arr = val.val<array>();
			for (auto &it : map)
				arr.emplace_back(copy(it.first));
			return val;
		}

		var values(const hash_map &map)
		{
			var val = var::make<array>();
			array &arr = val.val<array>();
			for (auto &it : map)
				arr.emplace_back(copy(it.second));
			return val;
		}

		void init()
		{
			(*hash_map_ext)
			.add_var("empty", make_cni(empty, true))
			.add_var("size", make_cni(size, callable::types::member_visitor))
			.add_var("clear", make_cni(clear, true))
			.add_var("insert", make_cni(insert, true))
			.add_var("erase", make_cni(erase, true))
			.add_var("at", make_cni(at, true))
			.add_var("exist", make_cni(exist, true))
			.add_var("keys", make_cni(keys, true))
			.add_var("values", make_cni(values, true));
		}
	} // namespace hash_map_cs_ext

	namespace iostream_cs_ext {
		using namespace cs;

		var fstream(const string &path, std::ios_base::openmode openmode)
		{
			if (openmode & std::ios_base::in)
				return var::make<istream>(new std::ifstream(path, openmode));
			else if (openmode & std::ios_base::out || openmode & std::ios_base::app)
				return var::make<ostream>(new std::ofstream(path, openmode));
			else
				throw lang_error("Unsupported openmode.");
		}

		void setprecision(const numeric &pre)
		{
			current_process->output_precision = pre.as_integer();
		}

		void init()
		{
			(*iostream_ext)
			.add_var("istream", make_namespace(istream_ext))
			.add_var("ostream", make_namespace(ostream_ext))
			.add_var("seekdir", make_namespace(seekdir_ext))
			.add_var("openmode", make_namespace(openmode_ext));
			(*seekdir_ext)
			.add_var("start", var::make_constant<std::ios_base::seekdir>(std::ios_base::beg))
			.add_var("finish", var::make_constant<std::ios_base::seekdir>(std::ios_base::end))
			.add_var("present", var::make_constant<std::ios_base::seekdir>(std::ios_base::cur));
			(*openmode_ext)
			.add_var("in", var::make_constant<std::ios_base::openmode>(std::ios_base::in))
			.add_var("bin_in",
			         var::make_constant<std::ios_base::openmode>(std::ios_base::in | std::ios_base::binary))
			.add_var("out", var::make_constant<std::ios_base::openmode>(std::ios_base::out))
			.add_var("bin_out",
			         var::make_constant<std::ios_base::openmode>(std::ios_base::out | std::ios_base::binary))
			.add_var("app", var::make_constant<std::ios_base::openmode>(std::ios_base::app))
			.add_var("bin_app",
			         var::make_constant<std::ios_base::openmode>(std::ios_base::app | std::ios_base::binary));
			(*iostream_ext)
			.add_var("char_buff",
			         var::make_protect<type_t>([]() -> var
			{ return std::make_shared<std::stringstream>(); },
			type_id(typeid(char_buff)), charbuff_ext))
			.add_var("fstream", make_cni(fstream))
			.add_var("ifstream", make_cni([](const string &path) {
				return var::make<istream>(new std::ifstream(path, std::ios_base::in));
			}))
			.add_var("ofstream", make_cni([](const string &path) {
				return var::make<ostream>(new std::ofstream(path, std::ios_base::out));
			}))
			.add_var("setprecision", make_cni(setprecision));
		}
	} // namespace iostream_cs_ext
	namespace charbuff_cs_ext {
		using namespace cs;

		void init()
		{
			(*charbuff_ext)
			.add_var("get_istream", make_cni([](char_buff &buff) -> cs::istream
			{ return std::shared_ptr<std::istream>(buff.get(), [](std::istream *) {}); }))
			.add_var("get_ostream", make_cni([](char_buff &buff) -> cs::ostream
			{ return std::shared_ptr<std::ostream>(buff.get(), [](std::ostream *) {}); }))
			.add_var("get_string", make_cni([](char_buff &buff) -> string
			{ return std::move(buff->str()); }));
		}
	} // namespace charbuff_cs_ext
	namespace istream_cs_ext {
		using namespace cs;

		var parse_value(const std::string &str)
		{
			if (str == "true")
				return true;
			if (str == "false")
				return false;
			try {
				return parse_number(str);
			}
			catch (...) {
				return str;
			}
			return str;
		}

// Input Stream
		char get(istream &in)
		{
			return in->get();
		}

		char peek(istream &in)
		{
			return in->peek();
		}

		void unget(istream &in)
		{
			in->unget();
		}

		string getline(istream &in)
		{
			std::string str;
			std::getline(*in, str);
			return std::move(str);
		}

		numeric tell(istream &in)
		{
			return in->tellg();
		}

		void seek(istream &in, const numeric &pos)
		{
			in->seekg(pos.as_integer());
		}

		void seek_from(istream &in, std::ios_base::seekdir dir, const numeric &offset)
		{
			in->seekg(offset.as_integer(), dir);
		}

		bool good(const istream &in)
		{
			return static_cast<bool>(*in);
		}

		bool eof(const istream &in)
		{
			return in->eof();
		}

		var input(istream &in)
		{
			std::string str;
			*in >> str;
			return parse_value(str);
		}

		void ignore(istream &in)
		{
			in->ignore((std::numeric_limits<std::streamsize>::max)(), '\n');
		}

		string read(istream &in, const numeric &n)
		{
			string buff(n.as_integer(), '\0');
			in->read(&buff[0], buff.size());
			buff.resize(in->gcount());
			return buff;
		}

		void init()
		{
			(*istream_ext)
			.add_var("get", make_cni(get))
			.add_var("peek", make_cni(peek))
			.add_var("unget", make_cni(unget))
			.add_var("getline", make_cni(getline))
			.add_var("tell", make_cni(tell))
			.add_var("seek", make_cni(seek))
			.add_var("seek_from", make_cni(seek_from))
			.add_var("good", make_cni(good))
			.add_var("eof", make_cni(eof))
			.add_var("input", make_cni(input))
			.add_var("ignore", make_cni(ignore))
			.add_var("read", make_cni(read));
		}
	} // namespace istream_cs_ext
	namespace ostream_cs_ext {
		using namespace cs;

// Output Stream
		void put(ostream &out, char c)
		{
			out->put(c);
		}

		numeric tell(ostream &out)
		{
			return out->tellp();
		}

		void seek(ostream &out, const numeric &pos)
		{
			out->seekp(pos.as_integer());
		}

		void seek_from(ostream &out, std::ios_base::seekdir dir, const numeric &offset)
		{
			out->seekp(offset.as_integer(), dir);
		}

		void flush(ostream &out)
		{
			out->flush();
		}

		bool good(const ostream &out)
		{
			return static_cast<bool>(*out);
		}

		void print(ostream &out, const var &val)
		{
			*out << val << std::flush;
		}

		void println(ostream &out, const var &val)
		{
			*out << val << std::endl;
		}

		void write(ostream &out, const string &str)
		{
			out->write(str.data(), str.size());
		}

		void init()
		{
			(*ostream_ext)
			.add_var("put", make_cni(put))
			.add_var("tell", make_cni(tell))
			.add_var("seek", make_cni(seek))
			.add_var("seek_from", make_cni(seek_from))
			.add_var("flush", make_cni(flush))
			.add_var("good", make_cni(good))
			.add_var("print", make_cni(print))
			.add_var("println", make_cni(println))
			.add_var("write", make_cni(write));
		}
	} // namespace ostream_cs_ext
	namespace list_cs_ext {
		using namespace cs;

// Element access
		var front(const list &lst)
		{
			if (lst.empty())
				throw lang_error("Call front() on empty list.");
			return lst.front();
		}

		var back(const list &lst)
		{
			if (lst.empty())
				throw lang_error("Call back() on empty list.");
			return lst.back();
		}

// Iterators
		list::iterator begin(list &lst)
		{
			return lst.begin();
		}

		list::iterator end(list &lst)
		{
			return lst.end();
		}

		list::iterator next(list::iterator &it)
		{
			return it++;
		}

		list::iterator prev(list::iterator &it)
		{
			return it--;
		}

		var data(list::iterator &it)
		{
			return *it;
		}

// Capacity
		bool empty(const list &lst)
		{
			return lst.empty();
		}

		numeric size(const list &lst)
		{
			return lst.size();
		}

// Modifiers
		void clear(list &lst)
		{
			lst.clear();
		}

		list::iterator insert(list &lst, list::iterator &pos, const var &val)
		{
			return lst.insert(pos, copy(val));
		}

		list::iterator erase(list &lst, list::iterator &pos)
		{
			return lst.erase(pos);
		}

		void push_front(list &lst, const var &val)
		{
			lst.push_front(copy(val));
		}

		var pop_front(list &lst)
		{
			var fval = front(lst);
			lst.pop_front();
			return fval;
		}

		void push_back(list &lst, const var &val)
		{
			lst.push_back(copy(val));
		}

		var pop_back(list &lst)
		{
			var bval = back(lst);
			lst.pop_back();
			return bval;
		}

// Operations
		void remove(list &lst, const var &val)
		{
			lst.remove(val);
		}

		void reverse(list &lst)
		{
			lst.reverse();
		}

		void unique(list &lst)
		{
			lst.unique();
		}

		void sort(list &lst, const var &func)
		{
			lst.sort([&](const var &lhs, const var &rhs) -> bool
			{ return invoke(func, lhs, rhs).const_val<boolean>(); });
		}

		void init()
		{
			(*list_iterator_ext)
			.add_var("next", make_cni(next, true))
			.add_var("prev", make_cni(prev, true))
			.add_var("data", make_cni(data, callable::types::member_visitor));
			(*list_ext)
			.add_var("iterator", make_namespace(list_iterator_ext))
			.add_var("front", make_cni(front, callable::types::member_visitor))
			.add_var("back", make_cni(back, callable::types::member_visitor))
			.add_var("begin", make_cni(begin, callable::types::member_visitor))
			.add_var("end", make_cni(end, callable::types::member_visitor))
			.add_var("empty", make_cni(empty, true))
			.add_var("size", make_cni(size, callable::types::member_visitor))
			.add_var("clear", make_cni(clear, true))
			.add_var("insert", make_cni(insert, true))
			.add_var("erase", make_cni(erase, true))
			.add_var("push_front", make_cni(push_front, true))
			.add_var("pop_front", make_cni(pop_front, true))
			.add_var("push_back", make_cni(push_back, true))
			.add_var("pop_back", make_cni(pop_back, true))
			.add_var("remove", make_cni(remove, true))
			.add_var("reverse", make_cni(reverse, true))
			.add_var("unique", make_cni(unique, true))
			.add_var("sort", make_cni(sort, true));
		}
	} // namespace list_cs_ext
	namespace math_cs_ext {
		using namespace cs;

		numeric abs(const numeric &n)
		{
			return std::abs(n.as_float());
		}

		numeric ln(const numeric &n)
		{
			return std::log(n.as_float());
		}

		numeric log10(const numeric &n)
		{
			return std::log10(n.as_float());
		}

		numeric log(const numeric &a, const numeric &b)
		{
			return std::log(b.as_float()) / std::log(a.as_float());
		}

		numeric sin(const numeric &n)
		{
			return std::sin(n.as_float());
		}

		numeric cos(const numeric &n)
		{
			return std::cos(n.as_float());
		}

		numeric tan(const numeric &n)
		{
			return std::tan(n.as_float());
		}

		numeric asin(const numeric &n)
		{
			return std::asin(n.as_float());
		}

		numeric acos(const numeric &n)
		{
			return std::acos(n.as_float());
		}

		numeric atan(const numeric &n)
		{
			return std::atan(n.as_float());
		}

		numeric sqrt(const numeric &n)
		{
			return std::sqrt(n.as_float());
		}

		numeric root(const numeric &a, const numeric &b)
		{
			return std::pow(a.as_float(), numeric_float(1) / b.as_float());
		}

		numeric pow(const numeric &a, const numeric &b)
		{
			return std::pow(a.as_float(), b.as_float());
		}

		numeric _min(const numeric &a, const numeric &b)
		{
			if (a.is_integer() && b.is_integer())
				return (std::min) (a.as_integer(), b.as_integer());
			else
				return (std::min) (a.as_float(), b.as_float());
		}

		numeric _max(const numeric &a, const numeric &b)
		{
			if (a.is_integer() && b.is_integer())
				return (std::max) (a.as_integer(), b.as_integer());
			else
				return (std::max) (a.as_float(), b.as_float());
		}

		numeric rand(const numeric &b, const numeric &e)
		{
			return cov::rand<numeric_float>(b.as_float(), e.as_float());
		}

		numeric randint(const numeric &b, const numeric &e)
		{
			return cov::rand<numeric_integer>(b.as_integer(), e.as_integer());
		}

		void init()
		{
			(*math_const_ext)
			.add_var("max", var::make_constant<numeric>((std::numeric_limits<numeric_float>::max)()))
			.add_var("min", var::make_constant<numeric>((std::numeric_limits<numeric_float>::min)()))
			.add_var("integer_max", var::make_constant<numeric>((std::numeric_limits<numeric_integer>::max)()))
			.add_var("integer_min", var::make_constant<numeric>((std::numeric_limits<numeric_integer>::min)()))
			.add_var("float_max", var::make_constant<numeric>((std::numeric_limits<numeric_float>::max)()))
			.add_var("float_min", var::make_constant<numeric>((std::numeric_limits<numeric_float>::min)()))
			.add_var("inf", var::make_constant<numeric>(std::numeric_limits<numeric_float>::infinity()))
			.add_var("nan", var::make_constant<numeric>(std::numeric_limits<numeric_float>::quiet_NaN()))
			.add_var("pi", var::make_constant<numeric>(std::asin(numeric_float(1)) * 2))
			.add_var("e", var::make_constant<numeric>(std::exp(numeric_float(1))));
			(*math_ext)
			.add_var("constants", make_namespace(math_const_ext))
			.add_var("abs", make_cni(abs, true))
			.add_var("ln", make_cni(ln, true))
			.add_var("log10", make_cni(log10, true))
			.add_var("log", make_cni(log, true))
			.add_var("sin", make_cni(sin, true))
			.add_var("cos", make_cni(cos, true))
			.add_var("tan", make_cni(tan, true))
			.add_var("asin", make_cni(asin, true))
			.add_var("acos", make_cni(acos, true))
			.add_var("atan", make_cni(atan, true))
			.add_var("sqrt", make_cni(sqrt, true))
			.add_var("root", make_cni(root, true))
			.add_var("pow", make_cni(pow, true))
			.add_var("min", make_cni(_min, true))
			.add_var("max", make_cni(_max, true))
			.add_var("rand", make_cni(rand))
			.add_var("randint", make_cni(randint));
		}
	} // namespace math_cs_ext
	namespace pair_cs_ext {
		void init()
		{
			using namespace cs;
			(*pair_ext)
			.add_var("first", make_member_visitor(&pair::first))
			.add_var("key", make_member_visitor(&pair::first))
			.add_var("second", make_member_visitor(&pair::second))
			.add_var("value", make_member_visitor(&pair::second));
		}
	} // namespace pair_cs_ext
	namespace time_cs_ext {
		using namespace cs;

		numeric sec(const std::tm &t)
		{
			return t.tm_sec;
		}

		numeric tm_min(const std::tm &t)
		{
			return t.tm_min;
		}

		numeric hour(const std::tm &t)
		{
			return t.tm_hour;
		}

		numeric wday(const std::tm &t)
		{
			return t.tm_wday;
		}

		numeric mday(const std::tm &t)
		{
			return t.tm_mday;
		}

		numeric yday(const std::tm &t)
		{
			return t.tm_yday;
		}

		numeric mon(const std::tm &t)
		{
			return t.tm_mon;
		}

		numeric year(const std::tm &t)
		{
			return t.tm_year;
		}

		bool is_dst(const std::tm &t)
		{
			return t.tm_isdst;
		}

		numeric unixtime(const std::tm &t)
		{
			return std::mktime(const_cast<tm *>(&t));
		}

		void init()
		{
			(*time_ext)
			.add_var("sec", make_cni(sec, callable::types::member_visitor))
			.add_var("min", make_cni(tm_min, callable::types::member_visitor))
			.add_var("hour", make_cni(hour, callable::types::member_visitor))
			.add_var("wday", make_cni(wday, callable::types::member_visitor))
			.add_var("mday", make_cni(mday, callable::types::member_visitor))
			.add_var("yday", make_cni(yday, callable::types::member_visitor))
			.add_var("mon", make_cni(mon, callable::types::member_visitor))
			.add_var("year", make_cni(year, callable::types::member_visitor))
			.add_var("is_dst", make_cni(is_dst, callable::types::member_visitor))
			.add_var("unixtime", make_cni(unixtime, callable::types::member_visitor));
		}
	} // namespace time_cs_ext

	namespace fiber_cs_ext {
		using namespace cs;

		class fiber_function final {
			const context_t &context;
			function const *func;
			vector args;

		public:
			fiber_function(function const *fn, vector data) : context(fn->get_context()), func(fn), args(std::move(data)) {}

			var operator()()
			{
				if (func == nullptr)
					throw lang_error("Asynchronous functions are not reentrant.");
				try {
					var ret = func->call(args);
					func = nullptr;
					args.clear();
					context->instance->clear_context();
					return std::move(ret);
				}
				catch (...) {
					func = nullptr;
					args.clear();
					context->instance->clear_context();
					throw;
				}
			}
		};

		var create(vector &args)
		{
			if (args.empty())
				throw lang_error("Empty arguments. Expected: fiber.create(function, arguments...)");
			const var &func = args.front();
			if (func.is_type_of<callable>()) {
				const callable::function_type &impl_f = func.const_val<callable>().get_raw_data();
				if (impl_f.target_type() != typeid(function))
					throw lang_error("Only can create coroutine from covscript function.");
				function const *fptr = impl_f.target<function>();
				return fiber::create(fptr->get_context(), fiber_function(fptr, vector(args.begin() + 1, args.end())));
			}
			else if (func.is_type_of<object_method>()) {
				const auto &om = func.const_val<object_method>();
				const callable::function_type &impl_f = om.callable.const_val<callable>().get_raw_data();
				if (impl_f.target_type() != typeid(function))
					throw lang_error("Only can create coroutine from covscript function.");
				function const *fptr = impl_f.target<function>();
				vector argument{om.object};
				argument.insert(argument.end(), args.begin() + 1, args.end());
				return fiber::create(fptr->get_context(), fiber_function(fptr, std::move(argument)));
			}
			return null_pointer;
		}

		var return_value(const fiber_t &fiber)
		{
			return fiber->return_value();
		}

		bool is_running(const fiber_t &fiber)
		{
			return fiber->get_state() == fiber_state::running;
		}

		bool is_suspended(const fiber_t &fiber)
		{
			return fiber->get_state() == fiber_state::suspended;
		}

		bool is_finished(const fiber_t &fiber)
		{
			return fiber->get_state() == fiber_state::finished;
		}

		void init()
		{
			(*fiber_ext)
			.add_var("create", var::make_protect<callable>(create))
			.add_var("return_value", make_cni(return_value))
			.add_var("is_running", make_cni(is_running))
			.add_var("is_suspended", make_cni(is_suspended))
			.add_var("is_finished", make_cni(is_finished))
			.add_var("resume", make_cni(fiber::resume))
			.add_var("yield", make_cni(fiber::yield));
		}
	} // namespace fiber_cs_ext

	namespace runtime_cs_ext {
		using namespace cs;

		class async_base {
		public:
			virtual ~async_base() = default;

			virtual void context_swap_in() {}

			virtual void context_swap_out() {}

			virtual void context_cleanup() {}

			virtual var call() = 0;
		};

		class async_function final : public async_base {
			process_context *this_context = current_process;
			std::unique_ptr<process_context> pcontext;
			stack_type<domain_type> stack;
			const context_t &context;
			function const *func;
			vector args;

		public:
			async_function(function const *fn, vector data) : context(fn->get_context()), pcontext(current_process->fork()), stack(current_process->child_stack_size()), func(fn), args(std::move(data)) {}

			void context_swap_in() override
			{
				context->instance->swap_context(&stack);
				current_process = pcontext.get();
			}

			void context_swap_out() override
			{
				context->instance->swap_context(nullptr);
				current_process = this_context;
			}

			void context_cleanup() override
			{
				context->instance->clear_context();
				func = nullptr;
				args.clear();
			}

			var call() override
			{
				if (func == nullptr)
					throw lang_error("Asynchronous functions are not reentrant.");
				try {
					context_swap_in();
					var ret = func->call(args);
					context_swap_out();
					context_cleanup();
					return std::move(ret);
				}
				catch (...) {
					context_swap_out();
					context_cleanup();
					current_process->eptr_mutex.lock();
					current_process->eptr = std::current_exception();
					current_process->eptr_mutex.unlock();
					return null_pointer;
				}
			}

			var operator()()
			{
				return call();
			}
		};

		bool is_native_callable(const callable &func)
		{
			return func.get_raw_data().target_type() != typeid(function) || func.get_raw_data().target<function>()->is_el_func();
		}

		class async_callable final : public async_base {
			callable func;
			vector args;

			void detach_args()
			{
				for (auto &val : args) {
					if (!val.is_rvalue()) {
						val.clone();
						val.detach();
					}
					else
						val.mark_trivial();
				}
			}

		public:
			async_callable(const callable &fn, vector data) : func(fn), args(std::move(data))
			{
				if (!is_native_callable(fn))
					throw lang_error("Invoke non-parallelizable object.");
				detach_args();
			}

			var call() override
			{
				try {
					return func.call(args);
				}
				catch (...) {
					current_process->eptr_mutex.lock();
					current_process->eptr = std::current_exception();
					current_process->eptr_mutex.unlock();
					return null_pointer;
				}
			}

			var operator()()
			{
				return call();
			}
		};

		std::unique_ptr<async_base> make_async_wrapper(const callable &func, vector data)
		{
			if (is_native_callable(func))
				return std::make_unique<async_function>(func.get_raw_data().target<function>(), std::move(data));
			else
				return std::make_unique<async_callable>(func, std::move(data));
		}

		var await_impl(const callable &fn, vector args)
		{
			if (!is_native_callable(fn))
				throw lang_error("Asynchronous waiting only available on native functions.");

			async_callable func(fn, std::move(args));
			var ret;
			if (!current_process->fiber_stack.empty()) {
				thread_guard guard;
				auto future = std::async(std::launch::async, func);
				while (future.wait_for(std::chrono::milliseconds(0)) == std::future_status::timeout)
					fiber::yield();
				ret = future.get();
			}
			else
				ret = func();

			std::exception_ptr e = nullptr;
			current_process->eptr_mutex.lock();
			if (current_process->eptr != nullptr)
				std::swap(current_process->eptr, e);
			current_process->eptr_mutex.unlock();
			if (e != nullptr)
				std::rethrow_exception(e);

			return std::move(ret);
		}

		string get_import_path()
		{
			return current_process->import_path;
		}

		void info()
		{
			std::cout << "Covariant Script Programming Language Interpreter\nVersion: " << current_process->version
			          << std::endl;
			std::cout << copyright_info << std::endl;
		}

		numeric time()
		{
			return cov::timer::time(cov::timer::time_unit::milli_sec);
		}

		var local_time(vector &args)
		{
			std::time_t t;
			switch (args.size()) {
			case 0:
				t = std::time(nullptr);
				return var::make<std::tm>(*std::localtime(&t));
			case 1:
				t = args[0].const_val<numeric>().as_integer();
				return var::make<std::tm>(*std::localtime(&t));
			default:
				throw runtime_error(
				    "Wrong size of the arguments. Expected 0 or 1, provided " + std::to_string(args.size()));
			}
		}

		var utc_time(vector &args)
		{
			std::time_t t;
			switch (args.size()) {
			case 0:
				t = std::time(nullptr);
				return var::make<std::tm>(*std::gmtime(&t));
			case 1:
				t = args[0].const_val<numeric>().as_integer();
				return var::make<std::tm>(*std::gmtime(&t));
			default:
				throw runtime_error(
				    "Wrong size of the arguments. Expected 0 or 1, provided " + std::to_string(args.size()));
			}
		}

		void delay(const numeric &time)
		{
			cov::timer::delay(cov::timer::time_unit::milli_sec, time.as_integer());
		}

		var exception(const string &str)
		{
			return var::make<lang_error>(str);
		}

		std::size_t hash(const var &val)
		{
			return val.hash();
		}

		var build(const context_t &context, const string &expr)
		{
			std::deque<char> buff;
			expression_t tree;
			for (auto &ch : expr)
				buff.push_back(ch);
			context->compiler->build_expr(buff, tree);
			return var::make<expression_t>(tree);
		}

		var solve(const context_t &context, expression_t &tree)
		{
			return context->instance->parse_expr(tree.root());
		}

		var cmd_args(const context_t &context)
		{
			return context->cmd_args;
		}

		var import(const context_t &context, const string &dir, const string &name)
		{
			try {
				return make_namespace(context->instance->import(dir, name));
			}
			catch (...) {
				return null_pointer;
			}
		}

		var source_import(const context_t &context, const string &path)
		{
			try {
				return make_namespace(context->instance->source_import(path));
			}
			catch (...) {
				return null_pointer;
			}
		}

		numeric argument_count(const var &func)
		{
			if (func.is_type_of<object_method>()) {
				const callable::function_type &target = func.const_val<object_method>().callable.const_val<callable>().get_raw_data();
				if (target.target_type() == typeid(function))
					return target.target<function>()->argument_count() - 1;
				else
					return target.target<cni>()->argument_count() - 1;
			}
			else if (func.is_type_of<callable>()) {
				const callable::function_type &target = func.const_val<callable>().get_raw_data();
				if (target.target_type() == typeid(function))
					return target.target<function>()->argument_count();
				else
					return target.target<cni>()->argument_count();
			}
			else
				throw lang_error("Not a function.");
		}

		void add_string_literal(const context_t &context, const std::string &literal, const callable &func)
		{
			context->instance->add_string_literal(literal, func);
		}

		var wait_for_impl(std::size_t mill_sec, const callable &func, vector args)
		{
			std::unique_ptr<async_base> async_fn(make_async_wrapper(func, std::move(args)));
			std::future<var> future = std::async(std::launch::async, [&]() {
				return async_fn->call();
			});
			if (future.wait_for(std::chrono::milliseconds(mill_sec)) != std::future_status::ready) {
				async_fn->context_swap_out();
				async_fn->context_cleanup();
				throw lang_error("Target function deferred or timeout.");
			}
			else {
				current_process->eptr_mutex.lock();
				if (current_process->eptr != nullptr) {
					std::exception_ptr e = nullptr;
					std::swap(current_process->eptr, e);
					current_process->eptr_mutex.unlock();
					std::rethrow_exception(e);
				}
				current_process->eptr_mutex.unlock();
				return future.get();
			}
		}

		var wait_until_impl(std::size_t mill_sec, const callable &func, vector args)
		{
			std::unique_ptr<async_base> async_fn(make_async_wrapper(func, std::move(args)));
			std::future<var> future = std::async(std::launch::async, [&]() {
				return async_fn->call();
			});
			if (future.wait_until(std::chrono::system_clock::now() + std::chrono::milliseconds(mill_sec)) !=
			        std::future_status::ready) {
				async_fn->context_swap_out();
				async_fn->context_cleanup();
				throw lang_error("Target function deferred or timeout.");
			}
			else {
				current_process->eptr_mutex.lock();
				if (current_process->eptr != nullptr) {
					std::exception_ptr e = nullptr;
					std::swap(current_process->eptr, e);
					current_process->eptr_mutex.unlock();
					std::rethrow_exception(e);
				}
				current_process->eptr_mutex.unlock();
				return future.get();
			}
		}

		var wait_for(const numeric &mill_sec, const var &func, const array &argument)
		{
			if (func.is_type_of<callable>()) {
				return wait_for_impl(mill_sec.as_integer(), func.const_val<callable>(), vector(argument.begin(), argument.end()));
			}
			else if (func.is_type_of<object_method>()) {
				const auto &om = func.const_val<object_method>();
				vector args{om.object};
				args.insert(args.end(), argument.begin(), argument.end());
				return wait_for_impl(mill_sec.as_integer(), om.callable.const_val<callable>(), std::move(args));
			}
			else
				throw lang_error("Invoke non-callable object.");
		}

		var wait_until(const numeric &mill_sec, const var &func, const array &argument)
		{
			if (func.is_type_of<callable>()) {
				return wait_until_impl(mill_sec.as_integer(), func.const_val<callable>(), vector(argument.begin(), argument.end()));
			}
			else if (func.is_type_of<object_method>()) {
				const auto &om = func.const_val<object_method>();
				vector args{om.object};
				args.insert(args.end(), argument.begin(), argument.end());
				return wait_for_impl(mill_sec.as_integer(), om.callable.const_val<callable>(), std::move(args));
			}
			else
				throw lang_error("Invoke non-callable object.");
		}

		var await(vector &args)
		{
			if (args.empty())
				throw lang_error("Empty arguments. Expected: runtime.await(function, arguments...)");
			const var &func = args.front();
			if (func.is_type_of<callable>()) {
				return await_impl(func.const_val<callable>(), vector(args.begin() + 1, args.end()));
			}
			else if (func.is_type_of<object_method>()) {
				const auto &om = func.const_val<object_method>();
				vector argument{om.object};
				argument.insert(argument.end(), args.begin() + 1, args.end());
				return await_impl(om.callable.const_val<callable>(), std::move(argument));
			}
			else
				throw lang_error("Invoke non-callable object.");
		}

		void link_var(const context_t &context, const string &a, const var &b)
		{
			context->instance->storage.get_var(a) = b;
		}

		void unlink_var(const context_t &context, const string &a)
		{
			var &_a = context->instance->storage.get_var(a);
			_a = copy(_a);
		}

		void init()
		{
			(*runtime_ext)
			.add_var("time_type", make_namespace(time_ext))
			.add_var("std_version", var::make_constant<numeric>(current_process->std_version))
			.add_var("get_sdk_path", make_cni(get_sdk_path))
			.add_var("get_import_path", make_cni(get_import_path, true))
			.add_var("info", make_cni(info))
			.add_var("time", make_cni(time))
			.add_var("local_time", var::make_protect<callable>(local_time))
			.add_var("utc_time", var::make_protect<callable>(utc_time))
			.add_var("delay", make_cni(delay))
			.add_var("exception", make_cni(exception))
			.add_var("hash", make_cni(hash, true))
			.add_var("build", make_cni(build))
			.add_var("solve", make_cni(solve))
			.add_var("cmd_args", make_cni(cmd_args, true))
			.add_var("import", make_cni(import, true))
			.add_var("source_import", make_cni(source_import, true))
			.add_var("argument_count", make_cni(argument_count, true))
			.add_var("add_literal", make_cni(add_string_literal, true))
			.add_var("get_current_dir", make_cni(file_system::get_current_dir))
			.add_var("wait_for", make_cni(wait_for))
			.add_var("wait_until", make_cni(wait_until))
			.add_var("await", var::make_protect<callable>(await));
			(*context_ext)
			.add_var("build", make_cni(build))
			.add_var("solve", make_cni(solve))
			.add_var("cmd_args", make_cni(cmd_args, callable::types::member_visitor))
			.add_var("import", make_cni(import, true))
			.add_var("source_import", make_cni(source_import, true))
			.add_var("add_literal", make_cni(add_string_literal, true))
			.add_var("link_var", make_cni(link_var))
			.add_var("unlink_var", make_cni(unlink_var));
		}
	} // namespace runtime_cs_ext

	namespace string_cs_ext {
		using namespace cs;

		string assign(string &str, const numeric &posit, char ch)
		{
			str.at(posit.as_integer()) = ch;
			return str;
		}

		string append(string &str, const var &val)
		{
			if (val.is_type_of<char>())
				str.push_back(val.const_val<char>());
			else if (val.is_type_of<string>())
				str.append(val.const_val<string>());
			else
				str.append(val.to_string());
			return str;
		}

		string insert(string &str, const numeric &posit, const var &val)
		{
			if (val.is_type_of<string>())
				str.insert(posit.as_integer(), val.const_val<string>());
			else
				str.insert(posit.as_integer(), val.to_string());
			return str;
		}

		string erase(string &str, const numeric &b, const numeric &e)
		{
			str.erase(b.as_integer(), e.as_integer());
			return str;
		}

		string replace(string &str, const numeric &posit, const numeric &count, const var &val)
		{
			if (val.is_type_of<string>())
				str.replace(posit.as_integer(), count.as_integer(), val.const_val<string>());
			else
				str.replace(posit.as_integer(), count.as_integer(), val.to_string());
			return str;
		}

		string substr(const string &str, const numeric &b, const numeric &e)
		{
			return str.substr(b.as_integer(), e.as_integer());
		}

		numeric find(const string &str, const string &s, const numeric &posit)
		{
			auto pos = str.find(s, posit.as_integer());
			if (pos == std::string::npos)
				return -1;
			else
				return pos;
		}

		numeric rfind(const string &str, const string &s, const numeric &posit)
		{
			std::size_t pos = 0;
			if (posit.as_integer() == -1)
				pos = str.rfind(s, std::string::npos);
			else
				pos = str.rfind(s, posit.as_integer());
			if (pos == std::string::npos)
				return -1;
			else
				return pos;
		}

		string cut(string &str, const numeric &n)
		{
			for (std::size_t i = 0; i < n.as_integer(); ++i)
				str.pop_back();
			return str;
		}

		bool empty(const string &str)
		{
			return str.empty();
		}

		void clear(string &str)
		{
			str.clear();
		}

		numeric size(const string &str)
		{
			return str.size();
		}

		string tolower(const string &str)
		{
			string s;
			for (auto &ch : str)
				s.push_back(std::tolower(ch));
			return std::move(s);
		}

		string toupper(const string &str)
		{
			string s;
			for (auto &ch : str)
				s.push_back(std::toupper(ch));
			return std::move(s);
		}

		numeric to_number(const string &str)
		{
			return parse_number(str);
		}

		array split(const string &str, const array &signals)
		{
			bool is_sep[256] = {false};
			for (auto &sig : signals)
				is_sep[static_cast<unsigned char>(sig.const_val<char>())] = true;
			array arr;
			string buf;
			for (auto ch : str) {
				if (is_sep[static_cast<unsigned char>(ch)]) {
					if (!buf.empty()) {
						arr.emplace_back(std::move(buf));
						buf.clear();
					}
				}
				else
					buf.push_back(ch);
			}
			if (!buf.empty())
				arr.emplace_back(buf);
			return arr;
		}

		string trim(const string &str)
		{
			if (str.empty())
				return "";
			std::size_t beg = 0;
			std::size_t end = str.size() - 1;
			while (beg <= end && (std::isspace(str[beg]) || std::iscntrl(str[beg])))
				++beg;
			while (end >= beg && (std::isspace(str[end]) || std::iscntrl(str[end])))
				--end;
			return str.substr(beg, end - beg + 1);
		}

		void init()
		{
			(*string_ext)
			.add_var("assign", make_cni(assign, true))
			.add_var("append", make_cni(append, true))
			.add_var("insert", make_cni(insert, true))
			.add_var("erase", make_cni(erase, true))
			.add_var("replace", make_cni(replace, true))
			.add_var("substr", make_cni(substr, true))
			.add_var("find", make_cni(find, true))
			.add_var("rfind", make_cni(rfind, true))
			.add_var("cut", make_cni(cut, true))
			.add_var("empty", make_cni(empty, true))
			.add_var("clear", make_cni(clear, true))
			.add_var("size", make_cni(size, callable::types::member_visitor))
			.add_var("tolower", make_cni(tolower, true))
			.add_var("to_lower", make_cni(tolower, true))
			.add_var("toupper", make_cni(toupper, true))
			.add_var("to_upper", make_cni(tolower, true))
			.add_var("to_number", make_cni(to_number, true))
			.add_var("split", make_cni(split, true))
			.add_var("trim", make_cni(trim, true));
		}
	} // namespace string_cs_ext

	namespace console_cs_ext {
		using namespace cs;
		using namespace cs_impl;

		numeric terminal_width()
		{
			return conio::terminal_width();
		}

		numeric terminal_height()
		{
			return conio::terminal_height();
		}

		void gotoxy(const numeric &x, const numeric &y)
		{
			conio::gotoxy(x.as_integer(), y.as_integer());
		}

		void echo(bool v)
		{
			conio::echo(v);
		}

		void cursor(bool v)
		{
			conio::cursor(v);
		}

		void clrscr()
		{
			conio::clrscr();
		}

		char getch()
		{
			return conio::getch();
		}

		bool kbhit()
		{
			return conio::kbhit();
		}

		void init()
		{
			(*console_ext)
			.add_var("terminal_width", make_cni(terminal_width))
			.add_var("terminal_height", make_cni(terminal_height))
			.add_var("gotoxy", make_cni(gotoxy))
			.add_var("echo", make_cni(echo))
			.add_var("cursor", make_cni(cursor))
			.add_var("clrscr", make_cni(clrscr))
			.add_var("getch", make_cni(getch))
			.add_var("kbhit", make_cni(kbhit));
		}
	} // namespace console_cs_ext

	namespace file_cs_ext {
		using namespace cs;
		using namespace cs_impl;

		numeric ctime(const std::string &path)
		{
			struct cs_sys_stat result;
			if (cs_sys_stat(path.c_str(), &result) == 0)
				return result.st_ctime;
			else
				return 0;
		}

		numeric mtime(const std::string &path)
		{
			struct cs_sys_stat result;
			if (cs_sys_stat(path.c_str(), &result) == 0)
				return result.st_mtime;
			else
				return 0;
		}

		void init()
		{
			using namespace cs_impl::file_system;
			(*file_ext)
			.add_var("copy", make_cni(copy))
			.add_var("rename", make_cni(move))
			.add_var("remove", make_cni(remove))
			.add_var("exist", make_cni(exist))
			.add_var("ctime", make_cni(ctime))
			.add_var("mtime", make_cni(mtime))
			.add_var("can_read", make_cni(can_read))
			.add_var("can_write", make_cni(can_write))
			.add_var("can_execute", make_cni(can_execute));
		}
	} // namespace file_cs_ext

	namespace path_cs_ext {
		using namespace cs;
		using namespace cs_impl;

		string name(const path_info &info)
		{
			return info.name;
		}

		int type(const path_info &info)
		{
			return info.type;
		}

		array scan(const string &path)
		{
			DIR *dir = ::opendir(path.c_str());
			if (dir == nullptr)
				throw lang_error("Path does not exist.");
			array
			entries;
			for (dirent *dp = ::readdir(dir); dp != nullptr; dp = ::readdir(dir))
				entries.push_back(var::make<path_info>(dp->d_name, dp->d_type));
			::closedir(dir);
			return std::move(entries);
		}

		void init()
		{
			using namespace cs_impl::file_system;
			(*path_type_ext)
			.add_var("unknown", var::make_constant<int>(DT_UNKNOWN))
			.add_var("fifo", var::make_constant<int>(DT_FIFO))
			.add_var("sock", var::make_constant<int>(DT_SOCK))
			.add_var("chr", var::make_constant<int>(DT_CHR))
			.add_var("dir", var::make_constant<int>(DT_DIR))
			.add_var("blk", var::make_constant<int>(DT_BLK))
			.add_var("reg", var::make_constant<int>(DT_REG))
			.add_var("lnk", var::make_constant<int>(DT_LNK));
			(*path_info_ext)
			.add_var("name", make_cni(name, callable::types::member_visitor))
			.add_var("type", make_cni(type, callable::types::member_visitor));
			(*path_ext)
			.add_var("type", make_namespace(path_type_ext))
			.add_var("info", make_namespace(path_info_ext))
			.add_var("separator", var::make_constant<char>(path_separator))
			.add_var("delimiter", var::make_constant<char>(path_delimiter))
			.add_var("scan", make_cni(scan))
			.add_var("copy", make_cni(copy))
			.add_var("rename", make_cni(move))
			.add_var("remove", make_cni(remove))
			.add_var("exist", make_cni(is_dir))
			.add_var("is_file", make_cni(is_file))
			.add_var("is_directory", make_cni(is_dir))
			.add_var("mkdir", make_cni(mkdir))
			.add_var("mkdir_p", make_cni(mkdir_p))
			.add_var("chmod", make_cni(chmod))
			.add_var("chmod_r", make_cni(chmod_r));
		}
	} // namespace path_cs_ext
	namespace system_cs_ext {
		using namespace cs;

		numeric run(const string &str)
		{
			return std::system(str.c_str());
		}

		string getenv(const string &name)
		{
			const char *str = std::getenv(name.c_str());
			if (str == nullptr)
				throw lang_error("Environment variable \"" + name + "\" is not exist.");
			return str;
		}

		void exit(const numeric &exit_code)
		{
			int code = exit_code.as_integer();
			current_process->on_process_exit.touch(&code);
		}

		void init()
		{
			console_cs_ext::init();
			file_cs_ext::init();
			path_cs_ext::init();
			(*system_ext)
			.add_var("console", make_namespace(console_ext))
			.add_var("file", make_namespace(file_ext))
			.add_var("path", make_namespace(path_ext))
			.add_var("in", var::make_protect<istream>(&std::cin, [](std::istream *) {}))
			.add_var("out", var::make_protect<ostream>(&std::cout, [](std::ostream *) {}))
			.add_var("err", var::make_protect<ostream>(&std::cerr, [](std::ostream *) {}))
			.add_var("log", var::make_protect<ostream>(&std::clog, [](std::ostream *) {}))
			.add_var("run", make_cni(run))
			.add_var("getenv", make_cni(getenv))
			.add_var("exit", make_cni(exit))
			.add_var("os_name", var::make_constant<string>(COVSCRIPT_PLATFORM_NAME))
			.add_var("arch_name", var::make_constant<string>(COVSCRIPT_ARCH_NAME))
			.add_var("compiler_name", var::make_constant<string>(COVSCRIPT_COMPILER_NAME))
			.add_var("is_platform_windows", make_cni(platform::is_platform_win32))
			.add_var("is_platform_linux", make_cni(platform::is_platform_linux))
			.add_var("is_platform_darwin", make_cni(platform::is_platform_darwin))
			.add_var("is_platform_unix", make_cni(platform::is_platform_unix));
		}
	} // namespace system_cs_ext

	void init_extensions()
	{
		static bool extensions_initiator = true;
		if (extensions_initiator) {
			extensions_initiator = false;
			member_visitor_cs_ext::init();
			iostream_cs_ext::init();
			charbuff_cs_ext::init();
			istream_cs_ext::init();
			ostream_cs_ext::init();
			system_cs_ext::init();
			time_cs_ext::init();
			fiber_cs_ext::init();
			runtime_cs_ext::init();
			math_cs_ext::init();
			except_cs_ext::init();
			number_cs_ext::init();
			char_cs_ext::init();
			string_cs_ext::init();
			list_cs_ext::init();
			array_cs_ext::init();
			pair_cs_ext::init();
			hash_set_cs_ext::init();
			hash_map_cs_ext::init();
		}
	}
} // namespace cs_impl
