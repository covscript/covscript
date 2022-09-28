#pragma once
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
* Copyright (C) 2017-2022 Michael Lee(李登淳)
*
* This software is registered with the National Copyright Administration
* of the People's Republic of China(Registration Number: 2020SR0408026)
* and is protected by the Copyright Law of the People's Republic of China.
*
* Email:   lee@covariant.cn, mikecovlee@163.com
* Github:  https://github.com/mikecovlee
* Website: http://covscript.org.cn
*/
#include <covscript/core/core.hpp>
#include <covscript/core/cni.hpp>
#include <ctime>

namespace cs_impl {
	namespace path_cs_ext {
		struct path_info final {
			std::string name;
			int type;

			path_info() = delete;

			path_info(const char *n, int t) : name(n), type(t) {}
		};
	}

	void init_extensions();

// Namespace declarations
	extern cs::namespace_t member_visitor_ext;
	extern cs::namespace_t except_ext;
	extern cs::namespace_t array_ext;
	extern cs::namespace_t array_iterator_ext;
	extern cs::namespace_t char_ext;
	extern cs::namespace_t math_ext;
	extern cs::namespace_t math_const_ext;
	extern cs::namespace_t list_ext;
	extern cs::namespace_t list_iterator_ext;
	extern cs::namespace_t hash_set_ext;
	extern cs::namespace_t hash_map_ext;
	extern cs::namespace_t pair_ext;
	extern cs::namespace_t time_ext;
	extern cs::namespace_t context_ext;
	extern cs::namespace_t runtime_ext;
	extern cs::namespace_t string_ext;
	extern cs::namespace_t iostream_ext;
	extern cs::namespace_t seekdir_ext;
	extern cs::namespace_t openmode_ext;
	extern cs::namespace_t charbuff_ext;
	extern cs::namespace_t istream_ext;
	extern cs::namespace_t ostream_ext;
	extern cs::namespace_t system_ext;
	extern cs::namespace_t console_ext;
	extern cs::namespace_t file_ext;
	extern cs::namespace_t path_ext;
	extern cs::namespace_t path_type_ext;
	extern cs::namespace_t path_info_ext;

// Detach
	template<>
	void detach<cs::pair>(cs::pair &val)
	{
		cs::copy_no_return(val.first);
		cs::copy_no_return(val.second);
	}

	template<>
	void detach<cs::list>(cs::list &val)
	{
		for (auto &it:val)
			cs::copy_no_return(it);
	}

	template<>
	void detach<cs::array>(cs::array &val)
	{
		for (auto &it:val)
			cs::copy_no_return(it);
	}

	template<>
	void detach<cs::hash_map>(cs::hash_map &val)
	{
		for (auto &it:val)
			cs::copy_no_return(it.second);
	}

// To String
	template<>
	std::string to_string<cs::number>(const cs::number &val)
	{
		if (!val.is_integer())
		{
			std::stringstream ss;
			std::string str;
			ss << std::setprecision(cs::current_process->output_precision) << val.as_number();
			ss >> str;
			return std::move(str);
		} else
			return std::to_string(val.as_integer());
	}

	template<>
	std::string to_string<char>(const char &c)
	{
		return std::string(1, c);
	}

	template<>
	std::string to_string<cs::list>(const cs::list &lst)
	{
		if (lst.empty())
			return "list => {}";
		std::string str = "list => {";
		for (const cs::var &it:lst)
			str += it.to_string() + ", ";
		str.resize(str.size() - 2);
		str += "}";
		return std::move(str);
	}

	template<>
	std::string to_string<cs::array>(const cs::array &arr)
	{
		if (arr.empty())
			return "{}";
		std::string str = "{";
		for (const cs::var &it:arr)
			str += it.to_string() + ", ";
		str.resize(str.size() - 2);
		str += "}";
		return std::move(str);
	}

	template<>
	std::string to_string<cs::pair>(const cs::pair &p)
	{
		return p.first.to_string() + " : " + p.second.to_string();
	}

	template<>
	std::string to_string<cs::hash_set>(const cs::hash_set &set)
	{
		if (set.empty())
			return "cs::hash_set => {}";
		std::string str = "cs::hash_set => {";
		for (const cs::var &it:set)
			str += it.to_string() + ", ";
		str.resize(str.size() - 2);
		str += "}";
		return std::move(str);
	}

	template<>
	std::string to_string<cs::hash_map>(const cs::hash_map &map)
	{
		if (map.empty())
			return "cs::hash_map => {}";
		std::string str = "cs::hash_map => {";
		for (const cs::pair &it:map)
			str += cs_impl::to_string(it) + ", ";
		str.resize(str.size() - 2);
		str += "}";
		return std::move(str);
	}

	template<>
	std::string to_string<cs::pointer>(const cs::pointer &ptr)
	{
		if (ptr == cs::null_pointer)
			return "null";
		else
			return "cs::pointer => " + ptr.data.to_string();
	}

	template<>
	std::string to_string<cs::type_id>(const cs::type_id &id)
	{
		if (id.type_hash != 0)
			return cxx_demangle(id.type_idx.name()) + "_" + to_string(id.type_hash);
		else
			return cxx_demangle(id.type_idx.name());
	}

