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
*
* Version: 1.0.1
*/
#ifndef CS_MINIMAL
#define CS_MATH_EXT
#define CS_FILE_EXT
#define CS_DARWIN_EXT
#else
#define CS_STATIC
#endif
#include "./covscript.hpp"
int main(int args_size,const char* args[])
{
	std::ios::sync_with_stdio(false);
	cs::reset();
	bool executed=true;
	try {
		cs::cs("./init.csc");
	}
	catch(const cs::fatal_error& fe) {
		executed=false;
	}
	if(args_size>1) {
		cs::array arg;
		for(int i=1; i<args_size; ++i)
			arg.push_back(std::string(args[i]));
		system_ext.add_var("args",arg);
		cs::cs(args[1]);
	}
	else if(!executed) {
		// throw cs::fatal_error("no input file.\nUsage: cs <file> <args...>");
        std::cout << "cs: no input files." << std::endl;
        std::cout << "Usage: cs <file> []<args...>]" << std::endl;
    }
	return 0;
}
