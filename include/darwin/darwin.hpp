#pragma once
#include "./timer.hpp"
#include "./core.hpp"
#include "./adapter.hpp"
#include "./graphics.hpp"
#include <string>
#include <cstdlib>
namespace darwin {
	class sync_clock final {
	private:
		timer_t mBegin;
		std::size_t mFreq;
	public:
		sync_clock():mBegin(timer::time()),mFreq(60) {}
		sync_clock(std::size_t freq):mBegin(timer::time()),mFreq(freq) {}
		~sync_clock()=default;
		std::size_t get_freq() const {
			return mFreq;
		}
		void set_freq(std::size_t freq) {
			mFreq=freq;
		}
		void reset() {
			mBegin=timer::time();
		}
		void sync() {
			timer_t spend=timer::time()-mBegin;
			timer_t period=1000/mFreq;
			if(period>spend)
				timer::delay(period-spend);
		}
	};
	class darwin_rt final {
	protected:
		timer_t m_time_out=1000;
		module_adapter* m_module=nullptr;
		platform_adapter* m_platform=nullptr;
		bool wait_for_module();
		bool wait_for_platform();
	public:
		darwin_rt()=delete;
		darwin_rt(module_adapter* module):m_module(module) {}
		darwin_rt(const darwin_rt&)=delete;
		darwin_rt(darwin_rt&&) noexcept=delete;
		~darwin_rt();
		void load(const std::string&);
		void exit(int);
		status get_state() const noexcept {
			if(m_module==nullptr) return status::error;
			if(m_platform==nullptr) return status::leisure;
			if(m_module->get_state()==status::busy || m_platform->get_state()==status::busy) return status::busy;
			if(m_module->get_state()==status::ready&&m_platform->get_state()==status::ready) return status::ready;
			return status::null;
		}
		void set_time_out(timer_t tl) noexcept {
			m_time_out=tl;
		}
		platform_adapter* get_adapter() noexcept {
			return m_platform;
		}
		drawable* get_drawable() noexcept {
			if(m_platform==nullptr) return nullptr;
			return m_platform->get_drawable();
		}
		results fit_drawable() noexcept {
			if(m_platform==nullptr) return results::failure;
			return m_platform->fit_drawable();
		}
		results update_drawable() noexcept {
			if(m_platform==nullptr) return results::failure;
			return m_platform->update_drawable();
		}
	};
}
bool darwin::darwin_rt::wait_for_module()
{
	if(m_module==nullptr) return false;
	for(timer_t nt=timer::time(); m_module->get_state()==status::busy&&timer::time()-nt<=m_time_out;);
	if(m_module->get_state()==status::busy) return false;
	else return true;
}
bool darwin::darwin_rt::wait_for_platform()
{
	if(m_platform==nullptr) return false;
	for(timer_t nt=timer::time(); m_platform->get_state()==status::busy&&timer::time()-nt<=m_time_out;);
	if(m_platform->get_state()==status::busy) return false;
	else return true;
}
darwin::darwin_rt::~darwin_rt()
{
	if(m_platform!=nullptr)
		if(wait_for_platform()&&m_platform->get_state()==status::ready)
			m_platform->stop();
	if(m_module!=nullptr)
		if(wait_for_module()&&m_module->get_state()==status::ready)
			m_module->free_module();
}
void darwin::darwin_rt::load(const std::string& file)
{
	if(get_state()!=status::leisure) Darwin_Error("Adapter Busy.");
	if(wait_for_module()&&m_module->get_state()==status::leisure) {
		if(m_module->load_module(file)==results::failure) Darwin_Error("Adapter returns failure.");
	} else
		Darwin_Error("Adapter Busy.");
	m_platform=m_module->get_platform_adapter();
	m_platform->init();
}
void darwin::darwin_rt::exit(int code=0)
{
	if(m_platform!=nullptr)
		if(wait_for_platform()&&m_platform->get_state()==status::ready)
			m_platform->stop();
	if(m_module!=nullptr)
		if(wait_for_module()&&m_module->get_state()==status::ready)
			m_module->free_module();
	m_platform=nullptr;
	m_module=nullptr;
	std::exit(code);
}
#if defined(DARWIN_FORCE_UNIX)
#include "./unix_module.hpp"
#else
#if defined(__WIN32__) || defined(WIN32)
#include "./win32_module.hpp"
#else
#include "./unix_module.hpp"
#endif
#endif
namespace darwin {
	static std::string screen_shot_path="./darwin_screen_shot.cdpf";
	void print_screen()
	{
		static std::deque<char> file_buffer;
		serial_picture(runtime.get_drawable(),file_buffer);
		outfs out(screen_shot_path);
		for(auto&it:file_buffer)
			out.printf("%c",it);
		out.flush();
	}
}