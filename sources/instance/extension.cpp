/*
* Covariant Script Extension
*
* Licensed under the Covariant Innovation General Public License,
* Version 1.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* https://covariant.cn/licenses/LICENSE-1.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
* Copyright (C) 2019 Michael Lee(李登淳)
* Email: mikecovlee@163.com
* Github: https://github.com/mikecovlee
*/
#include <covscript_impl/system/system.hpp>
#include <covscript_impl/dirent/dirent.hpp>
#include <covscript_impl/mozart/random.hpp>
#include <covscript_impl/mozart/timer.hpp>
#include <covscript/impl/impl.hpp>
#include <iostream>

namespace cs_impl {
	namespace array_cs_ext {
		using namespace cs;

// Element access
		var at(const array &arr, number posit)
		{
			return arr.at(posit);
		}

		var front(const array &arr)
		{
			return arr.front();
		}

		var back(const array &arr)
		{
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

		array::iterator forward(array::iterator &it)
		{
			return ++it;
		}

		array::iterator backward(array::iterator &it)
		{
			return --it;
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

		number size(const array &arr)
		{
			return arr.size();
		}

// Modifiers
		void clear(array &arr)
		{
			arr.clear();
		}

		array::iterator insert(array &arr, array::iterator &pos,
		                       const var &val
		                      )
		{
			return arr.
			       insert(pos, copy(val)
			             );
		}

		array::iterator erase(array &arr, array::iterator &pos)
		{
			return arr.erase(pos);
		}

		void push_front(array &arr, const var &val)
		{
			arr.push_front(copy(val));
		}

		void pop_front(array &arr)
		{
			arr.pop_front();
		}

		void push_back(array &arr, const var &val)
		{
			arr.push_back(copy(val));
		}

		void pop_back(array &arr)
		{
			arr.pop_back();
		}

// Operations
		var to_hash_map(const array &arr)
		{
			hash_map map;
			for (auto &it:arr) {
				if (it.type() == typeid(pair)) {
					const auto &p = it.const_val<pair>();
					map[p.first] = copy(p.second);
				}
				else
					throw lang_error("Wrong syntax for hash map.");
			}
			return var::make<hash_map>(std::move(map));
		}

		var to_list(const array &arr)
		{
			var lst = var::make<list>(arr.begin(), arr.end());
			lst.detach();
			return std::move(lst);
		}

		void init()
		{
			(*array_iterator_ext)
			.add_var("forward", make_cni(forward, true))
			.add_var("backward", make_cni(backward, true))
			.add_var("data", make_cni(data, true));
			(*array_ext)
			.add_var("iterator", make_namespace(array_iterator_ext))
			.add_var("at", make_cni(at, true))
			.add_var("front", make_cni(front, true))
			.add_var("back", make_cni(back, true))
			.add_var("begin", make_cni(begin, true))
			.add_var("end", make_cni(end, true))
			.add_var("empty", make_cni(empty, true))
			.add_var("size", make_cni(size, true))
			.add_var("clear", make_cni(clear, true))
			.add_var("insert", make_cni(insert, true))
			.add_var("erase", make_cni(erase, true))
			.add_var("push_front", make_cni(push_front, true))
			.add_var("pop_front", make_cni(pop_front, true))
			.add_var("push_back", make_cni(push_back, true))
			.add_var("pop_back", make_cni(pop_back, true))
			.add_var("to_hash_map", make_cni(to_hash_map, true))
			.add_var("to_list", make_cni(to_list, true));
		}

	}
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

		char from_ascii(number ascii)
		{
			if (ascii < 0 || ascii > 255)
				throw lang_error("Out of range.");
			return static_cast<char>(ascii);
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
			.add_var("from_ascii", make_cni(from_ascii, true));
		}
	}
	namespace except_cs_ext {
		using namespace cs;

		string what(const lang_error &le)
		{
			return le.what();
		}

		void init()
		{
			except_ext->add_var("what", make_cni(what));
		}
	}
	namespace hash_map_cs_ext {
		using namespace cs;

// Capacity
		bool empty(const hash_map &map)
		{
			return map.empty();
		}

		number size(const hash_map &map)
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
			if (map.count(key) > 0)
				map.at(key).swap(copy(val), true);
			else
				map.emplace(copy(key), copy(val));
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

		void init()
		{
			(*hash_map_ext)
			.add_var("empty", make_cni(empty, true))
			.add_var("size", make_cni(size, true))
			.add_var("clear", make_cni(clear, true))
			.add_var("insert", make_cni(insert, true))
			.add_var("erase", make_cni(erase, true))
			.add_var("at", make_cni(at, true))
			.add_var("exist", make_cni(exist, true));
		}
	}
	namespace iostream_cs_ext {
		using namespace cs;

