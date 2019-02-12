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
* Copyright (C) 2019 Michael Lee(李登淳)
* Email: mikecovlee@163.com
* Github: https://github.com/mikecovlee
* Website: http://covscript.org
*/
#define CS_DEBUGGER

#include <covscript_impl/console/conio.hpp>
#include <covscript_impl/variant.hpp>
#include <covscript/covscript.hpp>
#include <iostream>
#include <chrono>

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

class breakpoint_recorder final {
	struct breakpoint final {
		std::size_t id = 0;
		variant_impl::variant<std::size_t, cs::var> data;

		template<typename T>
		breakpoint(std::size_t _id, T &&_data):id(_id), data(std::forward<T>(_data)) {}
	};

	static std::size_t m_id;
	std::forward_list<breakpoint> m_breakpoints;
public:

	breakpoint_recorder() = default;

	std::size_t add_line(std::size_t line_num)
	{
		m_breakpoints.emplace_front(++m_id, line_num);
		return m_id;
	}

	std::size_t add_func(cs::var function)
	{
		if (function.type() == typeid(cs::object_method))
			function = function.const_val<cs::object_method>().callable;
		else if (function.type() != typeid(cs::callable))
			throw cs::lang_error("Debugger just can break at specific line or function.");
		const cs::callable::function_type &target = function.const_val<cs::callable>().get_raw_data();
		if (target.target_type() != typeid(cs::function))
			throw cs::lang_error("Debugger can not break at CNI function.");
		target.target<cs::function>()->set_debugger_state(true);
		m_breakpoints.emplace_front(++m_id, function);
		return m_id;
	}

	void remove(std::size_t id)
	{
		m_breakpoints.remove_if([id](const breakpoint &b) -> bool {
			if (b.id == id && b.data.type() == typeid(cs::var))
				b.data.get<cs::var>().const_val<cs::callable>().get_raw_data().target<cs::function>()->set_debugger_state(
				    false);
			return b.id == id;
		});
	}

	bool exist(std::size_t line_num) const
	{
		for (auto &b:m_breakpoints)
			if (b.data.type() == typeid(std::size_t) && b.data.get<std::size_t>() == line_num)
				return true;
		return false;
	}

	void list() const
	{
		std::cout << "ID\tBreakpoint\n" << std::endl;
		for (auto &b:m_breakpoints) {
			std::cout << b.id << "\t";
			if (b.data.type() == typeid(cs::var)) {
				const cs::function *func = b.data.get<cs::var>().const_val<cs::callable>().get_raw_data().target<cs::function>();
				std::cout << "line " << func->get_raw_statement()->get_line_num() << ", " << func->get_declaration()
				          << std::endl;
			}
			else
				std::cout << "line " << b.data.get<std::size_t>() << std::endl;
		}
	}
};

std::size_t time()
{
	static std::chrono::time_point<std::chrono::high_resolution_clock> timer(std::chrono::high_resolution_clock::now());
	return std::chrono::duration_cast<std::chrono::milliseconds>(
	           std::chrono::high_resolution_clock::now() - timer).count();
}

std::size_t breakpoint_recorder::m_id = 0;

using callback_t=std::function<bool(const std::string &)>;
std::string path;
cs::context_t context;
bool exec_by_step = false;
std::size_t current_level = 0;
bool step_into_function = false;
breakpoint_recorder breakpoints;
cs::map_t<std::string, callback_t> func_map;

bool covscript_debugger()
{
	std::string cmd, func, args;
	step_into_function = false;
	std::cout << "> " << std::flush;
	std::getline(std::cin, cmd);
	std::size_t posit = 0;
	for (; posit < cmd.size(); ++posit)
		if (std::isspace(cmd[posit]))
			break;
		else
			func.push_back(cmd[posit]);
	for (; posit < cmd.size(); ++posit)
		if (!std::isspace(cmd[posit]))
			break;
	for (; posit < cmd.size(); ++posit)
		args.push_back(cmd[posit]);
	if (func.empty())
		return true;
	if (func_map.count(func) == 0) {
		std::cout << "Undefined command: \"" << func << "\". Try \"help\"." << std::endl;
		return true;
	}
	else
		return func_map[func](args);
}

void cs_debugger_step_callback(cs::statement_base *stmt)
{
	if (!exec_by_step && stmt->get_file_path() == path && breakpoints.exist(stmt->get_line_num())) {
		std::cout << "\nHit breakpoint, at \"" << stmt->get_file_path() << "\", line " << stmt->get_line_num()
		          << std::endl;
		current_level = context->instance->fcall_stack.size();
		exec_by_step = true;
	}
	if (exec_by_step && (step_into_function ? true : context->instance->fcall_stack.size() <= current_level)) {
		std::cout << stmt->get_line_num() << "\t" << stmt->get_raw_code() << std::endl;
		while (covscript_debugger());
	}
}

