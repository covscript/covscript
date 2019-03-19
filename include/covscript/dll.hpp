#pragma once
/*
* Covariant Script Extension DLL
*
* Licensed under the Covariant Innovation General Public License,
* Version 1.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* https://covariant.cn/licenses/LICENSE-1.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
* Copyright (C) 2019 Michael Lee(李登淳)
* Email: mikecovlee@163.com
* Github: https://github.com/mikecovlee
*/
#include <covscript/impl/extension.hpp>

void cs_extension_main(cs::name_space *);

extern "C"
{
	int __CS_ABI_COMPATIBLE__()
	{
		return COVSCRIPT_ABI_VERSION;
	}
	void __CS_EXTENSION_MAIN__(cs::name_space *ext, cs::process_context *context)
	{
		cs_impl::init_extensions();
		cs::current_process = context;
		cs_extension_main(ext);
	}
}