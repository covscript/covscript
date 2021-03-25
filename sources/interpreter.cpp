/*
* Covariant Script Programming Language Interpreter
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
* Copyright (C) 2017-2021 Michael Lee(李登淳)
*
* This software is registered with the National Copyright Administration
* of the People's Republic of China(Registration Number: 2020SR0408026)
* and is protected by the Copyright Law of the People's Republic of China.
*
* Email:   lee@covariant.cn, mikecovlee@163.com
* Github:  https://github.com/mikecovlee
* Website: http://covscript.org.cn
*/
#include <covscript_impl/system.hpp>
#include <covscript/covscript.hpp>
#include <iostream>

#ifdef COVSCRIPT_PLATFORM_WIN32

#include <windows.h>

bool ctrlhandler(DWORD fdwctrltype)
{
	switch (fdwctrltype) {
	case CTRL_C_EVENT:
		std::cout << "Keyboard Interrupt (Ctrl+C Received)" << std::endl;
		cs::current_process->raise_sigint();
		return true;
	case CTRL_BREAK_EVENT: {
		int code = 0;
		cs::process_context::on_process_exit_default_handler(&code);
		return true;
	}
	default:
		return false;
	}
}

void activate_sigint_handler()
{
	::SetConsoleCtrlHandler((PHANDLER_ROUTINE) ctrlhandler, true);
}

#else

#include <unistd.h>
#include <signal.h>

void signal_handler(int sig)
{
	std::cout << "Keyboard Interrupt (Ctrl+C Received)" << std::endl;
	cs::current_process->raise_sigint();
}

void activate_sigint_handler()
{
	struct sigaction sa_usr {};
	sa_usr.sa_handler = &signal_handler;
	sigemptyset(&sa_usr.sa_mask);
	// sa_usr.sa_flags = SA_RESTART | SA_NODEFER;
	sigaction(SIGINT, &sa_usr, NULL);
}

#endif

std::string log_path;
bool repl = false;
bool silent = false;
bool dump_ast = false;
bool no_optimize = false;
bool compile_only = false;
bool show_help_info = false;
bool dump_dependency = false;
bool wait_before_exit = false;
bool show_version_info = false;

int covscript_args(int args_size, char *args[])
{
	int expect_log_path = 0;
	int expect_import_path = 0;
	int expect_stack_resize = 0;
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
		else if (expect_stack_resize == 1) {
			cs::current_process->resize_stack(std::stoul(args[index]));
			expect_stack_resize = 2;
		}
		else if (args[index][0] == '-') {
			if (std::strcmp(args[index], "--args") == 0 || std::strcmp(args[index], "-a") == 0) {
				repl = true;
				return ++index;
			}
			else if ((std::strcmp(args[index], "--silent") == 0 || std::strcmp(args[index], "-s") == 0) && !silent)
				silent = true;
			else if ((std::strcmp(args[index], "--dump-ast") == 0 || std::strcmp(args[index], "-d") == 0) && !dump_ast)
				dump_ast = true;
			else if ((std::strcmp(args[index], "--dependency") == 0 || std::strcmp(args[index], "-r") == 0) &&
			         !dump_dependency)
				dump_dependency = true;
			else if ((std::strcmp(args[index], "--no-optimize") == 0 || std::strcmp(args[index], "-o") == 0) &&
			         !no_optimize)
				no_optimize = true;
			else if ((std::strcmp(args[index], "--compile-only") == 0 || std::strcmp(args[index], "-c") == 0) &&
			         !compile_only)
				compile_only = true;
			else if ((std::strcmp(args[index], "--help") == 0 || std::strcmp(args[index], "-h") == 0) &&
			         !show_help_info)
				show_help_info = true;
			else if ((std::strcmp(args[index], "--wait-before-exit") == 0 || std::strcmp(args[index], "-w") == 0) &&
			         !wait_before_exit)
				wait_before_exit = true;
			else if ((std::strcmp(args[index], "--version") == 0 || std::strcmp(args[index], "-v") == 0) &&
			         !show_version_info)
				show_version_info = true;
			else if ((std::strcmp(args[index], "--log-path") == 0 || std::strcmp(args[index], "-l") == 0) &&
			         expect_log_path == 0)
				expect_log_path = 1;
			else if ((std::strcmp(args[index], "--import-path") == 0 || std::strcmp(args[index], "-i") == 0) &&
			         expect_import_path == 0)
				expect_import_path = 1;
			else if ((std::strcmp(args[index], "--stack-resize") == 0 || std::strcmp(args[index], "-S") == 0) &&
			         expect_stack_resize == 0)
				expect_stack_resize = 1;
			else
				throw cs::fatal_error("argument syntax error.");
		}
		else
			break;
	}
	if (expect_log_path == 1 || expect_import_path == 1 || expect_import_path == 1)
		throw cs::fatal_error("argument syntax error.");
	return index;
}

