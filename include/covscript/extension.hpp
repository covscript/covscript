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
#include <covscript/core.hpp>

namespace cs_impl {
	namespace dll_resources {
		const char* dll_main_entrance = "__CS_EXTENSION_MAIN__";
		typedef void(*dll_main_entrance_t)(cs::extension*, cs::process_context*);
		const char* dll_compatible_entrance = "__CS_EXTENSION__";
		typedef cs::extension*(*dll_compatible_entrance_t)(int *, cs::cs_exception_handler, cs::std_exception_handler);
	}
	class dll_manager final {
		cov::dll m_dll;
		bool m_compatible=false;
		cs::name_space* m_ns=nullptr;
	public:
		dll_manager()=delete;
		dll_manager(const dll_manager&)=delete;
		explicit dll_manager(const std::string& path):m_dll(path)
		{
			using namespace dll_resources;
			dll_main_entrance_t dll_main=reinterpret_cast<dll_main_entrance_t>(m_dll.get_address(dll_main_entrance));
			if(dll_main!=nullptr)
			{
				m_ns=new cs::name_space;
				dll_main(m_ns, cs::current_process);
			}else{
				dll_compatible_entrance_t dll_compatible=reinterpret_cast<dll_compatible_entrance_t>(m_dll.get_address(dll_compatible_entrance));
				if(dll_compatible!=nullptr)
				{
					m_compatible=true;
					m_ns=dll_compatible(&cs::current_process->output_precision, cs::current_process->cs_eh_callback, cs::current_process->std_eh_callback);
				}else
					throw cs::lang_error("Incompatible DLL.");
			}
		}
		~dll_manager()
		{
			if(!m_compatible)
				delete m_ns;
		}
	};
}