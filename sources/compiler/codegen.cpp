/*
 * Covariant Script Code Generating
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
 * Copyright (C) 2017-2026 Michael Lee(李登淳)
 *
 * This software is registered with the National Copyright Administration
 * of the People's Republic of China(Registration Number: 2020SR0408026)
 * and is protected by the Copyright Law of the People's Republic of China.
 *
 * Email:   mikecovlee@163.com
 * Github:  https://github.com/mikecovlee
 * Website: http://covscript.org.cn
 */
#include <covscript/impl/codegen.hpp>

namespace cs
{
	// Frees body statements if translate() throws; release() after transfer.
	class body_guard
	{
		std::deque<statement_base *> &m_body;
		bool m_released = false;

	   public:
		explicit body_guard(std::deque<statement_base *> &body) : m_body(body) {}

		body_guard(const body_guard &) = delete;

		body_guard &operator=(const body_guard &) = delete;

		void release() noexcept
		{
			m_released = true;
		}

		~body_guard()
		{
			if (!m_released && !m_body.empty())
				statement_base::delete_children(m_body);
		}
	};

	statement_base *
	method_expression::translate(context_type *context, const std::deque<std::deque<token_base *>> &raw)
	{
		tree_type<token_base *> &tree = static_cast<token_expr *>(raw.front().front())->get_tree();
		token_base *root = tree.root().usable() ? tree.root().data() : nullptr;
		if (root == nullptr)
			throw compile_error("Invalid expression statement: the expression is empty");
		if (context->compiler->fold_expr && root->get_type() == token_types::value)
			return nullptr;
		else
			return new statement_expression(tree, context, raw.front().back());
	}

	void method_import::preprocess(context_type *context, const std::deque<std::deque<token_base *>> &raw)
	{
		tree_type<token_base *> &tree = static_cast<token_expr *>(raw.front().at(1))->get_tree();
		if (tree.root().data() == nullptr)
			throw internal_error("Null pointer accessed.");
		std::vector<std::pair<std::string, var>> var_list;
		auto process = [&context, &var_list](tree_type<token_base *> &t)
		{
			token_base *token = t.root().data();
			if (token == nullptr || token->get_type() != token_types::id)
				throw compile_error("Invalid 'import' statement: expected a package name");
			const var_id &package_name = static_cast<token_id *>(token)->get_id();
			const var &ext = make_namespace(context->instance->import(current_process->import_path, package_name));
			context->compiler->add_constant(ext);
			context->instance->storage.add_var_no_return(package_name, ext);
			var_list.emplace_back(package_name, ext);
		};
		if (tree.root().data()->get_type() == token_types::parallel)
		{
			auto &parallel_list = static_cast<token_parallel *>(tree.root().data())->get_parallel();
			for (auto &t : parallel_list)
				process(t);
		}
		else
			process(tree);
		context->compiler->push_import_result(new statement_import(var_list, context, raw.front().back()));
	}

	statement_base *
	method_import::translate(context_type *context, const std::deque<std::deque<token_base *>> &raw)
	{
		statement_base *ptr = context->compiler->pop_import_result();
		if (ptr == nullptr)
			throw compile_error("Invalid 'import' statement: missing preprocessing result");
		return ptr;
	}

	var method_import_as::get_namespace(context_type *context, tree_type<token_base *>::iterator it)
	{
		token_base *token = it.data();
		if (token == nullptr)
			return var();
		if (token->get_type() == token_types::id)
		{
			const var_id &package_name = static_cast<token_id *>(token)->get_id();
			return make_namespace(context->instance->import(current_process->import_path, package_name));
		}
		else if (token->get_type() == token_types::signal &&
		         static_cast<token_signal *>(token)->get_signal() == signal_types::dot_)
		{
			const var &ext = get_namespace(context, it.left());
			token_base *id = it.right().data();
			if (id == nullptr || id->get_type() != token_types::id)
				throw compile_error(
				    "Invalid 'import' statement: expected '<package name>' or '<package name>.<namespace id>...'");
			if (ext.is_type_of<namespace_t>())
				return ext.const_val<namespace_t>()->get_var(static_cast<token_id *>(id)->get_id());
			else
				throw compile_error("Invalid 'import' statement: the value at the left-hand side of '.' is not a namespace");
		}
		else
			throw compile_error(
			    "Invalid 'import' statement: expected '<package name>' or '<package name>.<namespace id>...'");
	}

