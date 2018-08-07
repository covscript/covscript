#pragma once
/*
* Covariant Script Pair Extension
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

static cs::extension pair_ext;
static cs::extension_t pair_ext_shared = cs::make_shared_namespace(pair_ext);
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
