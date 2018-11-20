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

static cs::namespace array_ext;
static cs::namespace_t array_ext_shared = cs::make_shared_namespace(array_ext);
namespace cs_impl {
	template<>
	cs::namespace_t &get_ext<cs::array>()
	{
		return array_ext_shared;
	}
}
static cs::namespace array_iterator_ext;
static cs::namespace_t array_iterator_ext_shared = cs::make_shared_namespace(array_iterator_ext);
namespace cs_impl {
	template<>
	cs::namespace_t &get_ext<cs::array::iterator>()
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

	array::iterator insert(array &arr, array::iterator &pos,
	                       const var &val
	                      )
	{
		return arr.
		       insert(pos, copy(val)
		             );
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
				const auto &p = it.const_val<pair>();
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
		array_iterator_ext
		.add_var("forward", make_cni(forward, true))
		.add_var("backward", make_cni(backward, true))
		.add_var("data", make_cni(data, true));
		array_ext
		.add_var("iterator", make_namespace(array_iterator_ext_shared))
		.add_var("at", make_cni(at, true))
		.add_var("front", make_cni(front, true))
		.add_var("back", make_cni(back, true))
		.add_var("begin", make_cni(begin, true))
		.add_var("term", make_cni(term, true))
		.add_var("empty", make_cni(empty, true))
		.add_var("size", make_cni(size, true))
		.add_var("clear", make_cni(clear, true))
		.add_var("insert", make_cni(insert, true))
		.add_var("erase", make_cni(erase, true))
		.add_var("push_front", make_cni(push_front, true))
		.add_var("pop_front", make_cni(pop_front, true))
		.add_var("push_back", make_cni(push_back, true))
		.add_var("pop_back", make_cni(pop_back, true))
		.add_var("to_hash_map", make_cni(to_hash_map, true))
		.add_var("to_list", make_cni(to_list, true));
	}

}
