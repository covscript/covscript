#pragma once
#include "../cni.hpp"
#include <iostream>
#include <cstdlib>
#include <limits>
static cs::extension system_ext;
namespace system_cs_ext {
	using namespace cs;
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
		system_ext.add_var("max",var::make_constant<number>(std::numeric_limits<number>::max()));
		system_ext.add_var("inf",var::make_constant<number>(std::numeric_limits<number>::infinity()));
		system_ext.add_var("in",var::make_protect<istream>(&std::cin,[](std::istream*) {}));
		system_ext.add_var("out",var::make_protect<ostream>(&std::cout,[](std::ostream*) {}));
		system_ext.add_var("run",var::make_protect<callable>(cni(run)));
		system_ext.add_var("getenv",var::make_protect<callable>(cni(getenv)));
		system_ext.add_var("exit",var::make_protect<callable>(cni(exit)));
	}
}
