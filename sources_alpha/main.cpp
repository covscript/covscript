#include "./covbasic.hpp"
#include <iostream>
#include <fstream>
#define add_function(name) cov_basic::storage.add_var_global(#name,cov_basic::native_interface(name));
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