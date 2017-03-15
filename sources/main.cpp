#define DARWIN_FORCE_BUILTIN
#include "../include/darwin/darwin.hpp"
#include "./covbasic.hpp"
#include <iostream>
#include <fstream>
#include <cmath>
#define add_function(name) cov_basic::storage.add_var_global(#name,cov_basic::native_interface(name));
#define add_function_name(name,func) cov_basic::storage.add_var_global(name,cov_basic::native_interface(func));
namespace cov_basic {
	cov::any parse_value(const std::string& str)
	{
		if(str=="true"||str=="True"||str=="TRUE")
			return true;
		if(str=="false"||str=="False"||str=="FALSE")
			return false;
		enum types {
		    interger,floating,other
		} type=types::interger;
		for(auto& it:str) {
			if(!std::isdigit(it)&&it!='.') {
				type=other;
				break;
			}
			if(!std::isdigit(it)&&it=='.') {
				if(type==interger) {
					type=floating;
					continue;
				}
				if(type==floating) {
					type=other;
					break;
				}
			}
		}
		switch(type) {
		case interger:
			try {
				return number(std::stoi(str));
			} catch(std::out_of_range) {
				try {
					return number(std::stol(str));
				} catch(std::out_of_range) {
					return str;
				}
			}
		case floating:
			return number(std::stold(str));
		case other:
			break;
		}
		return str;
	}
	cov::any input(array& args)
	{
		if(args.empty()) {
			std::string str;
			std::cin>>str;
			return parse_value(str);
		}
		for(auto& it:args) {
			std::string str;
			std::cin>>str;
			it.assign(parse_value(str),true);
		}
		return number(0);
	}
	cov::any getline(array&)
	{
		std::string str;
		std::getline(std::cin,str);
		return str;
	}
	cov::any print(array& args)
	{
		for(auto& it:args)
			std::cout<<it;
		return number(0);
	}
	cov::any println(array& args)
	{
		for(auto& it:args)
			std::cout<<it;
		std::cout<<std::endl;
		return number(0);
	}
	cov::any time(array&)
	{
		return number(cov::timer::time(cov::timer::time_unit::milli_sec));
	}
	cov::any delay(array& args)
	{
		if(args.size()!=1)
			throw syntax_error("Wrong size of arguments.");
		if(args.front().type()!=typeid(number))
			throw syntax_error("Wrong type of arguments.(Request Number)");
		cov::timer::delay(cov::timer::time_unit::milli_sec,cov::timer::timer_t(args.front().const_val<number>()));
		return number(0);
	}
	cov::any rand(array& args)
	{
		if(args.size()!=2)
			throw syntax_error("Wrong size of arguments.");
		if(args.at(0).type()!=typeid(number)||args.at(1).type()!=typeid(number))
			throw syntax_error("Wrong type of arguments.(Request Number)");
		return number(cov::rand<number>(args.at(0).const_val<number>(),args.at(1).const_val<number>()));
	}
	cov::any randint(array& args)
	{
		if(args.size()!=2)
			throw syntax_error("Wrong size of arguments.");
		if(args.at(0).type()!=typeid(number)||args.at(1).type()!=typeid(number))
			throw syntax_error("Wrong type of arguments.(Request Number)");
		return number(cov::rand<long>(args.at(0).const_val<number>(),args.at(1).const_val<number>()));
	}
	cov::any _sizeof(array& args)
	{
		if(args.size()!=1)
			throw syntax_error("Wrong size of arguments.");
		if(args.front().type()==typeid(array))
			return number(args.front().const_val<array>().size());
		else if(args.front().type()==typeid(string))
			return number(args.front().const_val<string>().size());
		else
			throw syntax_error("Get size of non-array or string object.");
	}
	cov::any clone(array& args)
	{
		if(args.size()!=1)
			throw syntax_error("Wrong size of arguments.");
		args.front().clone();
		return args.front();
	}
// String
	cov::any append_string(array& args)
	{
		if(args.size()!=2)
			throw syntax_error("Wrong size of arguments.");
		if(args.at(0).type()!=typeid(string))
			throw syntax_error("Wrong type of arguments.(Request String)");
		args.at(0).val<string>(true).append(args.at(1).to_string());
		return number(0);
	}
	cov::any cut_string(array& args)
	{
		if(args.size()!=2)
			throw syntax_error("Wrong size of arguments.");
		if(args.at(0).type()!=typeid(string)||args.at(1).type()!=typeid(number))
			throw syntax_error("Wrong type of arguments.(Request String,Number)");
		for(std::size_t i=0; i<args.at(1).const_val<number>(); ++i)
			args.at(0).val<string>(true).pop_back();
		return number(0);
	}
	cov::any clear_string(array& args)
	{
		if(args.size()!=1)
			throw syntax_error("Wrong size of arguments.");
		if(args.at(0).type()!=typeid(string))
			throw syntax_error("Wrong type of arguments.(Request String)");
		args.at(0).val<string>(true).clear();
		return number(0);
	}
// Array
	cov::any push_front_array(array& args)
	{
		if(args.size()!=2)
			throw syntax_error("Wrong size of arguments.");
		if(args.at(0).type()!=typeid(array))
			throw syntax_error("Wrong type of arguments.(Request Array)");
		args.at(0).val<array>(true).push_front(args.at(1));
		return number(0);
	}
	cov::any pop_front_array(array& args)
	{
		if(args.size()!=1)
			throw syntax_error("Wrong size of arguments.");
		if(args.at(0).type()!=typeid(array))
			throw syntax_error("Wrong type of arguments.(Request Array)");
		args.at(0).val<array>(true).pop_front();
		return number(0);
	}
	cov::any push_back_array(array& args)
	{
		if(args.size()!=2)
			throw syntax_error("Wrong size of arguments.");
		if(args.at(0).type()!=typeid(array))
			throw syntax_error("Wrong type of arguments.(Request Array)");
		args.at(0).val<array>(true).push_back(args.at(1));
		return number(0);
	}
	cov::any pop_back_array(array& args)
	{
		if(args.size()!=1)
			throw syntax_error("Wrong size of arguments.");
		if(args.at(0).type()!=typeid(array))
			throw syntax_error("Wrong type of arguments.(Request Array)");
		args.at(0).val<array>(true).pop_back();
		return number(0);
	}
	cov::any clear_array(array& args)
	{
		if(args.size()!=1)
			throw syntax_error("Wrong size of arguments.");
		if(args.at(0).type()!=typeid(array))
			throw syntax_error("Wrong type of arguments.(Request Array)");
		args.at(0).val<array>(true).clear();
		return number(0);
	}
// Mathematics
	cov::any abs(array& args)
	{
		return number(std::abs(args.at(0).const_val<number>()));
	}
	cov::any ln(array& args)
	{
		return number(std::log(args.at(0).const_val<number>()));
	}
	cov::any log(array& args)
	{
		if(args.size()==1)
			return number(std::log10(args.at(0).const_val<number>()));
		else
			return number(std::log(args.at(1).const_val<number>()/args.at(0).const_val<number>()));
	}
	cov::any sin(array& args)
	{
		return number(std::sin(args.at(0).const_val<number>()));
	}
	cov::any cos(array& args)
	{
		return number(std::cos(args.at(0).const_val<number>()));
	}
	cov::any tan(array& args)
	{
		return number(std::tan(args.at(0).const_val<number>()));
	}
	cov::any asin(array& args)
	{
		return number(std::sin(args.at(0).const_val<number>()));
	}
	cov::any acos(array& args)
	{
		return number(std::cos(args.at(0).const_val<number>()));
	}
	cov::any atan(array& args)
	{
		return number(std::tan(args.at(0).const_val<number>()));
	}
	cov::any sqrt(array& args)
	{
		return number(std::sqrt(args.at(0).const_val<number>()));
	}
	cov::any root(array& args)
	{
		return number(std::pow(args.at(0).const_val<number>(),number(1)/args.at(1).const_val<number>()));
	}
	cov::any pow(array& args)
	{
		return number(std::pow(args.at(0).const_val<number>(),args.at(1).const_val<number>()));
	}
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
		add_function(input);
		add_function(getline);
		add_function(print);
		add_function(println);
		add_function(time);
		add_function(delay);
		add_function(rand);
		add_function(randint);
		add_function(clone);
		add_function(append_string);
		add_function(cut_string);
		add_function(clear_string);
		add_function(push_front_array);
		add_function(pop_front_array);
		add_function(push_back_array);
		add_function(pop_back_array);
		add_function(clear_array);
		add_function(abs);
		add_function(ln);
		add_function(log);
		add_function(sin);
		add_function(cos);
		add_function(tan);
		add_function(asin);
		add_function(acos);
		add_function(atan);
		add_function(sqrt);
		add_function(root);
		add_function(pow);
		add_function(init_graphics);
		add_function(fit_drawable);
		add_function(set_frame_limit);
		add_function(update_drawable);
		add_function(clear_drawable);
		add_function(fill_drawable);
		add_function(get_height_drawable);
		add_function(get_width_drawable);
		add_function(draw_point);
		add_function(draw_picture);
		add_function(draw_line);
		add_function(draw_rect);
		add_function(fill_rect);
		add_function(draw_triangle);
		add_function(fill_triangle);
		add_function(draw_string);
		add_function(is_kb_hit);
		add_function(get_kb_hit);
		add_function(get_ascii);
		add_function(get_color);
		add_function(darwin_pixel);
		add_function(darwin_picture);
		add_function_name("sizeof",_sizeof);
	}
}
int main(int args_size,const char* args[])
{
	using namespace cov_basic;
	if(args_size>1) {
		std::deque<char> buff;
		std::deque<token_base*> tokens;
		std::deque<statement_base*> statements;
		std::ifstream in(args[1]);
		std::string line;
		while(std::getline(in,line)) {
			for(auto& c:line)
				buff.push_back(c);
			buff.push_back('\n');
		}
		init();
		translate_into_tokens(buff,tokens);
		translate_into_statements(tokens,statements);
		for(auto& s:statements)
			s->run();
	} else {
		std::cout<<args[0]<<": fatal error: no input files\ncompilation terminated."<<std::endl;
	}
	return 0;
}
