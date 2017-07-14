#pragma once
#include "../cni.hpp"
#include <iostream>
#include <cstdlib>
#include <limits>
static cs::extension system_ext;
namespace system_cs_ext {
	using namespace cs;
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
	string getline()
	{
		std::string str;
		std::getline(std::cin,str);
		return str;
	}
	void setprecision(number pre)
	{
		output_precision=pre;
	}
	number run(const string& str)
	{
		return std::system(str.c_str());
	}
	string getenv(const string& name)
	{
		const char* str=std::getenv(name.c_str());
		if(str==nullptr)
			throw lang_error("Environment variable \""+name+"\" is not exist.");
		return str;
	}
	void exit(number code)
	{
		std::exit(code);
	}
	void init()
	{
		system_ext.add_var("max",cov::any::make_constant<number>(std::numeric_limits<number>::max()));
		system_ext.add_var("inf",cov::any::make_constant<number>(std::numeric_limits<number>::infinity()));
		system_ext.add_var("input",cov::any::make_protect<native_interface>(input));
		system_ext.add_var("print",cov::any::make_protect<native_interface>(print));
		system_ext.add_var("println",cov::any::make_protect<native_interface>(println));
		system_ext.add_var("getline",cov::any::make_protect<native_interface>(cni(getline)));
		system_ext.add_var("setprecision",cov::any::make_protect<native_interface>(cni(setprecision)));
		system_ext.add_var("run",cov::any::make_protect<native_interface>(cni(run)));
		system_ext.add_var("getenv",cov::any::make_protect<native_interface>(cni(getenv)));
		system_ext.add_var("exit",cov::any::make_protect<native_interface>(cni(exit)));
	}
}
