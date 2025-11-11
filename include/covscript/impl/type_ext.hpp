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

namespace cs {
	namespace operators {
		template <typename T>
		static inline var add(const T &lhs, const var &rhs)
		{
			throw lang_error("Type " + cs_impl::cxx_demangle(cs_impl::get_name_of_type<T>()) + " does not support + operator.");
		}

		template <typename T>
		static inline var sub(const T &lhs, const var &rhs)
		{
			throw lang_error("Type " + cs_impl::cxx_demangle(cs_impl::get_name_of_type<T>()) + " does not support - operator.");
		}

		template <typename T>
		static inline var mul(const T &lhs, const var &rhs)
		{
			throw lang_error("Type " + cs_impl::cxx_demangle(cs_impl::get_name_of_type<T>()) + " does not support * operator.");
		}

		template <typename T>
		static inline var div(const T &lhs, const var &rhs)
		{
			throw lang_error("Type " + cs_impl::cxx_demangle(cs_impl::get_name_of_type<T>()) + " does not support / operator.");
		}

		template <typename T>
		static inline var mod(const T &lhs, const var &rhs)
		{
			throw lang_error("Type " + cs_impl::cxx_demangle(cs_impl::get_name_of_type<T>()) + " does not support \% operator.");
		}

		template <typename T>
		static inline var pow(const T &lhs, const var &rhs)
		{
			throw lang_error("Type " + cs_impl::cxx_demangle(cs_impl::get_name_of_type<T>()) + " does not support ^ operator.");
		}

		template <typename T>
		static inline var minus(const T &val)
		{
			throw lang_error("Type " + cs_impl::cxx_demangle(cs_impl::get_name_of_type<T>()) + " does not support -val operator.");
		}

		template <typename T>
		static inline var &escape(T &val)
		{
			throw lang_error("Type " + cs_impl::cxx_demangle(cs_impl::get_name_of_type<T>()) + " does not support *val operator.");
		}

		template <typename T>
		static inline void selfinc(T &val)
		{
			throw lang_error("Type " + cs_impl::cxx_demangle(cs_impl::get_name_of_type<T>()) + " does not support ++ operator.");
		}

		template <typename T>
		static inline void selfdec(T &val)
		{
			throw lang_error("Type " + cs_impl::cxx_demangle(cs_impl::get_name_of_type<T>()) + " does not support -- operator.");
		}

		template <typename T>
		static inline bool compare(const T &a, const T &b)
		{
			return cs_impl::compare(a, b);
		}

		template <typename T>
		static inline bool abocmp(const T &lhs, const T &rhs)
		{
			throw lang_error("Type " + cs_impl::cxx_demangle(cs_impl::get_name_of_type<T>()) + " does not support > operator.");
		}

		template <typename T>
		static inline bool undcmp(const T &lhs, const T &rhs)
		{
			throw lang_error("Type " + cs_impl::cxx_demangle(cs_impl::get_name_of_type<T>()) + " does not support < operator.");
		}

		template <typename T>
		static inline bool aeqcmp(const T &lhs, const T &rhs)
		{
			throw lang_error("Type " + cs_impl::cxx_demangle(cs_impl::get_name_of_type<T>()) + " does not support >= operator.");
		}

		template <typename T>
		static inline bool ueqcmp(const T &lhs, const T &rhs)
		{
			throw lang_error("Type " + cs_impl::cxx_demangle(cs_impl::get_name_of_type<T>()) + " does not support <= operator.");
		}

		template <typename T>
		static inline var &index_ref(T &data, const var &index)
		{
			throw lang_error("Type " + cs_impl::cxx_demangle(cs_impl::get_name_of_type<T>()) + " does not support data[index] operator.");
		}

		template <typename T>
		static inline var index(const T &data, const var &idx)
		{
			return index_ref(const_cast<T &>(data), idx);
		}

		template <typename T>
		static inline var index(T &data, const var &idx)
		{
			return index_ref(data, idx);
		}

		template <typename T>
		static inline var &access_ref(T &data, const string &id)
		{
			throw lang_error("Type " + cs_impl::cxx_demangle(cs_impl::get_name_of_type<T>()) + " does not support data.member operator.");
		}

		template <typename T>
		static inline var access(const T &data, const string &id)
		{
			return access_ref(const_cast<T &>(data), id);
		}

		template <typename T>
		static inline var access(T &data, const string &id)
		{
			return access_ref(data, id);
		}

