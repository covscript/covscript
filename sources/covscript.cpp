/*
* Covariant Script Programming Language
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
* Copyright (C) 2017-2023 Michael Lee(李登淳)
*
* This software is registered with the National Copyright Administration
* of the People's Republic of China(Registration Number: 2020SR0408026)
* and is protected by the Copyright Law of the People's Republic of China.
*
* Email:   lee@covariant.cn, mikecovlee@163.com
* Github:  https://github.com/mikecovlee
* Website: http://covscript.org.cn
*/
#include <covscript/impl/codegen.hpp>
#include <covscript_impl/system.hpp>
#include <covscript/covscript.hpp>

#ifdef COVSCRIPT_PLATFORM_WIN32

#include <shlobj.h>

#pragma comment(lib, "shell32.lib")

#else

#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>

#endif

#ifdef _MSC_VER
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif
#include <windows.h>
#include <Dbghelp.h>
#pragma comment(lib, "DbgHelp")
namespace cs_impl {
	std::string cxx_demangle(const char* name)
	{
		char buffer[1024];
		DWORD length = UnDecorateSymbolName(name, buffer, sizeof(buffer), 0);
		if (length > 0)
			return std::string(buffer, length);
		else
			return name;
	}
}
#elif defined __GNUC__

#include <cxxabi.h>

namespace cs_impl {
	std::string cxx_demangle(const char *name)
	{
		char buffer[1024] = {0};
		size_t size = sizeof(buffer);
		int status;
		char *ret = abi::__cxa_demangle(name, buffer, &size, &status);
		if (ret != nullptr)
			return std::string(ret);
		else
			return name;
	}
}
#endif

std::ostream &operator<<(std::ostream &out, const cs_impl::any &val)
{
	out << val.to_string();
	return out;
}

namespace cs_impl {
	default_allocator<any::proxy> any::allocator;
	cs::namespace_t member_visitor_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t except_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t array_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t array_iterator_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t number_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t char_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t math_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t math_const_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t list_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t list_iterator_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t hash_set_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t hash_map_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t pair_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t time_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t context_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t runtime_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t string_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t iostream_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t seekdir_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t openmode_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t charbuff_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t istream_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t ostream_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t system_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t console_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t file_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t path_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t path_type_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t path_info_ext = cs::make_shared_namespace<cs::name_space>();
}

namespace cs {
	void exception::relocate_to_csym(const csym_info &csym)
	{
		if (mLine >= csym.map.size())
			throw fatal_error("Invalid line when relocating symbols in cSYM.");
		std::size_t relocated_line = csym.map[mLine - 1];
		if (relocated_line >= csym.codes.size())
			throw fatal_error("Broken cSYM file.");
		if (relocated_line > 0) {
			const std::string &relocated_code = csym.codes[relocated_line - 1];
			mStr = compose_what(csym.file, relocated_line, relocated_code, mWhat);
		}
		else
			mStr = compose_what_internal(csym.file, mWhat);
	}

	void process_context::cleanup_context()
	{
		while (!current_process->stack.empty())
			current_process->stack.pop_no_return();
#ifdef CS_DEBUGGER
		while(!current_process->stack_backtrace.empty())
			current_process->stack_backtrace.pop_no_return();
#endif
	}

	bool process_context::on_process_exit_default_handler(void *code)
	{
		cleanup_context();
		collect_garbage();
		std::exit(*static_cast<int *>(code));
		return true;
	}

	std::unique_ptr<process_context> process_context::fork()
	{
		std::unique_ptr<process_context> new_process(new process_context(current_process->child_stack_size()));
		new_process->output_precision = current_process->output_precision;
		new_process->import_path = current_process->import_path;
		process_context *curr = current_process;
		new_process->on_process_exit.add_listener([curr](void *data) -> bool {
			return curr->on_process_exit.touch(data);
		});
		new_process->on_process_sigint.add_listener([curr](void *data) -> bool {
			return curr->on_process_sigint.touch(data);
		});
		new_process->std_eh_callback = current_process->std_eh_callback;
		new_process->cs_eh_callback = current_process->cs_eh_callback;
		return std::move(new_process);
	}

	process_context this_process;
	process_context *current_process = &this_process;

	std::size_t struct_builder::mCount = 0;

	void copy_no_return(var &val)
	{
		if (!val.is_rvalue()) {
			val.clone();
			val.detach();
		}
		else
			val.mark_as_rvalue(false);
	}