void covscript_main(int args_size, char *args[])
{
	int index = covscript_args(args_size, args);
	cs::current_process->import_path += cs::path_delimiter + cs::get_import_path();
	if (show_help_info) {
		std::cout << "Usage:\n";
		std::cout << "    cs [options...] <FILE> [arguments...]\n";
		std::cout << "    cs [options...]\n";
		std::cout << std::endl;
		std::cout << "Interpreter Options:" << std::endl;
		std::cout << "    Option                Mnemonic   Function\n";
		std::cout << "  --compile-only         -c          Only compile\n";
		std::cout << "  --dump-ast             -d          Export abstract syntax tree\n";
		std::cout << "  --dependency           -r          Export module dependency\n";
		std::cout << std::endl;
		std::cout << "Interpreter REPL Options:" << std::endl;
		std::cout << "    Option                Mnemonic   Function\n";
		std::cout << "  --silent               -s          Close the command prompt\n";
		std::cout << "  --args <...>           -a <...>    Set the arguments\n";
		std::cout << std::endl;
		std::cout << "Common Options:" << std::endl;
		std::cout << "    Option                Mnemonic   Function\n";
		std::cout << "  --no-optimize          -o          Disable optimizer\n";
		std::cout << "  --help                 -h          Show help infomation\n";
		std::cout << "  --version              -v          Show version infomation\n";
		std::cout << "  --wait-before-exit     -w          Wait before process exit\n";
		std::cout << "  --stack-resize <SIZE>  -S <SIZE>   Reset the size of runtime stack\n";
		std::cout << "  --log-path     <PATH>  -l <PATH>   Set the log and AST exporting path\n";
		std::cout << "  --import-path  <PATH>  -i <PATH>   Set the import path\n";
		std::cout << std::endl;
		return;
	}
	else if (show_version_info) {
		std::cout << "Version: " << cs::current_process->version << std::endl;
		std::cout << cs::copyright_info << std::endl;
		std::cout << "\nMetadata:\n";
		std::cout << "  Import Path: " << cs::current_process->import_path << "\n";
		std::cout << "  STD Version: " << cs::current_process->std_version << "\n";
		std::cout << "  API Version: " << COVSCRIPT_API_VERSION << "\n";
		std::cout << "  ABI Version: " << COVSCRIPT_ABI_VERSION << "\n";
#ifdef COVSCRIPT_PLATFORM_WIN32
		std::cout << "  Runtime Env: WIN32\n";
#else
		std::cout << "  Runtime Env: UNIX\n";
#endif
		std::cout << std::endl;
		return;
	}
	if (!repl && index != args_size) {
		std::string path = cs::process_path(args[index]);
		if (!cs_impl::file_system::exists(path) || cs_impl::file_system::is_dir(path) ||
		        !cs_impl::file_system::can_read(path))
			throw cs::fatal_error("invalid input file.");
		cs::prepend_import_path(path, cs::current_process);
		cs::array arg;
		for (; index < args_size; ++index)
			arg.emplace_back(cs::var::make_constant<cs::string>(args[index]));
		cs::context_t context = cs::create_context(arg);
		cs::current_process->on_process_exit.add_listener([&context](void *code) -> bool {
			cs::current_process->exit_code = *static_cast<int *>(code);
			throw cs::fatal_error("CS_EXIT");
			return true;
		});
		context->compiler->disable_optimizer = no_optimize;
		try {
			context->instance->compile(path);
			if (dump_ast) {
				if (!log_path.empty()) {
					std::ofstream out(::log_path);
					context->instance->dump_ast(out);
				}
				else
					context->instance->dump_ast(std::cout);
			}
			if (dump_dependency) {
				if (!log_path.empty()) {
					std::ofstream out(::log_path);
					for (auto &it:context->compiler->modules)
						out << it.first << std::endl;
				}
				else {
					for (auto &it:context->compiler->modules)
						std::cout << it.first << std::endl;
				}
			}
			context->instance->run_flat(std::cout, compile_only);
		}
		catch (const std::exception &e) {
			if (std::strstr(e.what(), "CS_EXIT") == nullptr)
				throw;
		}
		catch (...) {
			cs::collect_garbage(context);
			throw;
		}
		cs::collect_garbage(context);
	}
	else {
		if (!silent)
			std::cout << "Covariant Script Programming Language Interpreter REPL\nVersion: "
			          << cs::current_process->version
			          << "\n"
			          "Copyright (C) 2017-2021 Michael Lee. All rights reserved.\n"
			          "Please visit <http://covscript.org.cn/> for more information."
			          << std::endl;
		cs::array
		arg{cs::var::make_constant<cs::string>("<REPL_ENV>")};
		for (; index < args_size; ++index)
			arg.emplace_back(cs::var::make_constant<cs::string>(args[index]));
		cs::context_t context = cs::create_context(arg);
		activate_sigint_handler();
		cs::current_process->on_process_exit.add_listener([](void *code) -> bool {
			cs::current_process->exit_code = *static_cast<int *>(code);
			throw cs::fatal_error("CS_EXIT");
		});
		cs::current_process->on_process_sigint.add_listener([](void *) -> bool {
			throw cs::fatal_error("CS_SIGINT");
		});
		cs::current_process->on_process_sigint.add_listener([](void *) -> bool {
			std::cin.clear();
			return false;
		});
		context->compiler->disable_optimizer = no_optimize;
		cs::repl repl(context);
		std::ofstream log_stream;
		std::string line;
		while (true) {
			try {
#ifdef COVSCRIPT_PLATFORM_WIN32
				if (!silent)
					std::cout << std::string(repl.get_level() * 2, '.') << "> " << std::flush;
				// Workaround: https://stackoverflow.com/a/26763490
				while (true) {
					cs::current_process->poll_event();
					std::getline(std::cin, line);
					if (std::cin)
						break;
				}
#else
				if (!std::cin) {
					int code = 0;
					cs::process_context::on_process_exit_default_handler(&code);
				}
				if (!silent)
					std::cout << std::string(repl.get_level() * 2, '.') << "> " << std::flush;
				std::getline(std::cin, line);
				cs::current_process->poll_event();
#endif
				repl.exec(line);
			}
			catch (const std::exception &e) {
				if (std::strstr(e.what(), "CS_SIGINT") != nullptr) {
					cs::process_context::cleanup_context();
					repl.reset_status();
					activate_sigint_handler();
				}
				else if (std::strstr(e.what(), "CS_EXIT") == nullptr) {
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
				else
					break;
			}
			catch (...) {
				cs::collect_garbage(context);
				throw;
			}
		}
		cs::collect_garbage(context);
	}
}

int main(int args_size, char *args[])
{
	int errorcode = 0;
	try {
		covscript_main(args_size, args);
		errorcode = cs::current_process->exit_code;
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