	void method_import_as::preprocess(context_type *context, const std::deque<std::deque<token_base *>> &raw)
	{
		tree_type<token_base *> &tree_package = static_cast<token_expr *>(raw.front().at(1))->get_tree();
		tree_type<token_base *> &tree_alias = static_cast<token_expr *>(raw.front().at(3))->get_tree();
		if (tree_package.root().data() == nullptr || tree_alias.root().data() == nullptr)
			throw internal_error("Null pointer accessed.");
		token_base *token_alias = tree_alias.root().data();
		if (token_alias->get_type() != token_types::id)
			throw compile_error("Invalid 'import ... as' statement: the alias must be an identifier");
		const var_id &alias_name = static_cast<token_id *>(token_alias)->get_id();
		var ext = get_namespace(context, tree_package.root());
		context->compiler->add_constant(ext);
		context->instance->storage.add_var_no_return(alias_name, ext);
		context->compiler->push_import_result(new statement_import({{alias_name, ext}}, context, raw.front().back()));
	}

	statement_base *
	method_import_as::translate(context_type *context, const std::deque<std::deque<token_base *>> &raw)
	{
		statement_base *ptr = context->compiler->pop_import_result();
		if (ptr == nullptr)
			throw compile_error("Invalid 'import' statement: missing preprocessing result");
		return ptr;
	}

	statement_base *
	method_package::translate(context_type *context, const std::deque<std::deque<token_base *>> &raw)
	{
		if (!context->package_name.empty())
			throw compile_error("Invalid 'package' declaration: this file already declared its package name as '" + context->package_name + "'");
		token_base *root = static_cast<token_expr *>(raw.front().at(1))->get_tree().root().usable()
		                       ? static_cast<token_expr *>(raw.front().at(1))->get_tree().root().data()
		                       : nullptr;
		if (root == nullptr || root->get_type() != token_types::id)
			throw compile_error("Invalid 'package' declaration: expected a package name");
		context->package_name = static_cast<token_id *>(root)->get_id();
		return nullptr;
	}

	void method_involve::preprocess(context_type *context, const std::deque<std::deque<token_base *>> &raw)
	{
		tree_type<token_base *> &tree = static_cast<token_expr *>(raw.front().at(1))->get_tree();
		token_value *vptr = dynamic_cast<token_value *>(tree.root().data());
		if (vptr != nullptr)
		{
			var ns = vptr->get_value();
			if (ns.is_type_of<namespace_t>())
			{
				auto &domain = ns.const_val<namespace_t>()->get_domain();
				for (auto &it : domain)
				{
					if (domain.get_var_by_id(it.second).is_protect())
						context->instance->storage.add_record(it.first);
				}
				context->instance->storage.involve_domain(domain);
			}
			else
				throw compile_error("Invalid 'using' statement: the target must be a namespace");
			context->compiler->push_import_result(new statement_involve(tree, true, context, raw.front().back()));
		}
		else if (tree.root().data() != nullptr && tree.root().data()->get_type() == token_types::id)
		{
			// 'using <id>': resolve from storage so a namespace's constants reach
			// compile-time folding even when it holds a script function.
			const std::string &name = static_cast<token_id *>(tree.root().data())->get_id();
			try
			{
				const var &ns = context->instance->storage.get_var(name);
				if (ns.is_type_of<namespace_t>())
				{
					auto &domain = ns.const_val<namespace_t>()->get_domain();
					for (auto &it : domain)
					{
						if (domain.get_var_by_id(it.second).is_protect())
							context->instance->storage.add_record(it.first);
					}
					context->instance->storage.involve_domain(domain);
					context->compiler->push_import_result(new statement_involve(tree, true, context, raw.front().back()));
					return;
				}
			}
			catch (...)
			{
				// Undefined or not a namespace: handled by the runtime path below.
			}
			context->compiler->push_import_result(new statement_involve(tree, false, context, raw.front().back()));
		}
		else
			context->compiler->push_import_result(new statement_involve(tree, false, context, raw.front().back()));
	}

	statement_base *
	method_involve::translate(context_type *context, const std::deque<std::deque<token_base *>> &raw)
	{
		statement_base *ptr = context->compiler->pop_import_result();
		if (ptr == nullptr)
			throw compile_error("Invalid 'using' statement: missing preprocessing result");
		return ptr;
	}

	void method_var::preprocess(context_type *context, const std::deque<std::deque<token_base *>> &raw)
	{
		context->instance->check_define_var(static_cast<token_expr *>(raw.front().at(1))->get_tree().root());
	}

