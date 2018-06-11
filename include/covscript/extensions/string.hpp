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

static cs::extension string_ext;
static cs::extension_t string_ext_shared = cs::make_shared_extension(string_ext);
namespace cs_impl {
	template<>
	cs::extension_t &get_ext<cs::string>()
	{
		return string_ext_shared;
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
		array arr;
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
		string_ext.add_var("append", var::make_protect<callable>(cni(append), true));
		string_ext.add_var("insert", var::make_protect<callable>(cni(insert), true));
		string_ext.add_var("erase", var::make_protect<callable>(cni(erase), true));
		string_ext.add_var("replace", var::make_protect<callable>(cni(replace), true));
		string_ext.add_var("substr", var::make_protect<callable>(cni(substr), true));
		string_ext.add_var("find", var::make_protect<callable>(cni(find), true));
		string_ext.add_var("rfind", var::make_protect<callable>(cni(rfind), true));
		string_ext.add_var("cut", var::make_protect<callable>(cni(cut), true));
		string_ext.add_var("empty", var::make_protect<callable>(cni(empty), true));
		string_ext.add_var("clear", var::make_protect<callable>(cni(clear), true));
		string_ext.add_var("size", var::make_protect<callable>(cni(size), true));
		string_ext.add_var("tolower", var::make_protect<callable>(cni(tolower), true));
		string_ext.add_var("toupper", var::make_protect<callable>(cni(toupper), true));
		string_ext.add_var("to_number", var::make_protect<callable>(cni(to_number), true));
		string_ext.add_var("split", var::make_protect<callable>(cni(split), true));
	}
}
