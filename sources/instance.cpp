/*
* Covariant Script Instance
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
* Copyright (C) 2018 Michael Lee(李登淳)
* Email: mikecovlee@163.com
* Github: https://github.com/mikecovlee
*/
#include <covscript/instance.hpp>
#include <covscript/statement.hpp>
#include <covscript/codegen.hpp>

namespace cs {
	const std::string &statement_base::get_file_path() const noexcept
	{
		return context->file_path;
	}

	const std::string &statement_base::get_package_name() const noexcept
	{
		return context->package_name;
	}

	const std::string &statement_base::get_raw_code() const noexcept
	{
		return context->file_buff.at(line_num - 1);
	}

	void instance_type::init_grammar()
	{
		// Expression Grammar
		translator.add_method({new token_expr(cov::tree<token_base *>()), new token_endline(0)},
		new method_expression(context));
		// Import Grammar
		translator.add_method({new token_action(action_types::import_), new token_expr(cov::tree<token_base *>()),
			                      new token_endline(0)
		}, new method_import(context));
		// Package Grammar
		translator.add_method({new token_action(action_types::package_), new token_expr(cov::tree<token_base *>()),
			                      new token_endline(0)
		}, new method_package(context));
		// Involve Grammar
		translator.add_method({new token_action(action_types::using_), new token_expr(cov::tree<token_base *>()),
			                      new token_endline(0)
		}, new method_involve(context));
		// Var Grammar
		translator.add_method(
		{new token_action(action_types::var_), new token_expr(cov::tree<token_base *>()), new token_endline(0)},
		new method_var(context));
		translator.add_method({new token_action(action_types::constant_), new token_action(action_types::var_),
			                      new token_expr(cov::tree<token_base *>()), new token_endline(0)
		},
		new method_constant(context));
		// End Grammar
		translator.add_method({new token_action(action_types::endblock_), new token_endline(0)},
		new method_end(context));
		// Block Grammar
		translator.add_method({new token_action(action_types::block_), new token_endline(0)},
		new method_block(context));
		// Namespace Grammar
		translator.add_method({new token_action(action_types::namespace_), new token_expr(cov::tree<token_base *>()),
			                      new token_endline(0)
		}, new method_namespace(context));
		// If Grammar
		translator.add_method(
		{new token_action(action_types::if_), new token_expr(cov::tree<token_base *>()), new token_endline(0)},
		new method_if(context));
		// Else Grammar
		translator.add_method({new token_action(action_types::else_), new token_endline(0)}, new method_else(context));
		// Switch Grammar
		translator.add_method({new token_action(action_types::switch_), new token_expr(cov::tree<token_base *>()),
			                      new token_endline(0)
		}, new method_switch(context));
		// Case Grammar
		translator.add_method({new token_action(action_types::case_), new token_expr(cov::tree<token_base *>()),
			                      new token_endline(0)
		}, new method_case(context));
		// Default Grammar
		translator.add_method({new token_action(action_types::default_), new token_endline(0)},
		new method_default(context));
		// While Grammar
		translator.add_method({new token_action(action_types::while_), new token_expr(cov::tree<token_base *>()),
			                      new token_endline(0)
		}, new method_while(context));
		// Until Grammar
		translator.add_method({new token_action(action_types::until_), new token_expr(cov::tree<token_base *>()),
			                      new token_endline(0)
		}, new method_until(context));
		// Loop Grammar
		translator.add_method({new token_action(action_types::loop_), new token_endline(0)}, new method_loop(context));
		// For Grammar
		translator.add_method({new token_action(action_types::for_), new token_expr(cov::tree<token_base *>()),
			                      new token_action(action_types::to_), new token_expr(cov::tree<token_base *>()),
			                      new token_action(action_types::step_), new token_expr(cov::tree<token_base *>()),
			                      new token_endline(0)
		}, new method_for_step(context));
		translator.add_method({new token_action(action_types::for_), new token_expr(cov::tree<token_base *>()),
			                      new token_action(action_types::to_), new token_expr(cov::tree<token_base *>()),
			                      new token_endline(0)
		}, new method_for(context));
		translator.add_method({new token_action(action_types::for_), new token_expr(cov::tree<token_base *>()),
			                      new token_action(action_types::iterate_), new token_expr(cov::tree<token_base *>()),
			                      new token_endline(0)
		}, new method_foreach(context));
		// Break Grammar
		translator.add_method({new token_action(action_types::break_), new token_endline(0)},
		new method_break(context));
		// Continue Grammar
		translator.add_method({new token_action(action_types::continue_), new token_endline(0)},
		new method_continue(context));
		// Function Grammar
		translator.add_method({new token_action(action_types::function_), new token_expr(cov::tree<token_base *>()),
			                      new token_endline(0)
		}, new method_function(context));
		// Return Grammar
		translator.add_method({new token_action(action_types::return_), new token_expr(cov::tree<token_base *>()),
			                      new token_endline(0)
		}, new method_return(context));
		translator.add_method({new token_action(action_types::return_), new token_endline(0)},
		new method_return_no_value(context));
		// Struct Grammar
		translator.add_method({new token_action(action_types::struct_), new token_expr(cov::tree<token_base *>()),
			                      new token_endline(0)
		}, new method_struct(context));
		translator.add_method({new token_action(action_types::struct_), new token_expr(cov::tree<token_base *>()), new token_action(action_types::extends_), new token_expr(cov::tree<token_base *>()), new token_endline(0)
		}, new method_struct_extends(context));
		// Try Grammar
		translator.add_method({new token_action(action_types::try_), new token_endline(0)}, new method_try(context));
		// Catch Grammar
		translator.add_method({new token_action(action_types::catch_), new token_expr(cov::tree<token_base *>()),
			                      new token_endline(0)
		}, new method_catch(context));
		// Throw Grammar
		translator.add_method({new token_action(action_types::throw_), new token_expr(cov::tree<token_base *>()),
			                      new token_endline(0)
		}, new method_throw(context));
	}