	statement_base *method_var::translate(context_type *context, const std::deque<std::deque<token_base *>> &raw)
	{
		return new statement_var(static_cast<token_expr *>(raw.front().at(1))->get_tree(), context, false,
		                         raw.front().back());
	}

	void method_link::preprocess(context_type *context, const std::deque<std::deque<token_base *>> &raw)
	{
		context->instance->check_define_var(static_cast<token_expr *>(raw.front().at(1))->get_tree().root());
	}

	statement_base *method_link::translate(context_type *context, const std::deque<std::deque<token_base *>> &raw)
	{
		return new statement_var(static_cast<token_expr *>(raw.front().at(1))->get_tree(), context, true,
		                         raw.front().back());
	}

	void method_constant::preprocess(context_type *context, const std::deque<std::deque<token_base *>> &raw)
	{
		tree_type<token_base *> &tree = static_cast<token_expr *>(raw.front().at(1))->get_tree();
		context->compiler->force_fold(tree);
		context->instance->check_define_var(tree.root(), false, true);
		context->instance->parse_define_var(tree.root(), true);
	}

	statement_base *
	method_constant::translate(context_type *context, const std::deque<std::deque<token_base *>> &raw)
	{
		tree_type<token_base *> &tree = static_cast<token_expr *>(raw.front().at(1))->get_tree();
		return new statement_constant(tree, context, raw.front().back());
	}

	statement_base *method_block::translate(context_type *context, const std::deque<std::deque<token_base *>> &raw)
	{
		std::deque<statement_base *> body;
		body_guard guard(body);
		context->compiler->translate({raw.begin() + 1, raw.end()}, body);
		auto *stmt = new statement_block(std::move(body), context, raw.front().back());
		guard.release();
		return stmt;
	}

	void method_namespace::preprocess(context_type *context, const std::deque<std::deque<token_base *>> &raw)
	{
		tree_type<token_base *> &tree = static_cast<token_expr *>(raw.front().at(1))->get_tree();
		token_base *root = tree.root().usable() ? tree.root().data() : nullptr;
		if (root == nullptr || root->get_type() != token_types::id)
		{
			std::size_t line_num = static_cast<token_endline *>(raw.front().back())->get_line_num();
			const char *what = root != nullptr && root->get_type() == token_types::value
			                       ? "Invalid 'namespace' declaration: the namespace name is already defined"
			                       : "Invalid 'namespace' declaration: expected a namespace name";
			throw exception(line_num, context->file_path, context->get_file_line(line_num), what);
		}
		const var_id &name = static_cast<token_id *>(root)->get_id();
		context->instance->storage.add_var_no_return("__PRAGMA_CS_NAMESPACE_DEFINITION__",
		                                             var::make<const var_id *>(&name));
	}

	statement_base *
	method_namespace::translate(context_type *context, const std::deque<std::deque<token_base *>> &raw)
	{
		std::deque<statement_base *> body;
		body_guard guard(body);
		context->compiler->translate({raw.begin() + 1, raw.end()}, body);
		for (auto &ptr : body)
			if (ptr->get_type() != statement_types::import_ && ptr->get_type() != statement_types::involve_ &&
			    ptr->get_type() != statement_types::var_ && ptr->get_type() != statement_types::function_ &&
			    ptr->get_type() != statement_types::namespace_ && ptr->get_type() != statement_types::struct_)
				throw compile_error("Invalid 'namespace' body: only 'import', 'using', variable declarations, function definitions, 'namespace' definitions, and 'struct' definitions are allowed");
		auto *stmt = new statement_namespace(static_cast<token_expr *>(raw.front().at(1))->get_tree().root().data(),
		                                     std::move(body), context, raw.front().back());
		guard.release();
		return stmt;
	}

	void method_namespace::postprocess(context_type *context, const domain_type &domain)
	{
		context->instance->storage.add_var_no_return(*domain.get_var("__PRAGMA_CS_NAMESPACE_DEFINITION__").const_val<const var_id *>(),
		                                             make_namespace(make_shared_namespace<name_space>(domain)));
	}

