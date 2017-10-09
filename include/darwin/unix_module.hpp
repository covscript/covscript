#pragma once

#include <csignal>
#include <cstdlib>
#include <dlfcn.h>

#ifdef DARWIN_FORCE_BUILTIN
#include "./unix_adapter.cpp"
#endif
namespace darwin {
	class unix_module_adapter : public module_adapter {
		void *m_handle = nullptr;
		platform_adapter *m_adapter = nullptr;

		static void force_exit(int);

		static void handle_segfault(int);

	public:
		unix_module_adapter() = default;

		virtual ~unix_module_adapter() = default;

		virtual status get_state() const noexcept override
		{
			if (m_adapter != nullptr)
				return status::ready;
			else
				return status::leisure;
		}

		virtual results load_module(const std::string &path) noexcept override
		{
			signal(SIGSEGV, handle_segfault);
			signal(SIGINT, force_exit);
			signal(SIGABRT, force_exit);
#ifdef DARWIN_FORCE_BUILTIN
			m_adapter=module_resource();
#else
			m_handle = dlopen(path.c_str(), RTLD_LAZY);
			if (m_handle == nullptr) return results::failure;
			module_enterance enterance = (module_enterance) dlsym(m_handle, module_enterance_name);
			m_adapter = enterance();
#endif
			if (m_adapter == nullptr) return results::failure;
			return results::success;
		}

		virtual results free_module() noexcept override
		{
			signal(SIGSEGV, nullptr);
			signal(SIGINT, nullptr);
			signal(SIGABRT, nullptr);
#ifndef DARWIN_FORCE_BUILTIN
			dlclose(m_handle);
			m_handle = nullptr;
#endif
			m_adapter = nullptr;
			return results::success;
		}

		virtual platform_adapter *get_platform_adapter() noexcept override
		{
			return this->m_adapter;
		}
	} dunixmodule;

	static darwin_rt runtime(&dunixmodule);
}

void darwin::unix_module_adapter::force_exit(int flag)
{
	printf("Darwin have been exited safety.\n");
	runtime.exit();
	std::exit(0);
}

void darwin::unix_module_adapter::handle_segfault(int flag)
{
	printf("Your program have some problem about the Segmentation Fault.Please check your program after we terminate this program.\n");
	printf("Darwin have been exited safety.\n");
	runtime.exit();
	std::exit(-1);
}
