#pragma once
/*
* Covariant Script Exception Extension
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

static cs::extension except_ext;
static cs::extension_t except_ext_shared = cs::make_shared_namespace(except_ext);
namespace cs_impl {
	template<>
	cs::extension_t &get_ext<cs::lang_error>()
	{
		return except_ext_shared;
	}
}
namespace except_cs_ext {
	using namespace cs;

	string what(const lang_error &le)
	{
		return le.what();
	}

	void init()
	{
		except_ext.add_var("what", make_cni(what));
	}
}
