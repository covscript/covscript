/*
* Covariant Script Programming Language
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Affero General Public License as published
* by the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Affero General Public License for more details.
*
* You should have received a copy of the GNU Affero General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
* Copyright (C) 2017 Michael Lee(李登淳)
* Email: mikecovlee@163.com
* Github: https://github.com/mikecovlee
* Website: http://covariant.cn/cs
*/
#include "./covscript.hpp"

const char *log_path = "./cs_runtime.log";
bool check = false;
bool debug = false;

void covscript_main(int args_size, const char *args[])
{
	if (args_size > 1) {
		int index = 1;
		for (; index < args_size; ++index) {
			if (args[index][0] == '-') {
				if (std::strcmp(args[index], "--check") == 0 && !check)
					check = true;
				else if (std::strcmp(args[index], "--debug") == 0 && !debug)
					debug = true;
				else
					throw cs::fatal_error("argument grammar error.");
			}
			else
				break;
		}
		if (index == args_size)
			throw cs::fatal_error("no input file.");
		const char *path = args[index];
		cs::array arg;
		for (; index < args_size; ++index)
			arg.emplace_back(cs::var::make_constant<cs::string>(args[index]));
		system_ext.add_var("args", cs::var::make_constant<cs::array>(arg));
		cs::init_grammar();
		cs::init_ext();
		cs::covscript(path, check);
	}
	else
		throw cs::fatal_error("no input file.");
}

int main(int args_size, const char *args[])
{
	try {
		covscript_main(args_size, args);
	}
	catch (const std::exception &e) {
		if (debug) {
			std::ofstream out(::log_path);
			out << e.what();
			out.flush();
		}
		std::cerr << e.what() << std::endl;
		return -1;
	}
	return 0;
}
