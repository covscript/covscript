/*
* Covariant Script Programming Language
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
#include <covscript/impl/codegen.hpp>
#include <covscript/covscript.hpp>

#ifdef COVSCRIPT_PLATFORM_WIN32

#include <shlobj.h>

#pragma comment(lib, "shell32.lib")
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
	cs::namespace_t except_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t array_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t array_iterator_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t char_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t math_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t math_const_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t list_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t list_iterator_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t hash_map_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t pair_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t context_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t runtime_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t string_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t iostream_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t seekdir_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t openmode_ext = cs::make_shared_namespace<cs::name_space>();
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
	bool process_context::on_process_exit_default_handler(void *code)
	{
		while (!current_process->stack.empty())
			current_process->stack.pop_no_return();
#ifdef CS_DEBUGGER
		while(!current_process->stack_backtrace.empty())
			current_process->stack_backtrace.pop_no_return();
#endif
		collect_garbage();
		std::exit(*static_cast<int *>(code));
		return true;
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

	number parse_number(const std::string &str)
	{
		int point_count = 0;
		for (auto &ch:str) {
			if (!std::isdigit(ch)) {
				if (ch != '.' || ++point_count > 1)
					throw runtime_error("Wrong literal format.");
			}
		}
		return std::stold(str);
	}

	garbage_collector<cov::dll> extension::gc;

	garbage_collector<token_base> token_base::gc;

	garbage_collector<statement_base> statement_base::gc;

	garbage_collector<method_base> method_base::gc;

#ifdef COVSCRIPT_PLATFORM_WIN32

	std::string get_sdk_path()
	{
#ifdef COVSCRIPT_HOME
		return COVSCRIPT_HOME;
#else
		CHAR path[MAX_PATH];
		SHGetFolderPathA(nullptr, CSIDL_PERSONAL, nullptr, SHGFP_TYPE_CURRENT, path);
		return std::strcat(path, "\\CovScript");
#endif
	}

#else

	std::string get_sdk_path()
	{
#ifdef COVSCRIPT_HOME
		return COVSCRIPT_HOME;
#else
		return "/usr/share/covscript";
#endif
	}

#endif

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

	std::string get_import_path()
	{
		const char *import_path = std::getenv("CS_IMPORT_PATH");
		if (import_path != nullptr)
			return process_path(import_path);
		else
			return process_path(get_sdk_path() + cs::path_separator + "imports");
	}

	array parse_cmd_args(int argc, const char *argv[])
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
			cs_impl::check_args<number>(args);
			return var::make_constant<range_type>(0, args[0].const_val<number>(), 1);
		case 2:
			cs_impl::check_args<number, number>(args);
			return var::make_constant<range_type>(args[0].const_val<number>(), args[1].const_val<number>(), 1);
		case 3:
			cs_impl::check_args<number, number, number>(args);
			return var::make_constant<range_type>(args[0].const_val<number>(), args[1].const_val<number>(),
			                                      args[2].const_val<number>());
		default:
			throw cs::runtime_error(
			    "Wrong size of the arguments. Expected 1, 2 or 3, provided " + std::to_string(args.size()));
		}
	}

	number to_integer(const var &val)
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
		context->instance = std::make_shared<instance_type>(context);
		context->cmd_args = cs::var::make_constant<cs::array>(args);
		// Init Grammars
		(*context->compiler)
		// Expression Grammar
		.add_method({new token_expr(tree_type<token_base *>()), new token_endline(0)},
		new method_expression)
		// Import Grammar
		.add_method({new token_action(action_types::import_), new token_expr(tree_type<token_base *>()),
			            new token_endline(0)}, new method_import)
		// Package Grammar
		.add_method({new token_action(action_types::package_), new token_expr(tree_type<token_base *>()),
			            new token_endline(0)}, new method_package)
		// Involve Grammar
		.add_method({new token_action(action_types::using_), new token_expr(tree_type<token_base *>()),
			            new token_endline(0)}, new method_involve)
		// Var Grammar
		.add_method({new token_action(action_types::var_), new token_expr(tree_type<token_base *>()),
			            new token_endline(0)}, new method_var)
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
		.add_buildin_type("number", []() -> var { return var::make<number>(0); }, typeid(number))
		.add_buildin_type("boolean", []() -> var { return var::make<boolean>(true); }, typeid(boolean))
		.add_buildin_type("pointer", []() -> var { return var::make<pointer>(null_pointer); }, typeid(pointer))
		.add_buildin_type("string", []() -> var { return var::make<string>(); }, typeid(string),
		                  cs_impl::string_ext)
		.add_buildin_type("list", []() -> var { return var::make<list>(); }, typeid(list), cs_impl::list_ext)
		.add_buildin_type("array", []() -> var { return var::make<array>(); }, typeid(array),
		                  cs_impl::array_ext)
		.add_buildin_type("pair", []() -> var { return var::make<pair>(number(0), number(0)); }, typeid(pair),
		                  cs_impl::pair_ext)
		.add_buildin_type("hash_map", []() -> var { return var::make<hash_map>(); }, typeid(hash_map),
		                  cs_impl::hash_map_ext)
		// Context
		.add_buildin_var("context", var::make_constant<context_t>(context))
		// Add Internal Functions to storage
		.add_buildin_var("range", var::make_protect<callable>(range, callable::types::constant))
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
		context->instance = std::make_shared<instance_type>(context);
		context->compiler = cxt->compiler;
		context->cmd_args = cxt->cmd_args;
		// Init Runtime
		context->instance->storage
		// Internal Types
		.add_buildin_type("char", []() -> var { return var::make<char>('\0'); }, typeid(char),
		                  cs_impl::char_ext)
		.add_buildin_type("number", []() -> var { return var::make<number>(0); }, typeid(number))
		.add_buildin_type("boolean", []() -> var { return var::make<boolean>(true); }, typeid(boolean))
		.add_buildin_type("pointer", []() -> var { return var::make<pointer>(null_pointer); }, typeid(pointer))
		.add_buildin_type("string", []() -> var { return var::make<string>(); }, typeid(string),
		                  cs_impl::string_ext)
		.add_buildin_type("list", []() -> var { return var::make<list>(); }, typeid(list), cs_impl::list_ext)
		.add_buildin_type("array", []() -> var { return var::make<array>(); }, typeid(array),
		                  cs_impl::array_ext)
		.add_buildin_type("pair", []() -> var { return var::make<pair>(number(0), number(0)); }, typeid(pair),
		                  cs_impl::pair_ext)
		.add_buildin_type("hash_map", []() -> var { return var::make<hash_map>(); }, typeid(hash_map),
		                  cs_impl::hash_map_ext)
		// Context
		.add_buildin_var("context", var::make_constant<context_t>(context))
		// Add Internal Functions to storage
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
		for (auto &ch:expr)
			buff.push_back(ch);
		context->compiler->build_expr(buff, tree);
		return context->instance->parse_expr(tree.root());
	}
}