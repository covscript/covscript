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
#include <covscript/core.hpp>
#include <covscript/extensions/exception.hpp>
#include <covscript/extensions/char.hpp>
#include <covscript/extensions/string.hpp>
#include <covscript/extensions/list.hpp>
#include <covscript/extensions/array.hpp>
#include <covscript/extensions/pair.hpp>
#include <covscript/extensions/hash_map.hpp>

cs::extension *cs_extension();

extern "C"
{
	cs::extension *__CS_EXTENSION__(int *ref, cs::cs_exception_handler ceh, cs::std_exception_handler seh)
	{
		cs::output_precision_ref = ref;
		cs::exception_handler::cs_eh_callback = ceh;
		cs::exception_handler::std_eh_callback = seh;
		except_cs_ext::init();
		char_cs_ext::init();
		string_cs_ext::init();
		list_cs_ext::init();
		array_cs_ext::init();
		pair_cs_ext::init();
		hash_map_cs_ext::init();
		return cs_extension();
	}
}
