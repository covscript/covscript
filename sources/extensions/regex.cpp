/*
* Covariant Script Regex Extension
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
#include <regex>
#include <covscript/extension.hpp>
#include <covscript/cni.hpp>
#include <covscript/extensions/string.hpp>

static cs::extension regex_ext;
static cs::extension regex_result_ext;
static cs::extension_t regex_ext_shared = cs::make_shared_extension(regex_ext);
static cs::extension_t regex_result_ext_shared = cs::make_shared_extension(regex_result_ext);
namespace cs_impl {
	template<>
	cs::extension_t &get_ext<std::regex>()
	{
		return regex_ext_shared;
	}

	template<>
	cs::extension_t &get_ext<std::smatch>()
	{
		return regex_result_ext_shared;
	}

	template<>
	constexpr const char *get_name_of_type<std::regex>()
	{
		return "cs::regex";
	}

	template<>
	constexpr const char *get_name_of_type<std::smatch>()
	{
		return "cs::regex::result";
	}
}
namespace regex_cs_ext {
	using namespace cs;

	var build(const string &str)
	{
		return var::make<std::regex>(str);
	}

	std::smatch match(std::regex &reg, const string &str)
	{
		std::smatch m;
		std::regex_search(str, m, reg);
		return std::move(m);
	}

	std::smatch search(std::regex &reg, const string &str)
	{
		std::smatch m;
		std::regex_search(str, m, reg);
		return std::move(m);
	}

	string replace(std::regex &reg, const string &str, const string &fmt)
	{
		return std::regex_replace(str, reg, fmt);
	}

	bool ready(const std::smatch &m)
	{
		return m.ready();
	}

	bool empty(const std::smatch &m)
	{
		return m.empty();
	}

	number size(const std::smatch &m)
	{
		return m.size();
	}

	number length(const std::smatch &m, number index)
	{
		return m.length(index);
	}

	number position(const std::smatch &m, number index)
	{
		return m.position(index);
	}

	string str(const std::smatch &m, number index)
	{
		return m.str(index);
	}

	string prefix(const std::smatch &m)
	{
		return m.prefix().str();
	}

	string suffix(const std::smatch &m)
	{
		return m.suffix().str();
	}

	string format(const std::smatch &m, const string &fmt)
	{
		return m.format(fmt.c_str());
	}

	void init()
	{
		string_cs_ext::init();
		regex_ext.add_var("result", var::make_protect<extension_t>(regex_result_ext_shared));
		regex_ext.add_var("build", var::make_protect<callable>(cni(build)));
		regex_ext.add_var("match", var::make_protect<callable>(cni(match)));
		regex_ext.add_var("search", var::make_protect<callable>(cni(search)));
		regex_ext.add_var("replace", var::make_protect<callable>(cni(replace)));
		regex_result_ext.add_var("ready", var::make_protect<callable>(cni(ready)));
		regex_result_ext.add_var("empty", var::make_protect<callable>(cni(empty)));
		regex_result_ext.add_var("size", var::make_protect<callable>(cni(size)));
		regex_result_ext.add_var("length", var::make_protect<callable>(cni(length)));
		regex_result_ext.add_var("position", var::make_protect<callable>(cni(position)));
		regex_result_ext.add_var("str", var::make_protect<callable>(cni(str)));
		regex_result_ext.add_var("prefix", var::make_protect<callable>(cni(prefix)));
		regex_result_ext.add_var("suffix", var::make_protect<callable>(cni(suffix)));
		regex_result_ext.add_var("format", var::make_protect<callable>(cni(format)));
	}
}

cs::extension *cs_extension()
{
	regex_cs_ext::init();
	return &regex_ext;
}