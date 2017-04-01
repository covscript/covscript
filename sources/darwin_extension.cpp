#define DARWIN_FORCE_BUILTIN
#include "../include/darwin/darwin.hpp"
#include "../include/mozart/switcher.hpp"
#ifndef CBS_DARWIN_EXT
#include "./extension.hpp"
#endif
static cov_basic::extension darwin_ext;
#define add_function_ext(name) darwin_ext.add_var(#name,cov_basic::native_interface(name));
namespace darwin_cbs_ext {
	using namespace cov_basic;
// Graphics
	darwin::sync_clock clock(30);
	cov::any init_graphics(array& args)
	{
		if(args.empty())
			darwin::runtime.load("./darwin.module");
		else
			darwin::runtime.load(args.at(0).const_val<std::string>());
		return number(0);
	}
	cov::any fit_drawable(array& args)
	{
		darwin::runtime.fit_drawable();
		return number(0);
	}
	cov::any set_frame_limit(array& args)
	{
		clock.set_freq(args.at(0).const_val<number>());
		return number(0);
	}
	cov::any update_drawable(array& args)
	{
		darwin::runtime.update_drawable();
		clock.sync();
		clock.reset();
		return number(0);
	}
	cov::any clear_drawable(array& args)
	{
		if(args.empty())
			darwin::runtime.get_drawable()->clear();
		else
			args.at(0).val<darwin::picture>(true).clear();
		return number(0);
	}
	cov::any fill_drawable(array& args)
	{
		if(args.size()==1&&args.at(0).type()==typeid(darwin::pixel))
			darwin::runtime.get_drawable()->fill(args.at(0).const_val<darwin::pixel>());
		else if(args.size()==2&&args.at(0).type()==typeid(darwin::picture)&&args.at(1).type()==typeid(darwin::pixel))
			args.at(0).val<darwin::picture>(true).fill(args.at(1).const_val<darwin::pixel>());
		else
			throw syntax_error("Arguments error.");
		return number(0);
	}
	cov::any resize_drawable(array& args)
	{
		if(args.at(0).type()==typeid(number))
			darwin::runtime.get_drawable()->resize(args.at(0).const_val<number>(),args.at(1).const_val<number>());
		else if(args.at(0).type()==typeid(darwin::picture))
			args.at(0).val<darwin::picture>(true).resize(args.at(1).const_val<number>(),args.at(2).const_val<number>());
		else
			throw syntax_error("Arguments error.");
		return number(0);
	}
	cov::any get_height_drawable(array& args)
	{
		if(args.empty())
			return number(darwin::runtime.get_drawable()->get_height());
		else
			return number(args.at(0).const_val<darwin::picture>().get_height());
	}
	cov::any get_width_drawable(array& args)
	{
		if(args.empty())
			return number(darwin::runtime.get_drawable()->get_width());
		else
			return number(args.at(0).const_val<darwin::picture>().get_width());
	}
	cov::any draw_point(array& args)
	{
		if(args.at(0).type()==typeid(number))
			darwin::runtime.get_drawable()->draw_pixel(args.at(0).const_val<number>(),args.at(1).const_val<number>(),args.at(2).const_val<darwin::pixel>());
		else if(args.at(0).type()==typeid(darwin::picture))
			args.at(0).val<darwin::picture>(true).draw_pixel(args.at(1).const_val<number>(),args.at(2).const_val<number>(),args.at(3).const_val<darwin::pixel>());
		else
			throw syntax_error("Arguments error.");
		return number(0);
	}
	cov::any draw_picture(array& args)
	{
		if(args.at(0).type()==typeid(number))
			darwin::runtime.get_drawable()->draw_picture(args.at(0).const_val<number>(),args.at(1).const_val<number>(),args.at(2).const_val<darwin::picture>());
		else if(args.at(0).type()==typeid(darwin::picture))
			args.at(0).val<darwin::picture>(true).draw_picture(args.at(1).const_val<number>(),args.at(2).const_val<number>(),args.at(3).const_val<darwin::picture>());
		else
			throw syntax_error("Arguments error.");
		return number(0);
	}
	cov::any draw_line(array& args)
	{
		if(args.at(0).type()==typeid(number))
			darwin::runtime.get_drawable()->draw_line(args.at(0).const_val<number>(),args.at(1).const_val<number>(),args.at(2).const_val<number>(),args.at(3).const_val<number>(),args.at(4).const_val<darwin::pixel>());
		else if(args.at(0).type()==typeid(darwin::picture))
			args.at(0).val<darwin::picture>(true).draw_line(args.at(1).const_val<number>(),args.at(2).const_val<number>(),args.at(3).const_val<number>(),args.at(4).const_val<number>(),args.at(5).const_val<darwin::pixel>());
		else
			throw syntax_error("Arguments error.");
		return number(0);
	}
	cov::any draw_rect(array& args)
	{
		if(args.at(0).type()==typeid(number))
			darwin::runtime.get_drawable()->draw_rect(args.at(0).const_val<number>(),args.at(1).const_val<number>(),args.at(2).const_val<number>(),args.at(3).const_val<number>(),args.at(4).const_val<darwin::pixel>());
		else if(args.at(0).type()==typeid(darwin::picture))
			args.at(0).val<darwin::picture>(true).draw_rect(args.at(1).const_val<number>(),args.at(2).const_val<number>(),args.at(3).const_val<number>(),args.at(4).const_val<number>(),args.at(5).const_val<darwin::pixel>());
		else
			throw syntax_error("Arguments error.");
		return number(0);
	}
	cov::any fill_rect(array& args)
	{
		if(args.at(0).type()==typeid(number))
			darwin::runtime.get_drawable()->fill_rect(args.at(0).const_val<number>(),args.at(1).const_val<number>(),args.at(2).const_val<number>(),args.at(3).const_val<number>(),args.at(4).const_val<darwin::pixel>());
		else if(args.at(0).type()==typeid(darwin::picture))
			args.at(0).val<darwin::picture>(true).fill_rect(args.at(1).const_val<number>(),args.at(2).const_val<number>(),args.at(3).const_val<number>(),args.at(4).const_val<number>(),args.at(5).const_val<darwin::pixel>());
		else
			throw syntax_error("Arguments error.");
		return number(0);
	}
	cov::any draw_triangle(array& args)
	{
		if(args.at(0).type()==typeid(number))
			darwin::runtime.get_drawable()->draw_triangle(args.at(0).const_val<number>(),args.at(1).const_val<number>(),args.at(2).const_val<number>(),args.at(3).const_val<number>(),args.at(4).const_val<number>(),args.at(5).const_val<number>(),args.at(6).const_val<darwin::pixel>());
		else if(args.at(0).type()==typeid(darwin::picture))
			args.at(0).val<darwin::picture>(true).draw_triangle(args.at(1).const_val<number>(),args.at(2).const_val<number>(),args.at(3).const_val<number>(),args.at(4).const_val<number>(),args.at(5).const_val<number>(),args.at(6).const_val<number>(),args.at(7).const_val<darwin::pixel>());
		else
			throw syntax_error("Arguments error.");
		return number(0);
	}
	cov::any fill_triangle(array& args)
	{
		if(args.at(0).type()==typeid(number))
			darwin::runtime.get_drawable()->fill_triangle(args.at(0).const_val<number>(),args.at(1).const_val<number>(),args.at(2).const_val<number>(),args.at(3).const_val<number>(),args.at(4).const_val<number>(),args.at(5).const_val<number>(),args.at(6).const_val<darwin::pixel>());
		else if(args.at(0).type()==typeid(darwin::picture))
			args.at(0).val<darwin::picture>(true).fill_triangle(args.at(1).const_val<number>(),args.at(2).const_val<number>(),args.at(3).const_val<number>(),args.at(4).const_val<number>(),args.at(5).const_val<number>(),args.at(6).const_val<number>(),args.at(7).const_val<darwin::pixel>());
		else
			throw syntax_error("Arguments error.");
		return number(0);
	}
	cov::any draw_string(array& args)
	{
		if(args.at(0).type()==typeid(number))
			darwin::runtime.get_drawable()->draw_string(args.at(0).const_val<number>(),args.at(1).const_val<number>(),args.at(2).const_val<string>(),args.at(3).const_val<darwin::pixel>());
		else if(args.at(0).type()==typeid(darwin::picture))
			args.at(0).val<darwin::picture>(true).draw_string(args.at(1).const_val<number>(),args.at(2).const_val<number>(),args.at(3).const_val<string>(),args.at(4).const_val<darwin::pixel>());
		else
			throw syntax_error("Arguments error.");
		return number(0);
	}
	cov::any darwin_log(array& args)
	{
		Darwin_Log(args.front().to_string().c_str());
		return number(0);
	}
	cov::any is_kb_hit(array& args)
	{
		return darwin::runtime.is_kb_hit();
	}
	cov::any get_kb_hit(array& args)
	{
		return number(darwin::runtime.get_kb_hit());
	}
	cov::any get_ascii(array& args)
	{
		return number(args.at(0).val<string>().at(0));
	}
	cov::any get_color(array& args)
	{
		number color;
		CovSwitch(args.at(0).val<string>()) {
			CovCase("black") {
				color=0;
			}
			EndCovCase;
			CovCase("white") {
				color=1;
			}
			EndCovCase;
			CovCase("red") {
				color=2;
			}
			EndCovCase;
			CovCase("green") {
				color=3;
			}
			EndCovCase;
			CovCase("blue") {
				color=4;
			}
			EndCovCase;
			CovCase("pink") {
				color=5;
			}
			EndCovCase;
			CovCase("yellow") {
				color=6;
			}
			EndCovCase;
			CovCase("cyan") {
				color=7;
			}
			EndCovCase;
		}
		EndCovSwitch;
		return color;
	}
	cov::any darwin_pixel(array& args)
	{
		char ch=args.at(0).const_val<std::string>().at(0);
		bool bright=args.at(1).const_val<bool>();
		bool underline=args.at(2).const_val<bool>();
		darwin::colors fc,bc;
		switch(int(args.at(3).const_val<number>())) {
		case 0:
			fc=darwin::colors::black;
			break;
		case 1:
			fc=darwin::colors::white;
			break;
		case 2:
			fc=darwin::colors::red;
			break;
		case 3:
			fc=darwin::colors::green;
			break;
		case 4:
			fc=darwin::colors::blue;
			break;
		case 5:
			fc=darwin::colors::pink;
			break;
		case 6:
			fc=darwin::colors::yellow;
			break;
		case 7:
			fc=darwin::colors::cyan;
			break;
		}
		switch(int(args.at(4).const_val<number>())) {
		case 0:
			bc=darwin::colors::black;
			break;
		case 1:
			bc=darwin::colors::white;
			break;
		case 2:
			bc=darwin::colors::red;
			break;
		case 3:
			bc=darwin::colors::green;
			break;
		case 4:
			bc=darwin::colors::blue;
			break;
		case 5:
			bc=darwin::colors::pink;
			break;
		case 6:
			bc=darwin::colors::yellow;
			break;
		case 7:
			bc=darwin::colors::cyan;
			break;
		}
		return darwin::pixel(ch,bright,underline,fc,bc);
	}
	cov::any darwin_picture(array& args)
	{
		if(args.empty())
			return darwin::picture();
		else
			return darwin::picture(args.at(0).const_val<number>(),args.at(1).const_val<number>());
	}
	void init()
	{
		add_function_ext(init_graphics);
		add_function_ext(fit_drawable);
		add_function_ext(set_frame_limit);
		add_function_ext(update_drawable);
		add_function_ext(clear_drawable);
		add_function_ext(fill_drawable);
		add_function_ext(resize_drawable);
		add_function_ext(get_height_drawable);
		add_function_ext(get_width_drawable);
		add_function_ext(draw_point);
		add_function_ext(draw_picture);
		add_function_ext(draw_line);
		add_function_ext(draw_rect);
		add_function_ext(fill_rect);
		add_function_ext(draw_triangle);
		add_function_ext(fill_triangle);
		add_function_ext(draw_string);
		add_function_ext(darwin_log);
		add_function_ext(is_kb_hit);
		add_function_ext(get_kb_hit);
		add_function_ext(get_ascii);
		add_function_ext(get_color);
		add_function_ext(darwin_pixel);
		add_function_ext(darwin_picture);
	}
}
#ifndef CBS_DARWIN_EXT
cov_basic::extension* cbs_extension()
{
	darwin_cbs_ext::init();
	return &darwin_ext;
}
#endif