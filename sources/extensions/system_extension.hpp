#pragma once

#include "./console/conio.hpp"
#include "../cni.hpp"
#include <iostream>
#include <cstdlib>
#include <limits>

static cs::extension system_ext;
static cs::extension console_ext;
static cs::extension_t console_ext_shared = std::make_shared<cs::extension_holder>(&console_ext);
namespace console_cs_ext {
	using namespace cs;
	using namespace cs_impl;

	number terminal_width()
	{
		return conio::terminal_width();
	}

	number terminal_height()
	{
		return conio::terminal_height();
	}

	void gotoxy(number x,number y)
	{
		conio::gotoxy(x,y);
	}

	void echo(boolean v)
	{
		conio::echo(v);
	}

	void clrscr()
	{
		conio::clrscr();
	}

	char getch()
	{
		return conio::getch();
	}

	boolean kbhit()
	{
		return conio::kbhit();
	}

	void init()
	{
		console_ext.add_var("terminal_width", var::make_protect<callable>(cni(terminal_width)));
		console_ext.add_var("terminal_height", var::make_protect<callable>(cni(terminal_height)));
		console_ext.add_var("gotoxy", var::make_protect<callable>(cni(gotoxy)));
		console_ext.add_var("echo", var::make_protect<callable>(cni(echo)));
		console_ext.add_var("clrscr", var::make_protect<callable>(cni(clrscr)));
		console_ext.add_var("getch", var::make_protect<callable>(cni(getch)));
		console_ext.add_var("kbhit", var::make_protect<callable>(cni(kbhit)));
	}
}

namespace system_cs_ext {
	using namespace cs;

	number run(const string &str)
	{
		return std::system(str.c_str());
	}

	string getenv(const string &name)
	{
		const char *str = std::getenv(name.c_str());
		if (str == nullptr)
			throw lang_error("Environment variable \"" + name + "\" is not exist.");
		return str;
	}

	void exit(number code)
	{
		std::exit(code);
	}

	void init()
	{
		console_cs_ext::init();
		system_ext.add_var("console", var::make_protect<extension_t>(console_ext_shared));
		system_ext.add_var("max", var::make_constant<number>(std::numeric_limits<number>::max()));
		system_ext.add_var("inf", var::make_constant<number>(std::numeric_limits<number>::infinity()));
		system_ext.add_var("in", var::make_protect<istream>(&std::cin, [](std::istream *) {}));
		system_ext.add_var("out", var::make_protect<ostream>(&std::cout, [](std::ostream *) {}));
		system_ext.add_var("run", var::make_protect<callable>(cni(run)));
		system_ext.add_var("getenv", var::make_protect<callable>(cni(getenv)));
		system_ext.add_var("exit", var::make_protect<callable>(cni(exit)));
	}
}
