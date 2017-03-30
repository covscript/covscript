#define DARWIN_FORCE_BUILTIN
#include "../include/darwin/darwin.hpp"
#include "../include/mozart/switcher.hpp"
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
	cov::any _clone(cov::any val)
	{
		val.clone();
		if(val.type()==typeid(array))
			for(cov::any& v:val.val<array>(true))
				v.clone();
		return val;
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
	cov::any to_integer(array& args)
	{
		if(args.size()!=1)
			throw syntax_error("Wrong size of arguments.");
		if(args.front().type()==typeid(number))
			return number(long(args.at(0).const_val<number>()));
		else if(args.front().type()==typeid(string))
			return number(std::stold(args.at(0).const_val<string>()));
		else
			throw syntax_error("Wrong type of arguments.(Request Number or String)");
	}
	cov::any to_number(array& args)
	{
		if(args.size()!=1)
			throw syntax_error("Wrong size of arguments.");
		if(args.front().type()==typeid(string))
			return number(std::stold(args.at(0).const_val<string>()));
		else
			throw syntax_error("Wrong type of arguments.(Request String)");
	}
	cov::any to_string(array& args)
	{
		if(args.size()!=1)
			throw syntax_error("Wrong size of arguments.");
		return args.front().to_string();
	}
	cov::any clone(array& args)
	{
		if(args.size()!=1)
			throw syntax_error("Wrong size of arguments.");
		return _clone(args.front());
	}
	cov::any error(array& args)
	{
		throw lang_error(args.front().to_string().c_str());
		return number(0);
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
		args.at(0).val<array>(true).push_front(_clone(args.at(1)));
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
		args.at(0).val<array>(true).push_back(_clone(args.at(1)));
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
		// Expression Grammar
		translator.add_method({new token_expr(cov::tree<token_base*>()),new token_endline(0)},method_type{grammar_type::single,[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				return new statement_expression(dynamic_cast<token_expr*>(raw.front().front())->get_tree(),raw.front().back());
			}});
		// Define Grammar
		translator.add_method({new token_action(action_types::define_),new token_expr(cov::tree<token_base*>()),new token_endline(0)},method_type{grammar_type::single,[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				return new statement_define(dynamic_cast<token_expr*>(raw.front().at(1))->get_tree(),raw.front().back());
			}});
		translator.add_method({new token_action(action_types::define_),new token_expr(cov::tree<token_base*>()),new token_action(action_types::as_),new token_expr(cov::tree<token_base*>()),new token_endline(0)},method_type{grammar_type::single,[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				return new statement_define(dynamic_cast<token_expr*>(raw.front().at(1))->get_tree(),dynamic_cast<token_expr*>(raw.front().at(3))->get_tree().root().data(),raw.front().back());
			}});
		// End Grammar
		translator.add_method({new token_action(action_types::endblock_),new token_endline(0)},method_type{grammar_type::single,[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				return new statement_end;
			}});
		// Block Grammar
		translator.add_method({new token_action(action_types::block_),new token_endline(0)},method_type{grammar_type::block,[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				std::deque<statement_base*> body;
				kill_action({raw.begin()+1,raw.end()},body);
				return new statement_block(body,raw.front().back());
			}});
		// If Grammar
		translator.add_method({new token_action(action_types::if_),new token_expr(cov::tree<token_base*>()),new token_endline(0)},method_type{grammar_type::block,[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				bool have_else=false;
				std::deque<statement_base*>* body=new std::deque<statement_base*>;
				kill_action({raw.begin()+1,raw.end()},*body);
				for(auto& ptr:*body)
				{
					if(ptr->get_type()==statement_types::else_) {
						if(!have_else)
							have_else=true;
						else
							throw syntax_error("Multi Else Grammar.");
					}
				}
				if(have_else)
				{
					std::deque<statement_base*>* body_true=new std::deque<statement_base*>;
					std::deque<statement_base*>* body_false=new std::deque<statement_base*>;
					bool now_place=true;
					for(auto& ptr:*body) {
						if(ptr->get_type()==statement_types::else_) {
							now_place=false;
							continue;
						}
						if(now_place)
							body_true->push_back(ptr);
						else
							body_false->push_back(ptr);
					}
					delete body;
					return new statement_if(dynamic_cast<token_expr*>(raw.front().at(1))->get_tree(),body_true,body_false,raw.front().back());
				} else
					return new statement_if(dynamic_cast<token_expr*>(raw.front().at(1))->get_tree(),body,nullptr,raw.front().back());
			}});
		// Else Grammar
		translator.add_method({new token_action(action_types::else_),new token_endline(0)},method_type{grammar_type::single,[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				return new statement_else;
			}});
		// While Grammar
		translator.add_method({new token_action(action_types::while_),new token_expr(cov::tree<token_base*>()),new token_endline(0)},method_type{grammar_type::block,[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				std::deque<statement_base*> body;
				kill_action({raw.begin()+1,raw.end()},body);
				return new statement_while(dynamic_cast<token_expr*>(raw.front().at(1))->get_tree(),body,raw.front().back());
			}});
		// Break Grammar
		translator.add_method({new token_action(action_types::break_),new token_endline(0)},method_type{grammar_type::single,[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				return new statement_break(raw.front().back());
			}});
		// Continue Grammar
		translator.add_method({new token_action(action_types::continue_),new token_endline(0)},method_type{grammar_type::single,[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				return new statement_continue(raw.front().back());
			}});
		// Function Grammar
		translator.add_method({new token_action(action_types::function_),new token_expr(cov::tree<token_base*>()),new token_endline(0)},method_type{grammar_type::block,[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				cov::tree<token_base*>& t=dynamic_cast<token_expr*>(raw.front().at(1))->get_tree();
				std::string name=dynamic_cast<token_id*>(t.root().left().data())->get_id();
				std::deque<std::string> args;
				for(auto& it:dynamic_cast<token_arglist*>(t.root().right().data())->get_arglist())
					args.push_back(dynamic_cast<token_id*>(it.root().data())->get_id());
				std::deque<statement_base*> body;
				kill_action({raw.begin()+1,raw.end()},body);
				return new statement_function(name,args,body,raw.front().back());
			}});
		// Return Grammar
		translator.add_method({new token_action(action_types::return_),new token_expr(cov::tree<token_base*>()),new token_endline(0)},method_type{grammar_type::single,[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				return new statement_return(dynamic_cast<token_expr*>(raw.front().at(1))->get_tree(),raw.front().back());
			}});
		// Struct Grammar
		translator.add_method({new token_action(action_types::struct_),new token_expr(cov::tree<token_base*>()),new token_endline(0)},method_type{grammar_type::block,[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				cov::tree<token_base*>& t=dynamic_cast<token_expr*>(raw.front().at(1))->get_tree();
				std::string name=dynamic_cast<token_id*>(t.root().data())->get_id();
				std::deque<statement_base*> body;
				kill_action({raw.begin()+1,raw.end()},body);
				return new statement_struct(name,body,raw.front().back());
			}});
		storage.add_type("number",[]()->cov::any {return number(0);});
		storage.add_type("boolean",[]()->cov::any {return boolean(true);});
		storage.add_type("string",[]()->cov::any {return string();});
		add_function(input);
		add_function(getline);
		add_function(print);
		add_function(println);
		add_function(time);
		add_function(delay);
		add_function(rand);
		add_function(randint);
		add_function(to_integer);
		add_function(to_number);
		add_function(to_string);
		add_function(clone);
		add_function(error);
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
		add_function(resize_drawable);
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
		add_function(darwin_log);
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
		std::ios::sync_with_stdio(false);
		std::deque<char> buff;
		std::deque<token_base*> tokens;
		std::deque<statement_base*> statements;
		std::ifstream in(args[1]);
		std::string line;
		std::size_t line_num=0;
		while(std::getline(in,line)) {
			++line_num;
			if(line.empty())
				continue;
			if(line[0]=='#')
				continue;
			for(auto& c:line)
				buff.push_back(c);
			try {
				translate_into_tokens(buff,tokens);
			} catch(const syntax_error& se) {
				throw syntax_error(line_num,se.what());
			}
			tokens.push_back(new token_endline(line_num));
			buff.clear();
		}
		init();
		translate_into_statements(tokens,statements);
		for(auto& ptr:statements) {
			try {
				ptr->run();
			} catch(const syntax_error& se) {
				throw syntax_error(ptr->get_line_num(),se.what());
			} catch(const lang_error& le) {
				throw lang_error(ptr->get_line_num(),le.what());
			}
		}
	} else {
		std::cout<<"Covariant Basic 2.0 Parser\nFatal Error: no input file.\nUsage: "<<args[0]<<" <file>\nCompilation terminated."<<std::endl;
	}
	return 0;
}
