#pragma once
/*
* Covariant Script Type Support
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
* Copyright (C) 2018 Michael Lee(李登淳)
* Email: mikecovlee@163.com
* Github: https://github.com/mikecovlee
*/
#include <mozart/traits.hpp>
#include <type_traits>
#include <functional>

namespace cs_impl {
// Type support auto-detection(SFINAE)
// Compare
	template<typename _Tp>
	class compare_helper {
		template<typename T, typename X=bool>
		struct matcher;

		template<typename T>
		static constexpr bool match(T *)
		{
			return false;
		}

		template<typename T>
		static constexpr bool match(matcher<T, decltype(std::declval<T>() == std::declval<T>())> *)
		{
			return true;
		}

	public:
		static constexpr bool value = match<_Tp>(nullptr);
	};

	template<typename, bool>
	struct compare_if;

	template<typename T>
	struct compare_if<T, true> {
		static bool compare(const T &a, const T &b)
		{
			return a == b;
		}
	};

	template<typename T>
	struct compare_if<T, false> {
		static bool compare(const T &a, const T &b)
		{
			return &a == &b;
		}
	};

// To String
	template<typename _Tp>
	class to_string_helper {
		template<typename T, typename X>
		struct matcher;

		template<typename T>
		static constexpr bool match(T *)
		{
			return false;
		}

		template<typename T>
		static constexpr bool match(matcher<T, decltype(std::to_string(std::declval<T>()))> *)
		{
			return true;
		}

	public:
		static constexpr bool value = match<_Tp>(nullptr);
	};

	template<typename, bool>
	struct to_string_if;

	template<typename T>
	struct to_string_if<T, true> {
		static std::string to_string(const T &val)
		{
			return std::to_string(val);
		}
	};

	template<typename T>
	struct to_string_if<T, false> {
		static std::string to_string(const T &)
		{
			throw cov::error("E000D");
		}
	};

// To Integer
	template<typename, bool>
	struct to_integer_if;

	template<typename T>
	struct to_integer_if<T, true> {
		static long to_integer(const T &val)
		{
			return static_cast<long>(val);
		}
	};

	template<typename T>
	struct to_integer_if<T, false> {
		static long to_integer(const T &)
		{
			throw cov::error("E000M");
		}
	};

// Hash
	template<typename _Tp>
	class hash_helper {
		template<typename T, decltype(&std::hash<T>::operator()) X>
		struct matcher;

		template<typename T>
		static constexpr bool match(T *)
		{
			return false;
		}

		template<typename T>
		static constexpr bool match(matcher<T, &std::hash<T>::operator()> *)
		{
			return true;
		}

	public:
		static constexpr bool value = match<_Tp>(nullptr);
	};

	template<typename T>
	struct hash_gen_base {
		static std::size_t base_code;
	};

	template<typename T> std::size_t hash_gen_base<T>::base_code = typeid(T).hash_code();

	template<typename, typename, bool>
	struct hash_if;

	template<typename T, typename X>
	struct hash_if<T, X, true> {
		static std::size_t hash(const X &val)
		{
			static std::hash<T> gen;
			return gen(static_cast<const T>(val)) + hash_gen_base<X>::base_code;
		}
	};

	template<typename T>
	struct hash_if<T, T, true> {
		static std::size_t hash(const T &val)
		{
			static std::hash<T> gen;
			return gen(val);
		}
	};

	template<typename T, typename X>
	struct hash_if<T, X, false> {
		static std::size_t hash(const X &val)
		{
			throw cov::error("E000F");
		}
	};

	template<typename, bool>
	struct hash_enum_resolver;

	template<typename T>
	struct hash_enum_resolver<T, true> {
		using type=hash_if<std::size_t, T, true>;
	};

	template<typename T>
	struct hash_enum_resolver<T, false> {
		using type=hash_if<T, T, hash_helper<T>::value>;
	};

	/*
	* Type support
	* You can specialize template functions to support type-related functions.
	* But if you do not have specialization, CovScript can also automatically detect related functions.
	*/
	template<typename T>
	bool compare(const T &a, const T &b)
	{
		return compare_if<T, compare_helper<T>::value>::compare(a, b);
	}

	template<typename T>
	std::string to_string(const T &val)
	{
		return to_string_if<T, to_string_helper<T>::value>::to_string(val);
	}

	template<typename T>
	long to_integer(const T &val)
	{
		return to_integer_if<T, cov::castable<T, long>::value>::to_integer(val);
	}

	template<typename T>
	std::size_t hash(const T &val)
	{
		using type=typename hash_enum_resolver<T, std::is_enum<T>::value>::type;
		return type::hash(val);
	}

	template<typename T>
	void detach(T &val)
	{
		// Do something if you want when data is copying.
	}

	template<typename T>
	constexpr const char *get_name_of_type()
	{
		return typeid(T).name();
	}

	template<typename T>
	cs::extension_t &get_ext()
	{
		throw cs::syntax_error("Target type does not support extensions.");
	}
}