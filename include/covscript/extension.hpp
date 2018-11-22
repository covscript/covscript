#pragma once
/*
* Covariant Script Extension
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
#define CS_EXTENSIONS_MINIMAL

#include <covscript/cni.hpp>
#include <covscript/extension.hpp>
#include <covscript/extensions/extensions.hpp>

void cs_extension_main(cs::name_space&);

extern "C"
{
	void __CS_EXTENSION_MAIN__(cs::name_space* ext, cs::process_context* context)
	{
		cs::current_process=context;
		cs_extension_main(*ext);
	}
}