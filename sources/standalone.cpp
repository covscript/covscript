/*
* Covariant Script Programming Language Standalone
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
#include <csignal>

std::string log_path;
bool compile_only = false;
bool wait_before_exit = false;

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
			cs::import_path = process_path(args[index]);
			expect_import_path = 2;
		}
		else if (args[index][0] == '-') {
			if (std::strcmp(args[index], "--compile-only") == 0 && !compile_only)
				compile_only = true;
			else if (std::strcmp(args[index], "--wait-before-exit") == 0 && !wait_before_exit)
				wait_before_exit = true;
			else if (std::strcmp(args[index], "--log-path") == 0 && expect_log_path == 0)
				expect_log_path = 1;
			else if (std::strcmp(args[index], "--import-path") == 0 && expect_import_path == 0)
				expect_import_path = 1;
			else
				throw cs::fatal_error("argument syntax error.");
		}
		else
			break;
	}
	if (expect_log_path == 1 || expect_import_path == 1)
		throw cs::fatal_error("argument syntax error.");
	return index;
}

void covscript_main(int args_size, const char *args[])
{
	if (args_size > 1) {
		cs::import_path = get_import_path();
		int index = covscript_args(args_size, args);
		if (index == args_size)
			throw cs::fatal_error("no input file.");
		std::string path = process_path(args[index]);
		cs::array arg;
		for (; index < args_size; ++index)
			arg.emplace_back(cs::var::make_constant<cs::string>(args[index]));
		system_ext.add_var("args", cs::var::make_constant<cs::array>(arg));
		cs::init_ext();
		cs::instance_type instance;
		instance.compile(path);
		if (!compile_only)
			instance.interpret();
	}
	else
		throw cs::fatal_error("no input file.");
}

void covscript_exit_handler()
{
	if (wait_before_exit) {
		std::cerr << "\nPress any key to exit..." << std::endl;
		std::setbuf(stdin, nullptr);
		cs_impl::conio::getch();
	}
}

void force_exit(int flag)
{
	std::exit(0);
}

void handle_segfault(int flag)
{
	std::cerr<<"Your program have some problem about the Segmentation Fault.Please check your program after we terminate this program."<<std::endl;
	std::exit(-1);
}

int main(int args_size, const char *args[])
{
	int errorcode = 0;
	std::atexit(covscript_exit_handler);
	std::signal(SIGSEGV, handle_segfault);
#if not defined(__WIN32__) && not defined(WIN32)
	std::signal(SIGINT, force_exit);
	std::signal(SIGABRT, force_exit);
#endif
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
	if (wait_before_exit)
		std::cerr << std::string("\nProcess finished with exit code ") + std::to_string(errorcode) << std::endl;
	return errorcode;
}
