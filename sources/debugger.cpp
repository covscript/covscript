/*
* Covariant Script Programming Language Debugger
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
* Copyright (C) 2017-2025 Michael Lee(李登淳)
*
* This software is registered with the National Copyright Administration
* of the People's Republic of China(Registration Number: 2020SR0408026)
* and is protected by the Copyright Law of the People's Republic of China.
*
* Email:   lee@covariant.cn, mikecovlee@163.com
* Github:  https://github.com/mikecovlee
* Website: http://covscript.org.cn
*/
#define CS_DEBUGGER

#include <covscript_impl/variant.hpp>
#include <covscript_impl/system.hpp>
#include <covscript/covscript.hpp>
#include <iostream>
#include <chrono>

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
std::string csym_path;
bool silent = false;
bool no_optimize = false;
bool show_help_info = false;
bool wait_before_exit = false;
bool show_version_info = false;

int covscript_args(int args_size, char *args[])
{
	int expect_csym = 0;
	int expect_log_path = 0;
	int expect_import_path = 0;
	int expect_stack_resize = 0;
	int index = 1;
	for (; index < args_size; ++index) {
		if (expect_csym == 1) {
			csym_path = cs::process_path(args[index]);
			expect_csym = 2;
		}
		else if (expect_log_path == 1) {
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
			if ((std::strcmp(args[index], "--help") == 0 || std::strcmp(args[index], "-h") == 0) &&
			        !show_help_info)
				show_help_info = true;
			else if ((std::strcmp(args[index], "--silent") == 0 || std::strcmp(args[index], "-s") == 0) && !silent)
				silent = true;
			else if ((std::strcmp(args[index], "--version") == 0 || std::strcmp(args[index], "-v") == 0) &&
			         !show_version_info)
				show_version_info = true;
			else if ((std::strcmp(args[index], "--wait-before-exit") == 0 || std::strcmp(args[index], "-w") == 0) &&
			         !wait_before_exit)
				wait_before_exit = true;
			else if ((std::strcmp(args[index], "--csym") == 0 || std::strcmp(args[index], "-g") == 0) &&
			         expect_csym == 0)
				expect_csym = 1;
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
	if (expect_csym == 1 || expect_log_path == 1 || expect_import_path == 1 || expect_import_path == 1)
		throw cs::fatal_error("argument syntax error.");
	return index;
}

class breakpoint_recorder final {
	struct breakpoint final {
		std::size_t id = 0;
		variant_impl::variant<std::size_t, std::string, cs::var> data;

		template<typename T>
		breakpoint(std::size_t _id, T &&_data):id(_id), data(std::forward<T>(_data)) {}
	};

	static std::size_t m_id;
	std::forward_list<breakpoint> m_breakpoints;
	cs::map_t<std::string, std::pair<std::size_t, bool>> m_pending;
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
			throw cs::runtime_error("Debugger just can break at specific line or function.");
		const cs::callable::function_type &target = function.const_val<cs::callable>().get_raw_data();
		if (target.target_type() != typeid(cs::function))
			throw cs::runtime_error("Debugger can not break at CNI function.");
		target.target<cs::function>()->set_debugger_state(true);
		m_breakpoints.emplace_front(++m_id, function);
		return m_id;
	}

	std::size_t add_pending(const std::string &func)
	{
		m_breakpoints.emplace_front(++m_id, func);
		m_pending.emplace(func, std::pair<std::size_t, bool>(m_id, true));
		return m_id;
	}

	void replace_pending(const std::string &name, const cs::var &function)
	{
		if (m_pending.count(name) > 0) {
			const cs::callable::function_type &target = function.const_val<cs::callable>().get_raw_data();
			target.target<cs::function>()->set_debugger_state(true);
			auto key = m_pending.find(name);
			if (key->second.second) {
				for (auto &it: m_breakpoints) {
					if (it.id == key->second.first) {
						it.data.emplace<cs::var>(function);
						key->second.second = false;
						break;
					}
				}
			}
		}
	}

	void remove(std::size_t id)
	{
		m_breakpoints.remove_if([this, id](const breakpoint &b) -> bool {
			if (b.id == id && b.data.type() == typeid(cs::var))
				b.data.get<cs::var>().const_val<cs::callable>().get_raw_data().target<cs::function>()->set_debugger_state(
				    false);
			else if (b.id == id && b.data.type() == typeid(std::string))
				m_pending.erase(m_pending.find(b.data.get<std::string>()));
			return b.id == id;
		});
		auto it = m_pending.begin();
		for (; it != m_pending.end(); ++it)
			if (it->second.first == id)
				break;
		if (it != m_pending.end())
			m_pending.erase(it);
	}

	bool exist(std::size_t line_num) const
	{
		for (auto &b: m_breakpoints)
			if (b.data.type() == typeid(std::size_t) && b.data.get<std::size_t>() == line_num)
				return true;
		return false;
	}

	void list() const
	{
		std::cout << "ID\tBreakpoint\n" << std::endl;
		for (auto &b: m_breakpoints) {
			std::cout << b.id << "\t";
			if (b.data.type() == typeid(cs::var)) {
				auto func = b.data.get<cs::var>().const_val<cs::callable>().get_raw_data().target<cs::function>();
				std::cout << "line " << func->get_raw_statement()->get_line_num() << ", " << func->get_declaration()
				          << std::endl;
			}
			else if (b.data.type() == typeid(std::string))
				std::cout << "\"" << b.data.get<std::string>() << "\"(pending)" << std::endl;
			else
				std::cout << "line " << b.data.get<std::size_t>() << std::endl;
		}
	}

	void reset()
	{
		for (auto &it: m_pending) {
			it.second.second = true;
			for (auto &b: m_breakpoints) {
				if (b.id == it.second.first) {
					b.data.emplace<std::string>(it.first);
					break;
				}
			}
		}
	}
};

using callback_t = std::function<bool(const std::string &)>;

class function_map_t final {
	cs::map_t<std::string, callback_t> m_map;
public:
	function_map_t() = default;

	template<typename T>
	void add_func(const std::string &name, const std::string &shortcut, T &&func)
	{
		m_map.emplace(name, std::forward<T>(func));
		m_map.emplace(shortcut, std::forward<T>(func));
	}

	bool exist(const std::string &name)
	{
		return m_map.count(name) > 0;
	}

	bool call(const std::string &name, const std::string &cmd)
	{
		return m_map.at(name)(cmd);
	}
};

std::size_t time()
{
	static std::chrono::time_point<std::chrono::high_resolution_clock> timer(std::chrono::high_resolution_clock::now());
	return std::chrono::duration_cast<std::chrono::milliseconds>(
	           std::chrono::high_resolution_clock::now() - timer).count();
}

std::size_t breakpoint_recorder::m_id = 0;

std::string path;
cs::context_t context;
std::ofstream log_stream;

bool quit_sig = false;
bool exec_by_step = false;
std::size_t current_level = 0;
bool step_into_function = false;

function_map_t func_map;
breakpoint_recorder breakpoints;

void reset_status()
{
	exec_by_step = false;
	current_level = 0;
	step_into_function = false;
	breakpoints.reset();
}

bool covscript_debugger()
{
	std::string cmd, func, args;
#ifdef COVSCRIPT_PLATFORM_WIN32
	std::cout << "> " << std::flush;
	// Workaround: https://stackoverflow.com/a/26763490
	while (true) {
		cs::current_process->poll_event();
		std::getline(std::cin, cmd);
		if (std::cin)
			break;
	}
#else
	if (!std::cin) {
		int code = 0;
		cs::process_context::on_process_exit_default_handler(&code);
	}
	std::cout << "> " << std::flush;
	std::getline(std::cin, cmd);
	cs::current_process->poll_event();
#endif
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
	if (!func_map.exist(func)) {
		std::cout << "Undefined command: \"" << func << R"(". Try "help".)" << std::endl;
		return true;
	}
	else {
		try {
			return func_map.call(func, args);
		}
		catch (const std::exception &e) {
			if (context.get() == nullptr) {
				if (!log_path.empty()) {
					if (!log_stream.is_open())
						log_stream.open(::log_path);
					if (log_stream)
						log_stream << e.what() << std::endl;
					else
						std::cerr << "Write log failed." << std::endl;
				}
				std::cerr << e.what() << std::endl;
				return true;
			}
			else
				throw;
		}
	}
}

