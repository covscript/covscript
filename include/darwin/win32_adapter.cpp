#include "./module.hpp"
#include "./graphics.hpp"
#include "./win32_conio.hpp"
#include <cstdio>
#include <string>
namespace darwin {
	class win32_adapter:public platform_adapter {
		bool mReady=false;
		picture mDrawable;
	public:
		win32_adapter()=default;
		virtual ~win32_adapter()=default;
		virtual status get_state() const noexcept override
		{
			if(mReady)
				return status::ready;
			else
				return status::leisure;
		}
		virtual results init() noexcept override
		{
			conio::set_title("Covariant Darwin UCGL");
			conio::reset();
			conio::clrscr();
			conio::echo(false);
			mReady = true;
			return results::success;
		}
		virtual results stop() noexcept override
		{
			conio::reset();
			//conio::clrscr();
			conio::echo(true);
			mReady = false;
			return results::success;
		}
		virtual results exec_commands(commands c) noexcept override
		{
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
		virtual bool is_kb_hit() noexcept
		{
			return conio::kbhit();
		}
		virtual int get_kb_hit() noexcept
		{
			return conio::getch();
		}
		virtual results fit_drawable() noexcept override
		{
			mDrawable.resize(conio::terminal_width(),conio::terminal_height());
			return results::success;
		}
		virtual drawable* get_drawable() noexcept override
		{
			return &mDrawable;
		}
		virtual results update_drawable() noexcept override
		{
			conio::gotoxy(0,0);
			int sw(conio::terminal_width()),sh(conio::terminal_height());
			conio::console buf(sw,sh);
			for(std::size_t y=0; y<mDrawable.get_height()&&y<sh; ++y) {
				for(std::size_t x=0; x<mDrawable.get_width()&&x<sw; ++x) {
					const pixel& pix=mDrawable.get_pixel(x,y);
					int fcolor,bcolor;
					switch(pix.get_front_color()) {
					case colors::white:
						fcolor=15;
						break;
					case colors::black:
						fcolor=0;
						break;
					case colors::red:
						fcolor=12;
						break;
					case colors::green:
						fcolor=10;
						break;
					case colors::blue:
						fcolor=9;
						break;
					case colors::pink:
						fcolor=13;
						break;
					case colors::yellow:
						fcolor=14;
						break;
					case colors::cyan:
						fcolor=11;
						break;
					default:
						return results::failure;
						break;
					}
					switch(pix.get_back_color()) {
					case colors::white:
						bcolor=15;
						break;
					case colors::black:
						bcolor=8;
						break;
					case colors::red:
						bcolor=12;
						break;
					case colors::green:
						bcolor=10;
						break;
					case colors::blue:
						bcolor=9;
						break;
					case colors::pink:
						bcolor=13;
						break;
					case colors::yellow:
						bcolor=14;
						break;
					case colors::cyan:
						bcolor=11;
						break;
					default:
						return results::failure;
						break;
					}
					buf.set_color(fcolor,bcolor);
					buf.put_char(pix.get_char());
				}
				buf.reset();
			}
			buf.flush();
			return results::success;
		}
	} dwin32adapter;
	platform_adapter* module_resource()
	{
		return &dwin32adapter;
	}
}