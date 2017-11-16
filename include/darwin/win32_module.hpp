#pragma once
/*
* Covariant Darwin Universal Character Graphics Library
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
#include <csignal>
#include <cstdlib>
#include <windows.h>

#ifdef DARWIN_FORCE_BUILTIN

#include "./win32_adapter.cpp"

#endif
namespace darwin {
	class win32_module_adapter : public module_adapter {
		HINSTANCE m_handle = nullptr;
		platform_adapter *m_adapter = nullptr;

		static void force_exit(int);

		static void handle_segfault(int);

	public:
		win32_module_adapter() {}

		virtual ~win32_module_adapter() {}

		virtual status get_state() const override
		{
			if (m_adapter != nullptr)
				return status::ready;
			else
				return status::leisure;
		}

		virtual results load_module(const std::string &path) override
		{
			signal(SIGSEGV, handle_segfault);
			signal(SIGINT, force_exit);
			signal(SIGABRT, force_exit);
#ifdef DARWIN_FORCE_BUILTIN
			m_adapter = module_resource();
#else
			m_handle=LoadLibrary(path.c_str());
			if(m_handle==nullptr) return results::failure;
			module_enterance enterance=(module_enterance)GetProcAddress(m_handle,module_enterance_name);
			m_adapter=enterance();
#endif
			if (m_adapter == nullptr) return results::failure;
			return results::success;
		}

		virtual results free_module() override
		{
			signal(SIGSEGV, nullptr);
			signal(SIGINT, nullptr);
			signal(SIGABRT, nullptr);
#ifndef DARWIN_FORCE_BUILTIN
			FreeLibrary(m_handle);
			m_handle=nullptr;
#endif
			m_adapter = nullptr;
			return results::success;
		}

		virtual platform_adapter *get_platform_adapter() override
		{
			return this->m_adapter;
		}
	} dwin32module;

	static darwin_rt runtime(&dwin32module);
}

void darwin::win32_module_adapter::force_exit(int flag)
{
	printf("Darwin have been exited safety.\n");
	runtime.exit();
	std::exit(0);
}

void darwin::win32_module_adapter::handle_segfault(int flag)
{
	printf("Your program have some problem about the Segmentation Fault.Please check your program after we terminate this program.\n");
	printf("Darwin have been exited safety.\n");
	runtime.exit();
	std::exit(-1);
}
