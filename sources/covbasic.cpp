/*
* Covariant Basic Programming Language
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
* Website: http://covbasic.org
*
* Version: 2.1.5.0
*/
#define DARWIN_DISABLE_LOG
#define CBS_MATH_EXT
#define CBS_FILE_EXT
#define CBS_DARWIN_EXT
#include "./covbasic.hpp"
int main(int args_size,const char* args[])
{
	if(args_size>1) {
		cov_basic::array arg;
		for(int i=1; i<args_size; ++i)
			arg.push_back(std::string(args[i]));
		system_ext.add_var("args",arg);
		cov_basic::reset();
		cov_basic::cov_basic(args[1]);
	}
	else
		throw cov_basic::fatal_error("no input file.\nUsage: cbs2.1 <file> <args...>");
	return 0;
}