	template<>
	std::string to_string<cs::range_type>(const cs::range_type &range)
	{
		if (range.empty())
			return "cs::range => {}";
		std::string str = "cs::range => {";
		for (cs::number it:range)
			str += to_string(it) + ", ";
		str.resize(str.size() - 2);
		str += "}";
		return std::move(str);
	}

	template<>
	std::string to_string<cs::char_buff>(const cs::char_buff &buff)
	{
		return buff->str();
	}

	template<>
	std::string to_string<std::tm>(const std::tm &t)
	{
		return std::asctime(&t);
	}

// To Integer
	template<>
	long to_integer<cs::number>(const cs::number &num)
	{
		return num.as_integer();
	}

	template<>
	long to_integer<std::string>(const std::string &str)
	{
		for (auto &ch:str) {
			if (!std::isdigit(ch))
				throw cs::runtime_error("Wrong literal format.");
		}
		return std::stol(str);
	}

// Hash
	template<>
	std::size_t hash<cs::type_id>(const cs::type_id &id)
	{
		if (id.type_hash == 0)
			return id.type_idx.hash_code();
		else
			throw cov::error("E000F");
	}

// Type name
	template<>
	constexpr const char *get_name_of_type<cs::context_t>()
	{
		return "cs::context";
	}

	template<>
	constexpr const char *get_name_of_type<cs::var>()
	{
		return "cs::var";
	}

	template<>
	constexpr const char *get_name_of_type<cs::number>()
	{
		return "cs::number";
	}

	template<>
	constexpr const char *get_name_of_type<cs::boolean>()
	{
		return "cs::boolean";
	}

	template<>
	constexpr const char *get_name_of_type<cs::pointer>()
	{
		return "cs::pointer";
	}

	template<>
	constexpr const char *get_name_of_type<char>()
	{
		return "cs::char";
	}

	template<>
	constexpr const char *get_name_of_type<cs::string>()
	{
		return "cs::string";
	}

	template<>
	constexpr const char *get_name_of_type<cs::list::iterator>()
	{
		return "cs::list::iterator";
	}

	template<>
	constexpr const char *get_name_of_type<cs::list>()
	{
		return "cs::list";
	}

	template<>
	constexpr const char *get_name_of_type<cs::array::iterator>()
	{
		return "cs::array::iterator";
	}

	template<>
	constexpr const char *get_name_of_type<cs::array>()
	{
		return "cs::array";
	}

	template<>
	constexpr const char *get_name_of_type<cs::pair>()
	{
		return "cs::pair";
	}

	template<>
	constexpr const char *get_name_of_type<cs::hash_set>()
	{
		return "cs::hash_set";
	}

	template<>
	constexpr const char *get_name_of_type<cs::hash_map>()
	{
		return "cs::hash_map";
	}

	template<>
	constexpr const char *get_name_of_type<cs::type_t>()
	{
		return "cs::type";
	}

	template<>
	constexpr const char *get_name_of_type<cs::namespace_t>()
	{
		return "cs::namespace";
	}

	template<>
	constexpr const char *get_name_of_type<cs::callable>()
	{
		return "cs::function";
	}

	template<>
	constexpr const char *get_name_of_type<cs::range_type>()
	{
		return "cs::range";
	}

	template<>
	constexpr const char *get_name_of_type<cs::structure>()
	{
		return "cs::structure";
	}

	template<>
	constexpr const char *get_name_of_type<cs::lang_error>()
	{
		return "cs::exception";
	}

	template<>
	constexpr const char *get_name_of_type<cs::char_buff>()
	{
		return "cs::char_buff";
	}

	template<>
	constexpr const char *get_name_of_type<cs::istream>()
	{
		return "cs::istream";
	}

	template<>
	constexpr const char *get_name_of_type<cs::ostream>()
	{
		return "cs::ostream";
	}

	template<>
	constexpr const char *get_name_of_type<std::tm>()
	{
		return "cs::time_type";
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
	constexpr const char *get_name_of_type<cs::tree_type<cs::token_base *>>()
	{
		return "cs::expression";
	}

	template<>
	constexpr const char *get_name_of_type<path_cs_ext::path_info>()
	{
		return "cs::system::path_info";
	}

// Type Extensions

	template<>
	cs::namespace_t &get_ext<cs::member_visitor>()
	{
		return member_visitor_ext;
	}

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
	cs::namespace_t &get_ext<char>()
	{
		return char_ext;
	}

	template<>
	cs::namespace_t &get_ext<cs::hash_set>()
	{
		return hash_set_ext;
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
	cs::namespace_t &get_ext<cs::pair>()
	{
		return pair_ext;
	}

	template<>
	cs::namespace_t &get_ext<cs::char_buff>()
	{
		return charbuff_ext;
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

	template<>
	cs::namespace_t &get_ext<std::tm>()
	{
		return time_ext;
	}

	template<>
	cs::namespace_t &get_ext<cs::context_t>()
	{
		return context_ext;
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
}