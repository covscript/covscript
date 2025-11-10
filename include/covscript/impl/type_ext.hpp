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
#include <covscript/core/core.hpp>
#include <covscript/core/cni.hpp>
#include <ctime>

namespace cs_impl {
	namespace operators {
		template <typename T>
		static any add(const T &lhs, const any &rhs)
		{
			throw cs::lang_error("Type " + cxx_demangle(get_name_of_type<T>()) + " does not support + operator.");
		}

		template <typename T>
		static any sub(const T &lhs, const any &rhs)
		{
			throw cs::lang_error("Type " + cxx_demangle(get_name_of_type<T>()) + " does not support - operator.");
		}

		template <typename T>
		static any mul(const T &lhs, const any &rhs)
		{
			throw cs::lang_error("Type " + cxx_demangle(get_name_of_type<T>()) + " does not support * operator.");
		}

		template <typename T>
		static any div(const T &lhs, const any &rhs)
		{
			throw cs::lang_error("Type " + cxx_demangle(get_name_of_type<T>()) + " does not support / operator.");
		}

		template <typename T>
		static any mod(const T &lhs, const any &rhs)
		{
			throw cs::lang_error("Type " + cxx_demangle(get_name_of_type<T>()) + " does not support \% operator.");
		}

		template <typename T>
		static any pow(const T &lhs, const any &rhs)
		{
			throw cs::lang_error("Type " + cxx_demangle(get_name_of_type<T>()) + " does not support ^ operator.");
		}

		template <typename T>
		static any minus(const T &val)
		{
			throw cs::lang_error("Type " + cxx_demangle(get_name_of_type<T>()) + " does not support -val operator.");
		}

		template <typename T>
		static any &escape(T &val)
		{
			throw cs::lang_error("Type " + cxx_demangle(get_name_of_type<T>()) + " does not support *val operator.");
		}

		template <typename T>
		static void selfinc(T &val)
		{
			throw cs::lang_error("Type " + cxx_demangle(get_name_of_type<T>()) + " does not support ++ operator.");
		}

		template <typename T>
		static void selfdec(T &val)
		{
			throw cs::lang_error("Type " + cxx_demangle(get_name_of_type<T>()) + " does not support -- operator.");
		}

		template <typename T>
		static bool compare(const T &a, const T &b)
		{
			return compare_if<T, compare_helper<T>::value>::compare(a, b);
		}

		template <typename T>
		static bool abocmp(const T &lhs, const T &rhs)
		{
			throw cs::lang_error("Type " + cxx_demangle(get_name_of_type<T>()) + " does not support > operator.");
		}

		template <typename T>
		static bool undcmp(const T &lhs, const T &rhs)
		{
			throw cs::lang_error("Type " + cxx_demangle(get_name_of_type<T>()) + " does not support < operator.");
		}

		template <typename T>
		static bool aeqcmp(const T &lhs, const T &rhs)
		{
			throw cs::lang_error("Type " + cxx_demangle(get_name_of_type<T>()) + " does not support >= operator.");
		}

		template <typename T>
		static bool ueqcmp(const T &lhs, const T &rhs)
		{
			throw cs::lang_error("Type " + cxx_demangle(get_name_of_type<T>()) + " does not support <= operator.");
		}

		template <typename T>
		static any &index(T &data, const any &index)
		{
			throw cs::lang_error("Type " + cxx_demangle(get_name_of_type<T>()) + " does not support data[index] operator.");
		}

		template <typename T>
		static any &access(T &data, const cs::string_borrower &meber)
		{
			throw cs::lang_error("Type " + cxx_demangle(get_name_of_type<T>()) + " does not support data.member operator.");
		}

		template <typename T>
		static any fcall(const T &func, cs::vector &args)
		{
			throw cs::lang_error("Type " + cxx_demangle(get_name_of_type<T>()) + " does not support func(...) operator.");
		}
	} // namespace operators
} // namespace cs_impl