	void instance_type::opt_expr(cov::tree<token_base *> &tree, cov::tree<token_base *>::iterator
	                             it)
	{
		if (!it.

		        usable()

		   )
			return;
		token_base *token = it.data();
		if (token == nullptr)
			return;
		switch (token->

		        get_type()

		       ) {
		default:
			break;
		case

				token_types::id: {
				const std::string &id = static_cast<token_id *>(token)->get_id();
				if (storage.exist_record(id)) {
					if (storage.var_exist_current(id) && storage.get_var_current(id).is_protect())
						it.data() = new_value(storage.get_var(id));
				}
				else if (storage.exist_record_in_struct(id)) {
					if (storage.var_exist(id) && storage.get_var(id).is_protect())
						it.data() = new_value(storage.get_var(id));
					else {
						it.data() = new token_signal(signal_types::dot_);
						tree.emplace_left_left(it, new token_id("this"));
						tree.emplace_right_right(it, token);
					}
				}
				else if (storage.var_exist(id) && storage.get_var(id).is_protect())
					it.data() = new_value(storage.get_var(id));
				return;
				break;
			}

		case

				token_types::expr: {
				cov::tree<token_base *> &t = static_cast<token_expr *>(it.data())->get_tree();
				optimize_expression(t);
				tree.merge(it, t);
				return;
				break;
			}

		case

				token_types::array: {
				bool is_optimizable = true;
				for (auto &tree:static_cast<token_array *>(token)->get_array()) {
					optimize_expression(tree);
					if (is_optimizable && !optimizable(tree.root()))
						is_optimizable = false;
				}
				if (is_optimizable) {
					array arr;
					for (auto &tree:static_cast<token_array *>(token)->get_array())
						arr.push_back((new_value(copy(parse_expr(tree.root()))))->get_value());
					it.data() = new_value(var::make<array>(std::move(arr)));
				}
				return;
				break;
			}

		case

				token_types::arglist: {
				for (auto &tree:static_cast<token_arglist *>(token)->get_arglist())
					optimize_expression(tree);
				return;
				break;
			}

		case

				token_types::signal: {
				switch (static_cast<token_signal *>(token)->get_signal()) {
				default:
					break;
				case signal_types::new_:
					if (it.left().data() != nullptr)
						throw syntax_error("Wrong grammar for new expression.");
					break;
				case signal_types::gcnew_:
					if (it.left().data() != nullptr)
						throw syntax_error("Wrong grammar for gcnew expression.");
					opt_expr(tree, it.right());
					return;
					break;
				case signal_types::typeid_:
					if (it.left().data() != nullptr)
						throw syntax_error("Wrong grammar for typeid expression.");
					break;
				case signal_types::not_:
					if (it.left().data() != nullptr)
						throw syntax_error("Wrong grammar for not expression.");
					break;
				case signal_types::sub_:
					if (it.left().data() == nullptr)
						it.data() = new token_signal(signal_types::minus_);
					break;
				case signal_types::mul_:
					if (it.left().data() == nullptr)
						it.data() = new token_signal(signal_types::escape_);
					break;
				case signal_types::asi_:
					if (it.left().data() == nullptr || it.right().data() == nullptr)
						throw syntax_error("Wrong grammar for assign expression.");
					opt_expr(tree, it.left());
					opt_expr(tree, it.right());
					return;
					break;
				case signal_types::choice_: {
					token_signal *sig = dynamic_cast<token_signal *>(it.right().data());
					if (sig == nullptr || sig->get_signal() != signal_types::pair_)
						throw syntax_error("Wrong grammar for choice expression.");
					opt_expr(tree, it.left());
					opt_expr(tree, it.right().left());
					opt_expr(tree, it.right().right());
					token_value *val = dynamic_cast<token_value *>(it.left().data());
					if (val != nullptr) {
						if (val->get_value().type() == typeid(boolean)) {
							if (val->get_value().const_val<boolean>())
								tree.reserve_left(it.right());
							else
								tree.reserve_right(it.right());
							tree.reserve_right(it);
						}
						else
							throw syntax_error("Choice condition must be boolean.");
					}
					return;
					break;
				}
				case signal_types::vardef_: {
					if (it.left().data() != nullptr)
						throw syntax_error("Wrong grammar for variable definition.");
					token_base *rptr = it.right().data();
					if (rptr == nullptr || rptr->get_type() != token_types::id)
						throw syntax_error("Wrong grammar for variable definition.");
					storage.add_record(static_cast<token_id *>(rptr)->get_id());
					it.data() = rptr;
					return;
					break;
				}
				case signal_types::arrow_:
					if (it.left().data() == nullptr || it.right().data() == nullptr ||
					        it.right().data()->get_type() != token_types::id)
						throw syntax_error("Wrong grammar for arrow expression.");
					return;
					break;
				case signal_types::dot_: {
					opt_expr(tree, it.left());
					token_base *lptr = it.left().data();
					token_base *rptr = it.right().data();
					if (rptr == nullptr || rptr->get_type() != token_types::id)
						throw syntax_error("Wrong grammar for dot expression.");
					if (lptr != nullptr && lptr->get_type() == token_types::value) {
						const var &a = static_cast<token_value *>(lptr)->get_value();
						token_base *orig_ptr = it.data();
						try {
							const var &v = parse_dot(a, rptr);
							if (v.is_protect())
								it.data() = new_value(v);
						}
						catch (...) {
							it.data() = orig_ptr;
						}
					}
					return;
					break;
				}
				case signal_types::fcall_: {
					opt_expr(tree, it.left());
					opt_expr(tree, it.right());
					token_base *lptr = it.left().data();
					token_base *rptr = it.right().data();
					if (lptr == nullptr || rptr == nullptr || rptr->get_type() != token_types::arglist)
						throw syntax_error("Wrong syntax for function call.");
					if (lptr->get_type() == token_types::value) {
						var &a = static_cast<token_value *>(lptr)->get_value();
						if (a.type() == typeid(callable) && a.const_val<callable>().is_constant()) {
							bool is_optimizable = true;
							for (auto &tree:static_cast<token_arglist *>(rptr)->get_arglist()) {
								if (is_optimizable && !optimizable(tree.root()))
									is_optimizable = false;
							}
							if (is_optimizable) {
								vector args;
								args.reserve(static_cast<token_arglist *>(rptr)->get_arglist().size());
								for (auto &tree:static_cast<token_arglist *>(rptr)->get_arglist())
									args.push_back(parse_expr(tree.root()));
								token_base *oldt = it.data();
								try {
									it.data() = new_value(a.const_val<callable>().call(args));
								}
								catch (...) {
									it.data() = oldt;
								}
							}
						}
						else if (a.type() == typeid(object_method) && a.const_val<object_method>().is_constant) {
							bool is_optimizable = true;
							for (auto &tree:static_cast<token_arglist *>(rptr)->get_arglist()) {
								if (is_optimizable && !optimizable(tree.root()))
									is_optimizable = false;
							}
							if (is_optimizable) {
								const object_method &om = a.const_val<object_method>();
								vector args{om.object};
								args.reserve(static_cast<token_arglist *>(rptr)->get_arglist().size());
								for (auto &tree:static_cast<token_arglist *>(rptr)->get_arglist())
									args.push_back(parse_expr(tree.root()));
								token_base *oldt = it.data();
								try {
									it.data() = new_value(om.callable.const_val<callable>().call(args));
								}
								catch (...) {
									it.data() = oldt;
								}
							}
						}
					}
					return;
					break;
				}
				case signal_types::emb_: {
					opt_expr(tree, it.left());
					opt_expr(tree, it.right());
					token_base *lptr = it.left().data();
					token_base *rptr = it.right().data();
					if (!inside_lambda || lptr != nullptr || rptr == nullptr ||
					        rptr->get_type() != token_types::arglist)
						throw syntax_error("Wrong grammar for lambda expression.");
					it.data() = rptr;
					return;
					break;
				}
				case signal_types::lambda_: {
					inside_lambda = true;
					opt_expr(tree, it.left());
					inside_lambda = false;
					opt_expr(tree, it.right());
					token_base *lptr = it.left().data();
					token_base *rptr = it.right().data();
					if (lptr == nullptr || rptr == nullptr || lptr->get_type() != token_types::arglist)
						throw syntax_error("Wrong grammar for lambda expression.");
					std::vector<std::string> args;
					for (auto &it:dynamic_cast<token_arglist *>(lptr)->get_arglist()) {
						if (it.root().data() == nullptr)
							throw internal_error("Null pointer accessed.");
						if (it.root().data()->get_type() != token_types::id)
							throw syntax_error("Wrong grammar for function definition.");
						const std::string &str = dynamic_cast<token_id *>(it.root().data())->get_id();
						for (auto &it:args)
							if (it == str)
								throw syntax_error("Redefinition of function argument.");
						args.push_back(str);
					}
					it.data() = new_value(var::make_protect<callable>(function(context, args,
					std::deque<statement_base *> {
						new statement_return(
						cov::tree<token_base *> {
							it.right()
						},
						context,
						new token_endline(
						    token->get_line_num()))
					})));
					return;
					break;
				}
				}
			}
		}
		opt_expr(tree, it
		         .

		         left()

		        );
		opt_expr(tree, it
		         .

		         right()

		        );
		if (
		    optimizable(it
		                .

		                left()

		               ) &&
		    optimizable(it
		                .

		                right()

		               )) {
			token_base *oldt = it.data();
			try {
				token_value *token = new_value(parse_expr(it));
				tree.
				erase_left(it);
				tree.
				erase_right(it);
				it.

				data() = token;

			}
			catch (...) {
				it.

				data() = oldt;

			}
		}
	}

