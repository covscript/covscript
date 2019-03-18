#pragma once
/*
* Covariant Mozart Utility Library: Traits
*
* Licensed under the Covariant Innovation General Public License,
* Version 1.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* https://covariant.cn/licenses/LICENSE-1.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
* Copyright (C) 2019 Michael Lee(李登淳)
* Email: mikecovlee@163.com
* Github: https://github.com/mikecovlee
*
* Version: 17.2.0
*/
#include <utility>

namespace cov {
	template<typename T, typename X>
	struct is_same_type {
		static constexpr bool value = false;
	};
	template<typename T>
	struct is_same_type<T, T> {
		static constexpr bool value = true;
	};
	template<typename T>
	struct is_constant {
		static constexpr bool value = false;
	};
	template<typename T>
	struct is_constant<T const> {
		static constexpr bool value = true;
	};
	template<typename _Tp>
	struct add_reference {
		typedef _Tp &type;
	};
	template<typename _Tp>
	struct add_reference<_Tp &> {
		typedef _Tp &type;
	};
	template<typename _Tp>
	struct add_reference<_Tp &&> {
		typedef _Tp &&type;
	};
	template<typename _Tp>
	struct add_reference<_Tp *> {
		typedef _Tp *type;
	};
	template<typename _Tp>
	struct add_constant_reference {
		typedef const _Tp &type;
	};
	template<typename _Tp>
	struct add_constant_reference<_Tp &> {
		typedef const _Tp &type;
	};
	template<typename _Tp>
	struct add_constant_reference<const _Tp &> {
		typedef const _Tp &type;
	};
	template<typename _Tp>
	struct add_constant_reference<_Tp &&> {
		typedef const _Tp &&type;
	};
	template<typename _Tp>
	struct add_constant_reference<const _Tp &&> {
		typedef const _Tp &&type;
	};
	template<typename _Tp>
	struct add_constant_reference<_Tp *> {
		typedef _Tp *type;
	};
	template<typename _Tp>
	struct add_constant_reference<const _Tp *> {
		typedef const _Tp *type;
	};
	template<typename T>
	struct remove_reference {
		using type=T;
	};
	template<typename T>
	struct remove_reference<T &> {
		using type=T;
	};
	template<typename T>
	struct remove_reference<T &&> {
		using type=T;
	};
	template<typename T>
	struct remove_reference<const T &> {
		using type=const T;
	};
	template<typename T>
	struct remove_constant {
		using type=T;
	};
	template<typename T>
	struct remove_constant<const T> {
		using type=T;
	};
	template<bool factor, typename Tx, typename Ty>
	struct replace_if;
	template<typename Tx, typename Ty>
	struct replace_if<true, Tx, Ty> {
		using result=Ty;
	};
	template<typename Tx, typename Ty>
	struct replace_if<false, Tx, Ty> {
		using result=Tx;
	};

	template<typename From, typename To>
	class castable final {
		template<typename To1>
		static void test(To1);

		template<typename From1, typename To1>
		static constexpr bool helper(...)
		{
			return false;
		}

		template<typename From1, typename To1, typename=decltype(test<To1>(std::declval<From1>()))>
		static constexpr bool helper(int)
		{
			return true;
		}

	public:
		static constexpr bool value = helper<From, To>(0);
	};
}