		template <typename T>
		static inline void prep_call(const T &func, vector &args)
		{
		}

		template <typename T>
		static inline var fcall(const T &func, vector &args)
		{
			throw lang_error("Type " + cs_impl::cxx_demangle(cs_impl::get_name_of_type<T>()) + " does not support func(...) operator.");
		}
	} // namespace operators
} // namespace cs

template <typename T>
cs_impl::operators::result cs_impl::operators::handler<T>::get(void *lhs, void *rhs)
{
	return result::from_ptr(lhs);
}

template <typename T>
cs_impl::operators::result cs_impl::operators::handler<T>::type_id(void *lhs, void *rhs)
{
	return result::from_ptr((void *) &typeid(T));
}

template <typename T>
cs_impl::operators::result cs_impl::operators::handler<T>::type_name(void *lhs, void *rhs)
{
	return result::from_ptr((void *) cs_impl::get_name_of_type<T>());
}

template <typename T>
cs_impl::operators::result cs_impl::operators::handler<T>::to_integer(void *lhs, void *rhs)
{
	return result::from_int(cs_impl::to_integer(*static_cast<const T *>(lhs)));
}

template <typename T>
cs_impl::operators::result cs_impl::operators::handler<T>::to_string(void *lhs, void *rhs)
{
	*static_cast<cs::string_borrower *>(rhs) = cs_impl::to_string(*static_cast<const T *>(lhs));
	return result();
}

template <typename T>
cs_impl::operators::result cs_impl::operators::handler<T>::hash(void *lhs, void *rhs)
{
	return result::from_uint(cs_impl::hash<T>(*static_cast<const T *>(lhs)));
}

template <typename T>
cs_impl::operators::result cs_impl::operators::handler<T>::detach(void *lhs, void *rhs)
{
	cs_impl::detach<T>(*static_cast<T *>(lhs));
	return result();
}

template <typename T>
cs_impl::operators::result cs_impl::operators::handler<T>::ext_ns(void *lhs, void *rhs)
{
	return result::from_ptr((void *) &cs_impl::get_ext<T>());
}

template <typename T>
cs_impl::operators::result cs_impl::operators::handler<T>::add(void *lhs, void *rhs)
{
	any result = cs::operators::add(*static_cast<const T *>(lhs), *static_cast<const any *>(rhs));
	any::proxy *pxy = nullptr;
	std::swap(result.mDat, pxy);
	return result::from_ptr(pxy);
}

template <typename T>
cs_impl::operators::result cs_impl::operators::handler<T>::sub(void *lhs, void *rhs)
{
	any result = cs::operators::sub(*static_cast<const T *>(lhs), *static_cast<const any *>(rhs));
	any::proxy *pxy = nullptr;
	std::swap(result.mDat, pxy);
	return result::from_ptr(pxy);
}

template <typename T>
cs_impl::operators::result cs_impl::operators::handler<T>::mul(void *lhs, void *rhs)
{
	any result = cs::operators::mul(*static_cast<const T *>(lhs), *static_cast<const any *>(rhs));
	any::proxy *pxy = nullptr;
	std::swap(result.mDat, pxy);
	return result::from_ptr(pxy);
}

template <typename T>
cs_impl::operators::result cs_impl::operators::handler<T>::div(void *lhs, void *rhs)
{
	any result = cs::operators::div(*static_cast<const T *>(lhs), *static_cast<const any *>(rhs));
	any::proxy *pxy = nullptr;
	std::swap(result.mDat, pxy);
	return result::from_ptr(pxy);
}

template <typename T>
cs_impl::operators::result cs_impl::operators::handler<T>::mod(void *lhs, void *rhs)
{
	any result = cs::operators::mod(*static_cast<const T *>(lhs), *static_cast<const any *>(rhs));
	any::proxy *pxy = nullptr;
	std::swap(result.mDat, pxy);
	return result::from_ptr(pxy);
}

template <typename T>
cs_impl::operators::result cs_impl::operators::handler<T>::pow(void *lhs, void *rhs)
{
	any result = cs::operators::pow(*static_cast<const T *>(lhs), *static_cast<const any *>(rhs));
	any::proxy *pxy = nullptr;
	std::swap(result.mDat, pxy);
	return result::from_ptr(pxy);
}