	void instance_type::parse_define_var(cov::tree<token_base *> &tree, define_var_profile &dvp)
	{
		const auto &it = tree.root();
		token_base *root = it.data();
		if (root == nullptr || root->get_type() != token_types::signal ||
		        static_cast<token_signal *>(root)->get_signal() != signal_types::asi_)
			throw syntax_error("Wrong grammar for variable definition.");
		token_base *left = it.left().data();
		const auto &right = it.right();
		if (left == nullptr || right.data() == nullptr || left->get_type() != token_types::id)
			throw syntax_error("Wrong grammar for variable definition.");
		dvp.id = static_cast<token_id *>(left)->get_id();
		dvp.expr = right;
	}

	extension_t instance_type::import(const std::string &path, const std::string &name)
	{
		std::vector<std::string> collection;
		{
			std::string tmp;
			for (auto &ch:path) {
				if (ch == cs::path_delimiter) {
					collection.push_back(tmp);
					tmp.clear();
				}
				else
					tmp.push_back(ch);
			}
			collection.push_back(tmp);
		}
		for (auto &it:collection) {
			std::string package_path = it + path_separator + name;
			if (std::ifstream(package_path + ".csp")) {
				refers.emplace_front();
				instance_type &instance = refers.front();
				instance.compile(package_path + ".csp");
				instance.interpret();
				context_t rt = instance.context;
				if (rt->package_name.empty())
					throw syntax_error("Target file is not a package.");
				if (rt->package_name != name)
					throw syntax_error("Package name is different from file name.");
				return std::make_shared<extension_holder>(rt->instance->storage.get_global());
			}
			else if (std::ifstream(package_path + ".cse"))
				return std::make_shared<extension_holder>(package_path + ".cse");
		}
		throw fatal_error("No such file or directory.");
	}