template <std::size_t align_size, template <typename> class allocator_t>
template <typename T>
typename cs_impl::basic_var<align_size, allocator_t>::var_op_result cs_impl::basic_var<align_size, allocator_t>::call_operator(
    cs_impl::operators::type op, void *lhs, void *rhs)
{
	switch (op) {
	case operators::type::type_id:
		return var_op_result::from_ptr((void *) &typeid(T));
	case operators::type::type_name:
		return var_op_result::from_ptr((void *) cs_impl::get_name_of_type<T>());
	case operators::type::to_integer:
		return var_op_result::from_int(cs_impl::to_integer(*static_cast<const T *>(lhs)));
	case operators::type::to_string:
		*static_cast<cs::string_borrower *>(rhs) = cs_impl::to_string(*static_cast<const T *>(lhs));
		return var_op_result();
	case operators::type::hash:
		return var_op_result::from_uint(cs_impl::hash<T>(*static_cast<const T *>(lhs)));
	case operators::type::detach:
		cs_impl::detach<T>(*static_cast<T *>(lhs));
		return var_op_result();
	case operators::type::ext_ns:
		return var_op_result::from_ptr(&cs_impl::get_ext<T>());
	case operators::type::add: {
		any result = operators::add(*static_cast<const T *>(lhs), *static_cast<const any *>(rhs));
		any::proxy *pxy = nullptr;
		std::swap(result.mDat, pxy);
		return var_op_result::from_ptr(pxy);
	}
	case operators::type::sub: {
		any result = operators::sub(*static_cast<const T *>(lhs), *static_cast<const any *>(rhs));
		any::proxy *pxy = nullptr;
		std::swap(result.mDat, pxy);
		return var_op_result::from_ptr(pxy);
	}
	case operators::type::mul: {
		any result = operators::mul(*static_cast<const T *>(lhs), *static_cast<const any *>(rhs));
		any::proxy *pxy = nullptr;
		std::swap(result.mDat, pxy);
		return var_op_result::from_ptr(pxy);
	}
	case operators::type::div: {
		any result = operators::div(*static_cast<const T *>(lhs), *static_cast<const any *>(rhs));
		any::proxy *pxy = nullptr;
		std::swap(result.mDat, pxy);
		return var_op_result::from_ptr(pxy);
	}
	case operators::type::mod: {
		any result = operators::mod(*static_cast<const T *>(lhs), *static_cast<const any *>(rhs));
		any::proxy *pxy = nullptr;
		std::swap(result.mDat, pxy);
		return var_op_result::from_ptr(pxy);
	}
	case operators::type::pow: {
		any result = operators::pow(*static_cast<const T *>(lhs), *static_cast<const any *>(rhs));
		any::proxy *pxy = nullptr;
		std::swap(result.mDat, pxy);
		return var_op_result::from_ptr(pxy);
	}
	case operators::type::minus: {
		any result = operators::minus(*static_cast<const T *>(lhs));
		any::proxy *pxy = nullptr;
		std::swap(result.mDat, pxy);
		return var_op_result::from_ptr(pxy);
	}
	case operators::type::escape:
		return var_op_result::from_ptr(&operators::escape(*static_cast<T *>(lhs)));
	case operators::type::selfinc:
		operators::selfinc(*static_cast<T *>(lhs));
		return var_op_result();
	case operators::type::selfdec:
		operators::selfdec(*static_cast<T *>(lhs));
		return var_op_result();
	// Special operators, type check finished outside.
	case operators::type::compare:
		return var_op_result::from_int(operators::compare(*static_cast<const T *>(lhs), static_cast<const basic_var *>(rhs)->template unchecked_get<T>()));
	case operators::type::abocmp:
		return var_op_result::from_int(operators::abocmp(*static_cast<const T *>(lhs), static_cast<const basic_var *>(rhs)->template unchecked_get<T>()));
	case operators::type::undcmp:
		return var_op_result::from_int(operators::undcmp(*static_cast<const T *>(lhs), static_cast<const basic_var *>(rhs)->template unchecked_get<T>()));
	case operators::type::aeqcmp:
		return var_op_result::from_int(operators::aeqcmp(*static_cast<const T *>(lhs), static_cast<const basic_var *>(rhs)->template unchecked_get<T>()));
	case operators::type::ueqcmp:
		return var_op_result::from_int(operators::ueqcmp(*static_cast<const T *>(lhs), static_cast<const basic_var *>(rhs)->template unchecked_get<T>()));
	case operators::type::index:
		return var_op_result::from_ptr(&operators::index(*static_cast<T *>(lhs), *static_cast<const any *>(rhs)));
	case operators::type::access:
		return var_op_result::from_ptr(&operators::access(*static_cast<T *>(lhs), *static_cast<const cs::string_borrower *>(rhs)));
	case operators::type::fcall: {
		any result = operators::fcall(*static_cast<const T *>(lhs), *static_cast<cs::vector *>(rhs));
		any::proxy *pxy = nullptr;
		std::swap(result.mDat, pxy);
		return var_op_result::from_ptr(pxy);
	}
	default:
		throw cs::internal_error("Unknown var operator.");
	}
}

