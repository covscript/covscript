/*
* Covariant Script Programming Language Standalone
*
* Licensed under the Covariant Innovation General Public License,
* Version 1.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* https://covariant.cn/licenses/LICENSE-1.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
* Copyright (C) 2019 Michael Lee(李登淳)
* Email: mikecovlee@163.com
* Github: https://github.com/mikecovlee
* Website: http://covscript.org
*/
#include <covscript_impl/console/conio.hpp>
#include <covscript/covscript.hpp>
#include <iostream>

std::string log_path;
bool dump_ast = false;
bool no_optimize = false;
bool compile_only = false;
bool show_help_info = false;
bool wait_before_exit = false;
bool show_version_info = false;

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
		else if (args[index][0] == '-') {
			if ((std::strcmp(args[index], "--dump-ast") == 0 || std::strcmp(args[index], "-d") == 0) && !dump_ast)
				dump_ast = true;
			else if ((std::strcmp(args[index], "--compile-only") == 0 || std::strcmp(args[index], "-c") == 0) &&
			         !compile_only)
				compile_only = true;
			else if ((std::strcmp(args[index], "--no-optimize") == 0 || std::strcmp(args[index], "-o") == 0) &&
			         !no_optimize)
				no_optimize = true;
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
		int index = covscript_args(args_size, args);
		cs::current_process->import_path += cs::path_delimiter + cs::get_import_path();
		if (show_help_info) {
			std::cout << "Usage: cs [options...] <FILE> [arguments...]\n" << "Options:\n";
			std::cout << "    Option               Mnemonic   Function\n";
			std::cout << "  --compile-only        -c          Only compile\n";
			std::cout << "  --no-optimize         -o          Disable optimizer\n";
			std::cout << "  --help                -h          Show help infomation\n";
			std::cout << "  --version             -v          Show version infomation\n";
			std::cout << "  --wait-before-exit    -w          Wait before process exit\n";
			std::cout << "  --dump-ast            -d          Export abstract syntax tree\n";
			std::cout << "  --log-path    <PATH>  -l <PATH>   Set the log and AST exporting path\n";
			std::cout << "  --import-path <PATH>  -i <PATH>   Set the import path\n";
			std::cout << std::endl;
			return;
		}
		else if (show_version_info) {
			std::cout << "Covariant Script Programming Language Interpreter\n";
			std::cout << "Version: " << cs::current_process->version << "\n";
			std::cout << "Copyright (C) 2019 Michael Lee. All rights reserved.\n";
			std::cout << "Licensed under the Covariant Innovation General Public License,\n";
			std::cout << "Version 1.0 (the \"License\");\n";
			std::cout << "you may not use this file except in compliance with the License.\n";
			std::cout << "You may obtain a copy of the License at\n";
			std::cout << "\nhttps://covariant.cn/licenses/LICENSE-1.0\n";
			std::cout << "\nUnless required by applicable law or agreed to in writing, software\n";
			std::cout << "distributed under the License is distributed on an \"AS IS\" BASIS,\n";
			std::cout << "WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.\n";
			std::cout << "See the License for the specific language governing permissions and\n";
			std::cout << "limitations under the License.\n";
			std::cout << "Please visit http://covscript.org for more information.\n";
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
		if (index == args_size)
			throw cs::fatal_error("no input file.");
		std::string path = cs::process_path(args[index]);
		cs::array
		arg;
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
			if (!compile_only)
				context->instance->interpret();
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
	else
		throw cs::fatal_error("no input file.");
}

int main(int args_size, const char *args[])
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