	var copy(var val)
	{
		if (!val.is_rvalue()) {
			val.clone();
			val.detach();
		}
		else
			val.mark_as_rvalue(false);
		return val;
	}

	var lvalue(const var &val)
	{
		val.mark_as_rvalue(false);
		return val;
	}

	var rvalue(const var &val)
	{
		val.mark_as_rvalue(true);
		return val;
	}

	var try_move(const var &val)
	{
		val.try_move();
		return val;
	}

	var make_namespace(const namespace_t &ns)
	{
		return var::make_protect<namespace_t>(ns);
	}

	numeric parse_number(const std::string &str)
	{
		try {
			if (str.find('.') != std::string::npos)
				return std::stold(str);
			else
				return std::stoll(str);
		}
		catch (const std::exception &e) {
			throw lang_error("Wrong literal format.");
		}
	}

	garbage_collector<cov::dll> extension::gc;

	garbage_collector<token_base> token_base::gc;

	garbage_collector<statement_base> statement_base::gc;

	garbage_collector<method_base> method_base::gc;

	std::string process_path(const std::string &raw)
	{
		auto pos0 = raw.find('\"');
		auto pos1 = raw.rfind('\"');
		if (pos0 != std::string::npos) {
			if (pos0 == pos1)
				throw cs::fatal_error("argument syntax error.");
			else
				return raw.substr(pos0 + 1, pos1 - pos0 - 1);
		}
		else
			return raw;
	}

#ifdef COVSCRIPT_PLATFORM_WIN32

	std::string get_sdk_path()
	{
#ifdef COVSCRIPT_HOME
		return COVSCRIPT_HOME;
#else
		const char *sdk_path = std::getenv("COVSCRIPT_HOME");
		if (sdk_path == nullptr) {
			CHAR path[MAX_PATH];
			SHGetFolderPathA(nullptr, CSIDL_PERSONAL, nullptr, SHGFP_TYPE_CURRENT, path);
			return process_path(std::string(path) + "\\CovScript");
		}
		else
			return process_path(sdk_path);
#endif
	}

#else

	std::string get_sdk_path()
	{
#ifdef COVSCRIPT_HOME
		return COVSCRIPT_HOME;
#else
		const char *sdk_path = std::getenv("COVSCRIPT_HOME");
		if (sdk_path == nullptr) {
			struct passwd *pw = getpwuid(getuid());
			return process_path(std::string(pw->pw_dir) + "/.covscript");
		}
		else
			return process_path(sdk_path);
#endif
	}

#endif

	std::string get_import_path()
	{
		const char *import_path = std::getenv("CS_IMPORT_PATH");
		std::string base_path = get_sdk_path() + cs::path_separator + "imports";
		base_path += cs::path_delimiter + std::string(COVSCRIPT_PLATFORM_HOME) + cs::path_separator + "imports";
		if (import_path != nullptr)
			return process_path(std::string(import_path) + cs::path_delimiter + base_path);
		else
			return process_path(base_path);
	}

	void prepend_import_path(const std::string &script, cs::process_context *context)
	{
		if (script.empty()) {
			return;
		}

		if (cs_impl::file_system::is_absolute_path(script)) {
			// If it's absolute path
			auto pos = script.find_last_of(cs::path_separator);

			// in case of: /main.csc
			if (pos > 0) {
				context->import_path = script.substr(0, pos)
				                       + cs::path_delimiter + context->import_path;
			}
			else {
				context->import_path = std::to_string(cs::path_separator)
				                       + cs::path_delimiter + context->import_path;
			}

		}
		else {
			// If it's relative path
			prepend_import_path(cs_impl::file_system::get_current_dir()
			                    + cs::path_separator
			                    + script, context);
		}
	}

	array parse_cmd_args(int argc, char *argv[])
	{
		cs::array arg;
		for (std::size_t i = 0; i < argc; ++i)
			arg.emplace_back(cs::var::make_constant<cs::string>(argv[i]));
		return std::move(arg);
	}

// Internal Functions

	var range(vector &args)
	{
		switch (args.size()) {
		case 1:
			cs_impl::check_args<numeric>(args);
			return var::make_constant<range_type>(0, args[0].const_val<numeric>(), 1);
		case 2:
			cs_impl::check_args<numeric, numeric>(args);
			return var::make_constant<range_type>(args[0].const_val<numeric>(), args[1].const_val<numeric>(), 1);
		case 3:
			cs_impl::check_args<numeric, numeric, numeric>(args);
			return var::make_constant<range_type>(args[0].const_val<numeric>(), args[1].const_val<numeric>(),
			                                      args[2].const_val<numeric>());
		default:
			throw cs::runtime_error(
			    "Wrong size of the arguments. Expected 1, 2 or 3, provided " + std::to_string(args.size()));
		}
	}

