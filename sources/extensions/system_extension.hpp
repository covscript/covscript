#pragma once
#include "../arglist.hpp"
#include <iostream>
#include <cstdlib>
#include <limits>
static cov_basic::extension system_ext;
namespace system_cbs_ext {
	using namespace cov_basic;
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
	cov::any getline(array& args)
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
	cov::any setprecision(array& args)
	{
		arglist::check<number>(args);
		output_precision=args.front().const_val<number>();
		return number(0);
	}
	cov::any run(array& args)
	{
		arglist::check<string>(args);
		return number(std::system(args.at(0).const_val<string>().c_str()));
	}
	cov::any getenv(array& args)
	{
		arglist::check<string>(args);
		const char* str=std::getenv(args.at(0).const_val<string>().c_str());
		if(str==nullptr)
			throw lang_error("Environment variable \""+args.at(0).const_val<string>()+"\" is not exist.");
		return string(str);
	}
	cov::any exit(array& args)
	{
		arglist::check<number>(args);
		std::exit(args.at(0).const_val<number>());
		return number(0);
	}
	void init()
	{
		system_ext.add_var("max",std::numeric_limits<number>::max());
		system_ext.add_var("inf",std::numeric_limits<number>::infinity());
		system_ext.add_var("input",cov::any::make_constant<native_interface>(input));
		system_ext.add_var("getline",cov::any::make_constant<native_interface>(getline));
		system_ext.add_var("print",cov::any::make_constant<native_interface>(print));
		system_ext.add_var("println",cov::any::make_constant<native_interface>(println));
		system_ext.add_var("setprecision",cov::any::make_constant<native_interface>(setprecision));
		system_ext.add_var("run",cov::any::make_constant<native_interface>(run));
		system_ext.add_var("getenv",cov::any::make_constant<native_interface>(getenv));
		runtime_ext.add_var("exit",cov::any::make_constant<native_interface>(exit));
	}
}