	void instance_type::compile(const std::string &path)
	{
		context->file_path = path;
		// Read from file
		std::deque<char> buff;
		std::ifstream in(path);
		if (!in.is_open())
			throw fatal_error(path + ": No such file or directory");
		for (int ch = in.get(); ch != EOF; ch = in.get())
			buff.push_back(ch);
		// Lexer
		std::deque<token_base *> tokens;
		translate_into_tokens(buff, tokens);
		// Parser
		translate_into_statements(tokens, statements);
		// Mark Constants
		mark_constant();
	}

	void instance_type::interpret()
	{
		// Run the instruction
		for (auto &ptr:statements) {
			try {
				ptr->run();
			}
			catch (const lang_error &le) {
				throw fatal_error(std::string("Uncaught exception: ") + le.what());
			}
			catch (const cs::exception &e) {
				throw e;
			}
			catch (const std::exception &e) {
				throw exception(ptr->get_line_num(), ptr->get_file_path(), ptr->get_raw_code(), e.what());
			}
		}
	}

	void repl::run(const string &code)
	{
		if (code.empty())
			return;
		std::deque<char> buff;
		for (auto &ch:code)
			buff.push_back(ch);
		try {
			// Lexer
			std::deque<token_base *> line;
			context->instance->process_char_buff(buff, line);
			line.push_back(new token_endline(line_num));
			// Parse
			context->instance->process_brackets(line);
			context->instance->kill_brackets(line);
			context->instance->kill_expr(line);
			method_base *m = context->instance->translator.match(line);
			switch (m->get_type()) {
			case method_types::null:
				throw syntax_error("Null type of grammar.");
				break;
			case method_types::single: {
				if (level > 0) {
					if (m->get_target_type() == statement_types::end_) {
						context->instance->storage.remove_set();
						context->instance->storage.remove_domain();
						--level;
					}
					if (level == 0) {
						method->translate(tmp)->repl_run();
						tmp.clear();
						method = nullptr;
					}
					else
						tmp.push_back(line);
				}
				else
					m->translate({line})->repl_run();
			}
			break;
			case method_types::block: {
				if (level == 0)
					method = m;
				++level;
				context->instance->storage.add_domain();
				context->instance->storage.add_set();
				m->preprocess({line});
				tmp.push_back(line);
			}
			break;
			case method_types::jit_command:
				m->translate({line});
				break;
			}
		}
		catch (const lang_error &le) {
			throw fatal_error(std::string("Uncaught exception: ") + le.what());
		}
		catch (const cs::exception &e) {
			throw e;
		}
		catch (const std::exception &e) {
			throw exception(line_num, context->file_path, code, e.what());
		}
		context->instance->mark_constant();
	}

	void repl::exec(const string &code)
	{
		// Preprocess
		++line_num;
		int mode = 0;
		for (auto &ch:code) {
			if (mode == 0) {
				if (!std::isspace(ch)) {
					switch (ch) {
					case '#':
						context->file_buff.emplace_back();
						return;
					case '@':
						mode = 1;
						break;
					default:
						mode = -1;
					}
				}
			}
			else if (mode == 1) {
				if (!std::isspace(ch))
					cmd_buff.push_back(ch);
			}
			else
				break;
		}
		switch (mode) {
		default:
			break;
		case 0:
			return;
		case 1:
			if (cmd_buff == "begin" && !multi_line) {
				multi_line = true;
				context->file_buff.emplace_back();
			}
			else if (cmd_buff == "end" && multi_line) {
				multi_line = false;
				this->run(line_buff);
				line_buff.clear();
			}
			else
				throw exception(line_num, context->file_path, cmd_buff, "Wrong grammar for preprocessor command.");
			cmd_buff.clear();
			return;
		}
		if (multi_line) {
			context->file_buff.emplace_back();
			line_buff.append(code);
		}
		else {
			context->file_buff.push_back(code);
			this->run(code);
		}
	}

}
