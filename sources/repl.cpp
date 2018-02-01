/*
* Covariant Script Programming Language Repl
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
* Copyright (C) 2018 Michael Lee(李登淳)
* Email: mikecovlee@163.com
* Github: https://github.com/mikecovlee
* Website: http://covariant.cn/cs
*/
#include "covscript.cpp"

std::string log_path;
bool wait_before_exit = false;
bool silent = false;

int covscript_args(int args_size, const char *args[])
{
	int expect_log_path = 0;
	int expect_import_path = 0;
	int index = 1;
	for (; index < args_size; ++index) {
		if (expect_log_path == 1) {
			log_path = process_path(args[index]);
			expect_log_path = 2;
		}
		else if (expect_import_path == 1) {
			cs::import_path += cs::path_delimiter + process_path(args[index]);
			expect_import_path = 2;
		}
		else if (std::strcmp(args[index], "--args") == 0)
			return ++index;
		else if (std::strcmp(args[index], "--wait-before-exit") == 0 && !wait_before_exit)
			wait_before_exit = true;
		else if (std::strcmp(args[index], "--silent") == 0 && !silent)
			silent = true;
		else if (std::strcmp(args[index], "--log-path") == 0 && expect_log_path == 0)
			expect_log_path = 1;
		else if (std::strcmp(args[index], "--import-path") == 0 && expect_import_path == 0)
			expect_import_path = 1;
		else
			throw cs::fatal_error("argument syntax error.");
	}
	if (expect_log_path == 1 || expect_import_path == 1)
		throw cs::fatal_error("argument syntax error.");
	return index;
}

void covscript_main(int args_size, const char *args[])
{
	int index = covscript_args(args_size, args);
	cs::import_path += cs::path_delimiter + get_import_path();
	if (!silent)
		std::cout << "Covariant Script Programming Language Interpreter REPL\nVersion: " << cs::version << "\n"
		          "Copyright (C) 2018 Michael Lee.All rights reserved.\n"
		          "Please visit <http://covscript.org/> for more information." << std::endl;
	cs::array arg{cs::var::make_constant<cs::string>("<REPL_ENV>")};
	for (; index < args_size; ++index)
		arg.emplace_back(cs::var::make_constant<cs::string>(args[index]));
	system_ext.add_var("args", cs::var::make_constant<cs::array>(arg));
	cs::init_ext();
	cs::instance_type instance;
	instance.context->file_path = "<REPL_ENV>";
	cs::repl repl(instance.context);
	std::ofstream log_stream;
	std::string line;
	while (std::cin) {
		if (!silent)
			std::cout << std::string(repl.get_level() * 2, '.') << ">" << std::flush;
		std::getline(std::cin, line);
		try {
			repl.exec(line);
		}
		catch (const std::exception &e) {
			if (!log_path.empty()) {
				if (!log_stream.is_open())
					log_stream.open(::log_path);
				if (log_stream)
					log_stream << e.what() << std::endl;
				else
					std::cerr << "Write log failed." << std::endl;
			}
			std::cerr << e.what() << std::endl;
		}
	}
}

int main(int args_size, const char *args[])
{
	int errorcode = 0;
	try {
		covscript_main(args_size, args);
	}
	catch (const std::exception &e) {
		if (!log_path.empty()) {
			std::ofstream out(::log_path);
			if (out) {
				out << e.what();
				out.flush();
			}
			else
				std::cerr << "Write log failed." << std::endl;
		}
		std::cerr << e.what() << std::endl;
		errorcode = -1;
	}
	if (wait_before_exit) {
		std::cerr << "\nProcess finished with exit code " << errorcode << std::endl;
		std::cerr << "\nPress any key to exit..." << std::endl;
		while (!cs_impl::conio::kbhit());
		cs_impl::conio::getch();
	}
	return errorcode;
}
