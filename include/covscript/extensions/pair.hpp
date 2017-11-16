#pragma once
/*
* Covariant Script Pair Extension
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
#include <covscript/cni.hpp>

static cs::extension pair_ext;
static cs::extension_t pair_ext_shared = cs::make_shared_extension(pair_ext);
namespace cs_impl {
	template<>
	cs::extension_t &get_ext<cs::pair>()
	{
		return pair_ext_shared;
	}
}
namespace pair_cs_ext {
	using namespace cs;

	var first(const pair &p)
	{
		return p.first;
	}

	var second(const pair &p)
	{
		return p.second;
	}

	void init()
	{
		pair_ext.add_var("first", var::make_protect<callable>(cni(first), true));
		pair_ext.add_var("second", var::make_protect<callable>(cni(second), true));
	}
}
