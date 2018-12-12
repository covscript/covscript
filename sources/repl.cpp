/*
* Covariant Script Programming Language Repl
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
* Copyright (C) 2018 Michael Lee(李登淳)
* Email: mikecovlee@163.com
* Github: https://github.com/mikecovlee
* Website: http://covscript.org
*/
//#include "covscript.cpp"
#include <covscript_impl/console/conio.hpp>
#include <covscript/covscript.hpp>
#include <iostream>

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
			log_path = cs::process_path(args[index]);
			expect_log_path = 2;
		}
		else if (expect_import_path == 1) {
			cs::current_process->import_path += cs::path_delimiter + cs::process_path(args[index]);
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
	cs::current_process->import_path += cs::path_delimiter + cs::get_import_path();
	if (!silent)
		std::cout << "Covariant Script Programming Language Interpreter REPL\nVersion: " << cs::current_process->version
		          << "\n"
		          "Copyright (C) 2018 Michael Lee.All rights reserved.\n"
		          "Please visit <http://covscript.org/> for more information."
		          << std::endl;
	cs::array
	arg{cs::var::make_constant<cs::string>("<REPL_ENV>")};
	for (; index < args_size; ++index)
		arg.emplace_back(cs::var::make_constant<cs::string>(args[index]));
	cs::context_t context = cs::create_context(arg);
	cs::repl repl(context);
	std::ofstream log_stream;
	std::string line;
	while (std::cin) {
		if (!silent)
			std::cout << std::string(repl.get_level() * 2, '.') << "> " << std::flush;
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
	catch (...) {
		if (!log_path.empty()) {
			std::ofstream out(::log_path);
			if (out) {
				out << "Uncaught exception: Unknown exception";
				out.flush();
			}
			else
				std::cerr << "Write log failed." << std::endl;
		}
		std::cerr << "Uncaught exception: Unknown exception" << std::endl;
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