void cs_debugger_step_callback(cs::statement_base *stmt)
{
	if (stmt->get_file_path() != path)
		return;
	if (context->compiler->csyms.count(path) > 0) {
		cs::csym_info &csym = context->compiler->csyms[path];
		std::size_t current_line = stmt->get_line_num();
		if (current_line >= csym.map.size())
			return;
		std::size_t actual_line = csym.map[current_line - 1];
		if (actual_line >= csym.codes.size() || actual_line == 0)
			return;
		const std::string &code = csym.codes[actual_line - 1];
		if (!exec_by_step && breakpoints.exist(actual_line)) {
			std::cout << "\nHit breakpoint, at \"" << csym.file << "\", line " << actual_line
			          << std::endl;
			current_level = cs::current_process->stack.size();
			exec_by_step = true;
		}
		if (exec_by_step && (step_into_function || cs::current_process->stack.size() <= current_level)) {
			std::cout << actual_line << "\t" << code << std::endl;
			current_level = cs::current_process->stack.size();
			step_into_function = false;
			while (covscript_debugger());
		}
	}
	else {
		if (!exec_by_step && breakpoints.exist(stmt->get_line_num())) {
			std::cout << "\nHit breakpoint, at \"" << stmt->get_file_path() << "\", line " << stmt->get_line_num()
			          << std::endl;
			current_level = cs::current_process->stack.size();
			exec_by_step = true;
		}
		if (exec_by_step && (step_into_function || cs::current_process->stack.size() <= current_level)) {
			std::cout << stmt->get_line_num() << "\t" << stmt->get_raw_code() << std::endl;
			current_level = cs::current_process->stack.size();
			step_into_function = false;
			while (covscript_debugger());
		}
	}
}

