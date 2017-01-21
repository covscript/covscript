#pragma once
#include "./core.hpp"
#include <functional>
namespace darwin {
	class platform_adapter {
	public:
		platform_adapter()=default;
		platform_adapter(const platform_adapter&)=delete;
		virtual ~platform_adapter()=default;
		virtual status get_state() const noexcept=0;
		virtual results init() noexcept=0;
		virtual results stop() noexcept=0;
		virtual results exec_commands(commands) noexcept=0;
		virtual bool is_kb_hit() noexcept=0;
		virtual int get_kb_hit() noexcept=0;
		virtual results fit_drawable() noexcept=0;
		virtual drawable* get_drawable() noexcept=0;
		virtual results update_drawable() noexcept=0;
	};
	class module_adapter {
	public:
		module_adapter()=default;
		module_adapter(const module_adapter&)=delete;
		virtual ~module_adapter()=default;
		virtual status get_state() const noexcept=0;
		virtual results load_module(const std::string&) noexcept=0;
		virtual results free_module() noexcept=0;
		virtual platform_adapter* get_platform_adapter() noexcept=0;
	};
	typedef platform_adapter*(*module_enterance)();
	const char* module_enterance_name="COV_DARWIN_MODULE_MAIN";
}