	statement_base *method_if::translate(context_type *context, const std::deque<std::deque<token_base *>> &raw)
	{
		bool have_else = false;
		std::deque<statement_base *> body;
		body_guard guard(body);
		context->compiler->translate({raw.begin() + 1, raw.end()}, body);
		for (auto &ptr : body)
		{
			if (ptr->get_type() == statement_types::else_)
			{
				if (!have_else)
					have_else = true;
				else
					throw compile_error("An 'if' block can only have one 'else' clause");
			}
		}
		tree_type<token_base *> &tree = static_cast<token_expr *>(raw.front().at(1))->get_tree();
		token_base *ptr = tree.root().data();
		if (have_else)
		{
			std::deque<statement_base *> body_true;
			std::deque<statement_base *> body_false;
			bool now_place = true;
			for (auto &ptr : body)
			{
				if (ptr->get_type() == statement_types::else_)
				{
					now_place = false;
					// The marker is consumed here and is not part of either branch.
					delete ptr;
					continue;
				}
				if (now_place)
					body_true.push_back(ptr);
				else
					body_false.push_back(ptr);
			}
			// Ownership moved into the branch deques; the guard must not see them.
			body.clear();
			if (ptr != nullptr && ptr->get_type() == token_types::value)
			{
				if (static_cast<token_value *>(ptr)->get_value().const_val<bool>())
				{
					statement_base::delete_children(body_false);
					return new statement_block(std::move(body_true), context, raw.front().back());
				}
				else
				{
					statement_base::delete_children(body_true);
					return new statement_block(std::move(body_false), context, raw.front().back());
				}
			}
			else
				return new statement_ifelse(tree, std::move(body_true), std::move(body_false), context,
				                            raw.front().back());
		}
		else if (ptr != nullptr && ptr->get_type() == token_types::value)
		{
			if (static_cast<token_value *>(ptr)->get_value().const_val<bool>())
			{
				auto *stmt = new statement_block(std::move(body), context, raw.front().back());
				guard.release();
				return stmt;
			}
			else
			{
				statement_base::delete_children(body);
				return nullptr;
			}
		}
		else
		{
			auto *stmt = new statement_if(tree, std::move(body), context, raw.front().back());
			guard.release();
			return stmt;
		}
	}

	void method_else::preprocess(context_type *context, const std::deque<std::deque<token_base *>> &)
	{
		context->instance->storage.clear_domain();
		context->instance->storage.clear_set();
	}

	statement_base *method_else::translate(context_type *context, const std::deque<std::deque<token_base *>> &raw)
	{
		return new statement_else;
	}

	statement_base *
	method_switch::translate(context_type *context, const std::deque<std::deque<token_base *>> &raw)
	{
		std::deque<statement_base *> body;
		body_guard guard(body);
		statement_block *dptr = nullptr;
		map_t<var, statement_block *> cases;
		try
		{
			// Translate inside the try so a failure reclaims the case/default
			// wrappers already translated (their blocks are not owned by the
			// wrappers' destructors).
			context->compiler->translate({raw.begin() + 1, raw.end()}, body);
			for (auto &it : body)
			{
				try
				{
					if (it->get_type() == statement_types::case_)
					{
						auto *scptr = static_cast<statement_case *>(it);
						if (cases.count(scptr->get_tag()) > 0)
							throw compile_error("Duplicate 'case' label in 'switch' statement");
						cases.emplace(scptr->get_tag(), scptr->get_block());
						delete it; // wrapper consumed; its block is now owned by the switch
						it = nullptr;
					}
					else if (it->get_type() == statement_types::default_)
					{
						auto *sdptr = static_cast<statement_default *>(it);
						if (dptr != nullptr)
							throw compile_error("A 'switch' statement can only have one 'default' case");
						dptr = sdptr->get_block();
						delete it; // wrapper consumed; its block is now owned by the switch
						it = nullptr;
					}
					else
						throw compile_error("Only 'case' and 'default' clauses are allowed inside a 'switch' statement");
				}
				catch (const cs::exception &)
				{
					throw;
				}
				catch (const std::exception &e)
				{
					throw exception(it->get_line_num(), it->get_file_path(), it->get_raw_code(), exception_message(e));
				}
			}
		}
		catch (...)
		{
			// Reclaim blocks already transferred and unconsumed wrappers.
			for (auto &kv : cases)
				delete kv.second;
			if (dptr != nullptr)
				delete dptr;
			for (auto *ptr : body)
			{
				if (ptr == nullptr)
					continue;
				if (ptr->get_type() == statement_types::case_)
					delete static_cast<statement_case *>(ptr)->get_block();
				else if (ptr->get_type() == statement_types::default_)
					delete static_cast<statement_default *>(ptr)->get_block();
				delete ptr;
			}
			guard.release();
			throw;
		}
		guard.release();
		return new statement_switch(static_cast<token_expr *>(raw.front().at(1))->get_tree(), cases, dptr, context,
		                            raw.front().back());
	}

