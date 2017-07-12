#pragma once
/*
* Covariant Mozart Utility Library: Traits
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
* Copyright (C) 2017 Michael Lee(李登淳)
* Email: mikecovlee@163.com
* Github: https://github.com/mikecovlee
*
* Version: 17.2.0
*/
#include <utility>
namespace cov {
	template < typename T, typename X > struct is_same_type {
		static constexpr bool value=false;
	};
	template < typename T > struct is_same_type<T,T> {
		static constexpr bool value=true;
	};
	template < typename T > struct is_constant {
		static constexpr bool value=false;
	};
	template < typename T > struct is_constant<T const> {
		static constexpr bool value=true;
	};
	template < typename _Tp > struct add_reference {
		typedef _Tp& type;
	};
	template < typename _Tp > struct add_reference<_Tp&> {
		typedef _Tp& type;
	};
	template < typename _Tp > struct add_reference<_Tp&&> {
		typedef _Tp&& type;
	};
	template < typename _Tp > struct add_reference<_Tp*> {
		typedef _Tp* type;
	};
	template < typename _Tp > struct add_constant_reference {
		typedef const _Tp& type;
	};
	template < typename _Tp > struct add_constant_reference<_Tp&> {
		typedef const _Tp& type;
	};
	template < typename _Tp > struct add_constant_reference<const _Tp&> {
		typedef const _Tp& type;
	};
	template < typename _Tp > struct add_constant_reference<_Tp&&> {
		typedef const _Tp&& type;
	};
	template < typename _Tp > struct add_constant_reference<const _Tp&&> {
		typedef const _Tp&& type;
	};
	template < typename _Tp > struct add_constant_reference<_Tp*> {
		typedef _Tp* type;
	};
	template < typename _Tp > struct add_constant_reference<const _Tp*> {
		typedef const _Tp* type;
	};
	template<typename T>struct remove_reference {
		using type=T;
	};
	template<typename T>struct remove_reference<T&> {
		using type=T;
	};
	template<typename T>struct remove_reference<T&&> {
		using type=T;
	};
	template<typename T>struct remove_reference<const T&> {
		using type=const T;
	};
	template<typename T>struct remove_constant {
		using type=T;
	};
	template<typename T>struct remove_constant<const T> {
		using type=T;
	};
	template<bool factor,typename Tx,typename Ty>struct replace_if;
	template<typename Tx,typename Ty>struct replace_if<true,Tx,Ty> {
		using result=Ty;
	};
	template<typename Tx,typename Ty>struct replace_if<false,Tx,Ty> {
		using result=Tx;
	};
	template<typename From,typename To>
	class castable final {
		template<typename To1>static void test(To1);
		template<typename From1,typename To1>static constexpr bool helper(...)
		{
			return false;
		}
		template<typename From1,typename To1,typename=decltype(test<To1>(std::declval<From1>()))>
		static constexpr bool helper(int)
		{
			return true;
		}
	public:
		static constexpr bool value=helper<From,To>(0);
	};
}
