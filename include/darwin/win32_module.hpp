#pragma once
#include "./win32_adapter.cpp"
namespace darwin {
	class win32_module_adapter:public module_adapter {
		bool mReady=false;
	public:
		win32_module_adapter() {}
		virtual ~win32_module_adapter() {}
		virtual status get_state() const noexcept override {
			if(mReady)
				return status::ready;
			else
				return status::leisure;
		}
		virtual results load_module(const std::string&) noexcept override {
			mReady=true;
			dwin32adapter.init();
			return results::success;
		}
		virtual results free_module() noexcept override {
			mReady=false;
			dwin32adapter.stop();
			return results::success;
		}
		virtual platform_adapter* get_platform_adapter() noexcept override {
			return &dwin32adapter;
		}
	} dwin32module;
	static darwin_rt runtime(&dwin32module);
}