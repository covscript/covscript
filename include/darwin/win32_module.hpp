#pragma once
namespace darwin {
	class win32_module_adapter:public module_adapter {
	public:
		win32_module_adapter() {
			Darwin_Warning("Darwin has not yet supported the Microsoft Windows platform.")
		}
		virtual ~win32_module_adapter() {}
		virtual status get_state() const noexcept override {
			return status::busy;
		}
		virtual results load_module(const std::string&) noexcept override {
			return results::failure;
		}
		virtual results free_module() noexcept override {
			return results::failure;
		}
		virtual platform_adapter* get_platform_adapter() noexcept override {
			return nullptr;
		}
	} dwin32module;
	static darwin_rt runtime(&dwin32module);
}