		var fstream(const string &path, std::ios_base::openmode openmode)
		{
			switch (openmode) {
			case std::ios_base::in:
				return var::make<istream>(new std::ifstream(path, std::ios_base::in));
				break;
			case std::ios_base::out:
				return var::make<ostream>(new std::ofstream(path, std::ios_base::out));
				break;
			case std::ios_base::app:
				return var::make<ostream>(new std::ofstream(path, std::ios_base::app));
				break;
			default:
				throw lang_error("Unsupported openmode.");
			}
		}

		void setprecision(number pre)
		{
			current_process->output_precision = pre;
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
			.add_var("out", var::make_constant<std::ios_base::openmode>(std::ios_base::out))
			.add_var("app", var::make_constant<std::ios_base::openmode>(std::ios_base::app));
			(*iostream_ext)
			.add_var("fstream", make_cni(fstream))
			.add_var("setprecision", make_cni(setprecision));
		}
	}
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

		number tell(istream &in)
		{
			return in->tellg();
		}

		void seek(istream &in, number pos)
		{
			in->seekg(pos);
		}

		void seek_from(istream &in, std::ios_base::seekdir dir, number offset)
		{
			in->seekg(offset, dir);
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
			.add_var("ignore", make_cni(ignore));
		}
	}
	namespace ostream_cs_ext {
		using namespace cs;

// Output Stream
		void put(ostream &out, char c)
		{
			out->put(c);
		}

		number tell(ostream &out)
		{
			return out->tellp();
		}

		void seek(ostream &out, number pos)
		{
			out->seekp(pos);
		}

		void seek_from(ostream &out, std::ios_base::seekdir dir, number offset)
		{
			out->seekp(offset, dir);
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
			.add_var("println", make_cni(println));
		}
	}
	namespace list_cs_ext {
		using namespace cs;

// Element access
		var front(const list &lst)
		{
			return lst.front();
		}