namespace cs_impl {
	namespace path_cs_ext {
		struct path_info final {
			std::string name;
			int type;

			path_info() = delete;

			path_info(const char *n, int t) : name(n), type(t) {}
		};
	} // namespace path_cs_ext

	void init_extensions();

// Namespace declarations
	extern cs::namespace_t member_visitor_ext;
	extern cs::namespace_t except_ext;
	extern cs::namespace_t array_ext;
	extern cs::namespace_t array_iterator_ext;
	extern cs::namespace_t number_ext;
	extern cs::namespace_t char_ext;
	extern cs::namespace_t math_ext;
	extern cs::namespace_t math_const_ext;
	extern cs::namespace_t list_ext;
	extern cs::namespace_t list_iterator_ext;
	extern cs::namespace_t hash_set_ext;
	extern cs::namespace_t hash_map_ext;
	extern cs::namespace_t pair_ext;
	extern cs::namespace_t time_ext;
	extern cs::namespace_t fiber_ext;
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
	template <>
	void detach<cs::pair>(cs::pair &val)
	{
		cs::copy_no_return(val.first);
		cs::copy_no_return(val.second);
	}

	template <>
	void detach<cs::list>(cs::list &val)
	{
		for (auto &it : val)
			cs::copy_no_return(it);
	}

	template <>
	void detach<cs::array>(cs::array &val)
	{
		for (auto &it : val)
			cs::copy_no_return(it);
	}

	template <>
	void detach<cs::hash_map>(cs::hash_map &val)
	{
		for (auto &it : val)
			cs::copy_no_return(it.second);
	}

// To String
	template <>
	cs::string_borrower to_string<cs::numeric>(const cs::numeric &val)
	{
		if (!val.is_integer()) {
			std::stringstream ss;
			std::string str;
			ss << std::setprecision(cs::current_process->output_precision) << val.as_float();
			ss >> str;
			return std::move(str);
		}
		else
			return std::to_string(val.as_integer());
	}

	template <>
	cs::string_borrower to_string<char>(const char &c)
	{
		return std::string(1, c);
	}

	template <>
	cs::string_borrower to_string<cs::list>(const cs::list &lst)
	{
		if (lst.empty())
			return "list => {}";
		std::string str = "list => {";
		for (const cs::var &it : lst)
			str += it.to_string().extract() + ", ";
		str.resize(str.size() - 2);
		str += "}";
		return std::move(str);
	}

	template <>
	cs::string_borrower to_string<cs::array>(const cs::array &arr)
	{
		if (arr.empty())
			return "{}";
		std::string str = "{";
		for (const cs::var &it : arr)
			str += it.to_string().extract() + ", ";
		str.resize(str.size() - 2);
		str += "}";
		return std::move(str);
	}

	template <>
	cs::string_borrower to_string<cs::pair>(const cs::pair &p)
	{
		return p.first.to_string().extract() + " : " + p.second.to_string();
	}

	template <>
	cs::string_borrower to_string<cs::hash_set>(const cs::hash_set &set)
	{
		if (set.empty())
			return "cs::hash_set => {}";
		std::string str = "cs::hash_set => {";
		for (const cs::var &it : set)
			str += it.to_string().extract() + ", ";
		str.resize(str.size() - 2);
		str += "}";
		return std::move(str);
	}

