#pragma once
/*
* Covariant Script String Extension
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Affero General Public License as published
* by the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Affero General Public License for more details.
*
* You should have received a copy of the GNU Affero General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
* Copyright (C) 2017 Michael Lee(李登淳)
* Email: mikecovlee@163.com
* Github: https://github.com/mikecovlee
*/
#include "../headers/cni.hpp"

static cs::extension string_ext;
static cs::extension_t string_ext_shared = std::make_shared<cs::extension_holder>(&string_ext);
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
	}
}
