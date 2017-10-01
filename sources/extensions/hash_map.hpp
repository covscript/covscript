#pragma once
/*
* Covariant Script Hash Map Extension
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
* Copyright (C) 2017 Michael Lee(李登淳)
* Email: mikecovlee@163.com
* Github: https://github.com/mikecovlee
*/
#include "../headers/cni.hpp"

static cs::extension hash_map_ext;
static cs::extension_t hash_map_ext_shared = std::make_shared<cs::extension_holder>(&hash_map_ext);
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
