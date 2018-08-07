#pragma once
/*
* Covariant Script List Extension
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

static cs::extension list_ext;
static cs::extension_t list_ext_shared = cs::make_shared_namespace(list_ext);
namespace cs_impl {
	template<>
	cs::extension_t &get_ext<cs::list>()
	{
		return list_ext_shared;
	}
}
static cs::extension list_iterator_ext;
static cs::extension_t list_iterator_ext_shared = cs::make_shared_namespace(list_iterator_ext);
namespace cs_impl {
	template<>
	cs::extension_t &get_ext<cs::list::iterator>()
	{
		return list_iterator_ext_shared;
	}

	template<>
	constexpr const char *get_name_of_type<cs::list::iterator>()
	{
		return "cs::list::iterator";
	}
}
namespace list_cs_ext {
	using namespace cs;

// Element access
	var front(const list &lst)
	{
		return lst.front();
	}

	var back(const list &lst)
	{
		return lst.back();
	}

// Iterators
	list::iterator begin(list &lst)
	{
		return lst.begin();
	}

	list::iterator term(list &lst)
	{
		return lst.end();
	}

	list::iterator forward(list::iterator &it)
	{
		return ++it;
	}

	list::iterator backward(list::iterator &it)
	{
		return --it;
	}

	var data(list::iterator &it)
	{
		return *it;
	}

// Capacity
	bool empty(const list &lst)
	{
		return lst.empty();
	}

	number size(const list &lst)
	{
		return lst.size();
	}

// Modifiers
	void clear(list &lst)
	{
		lst.clear();
	}

	list::iterator insert(list &lst, list::iterator &pos, const var &val)
	{
		return lst.insert(pos, copy(val));
	}

	list::iterator erase(list &lst, list::iterator &pos)
	{
		return lst.erase(pos);
	}

	void push_front(list &lst, const var &val)
	{
		lst.push_front(copy(val));
	}

	void pop_front(list &lst)
	{
		lst.pop_front();
	}

	void push_back(list &lst, const var &val)
	{
		lst.push_back(copy(val));
	}

	void pop_back(list &lst)
	{
		lst.pop_back();
	}

// Operations
	void remove(list &lst, const var &val)
	{
		lst.remove(val);
	}

	void reverse(list &lst)
	{
		lst.reverse();
	}

	void unique(list &lst)
	{
		lst.unique();
	}

	void init()
	{
		list_iterator_ext
		.add_var("forward", make_cni(forward, true))
		.add_var("backward", make_cni(backward, true))
		.add_var("data", make_cni(data, true));
		list_ext
		.add_var("iterator", make_namespace(list_iterator_ext_shared))
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
		.add_var("remove", make_cni(remove, true))
		.add_var("reverse", make_cni(reverse, true))
		.add_var("unique", make_cni(unique, true));
	}
}
