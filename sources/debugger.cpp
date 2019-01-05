/*
* Covariant Script Programming Language Debugger
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
#define CS_DEBUGGER
#include <covscript_impl/console/conio.hpp>
#include <covscript/covscript.hpp>
#include <iostream>

std::string log_path;
bool no_optimize = false;
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
			if ((std::strcmp(args[index], "--help") == 0 || std::strcmp(args[index], "-h") == 0) &&
			        !show_help_info)
				show_help_info = true;
			else if ((std::strcmp(args[index], "--version") == 0 || std::strcmp(args[index], "-v") == 0) &&
			         !show_version_info)
				show_version_info = true;
			else if ((std::strcmp(args[index], "--wait-before-exit") == 0 || std::strcmp(args[index], "-w") == 0) &&
			         !wait_before_exit)
				wait_before_exit = true;
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

using callback_t=std::function<bool(const std::string&)>;
std::string path;
std::string last_cmd;
cs::context_t context;
bool exec_by_step=false;
std::size_t current_level=0;
bool step_into_function=false;
cs::set_t<std::size_t> breakpoints;
cs::map_t<std::string, callback_t> func_map;

bool covscript_debugger(bool last=false)
{
	std::string cmd, func, args;
	step_into_function=false;
	if(!last) {
		std::cout<<"> "<<std::flush;
		std::getline(std::cin, cmd);
	}
	else
		cmd=last_cmd;
	std::size_t posit=0;
	for(; posit<cmd.size(); ++posit)
		if(std::isspace(cmd[posit]))
			break;
		else
			func.push_back(cmd[posit]);
	for(; posit<cmd.size(); ++posit)
		if(!std::isspace(cmd[posit]))
			break;
	for(; posit<cmd.size(); ++posit)
		args.push_back(cmd[posit]);
	if(func.empty()&&args.empty()&&!last_cmd.empty())
		return covscript_debugger(true);
	if(func.empty())
		return true;
	if(func_map.count(func)==0) {
		std::cout<<"Undefined command: \""<<func<<"\". Try \"help\"."<<std::endl;
		return true;
	}
	else
	{
		last_cmd=cmd;
		return func_map[func](args);
	}
}

void cs_debugger_step_callback(cs::statement_base *stmt)
{
	if(!exec_by_step&&stmt->get_file_path()==path&&breakpoints.count(stmt->get_line_num())>0) {
		std::cout<<"\nHit breakpoint, at "<<stmt->get_file_path()<<":"<<stmt->get_line_num()<<std::endl;
		current_level=context->instance->fcall_stack.size();
		exec_by_step=true;
	}
	if(exec_by_step&&(step_into_function?true:context->instance->fcall_stack.size()<=current_level)) {
		std::cout<<stmt->get_line_num()<<"\t"<<stmt->get_raw_code()<<std::endl;
		while(covscript_debugger());
	}
}

cs::array split(const std::string &str)
{
	cs::array arr;
	std::string buf;
	for (auto &ch:str) {
		if (std::isspace(ch)) {
			if (!buf.empty()) {
				arr.push_back(buf);
				buf.clear();
			}
		}
		else
			buf.push_back(ch);
	}
	if (!buf.empty())
		arr.push_back(buf);
	return std::move(arr);
}

void covscript_main(int args_size, const char *args[])
{
	if (args_size > 1) {
		int index = covscript_args(args_size, args);
		cs::current_process->import_path += cs::path_delimiter + cs::get_import_path();
		if (show_help_info) {
			std::cout << "Usage: cs_dbg [options...] <FILE> [arguments...]\n" << "Options:\n";
			std::cout << "    Option               Mnemonic   Function\n";
			std::cout << "  --help                -h          Show help infomation\n";
			std::cout << "  --version             -v          Show version infomation\n";
			std::cout << "  --wait-before-exit    -w          Wait before process exit\n";
			std::cout << "  --log-path    <PATH>  -l <PATH>   Set the log and AST exporting path\n";
			std::cout << "  --import-path <PATH>  -i <PATH>   Set the import path\n";
			std::cout << std::endl;
			return;
		}
		else if (show_version_info) {
			std::cout << "Covariant Script Programming Language Debugger\n";
			std::cout << "Version: " << cs::current_process->version << "\n";
			std::cout << "Copyright (C) 2018 Michael Lee.All rights reserved.\n";
			std::cout << "Licensed under the Apache License, Version 2.0 (the \"License\");\n";
			std::cout << "you may not use this file except in compliance with the License.\n";
			std::cout << "You may obtain a copy of the License at\n";
			std::cout << "\nhttp://www.apache.org/licenses/LICENSE-2.0\n";
			std::cout << "\nUnless required by applicable law or agreed to in writing, software\n";
			std::cout << "distributed under the License is distributed on an \"AS IS\" BASIS,\n";
			std::cout << "WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.\n";
			std::cout << "See the License for the specific language governing permissions and\n";
			std::cout << "limitations under the License.\n";
			std::cout << "Please visit http://covscript.org for more information.\n";
			std::cout << "\nMetadata:\n";
			std::cout << "  Standard Version  STD" << cs::current_process->std_version << "\n";
			std::cout << "  Import Path       " << cs::current_process->import_path << "\n";
#ifdef COVSCRIPT_PLATFORM_WIN32
			std::cout << "  Platform          Win32\n";
#else
			std::cout << "  Platform          Unix\n";
#endif
			std::cout << std::endl;
			return;
		}
		if (index == args_size)
			throw cs::fatal_error("no input file.");
		if (args_size-index>1)
			throw cs::fatal_error("argument syntax error.");
		path = cs::process_path(args[index]);
		cs::array
		arg;
		for (; index < args_size; ++index)
			arg.emplace_back(cs::var::make_constant<cs::string>(args[index]));
		;
		func_map.emplace("next", [](const std::string& cmd)->bool {
			return false;
		});
		func_map.emplace("step", [](const std::string& cmd)->bool {
			step_into_function=true;
			return false;
		});
		func_map.emplace("continue", [](const std::string& cmd)->bool {
			exec_by_step=false;
			return false;
		});
		func_map.emplace("break", [](const std::string& cmd)->bool {
			breakpoints.emplace(std::stoul(cmd));
			return true;
		});
		func_map.emplace("optimizer", [](const std::string& cmd)->bool {
			if(cmd=="on")
				no_optimize=false;
			else if(cmd=="off")
				no_optimize=true;
			else
				std::cout<<"Invalid option: \""<<cmd<<"\". Use \"on\" or \"off\"."<<std::endl;
			return true;
		});
		func_map.emplace("run", [](const std::string& cmd)->bool {
			context = cs::create_context(split(cmd));
			context->compiler->disable_optimizer = no_optimize;
			context->instance->compile(path);
			context->instance->interpret();
			return true;
		});
		while(covscript_debugger());
	}
	else
		throw cs::fatal_error("no input file.");
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