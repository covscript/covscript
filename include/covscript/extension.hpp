#pragma once
/*
* Covariant Script Extension
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
#include <covscript/instance.hpp>

extern cs::namespace_t except_ext;
extern cs::namespace_t array_ext;
extern cs::namespace_t array_iterator_ext;
extern cs::namespace_t char_ext;
extern cs::namespace_t math_ext;
extern cs::namespace_t math_const_ext;
extern cs::namespace_t list_ext;
extern cs::namespace_t list_iterator_ext;
extern cs::namespace_t hash_map_ext;
extern cs::namespace_t pair_ext;
extern cs::namespace_t context_ext;
extern cs::namespace_t runtime_ext;
extern cs::namespace_t string_ext;
extern cs::namespace_t iostream_ext;
extern cs::namespace_t seekdir_ext;
extern cs::namespace_t openmode_ext;
extern cs::namespace_t istream_ext;
extern cs::namespace_t ostream_ext;
extern cs::namespace_t system_ext;
extern cs::namespace_t console_ext;
extern cs::namespace_t file_ext;
extern cs::namespace_t path_ext;
extern cs::namespace_t path_type_ext;
extern cs::namespace_t path_info_ext;
namespace path_cs_ext {
	struct path_info final {
		std::string name;
		int type;

		path_info() = delete;

		path_info(const char *n, int t) : name(n), type(t) {}
	};
}
namespace cs_impl {
	template<>
	cs::namespace_t &get_ext<cs::lang_error>()
	{
		return except_ext;
	}

	template<>
	cs::namespace_t &get_ext<cs::array>()
	{
		return array_ext;
	}

	template<>
	cs::namespace_t &get_ext<cs::array::iterator>()
	{
		return array_iterator_ext;
	}

	template<>
	constexpr const char *get_name_of_type<cs::array::iterator>()
	{
		return "cs::array::iterator";
	}

	template<>
	cs::namespace_t &get_ext<char>()
	{
		return char_ext;
	}

	template<>
	cs::namespace_t &get_ext<cs::hash_map>()
	{
		return hash_map_ext;
	}

	template<>
	cs::namespace_t &get_ext<cs::list>()
	{
		return list_ext;
	}

	template<>
	cs::namespace_t &get_ext<cs::list::iterator>()
	{
		return list_iterator_ext;
	}

	template<>
	constexpr const char *get_name_of_type<cs::list::iterator>()
	{
		return "cs::list::iterator";
	}

	template<>
	cs::namespace_t &get_ext<cs::pair>()
	{
		return pair_ext;
	}

	template<>
	cs::namespace_t &get_ext<cs::istream>()
	{
		return istream_ext;
	}

	template<>
	cs::namespace_t &get_ext<cs::ostream>()
	{
		return ostream_ext;
	}

#ifndef _MSC_VER

	template<>
	constexpr const char *get_name_of_type<std::ios_base::seekdir>()
	{
		return "cs::iostream::seekdir";
	}

	template<>
	constexpr const char *get_name_of_type<std::ios_base::openmode>()
	{
		return "cs::iostream::openmode";
	}

#endif

	template<>
	cs::namespace_t &get_ext<cs::context_t>()
	{
		return context_ext;
	}

	template<>
	constexpr const char *get_name_of_type<cov::tree<cs::token_base *>>()
	{
		return "cs::expression";
	}

	template<>
	cs::namespace_t &get_ext<cs::string>()
	{
		return string_ext;
	}

	template<>
	cs::namespace_t &get_ext<path_cs_ext::path_info>()
	{
		return path_info_ext;
	}

	template<>
	constexpr const char *get_name_of_type<path_cs_ext::path_info>()
	{
		return "cs::system::path_info";
	}
}
namespace cs_impl {
	void init_extensions();
}