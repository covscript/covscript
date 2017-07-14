#define DARWIN_FORCE_BUILTIN
#define DARWIN_IGNORE_WARNING
#include "../../include/darwin/darwin.hpp"
#include "../../include/mozart/switcher.hpp"
#ifndef CS_DARWIN_EXT
#include "../extension.hpp"
#endif
static cs::extension darwin_ext;
namespace darwin_cs_ext {
	using namespace cs;
// Graphics
	darwin::sync_clock clock(30);
	cov::any load(array& args)
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
	cov::any get_height(array& args)
	{
		if(args.empty())
			return number(darwin::runtime.get_drawable()->get_height());
		else
			return number(args.at(0).const_val<darwin::picture>().get_height());
	}
	cov::any get_width(array& args)
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
	cov::any log(array& args)
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
		return char(darwin::runtime.get_kb_hit());
	}
	cov::any pixel(array& args)
	{
		return darwin::pixel(args.at(0).const_val<char>(),args.at(1).const_val<bool>(),args.at(2).const_val<bool>(),args.at(3).const_val<darwin::colors>(),args.at(4).const_val<darwin::colors>());
	}
	cov::any picture(array& args)
	{
		if(args.empty())
			return darwin::picture();
		else
			return darwin::picture(args.at(0).const_val<number>(),args.at(1).const_val<number>());
	}
	cov::any message_box(array& args)
	{
		arglist::check<string,string,string>(args);
		const std::string& title=args.at(0).const_val<string>();
		const std::string& message=args.at(1).const_val<string>();
		const std::string& button=args.at(2).const_val<string>();
		std::size_t width=std::max(title.size(),std::max(message.size(),button.size()))+4;
		darwin::sync_clock c(10);
		while(true) {
			c.reset();
			if(darwin::runtime.is_kb_hit()) {
				darwin::runtime.get_kb_hit();
				break;
			}
			darwin::runtime.fit_drawable();
			std::size_t x(0.5*(darwin::runtime.get_drawable()->get_width()-width)),y(0.5*(darwin::runtime.get_drawable()->get_height()-6));
			darwin::runtime.get_drawable()->clear();
			darwin::runtime.get_drawable()->fill_rect(x,y,width,6,darwin::pixel(' ',true,false,darwin::colors::white,darwin::colors::white));
			darwin::runtime.get_drawable()->fill_rect(x,y,width,1,darwin::pixel(' ',true,false,darwin::colors::white,darwin::colors::cyan));
			darwin::runtime.get_drawable()->fill_rect(x+0.5*(width-button.size()-2),y+4,button.size()+2,1,darwin::pixel(' ',true,false,darwin::colors::white,darwin::colors::cyan));
			darwin::runtime.get_drawable()->draw_string(x+0.5*(width-title.size()),y,title,darwin::pixel(' ',true,false,darwin::colors::black,darwin::colors::cyan));
			darwin::runtime.get_drawable()->draw_string(x+0.5*(width-message.size()),y+2,message,darwin::pixel(' ',true,false,darwin::colors::black,darwin::colors::white));
			darwin::runtime.get_drawable()->draw_string(x+0.5*(width-button.size()),y+4,button,darwin::pixel(' ',true,false,darwin::colors::white,darwin::colors::cyan));
			darwin::runtime.update_drawable();
			c.sync();
		}
		return number(0);
	}
	cov::any input_box(array& args)
	{
		arglist::check<string,string,string,bool>(args);
		const std::string& title=args.at(0).const_val<string>();
		const std::string& message=args.at(1).const_val<string>();
		std::string buff=args.at(2).const_val<string>();
		darwin::sync_clock c(10);
		while(true) {
			c.reset();
			if(darwin::runtime.is_kb_hit()) {
				char ch=darwin::runtime.get_kb_hit();
				if(ch=='\n'||ch=='\r')
					break;
				else if(ch=='\b'||ch==127) {
					if(!buff.empty())
						buff.pop_back();
				}
				else
					buff+=ch;
			}
			darwin::runtime.fit_drawable();
			std::size_t width=std::max(title.size(),std::max(message.size(),buff.size()))+4;
			std::size_t x(0.5*(darwin::runtime.get_drawable()->get_width()-width)),y(0.5*(darwin::runtime.get_drawable()->get_height()-6));
			darwin::runtime.get_drawable()->clear();
			darwin::runtime.get_drawable()->fill_rect(x,y,width,6,darwin::pixel(' ',true,false,darwin::colors::white,darwin::colors::white));
			darwin::runtime.get_drawable()->fill_rect(x,y,width,1,darwin::pixel(' ',true,false,darwin::colors::white,darwin::colors::cyan));
			darwin::runtime.get_drawable()->fill_rect(x+2,y+4,width-4,1,darwin::pixel(' ',true,false,darwin::colors::white,darwin::colors::black));
			darwin::runtime.get_drawable()->draw_string(x+0.5*(width-title.size()),y,title,darwin::pixel(' ',true,false,darwin::colors::black,darwin::colors::cyan));
			darwin::runtime.get_drawable()->draw_string(x+0.5*(width-message.size()),y+2,message,darwin::pixel(' ',true,false,darwin::colors::black,darwin::colors::white));
			darwin::runtime.get_drawable()->draw_string(x+2,y+4,buff,darwin::pixel(' ',true,false,darwin::colors::white,darwin::colors::black));
			darwin::runtime.update_drawable();
			c.sync();
		}
		if(args.at(3).const_val<bool>())
			return parse_value(buff);
		else
			return buff;
	}
	void init()
	{
		darwin_ext.add_var("load",cov::any::make_protect<native_interface>(load));
		darwin_ext.add_var("fit_drawable",cov::any::make_protect<native_interface>(fit_drawable));
		darwin_ext.add_var("set_frame_limit",cov::any::make_protect<native_interface>(set_frame_limit));
		darwin_ext.add_var("update_drawable",cov::any::make_protect<native_interface>(update_drawable));
		darwin_ext.add_var("clear_drawable",cov::any::make_protect<native_interface>(clear_drawable));
		darwin_ext.add_var("fill_drawable",cov::any::make_protect<native_interface>(fill_drawable));
		darwin_ext.add_var("resize_drawable",cov::any::make_protect<native_interface>(resize_drawable));
		darwin_ext.add_var("get_height",cov::any::make_protect<native_interface>(get_height));
		darwin_ext.add_var("get_width",cov::any::make_protect<native_interface>(get_width));
		darwin_ext.add_var("draw_point",cov::any::make_protect<native_interface>(draw_point));
		darwin_ext.add_var("draw_picture",cov::any::make_protect<native_interface>(draw_picture));
		darwin_ext.add_var("draw_line",cov::any::make_protect<native_interface>(draw_line));
		darwin_ext.add_var("draw_rect",cov::any::make_protect<native_interface>(draw_rect));
		darwin_ext.add_var("fill_rect",cov::any::make_protect<native_interface>(fill_rect));
		darwin_ext.add_var("draw_triangle",cov::any::make_protect<native_interface>(draw_triangle));
		darwin_ext.add_var("fill_triangle",cov::any::make_protect<native_interface>(fill_triangle));
		darwin_ext.add_var("draw_string",cov::any::make_protect<native_interface>(draw_string));
		darwin_ext.add_var("log",cov::any::make_protect<native_interface>(log));
		darwin_ext.add_var("is_kb_hit",cov::any::make_protect<native_interface>(is_kb_hit));
		darwin_ext.add_var("get_kb_hit",cov::any::make_protect<native_interface>(get_kb_hit));
		darwin_ext.add_var("pixel",cov::any::make_protect<native_interface>(pixel,true));
		darwin_ext.add_var("picture",cov::any::make_protect<native_interface>(picture,true));
		darwin_ext.add_var("message_box",cov::any::make_protect<native_interface>(message_box));
		darwin_ext.add_var("input_box",cov::any::make_protect<native_interface>(input_box));
		darwin_ext.add_var("black",cov::any::make_constant<darwin::colors>(darwin::colors::black));
		darwin_ext.add_var("white",cov::any::make_constant<darwin::colors>(darwin::colors::white));
		darwin_ext.add_var("red",cov::any::make_constant<darwin::colors>(darwin::colors::red));
		darwin_ext.add_var("green",cov::any::make_constant<darwin::colors>(darwin::colors::green));
		darwin_ext.add_var("blue",cov::any::make_constant<darwin::colors>(darwin::colors::blue));
		darwin_ext.add_var("pink",cov::any::make_constant<darwin::colors>(darwin::colors::pink));
		darwin_ext.add_var("yellow",cov::any::make_constant<darwin::colors>(darwin::colors::yellow));
		darwin_ext.add_var("cyan",cov::any::make_constant<darwin::colors>(darwin::colors::cyan));
	}
}
#ifndef CS_DARWIN_EXT
cs::extension* cs_extension()
{
	darwin_cs_ext::init();
	return &darwin_ext;
}
#endif
