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
		if(args.front().type()!=typeid(array))
			throw syntax_error("Get size of non-array object.");
		return number(args.front().const_val<array>().size());
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
	void init()
	{
		storage.add_var_global("e",number(2.7182818284));
		storage.add_var_global("pi",number(3.1415926535));
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