	statement_base *method_case::translate(context_type *context, const std::deque<std::deque<token_base *>> &raw)
	{
		tree_type<token_base *> &tree = static_cast<token_expr *>(raw.front().at(1))->get_tree();
		context->compiler->force_fold(tree);
		token_base *root = tree.root().usable() ? tree.root().data() : nullptr;
		if (root == nullptr || root->get_type() != token_types::value)
		{
			std::size_t line_num = static_cast<token_endline *>(raw.front().back())->get_line_num();
			const char *what = "A 'case' label must be a constant value";
			throw exception(line_num, context->file_path, context->get_file_line(line_num), what);
		}
		std::deque<statement_base *> body;
		// statement_case copies its body; clean up only on failure.
		try
		{
			context->compiler->translate({raw.begin() + 1, raw.end()}, body);
			return new statement_case(static_cast<token_value *>(root)->get_value(), body, context,
			                          raw.front().back());
		}
		catch (...)
		{
			statement_base::delete_children(body);
			throw;
		}
	}

	statement_base *
	method_default::translate(context_type *context, const std::deque<std::deque<token_base *>> &raw)
	{
		std::deque<statement_base *> body;
		// statement_default copies its body; clean up only on failure.
		try
		{
			context->compiler->translate({raw.begin() + 1, raw.end()}, body);
			return new statement_default(body, context, raw.front().back());
		}
		catch (...)
		{
			statement_base::delete_children(body);
			throw;
		}
	}

	statement_base *method_while::translate(context_type *context, const std::deque<std::deque<token_base *>> &raw)
	{
		std::deque<statement_base *> body;
		body_guard guard(body);
		context->compiler->translate({raw.begin() + 1, raw.end()}, body);
		tree_type<token_base *> &tree = static_cast<token_expr *>(raw.front().at(1))->get_tree();
		token_base *ptr = tree.root().data();
		if (ptr != nullptr && ptr->get_type() == token_types::value)
		{
			if (static_cast<token_value *>(ptr)->get_value().const_val<bool>())
			{
				auto *stmt = new statement_loop(std::move(body), context, raw.front().back());
				guard.release();
				return stmt;
			}
			else
			{
				statement_base::delete_children(body);
				return nullptr;
			}
		}
		else
		{
			auto *stmt = new statement_while(static_cast<token_expr *>(raw.front().at(1))->get_tree(), std::move(body),
			                                 context, raw.front().back());
			guard.release();
			return stmt;
		}
	}

	statement_base *method_until::translate_end(method_base *method, context_type *context,
	                                            std::deque<std::deque<token_base *>> &raw,
	                                            std::deque<token_base *> &code)
	{
		if (method != nullptr && method->get_target_type() == statement_types::loop_)
			return static_cast<method_loop *>(method)->translate(context, raw,
			                                                     static_cast<token_expr *>(code.at(1))->get_tree());
		else
			throw compile_error("The 'until' clause can only be used to close a 'loop' block");
	}

	statement_base *method_loop::translate(context_type *context, const std::deque<std::deque<token_base *>> &raw)
	{
		std::deque<statement_base *> body;
		body_guard guard(body);
		context->compiler->translate({raw.begin() + 1, raw.end()}, body);
		auto *stmt = new statement_loop(std::move(body), context, raw.front().back());
		guard.release();
		return stmt;
	}

	statement_base *method_loop::translate(context_type *context, const std::deque<std::deque<token_base *>> &raw,
	                                       const tree_type<token_base *> &cond)
	{
		std::deque<statement_base *> body;
		body_guard guard(body);
		context->compiler->translate({raw.begin() + 1, raw.end()}, body);
		token_base *ptr = cond.root().data();
		if (ptr != nullptr && ptr->get_type() == token_types::value)
		{
			if (static_cast<token_value *>(ptr)->get_value().const_val<bool>())
			{
				auto *stmt = new statement_block(std::move(body), context, raw.front().back());
				guard.release();
				return stmt;
			}
			else
			{
				auto *stmt = new statement_loop(std::move(body), context, raw.front().back());
				guard.release();
				return stmt;
			}
		}
		else
		{
			auto *stmt = new statement_loop_until(cond, std::move(body), context, raw.front().back());
			guard.release();
			return stmt;
		}
	}