void cs_debugger_func_breakpoint(const std::string &name, const cs::var &func)
{
	breakpoints.replace_pending(name, func);
}

void cs_debugger_func_callback(const std::string &decl, cs::statement_base *stmt)
{
	if (context->compiler->csyms.count(stmt->get_file_path()) > 0) {
		cs::csym_info &csym = context->compiler->csyms[stmt->get_file_path()];
		std::size_t current_line = stmt->get_line_num();
		if (current_line >= csym.map.size())
			return;
		std::size_t actual_line = csym.map[current_line - 1];
		if (actual_line >= csym.codes.size() || actual_line == 0)
			return;
		std::cout << "\nHit breakpoint, at \"" << csym.file << "\", line " << actual_line << ", " << decl << std::endl;
	}
	else
		std::cout << "\nHit breakpoint, at \"" << stmt->get_file_path() << "\", line " << stmt->get_line_num() << ", " << decl << std::endl;
	current_level = cs::current_process->stack.size();
	exec_by_step = true;
}

cs::array split(const std::string &str)
{
	cs::array arr{path};
	std::string buf;
	for (auto &ch: str) {
		if (std::isspace(ch)) {
			if (!buf.empty()) {
				arr.emplace_back(buf);
				buf.clear();
			}
		}
		else
			buf.push_back(ch);
	}
	if (!buf.empty())
		arr.emplace_back(buf);
	return std::move(arr);
}

