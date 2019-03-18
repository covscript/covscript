#pragma once
/*
* Cross-platform DLL library
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
#include <stdexcept>
#include <string>

#if defined(_WIN32) || defined(WIN32)

#include <windows.h>

namespace cov {
	class dll final {
		HMODULE m_handle = nullptr;
	public:
		dll() = default;

		dll(const dll &) = delete;

		explicit dll(const std::string &path)
		{
			open(path);
		}

		~dll()
		{
			if (m_handle != nullptr)
				::FreeLibrary(m_handle);
		}

		bool is_open() const noexcept
		{
			return m_handle != nullptr;
		}

		void open(const std::string &path)
		{
			if (m_handle != nullptr)
				::FreeLibrary(m_handle);
			m_handle = ::LoadLibrary(path.c_str());
			if (m_handle == nullptr) {
				static char szBuf[128];
				const char *args[] = {path.c_str()};
				::FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_ARGUMENT_ARRAY, nullptr, ::GetLastError(),
				                MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &szBuf, 128, (va_list *) args);
				throw std::logic_error(szBuf);
			}
		}

		void close()
		{
			if (m_handle != nullptr)
				::FreeLibrary(m_handle);
			m_handle = nullptr;
		}

		void *get_address(const std::string &method)
		{
			if (m_handle == nullptr)
				throw std::logic_error("Used an unopened file.");
			return reinterpret_cast<void *>(::GetProcAddress(m_handle, method.c_str()));
		}
	};
}
#else

#include <dlfcn.h>

namespace cov {
	class dll final {
		void *m_handle = nullptr;
	public:
		dll() = default;

		dll(const dll &) = delete;

		dll(const std::string &path)
		{
			open(path);
		}

		~dll()
		{
			if (m_handle != nullptr)
				::dlclose(m_handle);
		}

		bool is_open() const noexcept
		{
			return m_handle != nullptr;
		}

		void open(const std::string &path)
		{
			if (m_handle != nullptr)
				::dlclose(m_handle);
			m_handle = ::dlopen(path.c_str(), RTLD_NOW);
			if (m_handle == nullptr)
				throw std::logic_error(::dlerror());
		}

		void close()
		{
			if (m_handle != nullptr)
				::dlclose(m_handle);
			m_handle = nullptr;
		}

		void *get_address(const std::string &method)
		{
			if (m_handle == nullptr)
				throw std::logic_error("Used an unopened file.");
			return ::dlsym(m_handle, method.c_str());
		}
	};
}
#endif