	numeric to_integer(const var &val)
	{
		return val.to_integer();
	}

	string to_string(const var &val)
	{
		return val.to_string();
	}

	string type(const var &a)
	{
		return a.get_type_name();
	}

	var clone(const var &val)
	{
		return copy(val);
	}

	var move(const var &val)
	{
		return rvalue(val);
	}

	void swap(var &a, var &b)
	{
		a.swap(b, true);
	}

	context_t create_context(const array &args)
	{
		cs_impl::init_extensions();
		context_t context = std::make_shared<context_type>();
		context->compiler = std::make_shared<compiler_type>(context);
		context->instance = std::make_shared<instance_type>(context, current_process->stack_size);
		context->cmd_args = cs::var::make_constant<cs::array>(args);
		// Init Grammars
		(*context->compiler)
		// Expression Grammar
		.add_method({new token_expr(tree_type<token_base *>()), new token_endline(0)},
		new method_expression)
		// Import Grammar
		.add_method({new token_action(action_types::import_), new token_expr(tree_type<token_base *>()),
			            new token_endline(0)}, new method_import)
		.add_method({new token_action(action_types::import_), new token_expr(tree_type<token_base *>()),
			            new token_action(action_types::as_), new token_expr(tree_type<token_base *>()),
			            new token_endline(0)}, new method_import_as)
		// Package Grammar
		.add_method({new token_action(action_types::package_), new token_expr(tree_type<token_base *>()),
			            new token_endline(0)}, new method_package)
		// Involve Grammar
		.add_method({new token_action(action_types::using_), new token_expr(tree_type<token_base *>()),
			            new token_endline(0)}, new method_involve)
		// Var Grammar
		.add_method({new token_action(action_types::var_), new token_expr(tree_type<token_base *>()),
			            new token_endline(0)}, new method_var)
		.add_method({new token_action(action_types::link_), new token_expr(tree_type<token_base *>()),
			            new token_endline(0)}, new method_link)
		.add_method({new token_action(action_types::constant_), new token_expr(tree_type<token_base *>()),
			            new token_endline(0)},
		new method_constant)
		// End Grammar
		.add_method({new token_action(action_types::endblock_), new token_endline(0)}, new method_end)
		// Block Grammar
		.add_method({new token_action(action_types::block_), new token_endline(0)}, new method_block)
		// Namespace Grammar
		.add_method({new token_action(action_types::namespace_), new token_expr(tree_type<token_base *>()),
			            new token_endline(0)}, new method_namespace)
		// If Grammar
		.add_method({new token_action(action_types::if_), new token_expr(tree_type<token_base *>()),
			            new token_endline(0)}, new method_if)
		// Else Grammar
		.add_method({new token_action(action_types::else_), new token_endline(0)}, new method_else)
		// Switch Grammar
		.add_method({new token_action(action_types::switch_), new token_expr(tree_type<token_base *>()),
			            new token_endline(0)}, new method_switch)
		// Case Grammar
		.add_method({new token_action(action_types::case_), new token_expr(tree_type<token_base *>()),
			            new token_endline(0)}, new method_case)
		// Default Grammar
		.add_method({new token_action(action_types::default_), new token_endline(0)},
		new method_default)
		// While Grammar
		.add_method({new token_action(action_types::while_), new token_expr(tree_type<token_base *>()),
			            new token_endline(0)}, new method_while)
		// Until Grammar
		.add_method({new token_action(action_types::until_), new token_expr(tree_type<token_base *>()),
			            new token_endline(0)}, new method_until)
		// Loop Grammar
		.add_method({new token_action(action_types::loop_), new token_endline(0)}, new method_loop)
		// For Grammar
		.add_method({new token_action(action_types::for_), new token_expr(tree_type<token_base *>()),
			            new token_endline(0)}, new method_for)
		.add_method({new token_action(action_types::for_), new token_expr(tree_type<token_base *>()),
			            new token_action(action_types::do_), new token_expr(tree_type<token_base *>()),
			            new token_endline(0)}, new method_for_do)
		.add_method({new token_action(action_types::foreach_), new token_expr(tree_type<token_base *>()),
			            new token_action(action_types::in_), new token_expr(tree_type<token_base *>()),
			            new token_endline(0)}, new method_foreach)
		.add_method({new token_action(action_types::foreach_), new token_expr(tree_type<token_base *>()),
			            new token_action(action_types::in_), new token_expr(tree_type<token_base *>()),
			            new token_action(action_types::do_), new token_expr(tree_type<token_base *>()),
			            new token_endline(0)}, new method_foreach_do)
		// Break Grammar
		.add_method({new token_action(action_types::break_), new token_endline(0)}, new method_break)
		// Continue Grammar
		.add_method({new token_action(action_types::continue_), new token_endline(0)},
		new method_continue)
		// Function Grammar
		.add_method({new token_action(action_types::function_), new token_expr(tree_type<token_base *>()),
			            new token_endline(0)}, new method_function)
		.add_method({new token_action(action_types::function_), new token_expr(tree_type<token_base *>()),
			            new token_action(action_types::override_), new token_endline(0)},
		new method_function)
		// Return Grammar
		.add_method({new token_action(action_types::return_), new token_expr(tree_type<token_base *>()),
			            new token_endline(0)}, new method_return)
		.add_method({new token_action(action_types::return_), new token_endline(0)},
		new method_return_no_value)
		// Struct Grammar
		.add_method({new token_action(action_types::struct_), new token_expr(tree_type<token_base *>()),
			            new token_endline(0)}, new method_struct)
		.add_method({new token_action(action_types::struct_), new token_expr(tree_type<token_base *>()),
			            new token_action(action_types::extends_), new token_expr(tree_type<token_base *>()),
			            new token_endline(0)}, new method_struct)
		// Try Grammar
		.add_method({new token_action(action_types::try_), new token_endline(0)}, new method_try)
		// Catch Grammar
		.add_method({new token_action(action_types::catch_), new token_expr(tree_type<token_base *>()),
			            new token_endline(0)}, new method_catch)
		// Throw Grammar
		.add_method({new token_action(action_types::throw_), new token_expr(tree_type<token_base *>()),
			            new token_endline(0)}, new method_throw);
		// Init Runtime
		context->instance->storage
		// Internal Types
		.add_buildin_type("char", []() -> var { return var::make<char>('\0'); }, typeid(char),
		                  cs_impl::char_ext)
		.add_buildin_type("number", []() -> var { return var::make<numeric>(0); }, typeid(numeric),
		                  cs_impl::number_ext)
		.add_buildin_type("integer", []() -> var { return var::make<numeric>(0); }, typeid(numeric))
		.add_buildin_type("float", []() -> var { return var::make<numeric>(0.0); }, typeid(numeric))
		.add_buildin_type("boolean", []() -> var { return var::make<boolean>(true); }, typeid(boolean))
		.add_buildin_type("pointer", []() -> var { return var::make<pointer>(null_pointer); }, typeid(pointer))
		.add_buildin_type("string", []() -> var { return var::make<string>(); }, typeid(string),
		                  cs_impl::string_ext)
		.add_buildin_type("list", []() -> var { return var::make<list>(); }, typeid(list), cs_impl::list_ext)
		.add_buildin_type("array", []() -> var { return var::make<array>(); }, typeid(array),
		                  cs_impl::array_ext)
		.add_buildin_type("pair", []() -> var { return var::make<pair>(numeric(0), numeric(0)); }, typeid(pair),
		                  cs_impl::pair_ext)
		.add_buildin_type("hash_set", []() -> var { return var::make<hash_set>(); }, typeid(hash_set),
		                  cs_impl::hash_set_ext)
		.add_buildin_type("hash_map", []() -> var { return var::make<hash_map>(); }, typeid(hash_map),
		                  cs_impl::hash_map_ext)
		// Context
		.add_buildin_var("context", var::make_constant<context_t>(context))
		// Add Internal Functions to storage
		.add_buildin_var("range", var::make_protect<callable>(range, callable::types::request_fold))
		.add_buildin_var("to_integer", make_cni(to_integer, true))
		.add_buildin_var("to_string", make_cni(to_string, true))
		.add_buildin_var("type", make_cni(type, true))
		.add_buildin_var("clone", make_cni(clone))
		.add_buildin_var("move", make_cni(move))
		.add_buildin_var("swap", make_cni(swap, true))
		// Add extensions to storage
		.add_buildin_var("exception", make_namespace(cs_impl::except_ext))
		.add_buildin_var("iostream", make_namespace(cs_impl::iostream_ext))
		.add_buildin_var("system", make_namespace(cs_impl::system_ext))
		.add_buildin_var("runtime", make_namespace(cs_impl::runtime_ext))
		.add_buildin_var("math", make_namespace(cs_impl::math_ext));
		return context;
	}

