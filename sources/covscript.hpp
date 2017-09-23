#pragma once
/*
* Covariant Script Programming Language
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
* Copyright (C) 2017 Michael Lee(李登淳)
* Email: mikecovlee@163.com
* Github: https://github.com/mikecovlee
*/
#include "./cni.hpp"
#include "./lexer.hpp"
#include "./parser.hpp"
#include "./runtime.hpp"
#include "./codegen.hpp"
#include "./optimize.hpp"
#include "./statement.hpp"
#include "./extensions/iostream_extension.hpp"
#include "./extensions/system_extension.hpp"
#include "./extensions/runtime_extension.hpp"
#include "./extensions/except_extension.hpp"
#include "./extensions/char_extension.hpp"
#include "./extensions/string_extension.hpp"
#include "./extensions/list_extension.hpp"
#include "./extensions/array_extension.hpp"
#include "./extensions/pair_extension.hpp"
#include "./extensions/hash_map_extension.hpp"
#include "./extensions/math_extension.hpp"
#include "./extensions/darwin_extension.hpp"

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

	var clone(const var &val)
	{
		return copy(val);
	}

	void swap(var &a, var &b)
	{
		a.swap(b, true);
	}

	void init_grammar()
	{
		// Expression Grammar
		translator.add_method({new token_expr(cov::tree<token_base *>()), new token_endline(0)}, new method_expression);
		// Import Grammar
		translator.add_method({new token_action(action_types::import_), new token_expr(cov::tree<token_base *>()), new token_endline(0)}, new method_import);
		// Package Grammar
		translator.add_method({new token_action(action_types::package_), new token_expr(cov::tree<token_base *>()), new token_endline(0)}, new method_package);
		// Var Grammar
		translator.add_method({new token_action(action_types::var_), new token_expr(cov::tree<token_base *>()), new token_endline(0)}, new method_var);
		translator.add_method({new token_action(action_types::constant_), new token_action(action_types::var_), new token_expr(cov::tree<token_base *>()), new token_endline(0)}, new method_constant);
		// End Grammar
		translator.add_method({new token_action(action_types::endblock_), new token_endline(0)}, new method_end);
		// Block Grammar
		translator.add_method({new token_action(action_types::block_), new token_endline(0)}, new method_block);
		// Namespace Grammar
		translator.add_method({new token_action(action_types::namespace_), new token_expr(cov::tree<token_base *>()), new token_endline(0)}, new method_namespace);
		// If Grammar
		translator.add_method({new token_action(action_types::if_), new token_expr(cov::tree<token_base *>()), new token_endline(0)}, new method_if);
		// Else Grammar
		translator.add_method({new token_action(action_types::else_), new token_endline(0)}, new method_else);
		// Switch Grammar
		translator.add_method({new token_action(action_types::switch_), new token_expr(cov::tree<token_base *>()), new token_endline(0)}, new method_switch);
		// Case Grammar
		translator.add_method({new token_action(action_types::case_), new token_expr(cov::tree<token_base *>()), new token_endline(0)}, new method_case);
		// Default Grammar
		translator.add_method({new token_action(action_types::default_), new token_endline(0)}, new method_default);
		// While Grammar
		translator.add_method({new token_action(action_types::while_), new token_expr(cov::tree<token_base *>()), new token_endline(0)}, new method_while);
		// Until Grammar
		translator.add_method({new token_action(action_types::until_), new token_expr(cov::tree<token_base *>()), new token_endline(0)}, new method_until);
		// Loop Grammar
		translator.add_method({new token_action(action_types::loop_), new token_endline(0)}, new method_loop);
		// For Grammar
		translator.add_method({new token_action(action_types::for_), new token_expr(cov::tree<token_base *>()), new token_action(action_types::to_), new token_expr(cov::tree<token_base *>()), new token_action(action_types::step_), new token_expr(cov::tree<token_base *>()), new token_endline(0)}, new method_for_step);
		translator.add_method({new token_action(action_types::for_), new token_expr(cov::tree<token_base *>()), new token_action(action_types::to_), new token_expr(cov::tree<token_base *>()), new token_endline(0)}, new method_for);
		translator.add_method({new token_action(action_types::for_), new token_expr(cov::tree<token_base *>()), new token_action(action_types::iterate_), new token_expr(cov::tree<token_base *>()), new token_endline(0)}, new method_foreach);
		// Break Grammar
		translator.add_method({new token_action(action_types::break_), new token_endline(0)}, new method_break);
		// Continue Grammar
		translator.add_method({new token_action(action_types::continue_), new token_endline(0)}, new method_continue);
		// Function Grammar
		translator.add_method({new token_action(action_types::function_), new token_expr(cov::tree<token_base *>()), new token_endline(0)}, new method_function);
		// Return Grammar
		translator.add_method({new token_action(action_types::return_), new token_expr(cov::tree<token_base *>()), new token_endline(0)}, new method_return);
		translator.add_method({new token_action(action_types::return_), new token_endline(0)}, new method_return_no_value);
		// Struct Grammar
		translator.add_method({new token_action(action_types::struct_), new token_expr(cov::tree<token_base *>()), new token_endline(0)}, new method_struct);
		// Try Grammar
		translator.add_method({new token_action(action_types::try_), new token_endline(0)}, new method_try);
		// Catch Grammar
		translator.add_method({new token_action(action_types::catch_), new token_expr(cov::tree<token_base *>()), new token_endline(0)}, new method_catch);
		// Throw Grammar
		translator.add_method({new token_action(action_types::throw_), new token_expr(cov::tree<token_base *>()), new token_endline(0)}, new method_throw);
	}

	void init_ext()
	{
		// Init the extensions
		iostream_cs_ext::init();
		istream_cs_ext::init();
		ostream_cs_ext::init();
		system_cs_ext::init();
		runtime_cs_ext::init();
		except_cs_ext::init();
		char_cs_ext::init();
		string_cs_ext::init();
		list_cs_ext::init();
		array_cs_ext::init();
		pair_cs_ext::init();
		hash_map_cs_ext::init();
		math_cs_ext::init();
		darwin_cs_ext::init();
	}

	void init()
	{
		// Internal Types
		runtime->storage.add_type("char", []() -> var { return var::make<char>('\0'); }, cs_impl::hash<std::string>(typeid(char).name()), char_ext_shared);
		runtime->storage.add_type("number", []() -> var { return var::make<number>(0); }, cs_impl::hash<std::string>(typeid(number).name()));
		runtime->storage.add_type("boolean", []() -> var { return var::make<boolean>(true); }, cs_impl::hash<std::string>(typeid(boolean).name()));
		runtime->storage.add_type("pointer", []() -> var { return var::make<pointer>(null_pointer); }, cs_impl::hash<std::string>(typeid(pointer).name()));
		runtime->storage.add_type("string", []() -> var { return var::make<string>(); }, cs_impl::hash<std::string>(typeid(string).name()), string_ext_shared);
		runtime->storage.add_type("list", []() -> var { return var::make<list>(); }, cs_impl::hash<std::string>(typeid(list).name()), list_ext_shared);
		runtime->storage.add_type("array", []() -> var { return var::make<array>(); }, cs_impl::hash<std::string>(typeid(array).name()), array_ext_shared);
		runtime->storage.add_type("pair", []() -> var { return var::make<pair>(number(0), number(0)); }, cs_impl::hash<std::string>(typeid(pair).name()), pair_ext_shared);
		runtime->storage.add_type("hash_map", []() -> var { return var::make<hash_map>(); }, cs_impl::hash<std::string>(typeid(hash_map).name()), hash_map_ext_shared);
		// Add Internal Functions to storage
		runtime->storage.add_var_global("to_integer", cs::var::make_protect<cs::callable>(cs::cni(to_integer), true));
		runtime->storage.add_var_global("to_string", cs::var::make_protect<cs::callable>(cs::cni(to_string), true));
		runtime->storage.add_var_global("clone", cs::var::make_protect<cs::callable>(cs::cni(clone)));
		runtime->storage.add_var_global("swap", cs::var::make_protect<cs::callable>(cs::cni(swap)));
		// Add extensions to storage
		runtime->storage.add_var("exception", var::make_protect<std::shared_ptr<extension_holder>>(except_ext_shared));
		runtime->storage.add_var("iostream", var::make_protect<std::shared_ptr<extension_holder>>(std::make_shared<extension_holder>(&iostream_ext)));
		runtime->storage.add_var("system", var::make_protect<std::shared_ptr<extension_holder>>(std::make_shared<extension_holder>(&system_ext)));
		runtime->storage.add_var("runtime", var::make_protect<std::shared_ptr<extension_holder>>(std::make_shared<extension_holder>(&runtime_ext)));
		runtime->storage.add_var("math", var::make_protect<std::shared_ptr<extension_holder>>(std::make_shared<extension_holder>(&math_ext)));
		runtime->storage.add_var("darwin", var::make_protect<std::shared_ptr<extension_holder>>(std::make_shared<extension_holder>(&darwin_ext)));
	}

	std::shared_ptr<runtime_type> covscript(const std::string &path, bool compile_only)
	{
		// Read from file
		std::deque<char> buff;
		std::ifstream in(path);
		if (!in.is_open())
			throw fatal_error(path + ": No such file or directory");
		while (!in.eof())
			buff.push_back(in.get());
		// Init resources
		runtime.new_instance();
		token_value::clean();
		init();
		// Lexer
		std::deque<token_base *> tokens;
		translate_into_tokens(buff, tokens, path);
		// Parser
		std::deque<statement_base *> statements;
		translate_into_statements(tokens, statements);
		// Process constant values
		token_value::mark();
		// Exit if compile_only is true
		if (compile_only)
			return runtime.pop_instance();
		// Run the instruction
		for (auto &ptr:statements) {
			try {
				ptr->run();
			}
			catch (const lang_error &le) {
				throw fatal_error("Uncaught exception.");
			}
			catch (const cs::exception &e) {
				throw e;
			}
			catch (const std::exception &e) {
				throw exception(ptr->get_line_num(), ptr->get_file_path(), ptr->get_code(), e.what());
			}
		}
		return runtime.pop_instance();
	}
}
