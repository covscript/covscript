#pragma once
#include "./arglist.hpp"
#include <iostream>
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
	void init()
	{
		system_ext.add_var("input",native_interface(input));
		system_ext.add_var("getline",native_interface(getline));
		system_ext.add_var("print",native_interface(print));
		system_ext.add_var("println",native_interface(println));
	}
}