	context_t create_subcontext(const context_t &cxt)
	{
		cs_impl::init_extensions();
		context_t context = std::make_shared<context_type>();
		context->instance = std::make_shared<instance_type>(context, current_process->stack_size);
		context->compiler = cxt->compiler;
		context->cmd_args = cxt->cmd_args;
		// Init Runtime
		context->instance->storage
		// Internal Types
		.add_buildin_type("char", []() -> var { return var::make<char>('\0'); }, typeid(char),
		                  cs_impl::char_ext)
		.add_buildin_type("number", []() -> var { return var::make<numeric>(0); }, typeid(numeric))
		.add_buildin_type("integer", []() -> var { return var::make<numeric>(0); }, typeid(numeric))
		.add_buildin_type("float", []() -> var { return var::make<numeric>(0.0); }, typeid(numeric))
		.add_buildin_type("boolean", []() -> var { return var::make<boolean>(true); }, typeid(boolean))
		.add_buildin_type("pointer", []() -> var { return var::make<pointer>(null_pointer); }, typeid(pointer))
		.add_buildin_type("string", []() -> var { return var::make<string>(); }, typeid(string),
		                  cs_impl::string_ext)
		.add_buildin_type("list", []() -> var { return var::make<list>(); }, typeid(list), cs_impl::list_ext)
		.add_buildin_type("array", []() -> var { return var::make<array>(); }, typeid(array),
		                  cs_impl::array_ext)
		.add_buildin_type("pair", []() -> var { return var::make<pair>(numeric(0), numeric(0)); }, typeid(pair),
		                  cs_impl::pair_ext)
		.add_buildin_type("hash_set", []() -> var { return var::make<hash_set>(); }, typeid(hash_set),
		                  cs_impl::hash_set_ext)
		.add_buildin_type("hash_map", []() -> var { return var::make<hash_map>(); }, typeid(hash_map),
		                  cs_impl::hash_map_ext)
		// Context
		.add_buildin_var("context", var::make_constant<context_t>(context))
		// Add Internal Functions to storage
		.add_buildin_var("range", var::make_protect<callable>(range, callable::types::request_fold))
		.add_buildin_var("to_integer", make_cni(to_integer, true))
		.add_buildin_var("to_string", make_cni(to_string, true))
		.add_buildin_var("type", make_cni(type, true))
		.add_buildin_var("clone", make_cni(clone))
		.add_buildin_var("move", make_cni(move))
		.add_buildin_var("swap", make_cni(swap, true))
		// Add extensions to storage
		.add_buildin_var("exception", make_namespace(cs_impl::except_ext))
		.add_buildin_var("iostream", make_namespace(cs_impl::iostream_ext))
		.add_buildin_var("system", make_namespace(cs_impl::system_ext))
		.add_buildin_var("runtime", make_namespace(cs_impl::runtime_ext))
		.add_buildin_var("math", make_namespace(cs_impl::math_ext));
		return context;
	}

	void collect_garbage()
	{
		statement_base::gc.collect();
		method_base::gc.collect();
		token_base::gc.collect();
		extension::gc.collect();
	}

	void collect_garbage(context_t &context)
	{
		while (!current_process->stack.empty())
			current_process->stack.pop_no_return();
#ifdef CS_DEBUGGER
		while(!current_process->stack_backtrace.empty())
			current_process->stack_backtrace.pop_no_return();
#endif
		if (context) {
			context->instance->storage.clear_all_data();
			context->compiler->modules.clear();
			context->compiler->csyms.clear();
			context->compiler->swap_context(nullptr);
			context->instance->context = nullptr;
			context->compiler = nullptr;
			context->instance = nullptr;
			context = nullptr;
		}
		collect_garbage();
	}

	cs::var eval(const context_t &context, const std::string &expr)
	{
		tree_type<cs::token_base *> tree;
		std::deque<char> buff;
		for (auto &ch: expr)
			buff.push_back(ch);
		context->compiler->build_expr(buff, tree);
		return context->instance->parse_expr(tree.root());
	}
}