	void method_for::preprocess(context_type *context, const std::deque<std::deque<token_base *>> &raw)
	{
		tree_type<token_base *> &tree = static_cast<token_expr *>(raw.front().at(1))->get_tree();
		if (tree.root().data() == nullptr)
			throw internal_error("Null pointer accessed.");
		if (tree.root().data()->get_type() != token_types::parallel)
			throw compile_error("Invalid 'for' statement: expected '<id> = <expression>, <condition>, <process>'");
		auto &parallel_list = static_cast<token_parallel *>(tree.root().data())->get_parallel();
		if (parallel_list.size() != 3)
			throw compile_error("Invalid 'for' statement: expected '<id> = <expression>, <condition>, <process>'");
		context->instance->check_define_var(parallel_list[0].root(), true);
	}

	statement_base *method_for::translate(context_type *context, const std::deque<std::deque<token_base *>> &raw)
	{
		std::deque<statement_base *> body;
		body_guard guard(body);
		context->compiler->translate({raw.begin() + 1, raw.end()}, body);
		tree_type<token_base *> &tree = static_cast<token_expr *>(raw.front().at(1))->get_tree();
		auto &parallel_list = static_cast<token_parallel *>(tree.root().data())->get_parallel();
		auto *stmt = new statement_for(parallel_list, std::move(body), context, raw.front().back());
		guard.release();
		return stmt;
	}

	statement_base *
	method_for_do::translate(context_type *context, const std::deque<std::deque<token_base *>> &raw)
	{
		tree_type<token_base *> &tree = static_cast<token_expr *>(raw.front().at(1))->get_tree();
		if (tree.root().data() == nullptr)
			throw internal_error("Null pointer accessed.");
		if (tree.root().data()->get_type() != token_types::parallel)
			throw compile_error("Invalid 'for' statement: expected '<id> = <expression>, <condition>, <process>'");
		auto &parallel_list = static_cast<token_parallel *>(tree.root().data())->get_parallel();
		if (parallel_list.size() != 3)
			throw compile_error("Invalid 'for' statement: expected '<id> = <expression>, <condition>, <process>'");
		context->instance->check_define_var(parallel_list[0].root());
		return new statement_for(parallel_list, {new statement_expression(static_cast<token_expr *>(raw.front().at(3))->get_tree(), context, raw.front().back())}, context, raw.front().back());
	}

	void method_foreach::preprocess(context_type *context, const std::deque<std::deque<token_base *>> &raw)
	{
		tree_type<token_base *> &t = static_cast<token_expr *>(raw.front().at(1))->get_tree();
		if (t.root().data() == nullptr)
			throw internal_error("Null pointer accessed.");
		if (t.root().data()->get_type() != token_types::id)
			throw compile_error("Invalid 'foreach' statement: expected an identifier as the iterator");
		context->instance->storage.add_record(static_cast<token_id *>(t.root().data())->get_id().get_id());
	}

	statement_base *
	method_foreach::translate(context_type *context, const std::deque<std::deque<token_base *>> &raw)
	{
		tree_type<token_base *> &t = static_cast<token_expr *>(raw.front().at(1))->get_tree();
		const var_id &it = static_cast<token_id *>(t.root().data())->get_id();
		std::deque<statement_base *> body;
		body_guard guard(body);
		context->compiler->translate({raw.begin() + 1, raw.end()}, body);
		auto *stmt = new statement_foreach(it, static_cast<token_expr *>(raw.front().at(3))->get_tree(), std::move(body),
		                                   context, raw.front().back());
		guard.release();
		return stmt;
	}

	statement_base *
	method_foreach_do::translate(context_type *context, const std::deque<std::deque<token_base *>> &raw)
	{
		tree_type<token_base *> &t = static_cast<token_expr *>(raw.front().at(1))->get_tree();
		if (t.root().data() == nullptr)
			throw internal_error("Null pointer accessed.");
		if (t.root().data()->get_type() != token_types::id)
			throw compile_error("Invalid 'foreach' statement: expected an identifier as the iterator");
		const var_id &it = static_cast<token_id *>(t.root().data())->get_id();
		std::deque<statement_base *> body;
		body_guard guard(body);
		context->compiler->translate({raw.begin() + 1, raw.end()}, body);
		body.push_back(new statement_expression(static_cast<token_expr *>(raw.front().at(5))->get_tree(), context,
		                                        raw.front().back()));
		auto *stmt = new statement_foreach(it, static_cast<token_expr *>(raw.front().at(3))->get_tree(), std::move(body),
		                                   context, raw.front().back());
		guard.release();
		return stmt;
	}

