#pragma once
/*
* Covariant Script Char Extension
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

static cs::name_space char_ext;
static cs::namespace_t char_ext_shared = cs::make_shared_namespace(char_ext);
namespace cs_impl {
	template<>
	cs::namespace_t &get_ext<char>()
	{
		return char_ext_shared;
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
		char_ext
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