		var back(const list &lst)
		{
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

		list::iterator forward(list::iterator &it)
		{
			return ++it;
		}

		list::iterator backward(list::iterator &it)
		{
			return --it;
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

		number size(const list &lst)
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

		void pop_front(list &lst)
		{
			lst.pop_front();
		}

		void push_back(list &lst, const var &val)
		{
			lst.push_back(copy(val));
		}

		void pop_back(list &lst)
		{
			lst.pop_back();
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

		void init()
		{
			(*list_iterator_ext)
			.add_var("forward", make_cni(forward, true))
			.add_var("backward", make_cni(backward, true))
			.add_var("data", make_cni(data, true));
			(*list_ext)
			.add_var("iterator", make_namespace(list_iterator_ext))
			.add_var("front", make_cni(front, true))
			.add_var("back", make_cni(back, true))
			.add_var("begin", make_cni(begin, true))
			.add_var("end", make_cni(end, true))
			.add_var("empty", make_cni(empty, true))
			.add_var("size", make_cni(size, true))
			.add_var("clear", make_cni(clear, true))
			.add_var("insert", make_cni(insert, true))
			.add_var("erase", make_cni(erase, true))
			.add_var("push_front", make_cni(push_front, true))
			.add_var("pop_front", make_cni(pop_front, true))
			.add_var("push_back", make_cni(push_back, true))
			.add_var("pop_back", make_cni(pop_back, true))
			.add_var("remove", make_cni(remove, true))
			.add_var("reverse", make_cni(reverse, true))
			.add_var("unique", make_cni(unique, true));
		}
	}
	namespace math_cs_ext {
		using namespace cs;

		number abs(number n)
		{
			return std::abs(n);
		}

		number ln(number n)
		{
			return std::log(n);
		}

		number log10(number n)
		{
			return std::log10(n);
		}

		number log(number a, number b)
		{
			return std::log(b / a);
		}

		number sin(number n)
		{
			return std::sin(n);
		}

		number cos(number n)
		{
			return std::cos(n);
		}

		number tan(number n)
		{
			return std::tan(n);
		}

		number asin(number n)
		{
			return std::asin(n);
		}

		number acos(number n)
		{
			return std::acos(n);
		}

		number atan(number n)
		{
			return std::atan(n);
		}

		number sqrt(number n)
		{
			return std::sqrt(n);
		}

		number root(number a, number b)
		{
			return std::pow(a, number(1) / b);
		}

		number pow(number a, number b)
		{
			return std::pow(a, b);
		}

		number _min(number a, number b)
		{
			return (std::min)(a, b);
		}

		number _max(number a, number b)
		{
			return (std::max)(a, b);
		}

		number rand(number b, number e)
		{
			return cov::rand<number>(b, e);
		}

		number randint(number b, number e)
		{
			return cov::rand<long>(b, e);
		}

		void init()
		{
			(*math_const_ext)
			.add_var("max", var::make_constant<number>((std::numeric_limits<number>::max)()))
			.add_var("min", var::make_constant<number>((std::numeric_limits<number>::min)()))
			.add_var("inf", var::make_constant<number>(std::numeric_limits<number>::infinity()))
			.add_var("nan", var::make_constant<number>(std::numeric_limits<number>::quiet_NaN()))
			.add_var("pi", var::make_constant<number>(std::asin(number(1)) * 2))
			.add_var("e", var::make_constant<number>(std::exp(number(1))));
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
	}
	namespace pair_cs_ext {
		using namespace cs;

		var first(const pair &p)
		{
			return p.first;
		}

		var second(const pair &p)
		{
			return p.second;
		}

		void init()
		{
			(*pair_ext)
			.add_var("first", make_cni(first, true))
			.add_var("second", make_cni(second, true));
		}
	}
	namespace runtime_cs_ext {
		using namespace cs;

		string get_import_path()
		{
			return current_process->import_path;
		}

		void info()
		{
			std::cout << "Covariant Script Programming Language Interpreter\nVersion: " << current_process->version
			          << "\n"
			          "Copyright (C) 2019 Michael Lee. All rights reserved.\n"
			          "Licensed under the Covariant Innovation General Public License,\n"
			          "Version 1.0 (the \"License\");\n"
			          "you may not use this file except in compliance with the License.\n"
			          "You may obtain a copy of the License at\n"
			          "\nhttps://covariant.cn/licenses/LICENSE-1.0\n"
			          "Unless required by applicable law or agreed to in writing, software\n"
			          "distributed under the License is distributed on an \"AS IS\" BASIS,\n"
			          "WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.\n"
			          "See the License for the specific language governing permissions and\n"
			          "limitations under the License.\n"
			          "Please visit http://covscript.org for more information."
			          << std::endl;
		}

		number time()
		{
			return cov::timer::time(cov::timer::time_unit::milli_sec);
		}

		void delay(number time)
		{
			cov::timer::delay(cov::timer::time_unit::milli_sec, time);
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
			for (auto &ch:expr)
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
			return make_namespace(context->instance->import(dir, name));
		}

		var source_import(const context_t &context, const string &path)
		{
			return make_namespace(context->instance->source_import(path));
		}

		void init()
		{
			(*runtime_ext)
			.add_var("std_version", var::make_constant<number>(current_process->std_version))
			.add_var("get_import_path", make_cni(get_import_path, true))
			.add_var("info", make_cni(info))
			.add_var("time", make_cni(time))
			.add_var("delay", make_cni(delay))
			.add_var("exception", make_cni(exception))
			.add_var("hash", make_cni(hash, true))
			.add_var("build", make_cni(build))
			.add_var("solve", make_cni(solve))
			.add_var("cmd_args", make_cni(cmd_args, true))
			.add_var("import", make_cni(import, true))
			.add_var("source_import", make_cni(source_import, true));
			(*context_ext)
			.add_var("build", make_cni(build))
			.add_var("solve", make_cni(solve))
			.add_var("cmd_args", make_cni(cmd_args, true))
			.add_var("import", make_cni(import, true))
			.add_var("source_import", make_cni(source_import, true));
		}
	}
	namespace string_cs_ext {
		using namespace cs;

		string append(string &str, const var &val)
		{
			str.append(val.to_string());
			return str;
		}

		string insert(string &str, number posit, const var &val)
		{
			str.insert(posit, val.to_string());
			return str;
		}

		string erase(string &str, number b, number e)
		{
			str.erase(b, e);
			return str;
		}

		string replace(string &str, number posit, number count, const var &val)
		{
			str.replace(posit, count, val.to_string());
			return str;
		}

		string substr(string &str, number b, number e)
		{
			return str.substr(b, e);
		}

		number find(string &str, const string &s, number posit)
		{
			auto pos = str.find(s, posit);
			if (pos == std::string::npos)
				return -1;
			else
				return pos;
		}

		number rfind(string &str, const string &s, number posit)
		{
			std::size_t pos = 0;
			if (posit == -1)
				pos = str.rfind(s, std::string::npos);
			else
				pos = str.rfind(s, posit);
			if (pos == std::string::npos)
				return -1;
			else
				return pos;
		}

		string cut(string &str, number n)
		{
			for (std::size_t i = 0; i < n; ++i)
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

		number size(const string &str)
		{
			return str.size();
		}

		string tolower(const string &str)
		{
			string s;
			for (auto &ch:str)
				s.push_back(std::tolower(ch));
			return std::move(s);
		}

		string toupper(const string &str)
		{
			string s;
			for (auto &ch:str)
				s.push_back(std::toupper(ch));
			return std::move(s);
		}

		number to_number(const string &str)
		{
			return parse_number(str);
		}

		array split(const string &str, const array &signals)
		{
			array
			arr;
			string buf;
			bool found = false;
			for (auto &ch:str) {
				for (auto &sig:signals) {
					if (ch == sig.const_val<char>()) {
						if (!buf.empty()) {
							arr.push_back(buf);
							buf.clear();
						}
						found = true;
						break;
					}
				}
				if (found)
					found = false;
				else
					buf.push_back(ch);
			}
			if (!buf.empty())
				arr.push_back(buf);
			return std::move(arr);
		}

		void init()
		{
			(*string_ext)
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
			.add_var("size", make_cni(size, true))
			.add_var("tolower", make_cni(tolower, true))
			.add_var("toupper", make_cni(toupper, true))
			.add_var("to_number", make_cni(to_number, true))
			.add_var("split", make_cni(split, true));
		}
	}
	namespace console_cs_ext {
		using namespace cs;
		using namespace cs_impl;

		number terminal_width()
		{
			return conio::terminal_width();
		}

		number terminal_height()
		{
			return conio::terminal_height();
		}

		void gotoxy(number x, number y)
		{
			conio::gotoxy(x, y);
		}

		void echo(bool v)
		{
			conio::echo(v);
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
			.add_var("clrscr", make_cni(clrscr))
			.add_var("getch", make_cni(getch))
			.add_var("kbhit", make_cni(kbhit));
		}
	}

	namespace file_cs_ext {
		using namespace cs;
		using namespace cs_impl;

		bool copy(const string &source, const string &dest)
		{
			std::ifstream in(source, std::ios_base::in | std::ios_base::binary);
			std::ofstream out(dest, std::ios_base::out | std::ios_base::binary);
			if (!in || !out)
				return false;
			char buffer[256];
			while (!in.eof()) {
				in.read(buffer, 256);
				out.write(buffer, in.gcount());
			}
			return true;
		}

		bool remove(const string &path)
		{
			return std::remove(path.c_str()) == 0;
		}

		bool exists(const string &path)
		{
			return std::ifstream(path).is_open();
		}

		bool rename(const string &source, const string &dest)
		{
			return std::rename(source.c_str(), dest.c_str()) == 0;
		}

		void init()
		{
			(*file_ext)
			.add_var("copy", make_cni(copy))
			.add_var("remove", make_cni(remove))
			.add_var("exists", make_cni(exists))
			.add_var("rename", make_cni(rename));
		}
	}

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
				throw cs::lang_error("Path does not exist.");
			array
			entries;
			for (dirent *dp = ::readdir(dir); dp != nullptr; dp = ::readdir(dir))
				entries.push_back(var::make<path_info>(dp->d_name, dp->d_type));
			::closedir(dir);
			return std::move(entries);
		}

		void init()
		{
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
			.add_var("name", make_cni(name))
			.add_var("type", make_cni(type));
			(*path_ext)
			.add_var("type", make_namespace(path_type_ext))
			.add_var("info", make_namespace(path_info_ext))
			.add_var("separator", var::make_constant<char>(path_separator))
			.add_var("delimiter", var::make_constant<char>(path_delimiter))
			.add_var("scan", make_cni(scan));
		}
	}
	namespace system_cs_ext {
		using namespace cs;

		number run(const string &str)
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

		void exit(number code)
		{
			std::exit(code);
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
			.add_var("run", make_cni(run))
			.add_var("getenv", make_cni(getenv))
			.add_var("exit", make_cni(exit));
		}
	}

	static bool extensions_initiator = true;

	void init_extensions()
	{
		if (extensions_initiator) {
			extensions_initiator = false;
#ifndef CS_EXTENSIONS_MINIMAL
			iostream_cs_ext::init();
			istream_cs_ext::init();
			ostream_cs_ext::init();
			system_cs_ext::init();
			runtime_cs_ext::init();
			math_cs_ext::init();
#endif
			except_cs_ext::init();
			char_cs_ext::init();
			string_cs_ext::init();
			list_cs_ext::init();
			array_cs_ext::init();
			pair_cs_ext::init();
			hash_map_cs_ext::init();
		}
	}
}