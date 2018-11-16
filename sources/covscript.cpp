/*
* Covariant Script Programming Language
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
#include <covscript/covscript.hpp>
#include <covscript/console/conio.hpp>
#include <covscript/extensions/extensions.hpp>

#ifdef COVSCRIPT_PLATFORM_WIN32

#include <shlobj.h>

#pragma comment(lib, "shell32.lib")
#endif

// Compiler
#include "compiler/compiler.cpp"
#include "compiler/lexer.cpp"
#include "compiler/parser.cpp"

// Instance
#include "instance/instance.cpp"
#include "instance/runtime.cpp"
#include "instance/statement.cpp"
#include "instance/codegen.cpp"

namespace cs {
// Internal Functions
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
	void process_type::init_grammar()
	{
		translator
		// Expression Grammar
		.add_method({new token_expr(cov::tree<token_base *>()), new token_endline(0)},
		new method_expression(context))
		// Import Grammar
		.add_method({new token_action(action_types::import_), new token_expr(cov::tree<token_base *>()),
			            new token_endline(0)}, new method_import(context))
		// Package Grammar
		.add_method({new token_action(action_types::package_), new token_expr(cov::tree<token_base *>()),
			            new token_endline(0)}, new method_package(context))
		// Involve Grammar
		.add_method({new token_action(action_types::using_), new token_expr(cov::tree<token_base *>()),
			            new token_endline(0)}, new method_involve(context))
		// Var Grammar
		.add_method({new token_action(action_types::var_), new token_expr(cov::tree<token_base *>()),
			            new token_endline(0)}, new method_var(context))
		.add_method({new token_action(action_types::constant_), new token_expr(cov::tree<token_base *>()),
			            new token_endline(0)},
		new method_constant(context))
		// End Grammar
		.add_method({new token_action(action_types::endblock_), new token_endline(0)}, new method_end(context))
		// Block Grammar
		.add_method({new token_action(action_types::block_), new token_endline(0)}, new method_block(context))
		// Namespace Grammar
		.add_method({new token_action(action_types::namespace_), new token_expr(cov::tree<token_base *>()),
			            new token_endline(0)}, new method_namespace(context))
		// If Grammar
		.add_method({new token_action(action_types::if_), new token_expr(cov::tree<token_base *>()),
			            new token_endline(0)}, new method_if(context))
		// Else Grammar
		.add_method({new token_action(action_types::else_), new token_endline(0)}, new method_else(context))
		// Switch Grammar
		.add_method({new token_action(action_types::switch_), new token_expr(cov::tree<token_base *>()),
			            new token_endline(0)}, new method_switch(context))
		// Case Grammar
		.add_method({new token_action(action_types::case_), new token_expr(cov::tree<token_base *>()),
			            new token_endline(0)}, new method_case(context))
		// Default Grammar
		.add_method({new token_action(action_types::default_), new token_endline(0)},
		new method_default(context))
		// While Grammar
		.add_method({new token_action(action_types::while_), new token_expr(cov::tree<token_base *>()),
			            new token_endline(0)}, new method_while(context))
		// Until Grammar
		.add_method({new token_action(action_types::until_), new token_expr(cov::tree<token_base *>()),
			            new token_endline(0)}, new method_until(context))
		// Loop Grammar
		.add_method({new token_action(action_types::loop_), new token_endline(0)}, new method_loop(context))
		// For Grammar
		.add_method({new token_action(action_types::for_), new token_expr(cov::tree<token_base *>()),
			            new token_endline(0)}, new method_for(context))
		.add_method({new token_action(action_types::for_), new token_expr(cov::tree<token_base *>()),
			            new token_action(action_types::do_), new token_expr(cov::tree<token_base *>()),
			            new token_endline(0)}, new method_for_do(context))
		.add_method({new token_action(action_types::foreach_), new token_expr(cov::tree<token_base *>()),
			            new token_endline(0)}, new method_foreach(context))
		.add_method({new token_action(action_types::foreach_), new token_expr(cov::tree<token_base *>()),
			            new token_action(action_types::do_), new token_expr(cov::tree<token_base *>()),
			            new token_endline(0)}, new method_foreach_do(context))
		// Break Grammar
		.add_method({new token_action(action_types::break_), new token_endline(0)}, new method_break(context))
		// Continue Grammar
		.add_method({new token_action(action_types::continue_), new token_endline(0)},
		new method_continue(context))
		// Function Grammar
		.add_method({new token_action(action_types::function_), new token_expr(cov::tree<token_base *>()),
			            new token_endline(0)}, new method_function(context))
		.add_method({new token_action(action_types::function_), new token_expr(cov::tree<token_base *>()),
			            new token_action(action_types::override_), new token_endline(0)},
		new method_function(context))
		// Return Grammar
		.add_method({new token_action(action_types::return_), new token_expr(cov::tree<token_base *>()),
			            new token_endline(0)}, new method_return(context))
		.add_method({new token_action(action_types::return_), new token_endline(0)},
		new method_return_no_value(context))
		// Struct Grammar
		.add_method({new token_action(action_types::struct_), new token_expr(cov::tree<token_base *>()),
			            new token_endline(0)}, new method_struct(context))
		.add_method({new token_action(action_types::struct_), new token_expr(cov::tree<token_base *>()),
			            new token_action(action_types::extends_), new token_expr(cov::tree<token_base *>()),
			            new token_endline(0)}, new method_struct(context))
		// Try Grammar
		.add_method({new token_action(action_types::try_), new token_endline(0)}, new method_try(context))
		// Catch Grammar
		.add_method({new token_action(action_types::catch_), new token_expr(cov::tree<token_base *>()),
			            new token_endline(0)}, new method_catch(context))
		// Throw Grammar
		.add_method({new token_action(action_types::throw_), new token_expr(cov::tree<token_base *>()),
			            new token_endline(0)}, new method_throw(context));
	}

	void process_type::init_runtime()
	{
		runtime.storage
		// Internal Types
		.add_buildin_type("char", []() -> var { return var::make<char>('\0'); }, typeid(char), char_ext_shared)
		.add_buildin_type("number", []() -> var { return var::make<number>(0); }, typeid(number))
		.add_buildin_type("boolean", []() -> var { return var::make<boolean>(true); }, typeid(boolean))
		.add_buildin_type("pointer", []() -> var { return var::make<pointer>(null_pointer); }, typeid(pointer))
		.add_buildin_type("string", []() -> var { return var::make<string>(); }, typeid(string),
		                  string_ext_shared)
		.add_buildin_type("list", []() -> var { return var::make<list>(); }, typeid(list), list_ext_shared)
		.add_buildin_type("array", []() -> var { return var::make<array>(); }, typeid(array), array_ext_shared)
		.add_buildin_type("pair", []() -> var { return var::make<pair>(number(0), number(0)); }, typeid(pair),
		                  pair_ext_shared)
		.add_buildin_type("hash_map", []() -> var { return var::make<hash_map>(); }, typeid(hash_map),
		                  hash_map_ext_shared)
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
		.add_buildin_var("exception", make_namespace(except_ext_shared))
		.add_buildin_var("iostream", make_namespace(make_shared_namespace(iostream_ext)))
		.add_buildin_var("system", make_namespace(make_shared_namespace(system_ext)))
		.add_buildin_var("runtime", make_namespace(make_shared_namespace(runtime_ext)))
		.add_buildin_var("math", make_namespace(make_shared_namespace(math_ext)));
	}

	process_type::process_type(const array &args)
	{
		// Init extensions
		init_extensions();
		system_ext.add_var("args", cs::var::make_constant<cs::array>(args));
	}

	process_type::process_type(int argv, const char *args[])
	{
		// Init args
		cs::array
		arg;
		for (std::size_t i = 0; i < argv; ++i)
			arg.emplace_back(cs::var::make_constant<cs::string>(args[i]));
		process_type::process_type(arg);
	}
}