template <typename T>
cs_impl::operators::result cs_impl::operators::handler<T>::minus(void *lhs, void *rhs)
{
	any result = cs::operators::minus(*static_cast<const T *>(lhs));
	any::proxy *pxy = nullptr;
	std::swap(result.mDat, pxy);
	return result::from_ptr(pxy);
}

template <typename T>
cs_impl::operators::result cs_impl::operators::handler<T>::escape(void *lhs, void *rhs)
{
	return result::from_ptr(&cs::operators::escape(*static_cast<T *>(lhs)));
}

template <typename T>
cs_impl::operators::result cs_impl::operators::handler<T>::selfinc(void *lhs, void *rhs)
{
	cs::operators::selfinc(*static_cast<T *>(lhs));
	return result();
}

template <typename T>
cs_impl::operators::result cs_impl::operators::handler<T>::selfdec(void *lhs, void *rhs)
{
	cs::operators::selfdec(*static_cast<T *>(lhs));
	return result();
}

template <typename T>
cs_impl::operators::result cs_impl::operators::handler<T>::compare(void *lhs, void *rhs)
{
	return result::from_int(cs::operators::compare(*static_cast<const T *>(lhs), static_cast<const any *>(rhs)->template unchecked_get<T>()));
}

template <typename T>
cs_impl::operators::result cs_impl::operators::handler<T>::abocmp(void *lhs, void *rhs)
{
	return result::from_int(cs::operators::abocmp(*static_cast<const T *>(lhs), static_cast<const any *>(rhs)->template unchecked_get<T>()));
}

template <typename T>
cs_impl::operators::result cs_impl::operators::handler<T>::undcmp(void *lhs, void *rhs)
{
	return result::from_int(cs::operators::undcmp(*static_cast<const T *>(lhs), static_cast<const any *>(rhs)->template unchecked_get<T>()));
}

template <typename T>
cs_impl::operators::result cs_impl::operators::handler<T>::aeqcmp(void *lhs, void *rhs)
{
	return result::from_int(cs::operators::aeqcmp(*static_cast<const T *>(lhs), static_cast<const any *>(rhs)->template unchecked_get<T>()));
}

template <typename T>
cs_impl::operators::result cs_impl::operators::handler<T>::ueqcmp(void *lhs, void *rhs)
{
	return result::from_int(cs::operators::ueqcmp(*static_cast<const T *>(lhs), static_cast<const any *>(rhs)->template unchecked_get<T>()));
}

template <typename T>
cs_impl::operators::result cs_impl::operators::handler<T>::index(void *lhs, void *rhs)
{
	any result = cs::operators::index(*static_cast<T *>(lhs), *static_cast<const any *>(rhs));
	any::proxy *pxy = nullptr;
	std::swap(result.mDat, pxy);
	return result::from_ptr(pxy);
}

template <typename T>
cs_impl::operators::result cs_impl::operators::handler<T>::cindex(void *lhs, void *rhs)
{
	any result = cs::operators::index(*static_cast<const T *>(lhs), *static_cast<const any *>(rhs));
	any::proxy *pxy = nullptr;
	std::swap(result.mDat, pxy);
	return result::from_ptr(pxy);
}

template <typename T>
cs_impl::operators::result cs_impl::operators::handler<T>::index_ref(void *lhs, void *rhs)
{
	return result::from_ptr(&cs::operators::index_ref(*static_cast<T *>(lhs), *static_cast<const any *>(rhs)));
}

template <typename T>
cs_impl::operators::result cs_impl::operators::handler<T>::access(void *lhs, void *rhs)
{
	any result = cs::operators::access(*static_cast<T *>(lhs), *static_cast<const cs::string *>(rhs));
	any::proxy *pxy = nullptr;
	std::swap(result.mDat, pxy);
	return result::from_ptr(pxy);
}

template <typename T>
cs_impl::operators::result cs_impl::operators::handler<T>::caccess(void *lhs, void *rhs)
{
	any result = cs::operators::access(*static_cast<const T *>(lhs), *static_cast<const cs::string *>(rhs));
	any::proxy *pxy = nullptr;
	std::swap(result.mDat, pxy);
	return result::from_ptr(pxy);
}

template <typename T>
cs_impl::operators::result cs_impl::operators::handler<T>::access_ref(void *lhs, void *rhs)
{
	return result::from_ptr(&cs::operators::access_ref(*static_cast<T *>(lhs), *static_cast<const cs::string *>(rhs)));
}