void covscript_main(int args_size, char *args[])
{
	if (args_size > 1) {
		int index = covscript_args(args_size, args);
		cs::current_process->import_path += cs::path_delimiter + cs::get_import_path();
		if (show_help_info) {
			std::cout << "Usage: cs_dbg [options...] <FILE>\n" << "Options:\n";
			std::cout << "    Option                Mnemonic   Function\n";
			std::cout << "  --help                 -h          Show help infomation\n";
			std::cout << "  --silent               -s          Close the command prompt\n";
			std::cout << "  --version              -v          Show version infomation\n";
			std::cout << "  --wait-before-exit     -w          Wait before process exit\n";
			std::cout << "  --csym         <FILE>  -g <FILE>   Read cSYM from file\n";
			std::cout << "  --stack-resize <SIZE>  -S <SIZE>   Reset the size of runtime stack\n";
			std::cout << "  --log-path     <PATH>  -l <PATH>   Set the log path\n";
			std::cout << "  --import-path  <PATH>  -i <PATH>   Set the import path\n";
			std::cout << std::endl;
			return;
		}
		else if (show_version_info) {
			std::cout << "Covariant Script Programming Language Debugger\n";
			std::cout << "Version: " << cs::current_process->version << std::endl;
			std::cout << cs::copyright_info << std::endl;
			std::cout << "\nMetadata:\n";
			std::cout << "  Import Path: " << cs::current_process->import_path << "\n";
			std::cout << "  STD Version: " << cs::current_process->std_version << "\n";
			std::cout << "  API Version: " << CS_GET_VERSION_STR(COVSCRIPT_API_VERSION) << "\n";
			std::cout << "  ABI Version: " << CS_GET_VERSION_STR(COVSCRIPT_ABI_VERSION) << "\n";
			std::cout << "  Runtime Env: " << COVSCRIPT_PLATFORM_NAME << "-" << COVSCRIPT_ARCH_NAME "\n";
			std::cout << "  Compile Env: " << COVSCRIPT_COMPILER_NAME << "\n";
			std::cout << std::endl;
			return;
		}
		if (index == args_size)
			throw cs::fatal_error("no input file.");
		if (args_size - index > 1)
			throw cs::fatal_error("argument syntax error.");
		path = cs::process_path(args[index]);
		if (!cs_impl::file_system::exist(path) || cs_impl::file_system::is_dir(path) ||
		        !cs_impl::file_system::can_read(path))
			throw cs::fatal_error("invalid input file.");
		cs::prepend_import_path(path, cs::current_process);
		if (!silent) {
			std::cout << "Covariant Script Programming Language Debugger\nVersion: "
			          << cs::current_process->version << " [" << COVSCRIPT_COMPILER_NAME << " on " << COVSCRIPT_PLATFORM_NAME << "]\n"
			                                   "Copyright (C) 2017-2025 Michael Lee. All rights reserved.\n"
			                                   "Please visit <http://covscript.org.cn/> for more information."
			          << std::endl;
		}
		cs::current_process->on_process_exit.add_listener([](void *code) -> bool {
			cs::current_process->exit_code = *static_cast<int *>(code);
			throw cs::fatal_error("CS_DEBUGGER_EXIT");
		});
		cs::current_process->on_process_sigint.add_listener([](void *) -> bool {
			throw cs::fatal_error("CS_SIGINT");
		});
		cs::current_process->on_process_sigint.add_listener([](void *) -> bool {
			std::cin.clear();
			return false;
		});
		func_map.add_func("quit", "q", [](const std::string &cmd) -> bool {
			if (context.get() != nullptr)
			{
				std::cout
				        << "An interpreter instance is running, do you really want to quit?\nPress (y) to confirm or press any other key to cancel."
				        << std::endl;
				while (!cs_impl::conio::kbhit());
				switch (std::tolower(cs_impl::conio::getch())) {
				case 'y': {
					quit_sig = true;
					int code = 0;
					cs::current_process->on_process_exit.touch(&code);
					return false;
				}
				default:
					return true;
				}
			}
			return false;
		});
		func_map.add_func("help", "h", [](const std::string &cmd) -> bool {
			std::cout << "Command           Shortcut    Function\n\n";
			std::cout << "quit                     q    Exit the debugger\n";
			std::cout << "help                     h    Show help infomation\n";
			std::cout << "next                     n    Execute next statement\n";
			std::cout << "step                     s    Execute next statement and step into function\n";
			std::cout << "continue                 c    Continue execute program until next breakpint gets hit\n";
			std::cout << "backtrace               bt    Show stack backtrace\n";
			std::cout << "break [line|function]    b    Set breakpoint at specific line or function\n";
			std::cout << "lsbreak                 lb    List all breakpoints\n";
			std::cout << "rmbreak [id]            rb    Remove specific breakpoint\n";
			std::cout << "print [expression]       p    Evaluate the value of expression\n";
			std::cout << "optimizer [on|off]       o    Turn on or turn off the optimizer\n";
			std::cout << "run <...>                r    Run program with specific arguments\n";
			std::cout << std::endl;
			return true;
		});
		func_map.add_func("next", "n", [](const std::string &cmd) -> bool {
			if (context.get() == nullptr)
				throw cs::runtime_error("Please launch a interpreter instance first.");
			return false;
		});
		func_map.add_func("step", "s", [](const std::string &cmd) -> bool {
			if (context.get() == nullptr)
				throw cs::runtime_error("Please launch a interpreter instance first.");
			step_into_function = true;
			return false;
		});
		func_map.add_func("continue", "c", [](const std::string &cmd) -> bool {
			if (context.get() == nullptr)
				throw cs::runtime_error("Please launch a interpreter instance first.");
			exec_by_step = false;
			return false;
		});
		func_map.add_func("backtrace", "bt", [](const std::string &cmd) -> bool {
			if (context.get() == nullptr)
				throw cs::runtime_error("Please launch a interpreter instance first.");
			for (auto &func: cs::current_process->stack_backtrace)
				std::cout << func << std::endl;
			std::cout << "function main()" << std::endl;
			return true;
		});
		func_map.add_func("break", "b", [](const std::string &cmd) -> bool {
			bool is_line = true;
			for (auto &ch: cmd)
			{
				if (!std::isspace(ch) && !std::isdigit(ch)) {
					is_line = false;
					break;
				}
			}
			std::size_t id = 0;
			std::string result = " has been set.";
			if (!is_line)
			{
				if (context.get() == nullptr) {
					cs::array arr = split(cmd);
					if (arr.size() != 2) {
						std::cout << "Invalid option: \"" << cmd << "\"" << std::endl;
						return true;
					}
					id = breakpoints.add_pending(arr.back().const_val<std::string>());
					result = " pending.";
				}
				else {
					std::deque<char> buff;
					cs::expression_t tree;
					for (auto &ch: cmd)
						buff.push_back(ch);
					context->compiler->build_expr(buff, tree);
					id = breakpoints.add_func(context->instance->parse_expr(tree.root()));
				}
			}
			else
			{
				try {
					id = breakpoints.add_line(std::stoul(cmd));
				}
				catch (...) {
					std::cout << "Invalid option: \"" << cmd << "\"" << std::endl;
					return true;
				}
			}
			std::cout << "Breakpoint " << id << result << std::endl;
			return true;
		});
		func_map.add_func("lsbreak", "lb", [](const std::string &cmd) -> bool {
			breakpoints.list();
			return true;
		});
		func_map.add_func("rmbreak", "rb", [](const std::string &cmd) -> bool {
			breakpoints.remove(std::stoul(cmd));
			return true;
		});
		func_map.add_func("optimizer", "o", [](const std::string &cmd) -> bool {
			if (context.get() != nullptr)
			{
				std::cout << "Runtime Error: Can not tuning optimizer while interpreter is running." << std::endl;
				return true;
			}
			if (cmd == "on")
				no_optimize = false;
			else if (cmd == "off")
				no_optimize = true;
			else
				std::cout << "Invalid option: \"" << cmd << R"(". Use "on" or "off".)" << std::endl;
			return true;
		});
		func_map.add_func("run", "r", [](const std::string &cmd) -> bool {
			if (context.get() != nullptr)
			{
				std::cout << "Runtime Error: Can not run two or more instance at the same time." << std::endl;
				return true;
			}
			std::size_t start_time = 0;
			try
			{
				cs::current_process->exit_code = 0;
				context = cs::create_context(split(cmd));
				context->compiler->disable_optimizer = no_optimize;
				// Reads cSYM
				if (!csym_path.empty())
					context->compiler->import_csym(path, csym_path);
				std::cout << "Compiling..." << std::endl;
				start_time = time();
				context->instance->compile(path);
				std::cout << "The compiler has exited normally, up to " << time() - start_time << "ms." << std::endl;
				std::cout << "Launching new interpreter instance..." << std::endl;
				start_time = time();
				context->instance->interpret();
			}
			catch (const std::exception &e)
			{
				if (std::strstr(e.what(), "CS_SIGINT") != nullptr) {
					cs::process_context::cleanup_context();
					activate_sigint_handler();
				}
				else if (std::strstr(e.what(), "CS_DEBUGGER_EXIT") == nullptr) {
					cs::collect_garbage(context);
					std::cerr
					        << "\nFatal Error: An exception was detected, the interpreter instance will terminate immediately."
					        << std::endl;
					std::cerr << "The interpreter instance has exited unexpectedly, up to " << time() - start_time
					          << "ms."
					          << std::endl;
					reset_status();
					throw;
				}
			}
			catch (...)
			{
				cs::collect_garbage(context);
				std::cerr
				        << "\nFatal Error: An exception was detected, the interpreter instance will terminate immediately."
				        << std::endl;
				std::cerr << "The interpreter instance has exited unexpectedly, up to " << time() - start_time << "ms."
				          << std::endl;
				reset_status();
				throw;
			}
			cs::collect_garbage(context);
			std::cout << "\nThe interpreter instance has exited normally with exit code "
			          << cs::current_process->exit_code << ", up to " << time() - start_time << "ms."
			          << std::endl;
			reset_status();
			return !quit_sig;
		});
		func_map.add_func("print", "p", [](const std::string &cmd) -> bool {
			if (context.get() == nullptr)
			{
				std::cout << "Runtime Error: Please launch a interpreter instance first." << std::endl;
				return true;
			}
			try
			{
				std::deque<char> buff;
				cs::expression_t tree;
				for (auto &ch: cmd)
					buff.push_back(ch);
				context->compiler->build_expr(buff, tree);
				std::cout << context->instance->parse_expr(tree.root()) << std::endl;
			}
			catch (std::exception &e)
			{
				if (std::strstr(e.what(), "CS_SIGINT") != nullptr ||
				        std::strstr(e.what(), "CS_DEBUGGER_EXIT") != nullptr)
					throw;
				std::cout << "Evaluation Failed: " << e.what() << std::endl;
			}
			return true;
		});
		activate_sigint_handler();
		for (bool result = true; result;) {
			try {
				result = covscript_debugger();
			}
			catch (const std::exception &e) {
				if (std::strstr(e.what(), "CS_SIGINT") != nullptr) {
					cs::process_context::cleanup_context();
					cs::collect_garbage(context);
					reset_status();
					activate_sigint_handler();
				}
				else
					throw;
			}
		}
	}
	else
		throw cs::fatal_error("no input file.");
}

int main(int args_size, char *args[])
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
