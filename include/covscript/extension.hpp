#pragma once
/*
* Covariant Script Extension Header
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
#include <covscript/extensions/extensions.hpp>

cs::extension *cs_extension();

extern "C"
{
	cs::extension *
	__CS_EXTENSION__(int *ref, cs::map_t<std::type_index, std::size_t> *type_data, cs::cs_exception_handler ceh,
	                 cs::std_exception_handler seh)
	{
		cs::output_precision_ref = ref;
		cs_impl::type_id::sync_type_data(type_data);
		cs::exception_handler::cs_eh_callback = ceh;
		cs::exception_handler::std_eh_callback = seh;
		cs::init_extensions();
		return cs_extension();
	}
}
