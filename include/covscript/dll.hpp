#pragma once
/*
 * Covariant Script Extension DLL
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Copyright (C) 2017-2026 Michael Lee(李登淳)
 *
 * This software is registered with the National Copyright Administration
 * of the People's Republic of China(Registration Number: 2020SR0408026)
 * and is protected by the Copyright Law of the People's Republic of China.
 *
 * Email:   mikecovlee@163.com
 * Github:  https://github.com/mikecovlee
 * Website: http://covscript.org.cn
 */
#include <covscript/impl/type_ext.hpp>

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
