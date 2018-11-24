#pragma once
/*
* Covariant Script String Extension
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
* Copyright (C) 2018 Michael Lee(李登淳)
* Email: mikecovlee@163.com
* Github: https://github.com/mikecovlee
*/
#include <covscript/cni.hpp>
#include <cctype>

static cs::namespace_t string_ext = cs::make_shared_namespace<cs::name_space>();
namespace cs_impl {
	template<>
	cs::namespace_t &get_ext<cs::string>()
	{
		return string_ext;
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