template <typename T>
cs_impl::operators::result cs_impl::operators::handler<T>::prep_call(void *lhs, void *rhs)
{
	cs::operators::prep_call(*static_cast<const T *>(lhs), *static_cast<cs::vector *>(rhs));
	return result();
}

template <typename T>
cs_impl::operators::result cs_impl::operators::handler<T>::fcall(void *lhs, void *rhs)
{
	any result = cs::operators::fcall(*static_cast<const T *>(lhs), *static_cast<cs::vector *>(rhs));
	any::proxy *pxy = nullptr;
	std::swap(result.mDat, pxy);
	return result::from_ptr(pxy);
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

namespace cs {
// Operator +
	template <>
	var operators::add<cs::numeric>(const cs::numeric &lhs, const var &rhs)
	{
		return lhs + rhs.const_val<cs::numeric>();
	}

	template <>
	var operators::add<cs::string>(const cs::string &lhs, const var &rhs)
	{
		var str = var::make<cs::string>(lhs);
		cs::string &lhs_ref = str.val<cs::string>();
		if (rhs.is_type_of<cs::string>())
			lhs_ref.append(rhs.const_val<cs::string>());
		else
			lhs_ref.append(rhs.to_string());
		return str;
	}

	template <>
	var operators::add<cs::array>(const cs::array &lhs, const var &rhs)
	{
		var arr = var::make<cs::array>(lhs);
		cs::array &lhs_ref = arr.val<cs::array>();
		if (rhs.is_type_of<cs::array>()) {
			const cs::array &rhs_ref = rhs.const_val<cs::array>();
			lhs_ref.insert(lhs_ref.end(), rhs_ref.begin(), rhs_ref.end());
		}
		else
			lhs_ref.emplace_back(rhs);
		detach(lhs_ref);
		return arr;
	}

// Operator -
	template <>
	var operators::sub<cs::numeric>(const cs::numeric &lhs, const var &rhs)
	{
		return lhs - rhs.const_val<cs::numeric>();
	}

// Operator *
	template <>
	var operators::mul<cs::numeric>(const cs::numeric &lhs, const var &rhs)
	{
		return lhs * rhs.const_val<cs::numeric>();
	}

	template <>
	var operators::mul<cs::string>(const cs::string &lhs, const var &rhs)
	{
		var str = var::make<cs::string>();
		cs::string &lhs_ref = str.val<cs::string>();
		cs::numeric_integer times = rhs.const_val<cs::numeric>().as_integer();
		if (times <= 0)
			throw cs::lang_error("Can not multiply string with negative number.");
		lhs_ref.reserve(lhs.size() * times);
		while (times-- > 0)
			lhs_ref.append(lhs);
		return str;
	}

	template <>
	var operators::mul<cs::array>(const cs::array &lhs, const var &rhs)
	{
		var arr = var::make<cs::array>();
		cs::array &lhs_ref = arr.val<cs::array>();
		cs::numeric_integer times = rhs.const_val<cs::numeric>().as_integer();
		if (times <= 0)
			throw cs::lang_error("Can not multiply array with negative number.");
		while (times-- > 0)
			lhs_ref.insert(lhs_ref.end(), lhs.begin(), lhs.end());
		detach(lhs_ref);
		return arr;
	}

// Operator /
	template <>
	var operators::div<cs::numeric>(const cs::numeric &lhs, const var &rhs)
	{
		return lhs / rhs.const_val<cs::numeric>();
	}

// Operator %
	template <>
	var operators::mod<cs::numeric>(const cs::numeric &lhs, const var &rhs)
	{
		return lhs % rhs.const_val<cs::numeric>();
	}

// Operator ^
	template <>
	var operators::pow<cs::numeric>(const cs::numeric &lhs, const var &rhs)
	{
		return lhs ^ rhs.const_val<cs::numeric>();
	}

// Operator -val
	template <>
	var operators::minus<cs::numeric>(const cs::numeric &lhs)
	{
		if (lhs.is_float())
			return cs::numeric(-lhs.as_float());
		else
			return cs::numeric(-lhs.as_integer());
	}

// Operator *val
	template <>
	var &operators::escape<cs::pointer>(cs::pointer &ptr)
	{
		if (ptr.data.usable())
			return ptr.data;
		else
			throw cs::runtime_error("Escape from null pointer.");
	}

	template <>
	var &operators::escape<cs::array::iterator>(cs::array::iterator &it)
	{
		return *it;
	}

	template <>
	var &operators::escape<cs::list::iterator>(cs::list::iterator &it)
	{
		return *it;
	}

// Operator ++
	template <>
	void operators::selfinc<cs::numeric>(cs::numeric &lhs)
	{
		++lhs;
	}

	template <>
	void operators::selfinc<cs::array::iterator>(cs::array::iterator &lhs)
	{
		++lhs;
	}

	template <>
	void operators::selfinc<cs::list::iterator>(cs::list::iterator &lhs)
	{
		++lhs;
	}

// Operator --
	template <>
	void operators::selfdec<cs::numeric>(cs::numeric &lhs)
	{
		--lhs;
	}

	template <>
	void operators::selfdec<cs::array::iterator>(cs::array::iterator &lhs)
	{
		--lhs;
	}

	template <>
	void operators::selfdec<cs::list::iterator>(cs::list::iterator &lhs)
	{
		--lhs;
	}

// Operator >
	template <>
	bool operators::abocmp<cs::numeric>(const cs::numeric &lhs, const cs::numeric &rhs)
	{
		return lhs > rhs;
	}

	template <>
	bool operators::abocmp<cs::string>(const cs::string &lhs, const cs::string &rhs)
	{
		return lhs > rhs;
	}

// Operator >
	template <>
	bool operators::undcmp<cs::numeric>(const cs::numeric &lhs, const cs::numeric &rhs)
	{
		return lhs < rhs;
	}

	template <>
	bool operators::undcmp<cs::string>(const cs::string &lhs, const cs::string &rhs)
	{
		return lhs < rhs;
	}

// Operator >=
	template <>
	bool operators::aeqcmp<cs::numeric>(const cs::numeric &lhs, const cs::numeric &rhs)
	{
		return lhs >= rhs;
	}

	template <>
	bool operators::aeqcmp<cs::string>(const cs::string &lhs, const cs::string &rhs)
	{
		return lhs >= rhs;
	}

// Operator >=
	template <>
	bool operators::ueqcmp<cs::numeric>(const cs::numeric &lhs, const cs::numeric &rhs)
	{
		return lhs <= rhs;
	}

	template <>
	bool operators::ueqcmp<cs::string>(const cs::string &lhs, const cs::string &rhs)
	{
		return lhs <= rhs;
	}

// Operator []
	template <>
	var &operators::index_ref<cs::string>(cs::string &str, const var &idx)
	{
		throw cs::lang_error("Referencing elements of a string is not supported.");
	}

	template <>
	var operators::index<cs::string>(const cs::string &str, const var &idx)
	{
		cs::numeric_integer pos = idx.const_val<cs::numeric>().as_integer();
		if (pos < 0)
			pos = str.size() + pos;
		return var::make_constant<char>(str.at(pos));
	}

	template <>
	var &operators::index_ref<cs::array>(cs::array &arr, const var &pos)
	{
		cs::numeric_integer idx = pos.const_val<cs::numeric>().as_integer();
		if (idx < 0)
			idx = arr.size() + idx;
		if (idx > arr.size())
			arr.resize(idx + 1, cs::numeric(0));
		return arr[idx];
	}

	template <>
	var operators::index<cs::array>(const cs::array &arr, const var &idx)
	{
		return arr.at(idx.const_val<cs::numeric>().as_integer());
	}

	template <>
	var &operators::index_ref<cs::hash_map>(cs::hash_map &map, const var &key)
	{
		auto it = map.find(key);
		if (it == map.end())
			it = map.emplace(cs::copy(key), cs::numeric(0)).first;
		return it->second;
	}

	template <>
	var operators::index<cs::hash_map>(const cs::hash_map &map, const var &key)
	{
		return map.at(key);
	}

// Operator .
	template <>
	var &operators::access_ref<cs::hash_map>(cs::hash_map &map, const string &key)
	{
		auto it = map.find(key);
		if (it == map.end())
			throw runtime_error(std::string("Key \"") + key + "\" does not exist.");
		return it->second;
	}

// Operator ()
	template <>
	var operators::fcall<cs::callable>(const cs::callable &fn, cs::vector &args)
	{
		return fn.call(args);
	}

	template <>
	void operators::prep_call<cs::object_method>(const cs::object_method &fn, cs::vector &args)
	{
		args.push_back(fn.object);
	}

	template <>
	var operators::fcall<cs::object_method>(const cs::object_method &fn, cs::vector &args)
	{
		return fn.callable.const_val<cs::callable>().call(args);
	}
} // namespace cs
