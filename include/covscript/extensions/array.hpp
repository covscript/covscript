#pragma once
/*
* Covariant Script Array Extension
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

static cs::extension array_ext;
static cs::extension_t array_ext_shared = cs::make_shared_extension(array_ext);
namespace cs_impl {
	template<>
	cs::extension_t &get_ext<cs::array>()
	{
		return array_ext_shared;
	}
}
static cs::extension array_iterator_ext;
static cs::extension_t array_iterator_ext_shared = cs::make_shared_extension(array_iterator_ext);
namespace cs_impl {
	template<>
	cs::extension_t &get_ext<cs::array::iterator>()
	{
		return array_iterator_ext_shared;
	}

	template<>
	constexpr const char *get_name_of_type<cs::array::iterator>()
	{
		return "cs::array::iterator";
	}
}
namespace array_cs_ext {
	using namespace cs;

// Element access
	var at(const array &arr, number posit)
	{
		return arr.at(posit);
	}

	var front(const array &arr)
	{
		return arr.front();
	}

	var back(const array &arr)
	{
		return arr.back();
	}

// Iterators
	array::iterator begin(array &arr)
	{
		return arr.begin();
	}

	array::iterator term(array &arr)
	{
		return arr.end();
	}

	array::iterator forward(array::iterator &it)
	{
		return ++it;
	}

	array::iterator backward(array::iterator &it)
	{
		return --it;
	}

	var data(array::iterator &it)
	{
		return *it;
	}

// Capacity
	bool empty(const array &arr)
	{
		return arr.empty();
	}

	number size(const array &arr)
	{
		return arr.size();
	}

// Modifiers
	void clear(array &arr)
	{
		arr.clear();
	}

	array::iterator insert(array &arr, array::iterator &pos, const var &val)
	{
		return arr.insert(pos, copy(val));
	}

	array::iterator erase(array &arr, array::iterator &pos)
	{
		return arr.erase(pos);
	}

	void push_front(array &arr, const var &val)
	{
		arr.push_front(copy(val));
	}

	void pop_front(array &arr)
	{
		arr.pop_front();
	}

	void push_back(array &arr, const var &val)
	{
		arr.push_back(copy(val));
	}

	void pop_back(array &arr)
	{
		arr.pop_back();
	}

// Operations
	var to_hash_map(const array &arr)
	{
		hash_map map;
		for (auto &it:arr) {
			if (it.type() == typeid(pair)) {
				const pair &p = it.const_val<pair>();
				map[p.first] = copy(p.second);
			}
			else
				throw lang_error("Wrong syntax for hash map.");
		}
		return var::make<hash_map>(std::move(map));
	}

	var to_list(const array &arr)
	{
		var lst = var::make<list>(arr.begin(), arr.end());
		lst.detach();
		return std::move(lst);
	}

	void init()
	{
		array_ext.add_var("iterator", var::make_protect<extension_t>(array_iterator_ext_shared));
		array_ext.add_var("at", var::make_protect<callable>(cni(at), true));
		array_ext.add_var("front", var::make_protect<callable>(cni(front), true));
		array_ext.add_var("back", var::make_protect<callable>(cni(back), true));
		array_ext.add_var("begin", var::make_protect<callable>(cni(begin), true));
		array_ext.add_var("term", var::make_protect<callable>(cni(term), true));
		array_iterator_ext.add_var("forward", var::make_protect<callable>(cni(forward), true));
		array_iterator_ext.add_var("backward", var::make_protect<callable>(cni(backward), true));
		array_iterator_ext.add_var("data", var::make_protect<callable>(cni(data), true));
		array_ext.add_var("empty", var::make_protect<callable>(cni(empty), true));
		array_ext.add_var("size", var::make_protect<callable>(cni(size), true));
		array_ext.add_var("clear", var::make_protect<callable>(cni(clear), true));
		array_ext.add_var("insert", var::make_protect<callable>(cni(insert), true));
		array_ext.add_var("erase", var::make_protect<callable>(cni(erase), true));
		array_ext.add_var("push_front", var::make_protect<callable>(cni(push_front), true));
		array_ext.add_var("pop_front", var::make_protect<callable>(cni(pop_front), true));
		array_ext.add_var("push_back", var::make_protect<callable>(cni(push_back), true));
		array_ext.add_var("pop_back", var::make_protect<callable>(cni(pop_back), true));
		array_ext.add_var("to_hash_map", var::make_protect<callable>(cni(to_hash_map), true));
		array_ext.add_var("to_list", var::make_protect<callable>(cni(to_list), true));
	}
}