	statement_base *method_break::translate(context_type *context, const std::deque<std::deque<token_base *>> &raw)
	{
		if (context->compiler->loop_depth == 0)
			throw compile_error("Invalid 'break' statement: not inside any loop");
		return new statement_break(context, raw.front().back());
	}

	statement_base *
	method_continue::translate(context_type *context, const std::deque<std::deque<token_base *>> &raw)
	{
		if (context->compiler->loop_depth == 0)
			throw compile_error("Invalid 'continue' statement: not inside any loop");
		return new statement_continue(context, raw.front().back());
	}

	void method_function::preprocess(context_type *context, const std::deque<std::deque<token_base *>> &raw)
	{
		tree_type<token_base *> &t = static_cast<token_expr *>(raw.front().at(1))->get_tree();
		if (t.root().data() == nullptr)
			throw internal_error("Null pointer accessed.");
		if (t.root().data()->get_type() != token_types::signal ||
		    static_cast<token_signal *>(t.root().data())->get_signal() != signal_types::fcall_)
			throw compile_error("Invalid function definition: expected 'function <name>(<arguments>)'");
		if (t.root().left().data() == nullptr)
			throw internal_error("Null pointer accessed.");
		if (t.root().left().data()->get_type() != token_types::id)
			throw compile_error("Invalid function definition: expected a function name");
		if (t.root().right().data() == nullptr)
			throw internal_error("Null pointer accessed.");
		if (t.root().right().data()->get_type() != token_types::arglist)
			throw compile_error("Invalid function definition: expected an argument list");
		std::vector<std::string> args;
		for (auto &it : static_cast<token_arglist *>(t.root().right().data())->get_arglist())
		{
			if (it.root().data() == nullptr)
				throw internal_error("Null pointer accessed.");
			context->compiler->try_fix_this_deduction(it.root());
			if (it.root().data()->get_type() == token_types::id)
			{
				const std::string &str = static_cast<token_id *>(it.root().data())->get_id();
				for (auto &it : args)
					if (it == str)
						throw compile_error("Duplicate function argument name: " + str);
				context->instance->storage.add_record(str);
				args.push_back(str);
			}
			else if (it.root().data()->get_type() == token_types::vargs)
			{
				const std::string &str = static_cast<token_vargs *>(it.root().data())->get_id();
				if (!args.empty())
					throw compile_error("Invalid function definition: a variadic parameter '...<id>' must be the only parameter in the argument list");
				context->instance->storage.add_record(str);
				args.push_back(str);
			}
			else
				throw compile_error("Invalid function definition: unexpected element in the argument list; expected an identifier or a variadic parameter '...<id>'");
		}
	}

	statement_base *
	method_function::translate(context_type *context, const std::deque<std::deque<token_base *>> &raw)
	{
		tree_type<token_base *> &t = static_cast<token_expr *>(raw.front().at(1))->get_tree();
		std::string name = static_cast<token_id *>(t.root().left().data())->get_id();
		std::vector<std::string> args;
		bool is_vargs = false;
		for (auto &it : static_cast<token_arglist *>(t.root().right().data())->get_arglist())
		{
			if (it.root().data()->get_type() == token_types::id)
				args.push_back(static_cast<token_id *>(it.root().data())->get_id());
			else if (it.root().data()->get_type() == token_types::vargs)
			{
				args.push_back(static_cast<token_vargs *>(it.root().data())->get_id());
				is_vargs = true;
			}
		}
		std::deque<statement_base *> body;
		// statement_function copies its body; clean up only on failure.
		try
		{
			{
				value_guard<std::size_t> fn_guard(context->compiler->loop_depth, 0);
				context->compiler->translate({raw.begin() + 1, raw.end()}, body);
			}
#ifdef CS_DEBUGGER
			std::string decl = "function " + name + "(";
			if (args.size() != 0)
			{
				for (auto &it : args)
					decl += it + ", ";
				decl.pop_back();
				decl[decl.size() - 1] = ')';
			}
			else
				decl += ")";
			if (raw.front().size() == 4)
				decl += " override";
			return new statement_function(name, decl, args, body, raw.front().size() == 4, is_vargs, context, raw.front().back());
#else
			return new statement_function(name, args, body, raw.front().size() == 4, is_vargs, context, raw.front().back());
#endif
		}
		catch (...)
		{
			statement_base::delete_children(body);
			throw;
		}
	}