void cs_debugger_func_callback(const std::string &decl, cs::statement_base *stmt)
{
	std::cout << "\nHit breakpoint, at \"" << stmt->get_file_path() << "\", line " << stmt->get_line_num() << ", "
	          << decl << std::endl;
	current_level = context->instance->fcall_stack.size();
	exec_by_step = true;
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
			std::cout << "Usage: cs_dbg [options...] <FILE>\n" << "Options:\n";
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
			std::cout << "Copyright (C) 2019 Michael Lee.All rights reserved.\n";
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
		if (args_size - index > 1)
			throw cs::fatal_error("argument syntax error.");
		std::cout << "Covariant Script Programming Language Debugger\nVersion: " << cs::current_process->version
		          << "\n"
		          "Copyright (C) 2019 Michael Lee. All rights reserved.\n"
		          "Please visit <http://covscript.org/> for more information."
		          << std::endl;
		path = cs::process_path(args[index]);
		cs::array
		arg;
		for (; index < args_size; ++index)
			arg.emplace_back(cs::var::make_constant<cs::string>(args[index]));
		func_map.emplace("quit", [](const std::string &cmd) -> bool {
			std::exit(0);
			return false;
		});
		func_map.emplace("help", [](const std::string &cmd) -> bool {
			std::cout << "Command                   Function\n\n";
			std::cout << "quit                      Exit the debugger\n";
			std::cout << "help                      Show help infomation\n";
			std::cout << "next                      Execute next statement\n";
			std::cout << "step                      Execute next statement and step into function\n";
			std::cout << "continue                  Continue execute program until next breakpint gets hit\n";
			std::cout << "backtrace                 Show stack backtrace\n";
			std::cout << "break [line|function]     Set breakpoint at specific line or function\n";
			std::cout << "lsbreak                   List all breakpoints\n";
			std::cout << "rmbreak [id]              Remove specific breakpoint\n";
			std::cout << "print [expression]        Evaluate the value of expression\n";
			std::cout << "optimizer [on|off]        Turn on or turn off the optimizer\n";
			std::cout << "run <...>                 Run program with specific arguments\n";
			std::cout << std::endl;
			return true;
		});
		func_map.emplace("next", [](const std::string &cmd) -> bool {
			if (context.get() == nullptr)
				throw cs::runtime_error("Please launch a interpreter instance first.");
			return false;
		});
		func_map.emplace("step", [](const std::string &cmd) -> bool {
			if (context.get() == nullptr)
				throw cs::runtime_error("Please launch a interpreter instance first.");
			step_into_function = true;
			return false;
		});
		func_map.emplace("continue", [](const std::string &cmd) -> bool {
			if (context.get() == nullptr)
				throw cs::runtime_error("Please launch a interpreter instance first.");
			exec_by_step = false;
			return false;
		});
		func_map.emplace("backtrace", [](const std::string &cmd) -> bool {
			if (context.get() == nullptr)
				throw cs::runtime_error("Please launch a interpreter instance first.");
			for (auto &func:context->instance->stack_backtrace)
				std::cout << func << std::endl;
			return true;
		});
		func_map.emplace("break", [](const std::string &cmd) -> bool {
			bool is_line = true;
			for (auto &ch:cmd)
			{
				if (!std::isspace(ch) && !std::isdigit(ch)) {
					is_line = false;
					break;
				}
			}
			if (!is_line)
			{
				if (context.get() == nullptr)
					throw cs::runtime_error("Please launch a interpreter instance first.");
				std::deque<char> buff;
				cs::expression_t tree;
				for (auto &ch:cmd)
					buff.push_back(ch);
				context->compiler->build_expr(buff, tree);
				breakpoints.add_func(context->instance->parse_expr(tree.root()));
			}
			else
				breakpoints.add_line(std::stoul(cmd));
			return true;
		});
		func_map.emplace("lsbreak", [](const std::string &cmd) -> bool {
			breakpoints.list();
			return true;
		});
		func_map.emplace("rmbreak", [](const std::string &cmd) -> bool {
			breakpoints.remove(std::stoul(cmd));
			return true;
		});
		func_map.emplace("optimizer", [](const std::string &cmd) -> bool {
			if (context.get() != nullptr)
				throw cs::runtime_error("Can not tuning optimizer while interpreter is running.");
			if (cmd == "on")
				no_optimize = false;
			else if (cmd == "off")
				no_optimize = true;
			else
				std::cout << "Invalid option: \"" << cmd << "\". Use \"on\" or \"off\"." << std::endl;
			return true;
		});
		func_map.emplace("run", [](const std::string &cmd) -> bool {
			if (context.get() != nullptr)
				throw cs::runtime_error("Can not run two or more instance at the same time.");
			context = cs::create_context(split(cmd));
			context->compiler->disable_optimizer = no_optimize;
			context->instance->compile(path);
			std::cout << "Launching new interpreter instance..." << std::endl;
			std::size_t start_time = time();
			context->instance->interpret();
			std::cout << "The interpreter instance has exited normally, up to " << time() - start_time << "ms."
			          << std::endl;
			return true;
		});
		func_map.emplace("print", [](const std::string &cmd) -> bool {
			if (context.get() == nullptr)
				throw cs::runtime_error("Please launch a interpreter instance first.");
			std::deque<char> buff;
			cs::expression_t tree;
			for (auto &ch:cmd)
				buff.push_back(ch);
			context->compiler->build_expr(buff, tree);
			std::cout << context->instance->parse_expr(tree.root()) << std::endl;
			return true;
		});
		std::ofstream log_stream;
		while (std::cin) {
			try {
				covscript_debugger();
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