	template <>
	cs::string_borrower to_string<cs::hash_map>(const cs::hash_map &map)
	{
		if (map.empty())
			return "cs::hash_map => {}";
		std::string str = "cs::hash_map => {";
		for (const cs::pair &it : map)
			str += cs_impl::to_string(it).extract() + ", ";
		str.resize(str.size() - 2);
		str += "}";
		return std::move(str);
	}

	template <>
	cs::string_borrower to_string<cs::pointer>(const cs::pointer &ptr)
	{
		if (ptr == cs::null_pointer)
			return "null";
		else
			return "cs::pointer => " + ptr.data.to_string().extract();
	}

	template <>
	cs::string_borrower to_string<cs::type_id>(const cs::type_id &id)
	{
		if (id.type_hash != 0)
			return cxx_demangle(id.type_idx.name()) + "_" + to_string(id.type_hash);
		else
			return cxx_demangle(id.type_idx.name());
	}

	template <>
	cs::string_borrower to_string<cs::range_type>(const cs::range_type &range)
	{
		if (range.empty())
			return "cs::range => {}";
		std::string str = "cs::range => {";
		for (cs::numeric it : range)
			str += to_string(it).extract() + ", ";
		str.resize(str.size() - 2);
		str += "}";
		return std::move(str);
	}

	template <>
	cs::string_borrower to_string<cs::char_buff>(const cs::char_buff &buff)
	{
		return buff->str();
	}

	template <>
	cs::string_borrower to_string<std::tm>(const std::tm &t)
	{
		return std::asctime(&t);
	}

// To Integer
	template <>
	std::intptr_t to_integer<cs::numeric>(const cs::numeric &num)
	{
		return num.as_integer();
	}

	template <>
	std::intptr_t to_integer<std::string>(const std::string &str)
	{
		for (auto &ch : str) {
			if (!std::isdigit(ch))
				throw cs::runtime_error("Wrong literal format.");
		}
		return std::stol(str);
	}

// Hash
	template <>
	std::size_t hash<cs::type_id>(const cs::type_id &id)
	{
		if (id.type_hash == 0)
			return id.type_idx.hash_code();
		else
			throw cov::error("E000F");
	}

	template <>
	std::size_t hash<cs::numeric>(const cs::numeric &num)
	{
		if (num.is_integer())
			return hash(num.as_integer());
		else
			return hash(num.as_float());
	}

// Type name
	template <>
	constexpr const char *get_name_of_type<cs::context_t>()
	{
		return "cs::context";
	}

	template <>
	constexpr const char *get_name_of_type<cs::var>()
	{
		return "cs::var";
	}

	template <>
	constexpr const char *get_name_of_type<cs::numeric>()
	{
		return "cs::numeric";
	}

	template <>
	constexpr const char *get_name_of_type<cs::boolean>()
	{
		return "cs::boolean";
	}

	template <>
	constexpr const char *get_name_of_type<cs::pointer>()
	{
		return "cs::pointer";
	}

	template <>
	constexpr const char *get_name_of_type<char>()
	{
		return "cs::char";
	}

	template <>
	constexpr const char *get_name_of_type<cs::string>()
	{
		return "cs::string";
	}

	template <>
	constexpr const char *get_name_of_type<cs::list::iterator>()
	{
		return "cs::list::iterator";
	}

	template <>
	constexpr const char *get_name_of_type<cs::list>()
	{
		return "cs::list";
	}

	template <>
	constexpr const char *get_name_of_type<cs::array::iterator>()
	{
		return "cs::array::iterator";
	}

	template <>
	constexpr const char *get_name_of_type<cs::array>()
	{
		return "cs::array";
	}

	template <>
	constexpr const char *get_name_of_type<cs::pair>()
	{
		return "cs::pair";
	}

	template <>
	constexpr const char *get_name_of_type<cs::hash_set>()
	{
		return "cs::hash_set";
	}

	template <>
	constexpr const char *get_name_of_type<cs::hash_map>()
	{
		return "cs::hash_map";
	}

