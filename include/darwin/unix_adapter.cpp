#include "./module.hpp"
#include "./graphics.hpp"
#include "./unix_conio.hpp"
#include <functional>
#include <thread>
#include <atomic>
#include <cstdio>
#include <string>
namespace darwin {
	class unix_adapter:public platform_adapter {
		bool mReady=false;
		picture mDrawable;
	public:
		unix_adapter()=default;
		virtual ~unix_adapter()=default;
		virtual status get_state() const noexcept override {
			if(mReady)
				return status::ready;
			else
				return status::leisure;
		}
		virtual results init() noexcept override {
			conio::reset();
			conio::clrscr();
			conio::echo(false);
			mReady = true;
			return results::success;
		}
		virtual results stop() noexcept override {
			conio::reset();
			conio::clrscr();
			conio::echo(true);
			mReady = false;
			return results::success;
		}
		virtual results exec_commands(commands c) noexcept override {
			switch(c) {
			case commands::echo_on:
				conio::echo(true);
				break;
			case commands::echo_off:
				conio::echo(false);
				break;
			case commands::reset_cursor:
				conio::gotoxy(0,0);
				break;
			case commands::reset_attri:
				conio::reset();
				break;
			case commands::clrscr:
				conio::clrscr();
				break;
			}
			return results::success;
		}
		virtual bool is_kb_hit() noexcept {
			return conio::kbhit();
		}
		virtual int get_kb_hit() noexcept {
			return conio::getch();
		}
		virtual results fit_drawable() noexcept override {
			mDrawable.resize(conio::terminal_width(),conio::terminal_height());
			return results::success;
		}
		virtual drawable* get_drawable() noexcept override {
			return &mDrawable;
		}
		virtual results update_drawable() noexcept override {
			conio::gotoxy(0,0);
			int sw(conio::terminal_width()),sh(conio::terminal_height());
			for(std::size_t y=0; y<mDrawable.get_height()&&y<sh; ++y) {
				for(std::size_t x=0; x<mDrawable.get_width()&&x<sw; ++x) {
					const pixel& pix=mDrawable.get_pixel(x,y);
					std::string cmd="\e[0m\e[";
					switch(pix.get_front_color()) {
					case colors::white:
						cmd+="37;";
						break;
					case colors::black:
						cmd+="30;";
						break;
					case colors::red:
						cmd+="31;";
						break;
					case colors::green:
						cmd+="32;";
						break;
					case colors::blue:
						cmd+="34;";
						break;
					case colors::pink:
						cmd+="35;";
						break;
					case colors::yellow:
						cmd+="33;";
						break;
					case colors::cyan:
						cmd+="36;";
						break;
					default:
						return results::failure;
						break;
					}
					switch(pix.get_back_color()) {
					case colors::white:
						cmd+="47";
						break;
					case colors::black:
						cmd+="40";
						break;
					case colors::red:
						cmd+="41";
						break;
					case colors::green:
						cmd+="42";
						break;
					case colors::blue:
						cmd+="44";
						break;
					case colors::pink:
						cmd+="45";
						break;
					case colors::yellow:
						cmd+="43";
						break;
					case colors::cyan:
						cmd+="46";
						break;
					default:
						return results::failure;
						break;
					}
					if(pix.is_bright())
						cmd+=";1";
					if(pix.is_underline())
						cmd+=";4";
					cmd+="m";
					printf("%s%c",cmd.c_str(),pix.get_char());
				}
				conio::reset();
				printf("\n");
			}
			fflush(stdout);
			return results::success;
		}
	} dunixadapter;
	platform_adapter* module_resource()
	{
		return &dunixadapter;
	}
}