#pragma once
/*
* Covariant Script Hash Map Extension
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

static cs::extension hash_map_ext;
static cs::extension_t hash_map_ext_shared = cs::make_shared_extension(hash_map_ext);
namespace cs_impl {
	template<>
	cs::extension_t &get_ext<cs::hash_map>()
	{
		return hash_map_ext_shared;
	}
}
namespace hash_map_cs_ext {
	using namespace cs;

// Capacity
	bool empty(const hash_map &map)
	{
		return map.empty();
	}

	number size(const hash_map &map)
	{
		return map.size();
	}

// Modifiers
	void clear(hash_map &map)
	{
		map.clear();
	}

	void insert(hash_map &map, const var &key, const var &val)
	{
		if (map.count(key) > 0)
			map.at(key).swap(copy(val), true);
		else
			map.emplace(copy(key), copy(val));
	}

	void erase(hash_map &map, const var &key)
	{
		map.erase(key);
	}

// Lookup
	var at(hash_map &map, const var &key)
	{
		return map.at(key);
	}

	bool exist(hash_map &map, const var &key)
	{
		return map.count(key) > 0;
	}

	void init()
	{
		hash_map_ext.add_var("empty", var::make_protect<callable>(cni(empty), true));
		hash_map_ext.add_var("size", var::make_protect<callable>(cni(size), true));
		hash_map_ext.add_var("clear", var::make_protect<callable>(cni(clear), true));
		hash_map_ext.add_var("insert", var::make_protect<callable>(cni(insert), true));
		hash_map_ext.add_var("erase", var::make_protect<callable>(cni(erase), true));
		hash_map_ext.add_var("at", var::make_protect<callable>(cni(at), true));
		hash_map_ext.add_var("exist", var::make_protect<callable>(cni(exist), true));
	}
}