	template <>
	constexpr const char *get_name_of_type<cs::type_t>()
	{
		return "cs::type";
	}

	template <>
	constexpr const char *get_name_of_type<cs::namespace_t>()
	{
		return "cs::namespace";
	}

	template <>
	constexpr const char *get_name_of_type<cs::callable>()
	{
		return "cs::function";
	}

	template <>
	constexpr const char *get_name_of_type<cs::range_type>()
	{
		return "cs::range";
	}

	template <>
	constexpr const char *get_name_of_type<cs::structure>()
	{
		return "cs::structure";
	}

	template <>
	constexpr const char *get_name_of_type<cs::lang_error>()
	{
		return "cs::exception";
	}

	template <>
	constexpr const char *get_name_of_type<cs::char_buff>()
	{
		return "cs::char_buff";
	}

	template <>
	constexpr const char *get_name_of_type<cs::istream>()
	{
		return "cs::istream";
	}

	template <>
	constexpr const char *get_name_of_type<cs::ostream>()
	{
		return "cs::ostream";
	}

	template <>
	constexpr const char *get_name_of_type<std::tm>()
	{
		return "cs::time_type";
	}

#ifndef _MSC_VER

	template <>
	constexpr const char *get_name_of_type<std::ios_base::seekdir>()
	{
		return "cs::iostream::seekdir";
	}

	template <>
	constexpr const char *get_name_of_type<std::ios_base::openmode>()
	{
		return "cs::iostream::openmode";
	}

#endif

	template <>
	constexpr const char *get_name_of_type<cs::fiber_t>()
	{
		return "cs::fiber";
	}

	template <>
	constexpr const char *get_name_of_type<cs::tree_type<cs::token_base *>>()
	{
		return "cs::expression";
	}

	template <>
	constexpr const char *get_name_of_type<path_cs_ext::path_info>()
	{
		return "cs::system::path_info";
	}

// Type Extensions

	template <>
	cs::namespace_t &get_ext<cs::member_visitor>()
	{
		return member_visitor_ext;
	}

	template <>
	cs::namespace_t &get_ext<cs::lang_error>()
	{
		return except_ext;
	}

	template <>
	cs::namespace_t &get_ext<cs::array>()
	{
		return array_ext;
	}

	template <>
	cs::namespace_t &get_ext<cs::array::iterator>()
	{
		return array_iterator_ext;
	}

	template <>
	cs::namespace_t &get_ext<cs::numeric>()
	{
		return number_ext;
	}

	template <>
	cs::namespace_t &get_ext<char>()
	{
		return char_ext;
	}

	template <>
	cs::namespace_t &get_ext<cs::hash_set>()
	{
		return hash_set_ext;
	}

	template <>
	cs::namespace_t &get_ext<cs::hash_map>()
	{
		return hash_map_ext;
	}

	template <>
	cs::namespace_t &get_ext<cs::list>()
	{
		return list_ext;
	}

	template <>
	cs::namespace_t &get_ext<cs::list::iterator>()
	{
		return list_iterator_ext;
	}

	template <>
	cs::namespace_t &get_ext<cs::pair>()
	{
		return pair_ext;
	}

	template <>
	cs::namespace_t &get_ext<cs::char_buff>()
	{
		return charbuff_ext;
	}

	template <>
	cs::namespace_t &get_ext<cs::istream>()
	{
		return istream_ext;
	}

	template <>
	cs::namespace_t &get_ext<cs::ostream>()
	{
		return ostream_ext;
	}

	template <>
	cs::namespace_t &get_ext<std::tm>()
	{
		return time_ext;
	}

	template <>
	cs::namespace_t &get_ext<cs::fiber_t>()
	{
		return fiber_ext;
	}

	template <>
	cs::namespace_t &get_ext<cs::context_t>()
	{
		return context_ext;
	}

	template <>
	cs::namespace_t &get_ext<cs::string>()
	{
		return string_ext;
	}

	template <>
	cs::namespace_t &get_ext<path_cs_ext::path_info>()
	{
		return path_info_ext;
	}
} // namespace cs_impl
