#pragma once
/*
* Covariant Mozart Utility Library: Type List
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
* Version: 17.1.0
*/
#include "./base.hpp"
#include "./traits.hpp"
#include <cstdio>

namespace cov {
	namespace type_list {
		struct tlist_final {
		};
		template<typename fT, typename sT>
		struct tlist_node {
		};
		template<typename...>
		struct make;
		template<typename T>
		struct make<T> {
			using result=tlist_node<T, tlist_final>;
		};
		template<typename T, typename...Args>
		struct make<T, Args...> {
			using result=tlist_node<T, typename make<Args...>::result>;
		};
		template<typename T, int size = 0>
		struct get_size;
		template<int size>
		struct get_size<tlist_final, size> {
			static constexpr int result = size;
		};
		template<typename fT, typename sT, int size>
		struct get_size<tlist_node<fT, sT>, size> {
			static constexpr int result = get_size<sT, size + 1>::result;
		};
		template<typename T>
		struct iterator;

		template<>
		struct iterator<tlist_final> {
			static void show() {}
		};

		template<typename fT, typename sT>
		struct iterator<tlist_node<fT, sT>> {
			static void show()
			{
				printf("%s\t", typeid(fT).name());
				iterator<sT>::show();
			}
		};

		template<int posit, typename T>
		struct get_type;
		template<int posit, typename fT, typename sT>
		struct get_type<posit, tlist_node<fT, sT>> {
			using result=typename get_type<posit - 1, sT>::result;
		};
		template<typename fT, typename sT>
		struct get_type<0, tlist_node<fT, sT>> {
			using result=fT;
		};
		template<int posit, typename T, typename Arg>
		struct change_type;
		template<int posit, typename fT, typename sT, typename Arg>
		struct change_type<posit, tlist_node<fT, sT>, Arg> {
			using result=tlist_node<fT, typename change_type<posit - 1, sT, Arg>::result>;
		};
		template<typename fT, typename sT, typename Arg>
		struct change_type<0, tlist_node<fT, sT>, Arg> {
			using result=tlist_node<Arg, sT>;
		};
		template<typename T, typename Arg>
		struct push_front {
			using result=tlist_node<Arg, T>;
		};
		template<typename T>
		struct pop_front;
		template<typename fT, typename sT>
		struct pop_front<tlist_node<fT, sT>> {
			using result=sT;
		};
		template<typename T, typename Arg>
		struct push_back;
		template<typename fT, typename sT, typename Arg>
		struct push_back<tlist_node<fT, sT>, Arg> {
			using result=tlist_node<fT, typename push_back<sT, Arg>::result>;
		};
		template<typename T, typename Arg>
		struct push_back<tlist_node<T, tlist_final>, Arg> {
			using result=tlist_node<T, tlist_node<Arg, tlist_final>>;
		};
		template<typename T>
		struct pop_back;
		template<typename fT, typename sT>
		struct pop_back<tlist_node<fT, sT>> {
			using result=tlist_node<fT, typename pop_back<sT>::result>;
		};
		template<typename T>
		struct pop_back<tlist_node<T, tlist_final>> {
			using result=tlist_final;
		};
		template<int posit, typename T>
		struct erase;
		template<int posit, typename fT, typename sT>
		struct erase<posit, tlist_node<fT, sT>> {
			using result=tlist_node<fT, typename erase<posit - 1, sT>::result>;
		};
		template<typename fT, typename sT>
		struct erase<0, tlist_node<fT, sT>> {
			using result=sT;
		};
		template<int posit, typename T, typename Arg>
		struct insert;
		template<int posit, typename fT, typename sT, typename Arg>
		struct insert<posit, tlist_node<fT, sT>, Arg> {
			using result=tlist_node<fT, typename insert<posit - 1, sT, Arg>::result>;
		};
		template<typename fT, typename sT, typename Arg>
		struct insert<0, tlist_node<fT, sT>, Arg> {
			using result=tlist_node<fT, tlist_node<Arg, sT>>;
		};
		template<typename T, template<typename> class Helper>
		struct remove_if;
		template<typename fT, typename sT, template<typename> class Helper>
		struct remove_if<tlist_node<fT, sT>, Helper> {
			using result=typename replace_if<Helper<fT>::value, tlist_node<fT, typename remove_if<sT, Helper>::result>, typename remove_if<sT, Helper>::result>::result;
		};
		template<template<typename> class Helper>
		struct remove_if<tlist_final, Helper> {
			using result=tlist_final;
		};

		template<typename T, typename Arg>
		class remove {
			template<typename X> using Helper=is_same_type<Arg, X>;
		public:
			using result=typename remove_if<T, Helper>::result;
		};
	}
}
