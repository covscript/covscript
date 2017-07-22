#define DARWIN_FORCE_BUILTIN
#define DARWIN_IGNORE_WARNING
#ifndef CS_DARWIN_EXT
#include "../extension.hpp"
#endif
#include "../../include/darwin/darwin.hpp"
#include "../cni.hpp"
static cs::extension darwin_ext;
namespace darwin_cs_ext {
	using namespace cs;
// Graphics
	using drawable_t=std::shared_ptr<darwin::drawable>;
	darwin::sync_clock clock(30);
	void load(const string& str)
	{
		darwin::runtime.load(str);
	}
	void exit(number code)
	{
		darwin::runtime.exit(code);
	}
	bool is_kb_hit()
	{
		return darwin::runtime.is_kb_hit();
	}
	char get_kb_hit()
	{
		return darwin::runtime.get_kb_hit();
	}
	void fit_drawable()
	{
		darwin::runtime.fit_drawable();
	}
	cov::any get_drawable()
	{
		return cov::any::make<drawable_t>(darwin::runtime.get_drawable(),[](darwin::drawable*) {});
	}
	void update_drawable()
	{
		darwin::runtime.update_drawable();
		clock.sync();
		clock.reset();
	}
	void set_frame_limit(number freq)
	{
		clock.set_freq(freq);
	}
	void clear_drawable(const drawable_t& pic)
	{
		pic->clear();
	}
	void fill_drawable(const drawable_t& pic,const darwin::pixel& pix)
	{
		pic->fill(pix);
	}
	void resize_drawable(const drawable_t& pic,number w,number h)
	{
		pic->resize(w,h);
	}
	number get_height(const drawable_t& pic)
	{
		return pic->get_height();
	}
	number get_width(const drawable_t& pic)
	{
		return pic->get_width();
	}
	void draw_pixel(const drawable_t& pic,number x,number y,const darwin::pixel& pix)
	{
		pic->draw_pixel(x,y,pix);
	}
	void draw_picture(const drawable_t& pic,number x,number y,const drawable_t& p)
	{
		pic->draw_picture(x,y,*p);
	}
	void draw_line(const drawable_t& pic,number x1,number y1,number x2,number y2,const darwin::pixel& pix)
	{
		pic->draw_line(x1,y1,x2,y2,pix);
	}
	void draw_rect(const drawable_t& pic,number x,number y,number w,number h,const darwin::pixel& pix)
	{
		pic->draw_rect(x,y,w,h,pix);
	}
	void fill_rect(const drawable_t& pic,number x,number y,number w,number h,const darwin::pixel& pix)
	{
		pic->fill_rect(x,y,w,h,pix);
	}
	void draw_triangle(const drawable_t& pic,number x1,number y1,number x2,number y2,number x3,number y3,const darwin::pixel& pix)
	{
		pic->draw_triangle(x1,y1,x2,y2,x3,y3,pix);
	}
	void fill_triangle(const drawable_t& pic,number x1,number y1,number x2,number y2,number x3,number y3,const darwin::pixel& pix)
	{
		pic->fill_triangle(x1,y1,x2,y2,x3,y3,pix);
	}
	cov::any draw_string(const drawable_t& pic,number x,number y,const string& str,const darwin::pixel& pix)
	{
		pic->draw_string(x,y,str,pix);
	}
	cov::any pixel(char ch,darwin::colors fc,darwin::colors bc)
	{
		return cov::any::make_protect<darwin::pixel>(ch,true,false,fc,bc);
	}
	cov::any picture(number w,number h)
	{
		return cov::any::make_protect<drawable_t>(new darwin::picture(w,h));
	}
	void message_box(const string& title,const string& message,const string& button)
	{
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
	}
	cov::any input_box(const string& title,const string& message,string buff,bool format)
	{
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
		if(format)
			return parse_value(buff);
		else
			return buff;
	}
	void init()
	{
		darwin_ext.add_var("load",cov::any::make_protect<native_interface>(cni(load)));
		darwin_ext.add_var("exit",cov::any::make_protect<native_interface>(cni(exit)));
		darwin_ext.add_var("is_kb_hit",cov::any::make_protect<native_interface>(cni(is_kb_hit)));
		darwin_ext.add_var("get_kb_hit",cov::any::make_protect<native_interface>(cni(get_kb_hit)));
		darwin_ext.add_var("fit_drawable",cov::any::make_protect<native_interface>(cni(fit_drawable)));
		darwin_ext.add_var("get_drawable",cov::any::make_protect<native_interface>(cni(get_drawable)));
		darwin_ext.add_var("update_drawable",cov::any::make_protect<native_interface>(cni(update_drawable)));
		darwin_ext.add_var("set_frame_limit",cov::any::make_protect<native_interface>(cni(set_frame_limit)));
		darwin_ext.add_var("clear_drawable",cov::any::make_protect<native_interface>(cni(clear_drawable)));
		darwin_ext.add_var("fill_drawable",cov::any::make_protect<native_interface>(cni(fill_drawable)));
		darwin_ext.add_var("resize_drawable",cov::any::make_protect<native_interface>(cni(resize_drawable)));
		darwin_ext.add_var("get_height",cov::any::make_protect<native_interface>(cni(get_height)));
		darwin_ext.add_var("get_width",cov::any::make_protect<native_interface>(cni(get_width)));
		darwin_ext.add_var("draw_pixel",cov::any::make_protect<native_interface>(cni(draw_pixel)));
		darwin_ext.add_var("draw_picture",cov::any::make_protect<native_interface>(cni(draw_picture)));
		darwin_ext.add_var("draw_line",cov::any::make_protect<native_interface>(cni(draw_line)));
		darwin_ext.add_var("draw_rect",cov::any::make_protect<native_interface>(cni(draw_rect)));
		darwin_ext.add_var("fill_rect",cov::any::make_protect<native_interface>(cni(fill_rect)));
		darwin_ext.add_var("draw_triangle",cov::any::make_protect<native_interface>(cni(draw_triangle)));
		darwin_ext.add_var("fill_triangle",cov::any::make_protect<native_interface>(cni(fill_triangle)));
		darwin_ext.add_var("draw_string",cov::any::make_protect<native_interface>(cni(draw_string)));
		darwin_ext.add_var("pixel",cov::any::make_protect<native_interface>(cni(pixel),true));
		darwin_ext.add_var("picture",cov::any::make_protect<native_interface>(cni(picture),true));
		darwin_ext.add_var("message_box",cov::any::make_protect<native_interface>(cni(message_box)));
		darwin_ext.add_var("input_box",cov::any::make_protect<native_interface>(cni(input_box)));
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
