/*
* Covariant Script Programming Language
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
* Copyright (C) 2017 Michael Lee(李登淳)
* Email: mikecovlee@163.com
* Github: https://github.com/mikecovlee
* Website: http://covariant.cn/cs
*/
#include "./covscript.hpp"
#include <iostream>
#include <fstream>

void covscript_main(int args_size,const char* args[])
{
	std::ios::sync_with_stdio(false);
	if(args_size>1) {
		cs::array arg;
		for(int i=1; i<args_size; ++i)
			arg.push_back(std::string(args[i]));
		system_ext.add_var("args",arg);
		cs::init_grammar();
		cs::covscript(args[1]);
	}
	else
		throw cs::fatal_error("no input file.");
}

const char* log_path="./cs_runtime.log";
int main(int args_size,const char* args[])
{
	try {
		covscript_main(args_size,args);
	}
	catch(const std::exception& e) {
		std::ofstream out(::log_path);
		out<<e.what();
		out.flush();
		std::cerr<<e.what()<<std::endl;
		return -1;
	}
	return 0;
}