	statement_base *
	method_return::translate(context_type *context, const std::deque<std::deque<token_base *>> &raw)
	{
		return new statement_return(static_cast<token_expr *>(raw.front().at(1))->get_tree(), context,
		                            raw.front().back());
	}

	statement_base *
	method_return_no_value::translate(context_type *context, const std::deque<std::deque<token_base *>> &raw)
	{
		tree_type<token_base *> tree;
		tree.emplace_root_left(tree.root(), context->compiler->make_token<token_value>(null_pointer));
		return new statement_return(tree, context, raw.front().back());
	}

	void method_struct::preprocess(context_type *context, const std::deque<std::deque<token_base *>> &raw)
	{
		context->instance->storage.mark_set_as_struct(raw.front().size() == 5);
	}

	statement_base *
	method_struct::translate(context_type *context, const std::deque<std::deque<token_base *>> &raw)
	{
		tree_type<token_base *> &t = static_cast<token_expr *>(raw.front().at(1))->get_tree();
		if (t.root().data() == nullptr)
			throw internal_error("Null pointer accessed.");
		if (t.root().data()->get_type() != token_types::id)
			throw compile_error("Invalid 'struct' definition: expected a struct name");
		std::string name = static_cast<token_id *>(t.root().data())->get_id();
		std::deque<statement_base *> body;
		// statement_struct copies its method deque; clean up only on failure.
		try
		{
			context->compiler->translate({raw.begin() + 1, raw.end()}, body);
			for (auto &ptr : body)
			{
				try
				{
					switch (ptr->get_type())
					{
						default:
							throw compile_error("Invalid 'struct' body: only variable and function definitions are allowed");
						case statement_types::var_:
						case statement_types::constant_:
							break;
						case statement_types::function_:
							static_cast<statement_function *>(ptr)->set_mem_fn();
							break;
					}
				}
				catch (const cs::exception &)
				{
					throw;
				}
				catch (const std::exception &e)
				{
					throw exception(ptr->get_line_num(), ptr->get_file_path(), ptr->get_raw_code(), exception_message(e));
				}
			}
			if (raw.front().size() == 5)
				return new statement_struct(name, static_cast<token_expr *>(raw.front().at(3))->get_tree(), body, context,
				                            raw.front().back());
			else
				return new statement_struct(name, tree_type<token_base *>(), body, context, raw.front().back());
		}
		catch (...)
		{
			statement_base::delete_children(body);
			throw;
		}
	}

	statement_base *method_try::translate(context_type *context, const std::deque<std::deque<token_base *>> &raw)
	{
		std::deque<statement_base *> body;
		body_guard guard(body);
		context->compiler->translate({raw.begin() + 1, raw.end()}, body);
		std::string name;
		std::deque<statement_base *> tbody, cbody;
		body_guard guard_t(tbody);
		body_guard guard_c(cbody);
		bool founded = false;
		for (auto &ptr : body)
		{
			if (ptr->get_type() == statement_types::catch_)
			{
				name = static_cast<statement_catch *>(ptr)->get_name();
				founded = true;
				delete ptr; // marker consumed; not part of either body
				continue;
			}
			if (founded)
				cbody.push_back(ptr);
			else
				tbody.push_back(ptr);
		}
		// Ownership moved into the try/catch deques; the guard must not see them.
		body.clear();
		if (!founded)
			throw compile_error("Invalid 'try' statement: a matching 'catch' clause is required");
		auto *stmt = new statement_try(name, std::move(tbody), std::move(cbody), context, raw.front().back());
		guard_t.release();
		guard_c.release();
		return stmt;
	}

	statement_base *method_catch::translate(context_type *context, const std::deque<std::deque<token_base *>> &raw)
	{
		tree_type<token_base *> &t = static_cast<token_expr *>(raw.front().at(1))->get_tree();
		if (t.root().data() == nullptr)
			throw internal_error("Null pointer accessed.");
		if (t.root().data()->get_type() != token_types::id)
			throw compile_error("Invalid 'catch' statement: expected an identifier to bind the caught error");
		return new statement_catch(static_cast<token_id *>(t.root().data())->get_id(), context, raw.front().back());
	}

	statement_base *method_throw::translate(context_type *context, const std::deque<std::deque<token_base *>> &raw)
	{
		return new statement_throw(static_cast<token_expr *>(raw.front().at(1))->get_tree(), context,
		                           raw.front().back());
	}
} // namespace cs
