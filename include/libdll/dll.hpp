#pragma once
#include <stdexcept>
#include <string>
#if defined(__WIN32__) || defined(WIN32)
#include <windows.h>
namespace cov {
	class dll final {
		HMODULE m_handle=nullptr;
	public:
		dll()=default;
		dll(const dll&)=delete;
		dll(const std::string& path):m_handle(::LoadLibrary(path.c_str())) {}
		~dll()
		{
			if(m_handle!=nullptr)
				::FreeLibrary(m_handle);
		}
		bool is_open() const noexcept
		{
			return m_handle!=nullptr;
		}
		void open(const std::string& path)
		{
			if(m_handle!=nullptr)
				::FreeLibrary(m_handle);
			m_handle=::LoadLibrary(path.c_str());
		}
		void close()
		{
			if(m_handle!=nullptr)
				::FreeLibrary(m_handle);
			m_handle=nullptr;
		}
		void* get_address(const std::string& method)
		{
			if(m_handle==nullptr)
				throw std::logic_error("Used an unopened file.");
			return reinterpret_cast<void*>(::GetProcAddress(m_handle,method.c_str()));
		}
	};
}
#else
#include <dlfcn.h>
namespace cov {
	class dll final {
		void* m_handle=nullptr;
	public:
		dll()=default;
		dll(const dll&)=delete;
		dll(const std::string& path):m_handle(::dlopen(path.c_str(),RTLD_LAZY)) {}
		~dll()
		{
			if(m_handle!=nullptr)
				::dlclose(m_handle);
		}
		bool is_open() const noexcept
		{
			return m_handle!=nullptr;
		}
		void open(const std::string& path)
		{
			if(m_handle!=nullptr)
				::dlclose(m_handle);
			m_handle=::dlopen(path.c_str(),RTLD_LAZY);
		}
		void close()
		{
			if(m_handle!=nullptr)
				::dlclose(m_handle);
			m_handle=nullptr;
		}
		void* get_address(const std::string& method)
		{
			if(m_handle==nullptr)
				throw std::logic_error("Used an unopened file.");
			return ::dlsym(m_handle,method.c_str());
		}
	};
}
#endif