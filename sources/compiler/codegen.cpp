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
 * Copyright (C) 2017-2025 Michael Lee(李登淳)
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

namespace cs {
	statement_base *
	method_expression::translate(const context_t &context, const std::deque<std::deque<token_base *>> &raw)
	{
		tree_type<token_base *> &tree = static_cast<token_expr *>(raw.front().front())->get_tree();
		if (context->compiler->fold_expr && tree.root().usable() &&
		        tree.root().data()->get_type() == token_types::value)
			return nullptr;
		else
			return new statement_expression(tree, context, raw.front().back());
	}

	void method_import::preprocess(const context_t &context, const std::deque<std::deque<token_base *>> &raw)
	{
		tree_type<token_base *> &tree = static_cast<token_expr *>(raw.front().at(1))->get_tree();
		if (tree.root().data() == nullptr)
			throw internal_error("Null pointer accessed.");
		std::vector<std::pair<std::string, var>> var_list;
		auto process = [&context, &var_list](tree_type<token_base *> &t) {
			token_base *token = t.root().data();
			if (token == nullptr || token->get_type() != token_types::id)
				throw compile_error("Wrong grammar for import statement, expect <package name>");
			const var_id &package_name = static_cast<token_id *>(token)->get_id();
			const var &ext = make_namespace(context->instance->import(current_process->import_path, package_name));
			context->compiler->add_constant(ext);
			context->instance->storage.add_var_no_return(package_name, ext);
			var_list.emplace_back(package_name, ext);
		};
		if (tree.root().data()->get_type() == token_types::parallel) {
			auto &parallel_list = static_cast<token_parallel *>(tree.root().data())->get_parallel();
			for (auto &t : parallel_list)
				process(t);
		}
		else
			process(tree);
		mResult.emplace_back(new statement_import(var_list, context, raw.front().back()));
	}

	statement_base *
	method_import::translate(const context_t &context, const std::deque<std::deque<token_base *>> &raw)
	{
		statement_base *ptr = mResult.front();
		mResult.pop_front();
		return ptr;
	}

	var method_import_as::get_namespace(const context_t &context, tree_type<token_base *>::iterator it)
	{
		token_base *token = it.data();
		if (token == nullptr)
			return var();
		if (token->get_type() == token_types::id) {
			const var_id &package_name = static_cast<token_id *>(token)->get_id();
			return make_namespace(context->instance->import(current_process->import_path, package_name));
		}
		else if (token->get_type() == token_types::signal &&
		         static_cast<token_signal *>(token)->get_signal() == signal_types::dot_) {
			const var &ext = get_namespace(context, it.left());
			token_base *id = it.right().data();
			if (id == nullptr || id->get_type() != token_types::id)
				throw compile_error(
				    "Wrong grammar for import-as statement, expect <package name> or <package name>.<namespace id>...");
			if (ext.is_type_of<namespace_t>())
				return ext.const_val<namespace_t>()->get_var(static_cast<token_id *>(id)->get_id());
			else
				throw compile_error("Access non-namespace object.");
		}
		else
			throw compile_error(
			    "Wrong grammar for import-as statement, expect <package name> or <package name>.<namespace id>...");
	}

	void method_import_as::preprocess(const context_t &context, const std::deque<std::deque<token_base *>> &raw)
	{
		tree_type<token_base *> &tree_package = static_cast<token_expr *>(raw.front().at(1))->get_tree();
		tree_type<token_base *> &tree_alias = static_cast<token_expr *>(raw.front().at(3))->get_tree();
		if (tree_package.root().data() == nullptr || tree_alias.root().data() == nullptr)
			throw internal_error("Null pointer accessed.");
		token_base *token_alias = tree_alias.root().data();
		if (token_alias->get_type() != token_types::id)
			throw compile_error("Wrong grammar for import-as statement, expect <id> in alias.");
		const string &alias_name = static_cast<token_id *>(token_alias)->get_id().get_id();
		var ext = get_namespace(context, tree_package.root());
		context->compiler->add_constant(ext);
		context->instance->storage.add_var_no_return(alias_name, ext);
		mResult.emplace_back(new statement_import({{alias_name, ext}}, context, raw.front().back()));
	}

	statement_base *
	method_import_as::translate(const context_t &context, const std::deque<std::deque<token_base *>> &raw)
	{
		statement_base *ptr = mResult.front();
		mResult.pop_front();
		return ptr;
	}

	statement_base *
	method_package::translate(const context_t &context, const std::deque<std::deque<token_base *>> &raw)
	{
		if (!context->package_name.empty())
			throw compile_error(std::string("Redefinition of package: ") + context->package_name);
		context->package_name = static_cast<token_id *>(static_cast<token_expr *>(raw.front().at(
		                            1))
		                        ->get_tree()
		                        .root()
		                        .data())
		                        ->get_id();
		return nullptr;
	}

	void method_involve::preprocess(const context_t &context, const std::deque<std::deque<token_base *>> &raw)
	{
		tree_type<token_base *> &tree = static_cast<token_expr *>(raw.front().at(1))->get_tree();
		token_value *vptr = dynamic_cast<token_value *>(tree.root().data());
		if (vptr != nullptr) {
			var ns = vptr->get_value();
			if (ns.is_type_of<namespace_t>()) {
				auto &domain = ns.const_val<namespace_t>()->get_domain();
				for (auto &it : domain) {
					if (domain.get_var_by_id(it.second).is_protect())
						context->instance->storage.add_record(it.first.data());
				}
				context->instance->storage.involve_domain(domain);
			}
			else
				throw compile_error("Only support involve namespace.");
			mResult.emplace_back(new statement_involve(tree, true, context, raw.front().back()));
		}
		else
			mResult.emplace_back(new statement_involve(tree, false, context, raw.front().back()));
	}

	statement_base *
	method_involve::translate(const context_t &context, const std::deque<std::deque<token_base *>> &raw)
	{
		statement_base *ptr = mResult.front();
		mResult.pop_front();
		return ptr;
	}

	void method_var::preprocess(const context_t &context, const std::deque<std::deque<token_base *>> &raw)
	{
		context->instance->check_define_var(static_cast<token_expr *>(raw.front().at(1))->get_tree().root());
	}

	statement_base *method_var::translate(const context_t &context, const std::deque<std::deque<token_base *>> &raw)
	{
		return new statement_var(static_cast<token_expr *>(raw.front().at(1))->get_tree(), context, false,
		                         raw.front().back());
	}

	void method_link::preprocess(const context_t &context, const std::deque<std::deque<token_base *>> &raw)
	{
		context->instance->check_define_var(static_cast<token_expr *>(raw.front().at(1))->get_tree().root());
	}

	statement_base *method_link::translate(const context_t &context, const std::deque<std::deque<token_base *>> &raw)
	{
		return new statement_var(static_cast<token_expr *>(raw.front().at(1))->get_tree(), context, true,
		                         raw.front().back());
	}

	void method_constant::preprocess(const context_t &context, const std::deque<std::deque<token_base *>> &raw)
	{
		tree_type<token_base *> &tree = static_cast<token_expr *>(raw.front().at(1))->get_tree();
		context->instance->check_define_var(tree.root(), false, true);
		context->instance->parse_define_var(tree.root(), true);
	}

	statement_base *
	method_constant::translate(const context_t &context, const std::deque<std::deque<token_base *>> &raw)
	{
		tree_type<token_base *> &tree = static_cast<token_expr *>(raw.front().at(1))->get_tree();
		return new statement_constant(tree, context, raw.front().back());
	}

	statement_base *method_block::translate(const context_t &context, const std::deque<std::deque<token_base *>> &raw)
	{
		std::deque<statement_base *> body;
		context->compiler->translate({raw.begin() + 1, raw.end()}, body);
		return new statement_block(body, context, raw.front().back());
	}

	void method_namespace::preprocess(const context_t &context, const std::deque<std::deque<token_base *>> &raw)
	{
		std::string name = static_cast<token_id *>(static_cast<token_expr *>(raw.front().at(
		                       1))
		                   ->get_tree()
		                   .root()
		                   .data())
		                   ->get_id();
		context->instance->storage.add_var_no_return("__PRAGMA_CS_NAMESPACE_DEFINITION__", name);
	}

	statement_base *
	method_namespace::translate(const context_t &context, const std::deque<std::deque<token_base *>> &raw)
	{
		std::deque<statement_base *> body;
		context->compiler->translate({raw.begin() + 1, raw.end()}, body);
		for (auto &ptr : body)
			if (ptr->get_type() != statement_types::import_ && ptr->get_type() != statement_types::involve_ &&
			        ptr->get_type() != statement_types::var_ && ptr->get_type() != statement_types::function_ &&
			        ptr->get_type() != statement_types::namespace_ && ptr->get_type() != statement_types::struct_)
				throw compile_error("Unexpected statement in namespace definition.");
		return new statement_namespace(static_cast<token_expr *>(raw.front().at(1))->get_tree().root().data(), body,
		                               context, raw.front().back());
	}

	void method_namespace::postprocess(const context_t &context, const domain_type &domain)
	{
		context->instance->storage.add_var_no_return(domain.get_var("__PRAGMA_CS_NAMESPACE_DEFINITION__").const_val<string>(),
		                          make_namespace(make_shared_namespace<name_space>(domain)));
	}

	statement_base *method_if::translate(const context_t &context, const std::deque<std::deque<token_base *>> &raw)
	{
		bool have_else = false;
		std::deque<statement_base *> body;
		context->compiler->translate({raw.begin() + 1, raw.end()}, body);
		for (auto &ptr : body) {
			if (ptr->get_type() == statement_types::else_) {
				if (!have_else)
					have_else = true;
				else
					throw compile_error("Multi Else Grammar.");
			}
		}
		tree_type<token_base *> &tree = static_cast<token_expr *>(raw.front().at(1))->get_tree();
		token_base *ptr = tree.root().data();
		if (have_else) {
			std::deque<statement_base *> body_true;
			std::deque<statement_base *> body_false;
			bool now_place = true;
			for (auto &ptr : body) {
				if (ptr->get_type() == statement_types::else_) {
					now_place = false;
					continue;
				}
				if (now_place)
					body_true.push_back(ptr);
				else
					body_false.push_back(ptr);
			}
			if (ptr != nullptr && ptr->get_type() == token_types::value) {
				if (static_cast<token_value *>(ptr)->get_value().const_val<bool>())
					return new statement_block(body_true, context, raw.front().back());
				else
					return new statement_block(body_false, context, raw.front().back());
			}
			else
				return new statement_ifelse(tree, body_true, body_false, context, raw.front().back());
		}
		else if (ptr != nullptr && ptr->get_type() == token_types::value) {
			if (static_cast<token_value *>(ptr)->get_value().const_val<bool>())
				return new statement_block(body, context, raw.front().back());
			else
				return nullptr;
		}
		else
			return new statement_if(tree, body, context, raw.front().back());
	}

	void method_else::preprocess(const context_t &context, const std::deque<std::deque<token_base *>> &)
	{
		context->instance->storage.clear_domain();
		context->instance->storage.clear_set();
	}

	statement_base *method_else::translate(const context_t &context, const std::deque<std::deque<token_base *>> &raw)
	{
		return new statement_else;
	}

	statement_base *
	method_switch::translate(const context_t &context, const std::deque<std::deque<token_base *>> &raw)
	{
		std::deque<statement_base *> body;
		context->compiler->translate({raw.begin() + 1, raw.end()}, body);
		statement_block *dptr = nullptr;
		map_t<var, statement_block *> cases;
		for (auto &it : body) {
			try {
				if (it->get_type() == statement_types::case_) {
					auto *scptr = static_cast<statement_case *>(it);
					if (cases.count(scptr->get_tag()) > 0)
						throw compile_error("Redefinition of case.");
					cases.emplace(scptr->get_tag(), scptr->get_block());
				}
				else if (it->get_type() == statement_types::default_) {
					auto *sdptr = static_cast<statement_default *>(it);
					if (dptr != nullptr)
						throw compile_error("Redefinition of default case.");
					dptr = sdptr->get_block();
				}
				else
					throw compile_error("Unexpected statement in switch definition.");
			}
			catch (const cs::exception &e) {
				throw e;
			}
			catch (const std::exception &e) {
				throw exception(it->get_line_num(), it->get_file_path(), it->get_raw_code(), e.what());
			}
		}
		return new statement_switch(static_cast<token_expr *>(raw.front().at(1))->get_tree(), cases, dptr, context,
		                            raw.front().back());
	}

	statement_base *method_case::translate(const context_t &context, const std::deque<std::deque<token_base *>> &raw)
	{
		tree_type<token_base *> &tree = static_cast<token_expr *>(raw.front().at(1))->get_tree();
		if (tree.root().data()->get_type() != token_types::value) {
			std::size_t line_num = static_cast<token_endline *>(raw.front().back())->get_line_num();
			const char *what = "Case Tag must be a constant value.";
			throw exception(line_num, context->file_path, context->file_buff.at(line_num - 1), what);
		}
		std::deque<statement_base *> body;
		context->compiler->translate({raw.begin() + 1, raw.end()}, body);
		return new statement_case(static_cast<token_value *>(tree.root().data())->get_value(), body, context,
		                          raw.front().back());
	}

	statement_base *
	method_default::translate(const context_t &context, const std::deque<std::deque<token_base *>> &raw)
	{
		std::deque<statement_base *> body;
		context->compiler->translate({raw.begin() + 1, raw.end()}, body);
		return new statement_default(body, context, raw.front().back());
	}

	statement_base *method_while::translate(const context_t &context, const std::deque<std::deque<token_base *>> &raw)
	{
		std::deque<statement_base *> body;
		context->compiler->translate({raw.begin() + 1, raw.end()}, body);
		tree_type<token_base *> &tree = static_cast<token_expr *>(raw.front().at(1))->get_tree();
		token_base *ptr = tree.root().data();
		if (ptr != nullptr && ptr->get_type() == token_types::value) {
			if (static_cast<token_value *>(ptr)->get_value().const_val<bool>())
				return new statement_loop(body, context, raw.front().back());
			else
				return nullptr;
		}
		else
			return new statement_while(static_cast<token_expr *>(raw.front().at(1))->get_tree(), body, context,
			                           raw.front().back());
	}

	statement_base *method_until::translate_end(method_base *method, const context_t &context,
	        std::deque<std::deque<token_base *>> &raw,
	        std::deque<token_base *> &code)
	{
		if (method != nullptr && method->get_target_type() == statement_types::loop_)
			return static_cast<method_loop *>(method)->translate(context, raw,
			        static_cast<token_expr *>(code.at(1))->get_tree());
		else
			throw compile_error("Use until in non-loop statement.");
	}

	statement_base *method_loop::translate(const context_t &context, const std::deque<std::deque<token_base *>> &raw)
	{
		std::deque<statement_base *> body;
		context->compiler->translate({raw.begin() + 1, raw.end()}, body);
		return new statement_loop(body, context, raw.front().back());
	}

	statement_base *method_loop::translate(const context_t &context, const std::deque<std::deque<token_base *>> &raw,
	                                       const tree_type<token_base *> &cond)
	{
		std::deque<statement_base *> body;
		context->compiler->translate({raw.begin() + 1, raw.end()}, body);
		token_base *ptr = cond.root().data();
		if (ptr != nullptr && ptr->get_type() == token_types::value) {
			if (static_cast<token_value *>(ptr)->get_value().const_val<bool>())
				return new statement_block(body, context, raw.front().back());
			else
				return new statement_loop(body, context, raw.front().back());
		}
		else
			return new statement_loop_until(cond, body, context, raw.front().back());
	}

	void method_for::preprocess(const context_t &context, const std::deque<std::deque<token_base *>> &raw)
	{
		tree_type<token_base *> &tree = static_cast<token_expr *>(raw.front().at(1))->get_tree();
		if (tree.root().data() == nullptr)
			throw internal_error("Null pointer accessed.");
		if (tree.root().data()->get_type() != token_types::parallel)
			throw compile_error("Wrong grammar in for statement, expect <id> = <expression>, <condition>, <process>");
		auto &parallel_list = static_cast<token_parallel *>(tree.root().data())->get_parallel();
		if (parallel_list.size() != 3)
			throw compile_error("Wrong grammar in for statement, expect <id> = <expression>, <condition>, <process>");
		context->instance->check_define_var(parallel_list[0].root(), true);
	}

	statement_base *method_for::translate(const context_t &context, const std::deque<std::deque<token_base *>> &raw)
	{
		std::deque<statement_base *> body;
		context->compiler->translate({raw.begin() + 1, raw.end()}, body);
		tree_type<token_base *> &tree = static_cast<token_expr *>(raw.front().at(1))->get_tree();
		auto &parallel_list = static_cast<token_parallel *>(tree.root().data())->get_parallel();
		return new statement_for(parallel_list, body, context, raw.front().back());
	}

	statement_base *
	method_for_do::translate(const context_t &context, const std::deque<std::deque<token_base *>> &raw)
	{
		tree_type<token_base *> &tree = static_cast<token_expr *>(raw.front().at(1))->get_tree();
		if (tree.root().data() == nullptr)
			throw internal_error("Null pointer accessed.");
		if (tree.root().data()->get_type() != token_types::parallel)
			throw compile_error("Wrong grammar in for statement, expect <id> = <expression>, <condition>, <process>");
		auto &parallel_list = static_cast<token_parallel *>(tree.root().data())->get_parallel();
		if (parallel_list.size() != 3)
			throw compile_error("Wrong grammar in for statement, expect <id> = <expression>, <condition>, <process>");
		context->instance->check_define_var(parallel_list[0].root());
		return new statement_for(parallel_list, {new statement_expression(static_cast<token_expr *>(raw.front().at(3))->get_tree(), context, raw.front().back())}, context, raw.front().back());
	}

	void method_foreach::preprocess(const context_t &context, const std::deque<std::deque<token_base *>> &raw)
	{
		tree_type<token_base *> &t = static_cast<token_expr *>(raw.front().at(1))->get_tree();
		if (t.root().data() == nullptr)
			throw internal_error("Null pointer accessed.");
		if (t.root().data()->get_type() != token_types::id)
			throw compile_error("Wrong grammar in foreach statement, expect <id>");
		context->instance->storage.add_record(static_cast<token_id *>(t.root().data())->get_id());
	}

	statement_base *
	method_foreach::translate(const context_t &context, const std::deque<std::deque<token_base *>> &raw)
	{
		tree_type<token_base *> &t = static_cast<token_expr *>(raw.front().at(1))->get_tree();
		const std::string &it = static_cast<token_id *>(t.root().data())->get_id();
		std::deque<statement_base *> body;
		context->compiler->translate({raw.begin() + 1, raw.end()}, body);
		return new statement_foreach(it, static_cast<token_expr *>(raw.front().at(3))->get_tree(), body, context,
		                             raw.front().back());
	}

	statement_base *
	method_foreach_do::translate(const context_t &context, const std::deque<std::deque<token_base *>> &raw)
	{
		tree_type<token_base *> &t = static_cast<token_expr *>(raw.front().at(1))->get_tree();
		if (t.root().data() == nullptr)
			throw internal_error("Null pointer accessed.");
		if (t.root().data()->get_type() != token_types::id)
			throw compile_error("Wrong grammar in foreach statement, expect <id>");
		const std::string &it = static_cast<token_id *>(t.root().data())->get_id();
		std::deque<statement_base *> body;
		context->compiler->translate({raw.begin() + 1, raw.end()}, body);
		return new statement_foreach(it, static_cast<token_expr *>(raw.front().at(3))->get_tree(), {new statement_expression(static_cast<token_expr *>(raw.front().at(5))->get_tree(), context, raw.front().back())}, context,
		                             raw.front().back());
	}

	statement_base *method_break::translate(const context_t &context, const std::deque<std::deque<token_base *>> &raw)
	{
		return new statement_break(context, raw.front().back());
	}

	statement_base *
	method_continue::translate(const context_t &context, const std::deque<std::deque<token_base *>> &raw)
	{
		return new statement_continue(context, raw.front().back());
	}

	void method_function::preprocess(const context_t &context, const std::deque<std::deque<token_base *>> &raw)
	{
		tree_type<token_base *> &t = static_cast<token_expr *>(raw.front().at(1))->get_tree();
		if (t.root().data() == nullptr)
			throw internal_error("Null pointer accessed.");
		if (t.root().data()->get_type() != token_types::signal ||
		        static_cast<token_signal *>(t.root().data())->get_signal() != signal_types::fcall_)
			throw compile_error("Wrong grammar for function definition.");
		if (t.root().left().data() == nullptr)
			throw internal_error("Null pointer accessed.");
		if (t.root().left().data()->get_type() != token_types::id)
			throw compile_error("Wrong grammar for function definition, expect function name.");
		if (t.root().right().data() == nullptr)
			throw internal_error("Null pointer accessed.");
		if (t.root().right().data()->get_type() != token_types::arglist)
			throw compile_error("Wrong grammar for function definition, expect argument list.");
		std::vector<std::string> args;
		for (auto &it : static_cast<token_arglist *>(t.root().right().data())->get_arglist()) {
			if (it.root().data() == nullptr)
				throw internal_error("Null pointer accessed.");
			context->compiler->try_fix_this_deduction(it.root());
			if (it.root().data()->get_type() == token_types::id) {
				const std::string &str = static_cast<token_id *>(it.root().data())->get_id();
				for (auto &it : args)
					if (it == str)
						throw compile_error("Redefinition of function argument.");
				context->instance->storage.add_record(str);
				args.push_back(str);
			}
			else if (it.root().data()->get_type() == token_types::vargs) {
				const std::string &str = static_cast<token_vargs *>(it.root().data())->get_id();
				if (!args.empty())
					throw compile_error("Redefinition of function argument(Multi-define of vargs).");
				context->instance->storage.add_record(str);
				args.push_back(str);
			}
			else
				throw compile_error("Unexpected element in function argument list.");
		}
	}

	statement_base *
	method_function::translate(const context_t &context, const std::deque<std::deque<token_base *>> &raw)
	{
		tree_type<token_base *> &t = static_cast<token_expr *>(raw.front().at(1))->get_tree();
		std::string name = static_cast<token_id *>(t.root().left().data())->get_id();
		std::vector<std::string> args;
		bool is_vargs = false;
		for (auto &it : static_cast<token_arglist *>(t.root().right().data())->get_arglist()) {
			if (it.root().data()->get_type() == token_types::id)
				args.push_back(static_cast<token_id *>(it.root().data())->get_id());
			else if (it.root().data()->get_type() == token_types::vargs) {
				args.push_back(static_cast<token_vargs *>(it.root().data())->get_id());
				is_vargs = true;
			}
		}
		std::deque<statement_base *> body;
		context->compiler->translate({raw.begin() + 1, raw.end()}, body);
#ifdef CS_DEBUGGER
		std::string decl = "function " + name + "(";
		if (args.size() != 0) {
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

	statement_base *
	method_return::translate(const context_t &context, const std::deque<std::deque<token_base *>> &raw)
	{
		return new statement_return(static_cast<token_expr *>(raw.front().at(1))->get_tree(), context,
		                            raw.front().back());
	}

	statement_base *
	method_return_no_value::translate(const context_t &context, const std::deque<std::deque<token_base *>> &raw)
	{
		tree_type<token_base *> tree;
		tree.emplace_root_left(tree.root(), new token_value(null_pointer));
		return new statement_return(tree, context, raw.front().back());
	}

	void method_struct::preprocess(const context_t &context, const std::deque<std::deque<token_base *>> &raw)
	{
		context->instance->storage.mark_set_as_struct(raw.front().size() == 5);
	}

	statement_base *
	method_struct::translate(const context_t &context, const std::deque<std::deque<token_base *>> &raw)
	{
		tree_type<token_base *> &t = static_cast<token_expr *>(raw.front().at(1))->get_tree();
		if (t.root().data() == nullptr)
			throw internal_error("Null pointer accessed.");
		if (t.root().data()->get_type() != token_types::id)
			throw compile_error("Wrong grammar for struct definition, expect <id>");
		std::string name = static_cast<token_id *>(t.root().data())->get_id();
		std::deque<statement_base *> body;
		context->compiler->translate({raw.begin() + 1, raw.end()}, body);
		for (auto &ptr : body) {
			try {
				switch (ptr->get_type()) {
				default:
					throw compile_error("Unexpected statement in structure definition.");
				case statement_types::var_:
					break;
				case statement_types::function_:
					static_cast<statement_function *>(ptr)->set_mem_fn();
					break;
				}
			}
			catch (const cs::exception &e) {
				throw e;
			}
			catch (const std::exception &e) {
				throw exception(ptr->get_line_num(), ptr->get_file_path(), ptr->get_raw_code(), e.what());
			}
		}
		if (raw.front().size() == 5)
			return new statement_struct(name, static_cast<token_expr *>(raw.front().at(3))->get_tree(), body, context,
			                            raw.front().back());
		else
			return new statement_struct(name, tree_type<token_base *>(), body, context, raw.front().back());
	}

	statement_base *method_try::translate(const context_t &context, const std::deque<std::deque<token_base *>> &raw)
	{
		std::deque<statement_base *> body;
		context->compiler->translate({raw.begin() + 1, raw.end()}, body);
		std::string name;
		std::deque<statement_base *> tbody, cbody;
		bool founded = false;
		for (auto &ptr : body) {
			if (ptr->get_type() == statement_types::catch_) {
				name = static_cast<statement_catch *>(ptr)->get_name();
				founded = true;
				continue;
			}
			if (founded)
				cbody.push_back(ptr);
			else
				tbody.push_back(ptr);
		}
		if (!founded)
			throw compile_error("Wrong grammar for try statement, expect catch statement.");
		return new statement_try(name, tbody, cbody, context, raw.front().back());
	}

	statement_base *method_catch::translate(const context_t &context, const std::deque<std::deque<token_base *>> &raw)
	{
		tree_type<token_base *> &t = static_cast<token_expr *>(raw.front().at(1))->get_tree();
		if (t.root().data() == nullptr)
			throw internal_error("Null pointer accessed.");
		if (t.root().data()->get_type() != token_types::id)
			throw compile_error("Wrong grammar for catch statement, expect <id>");
		return new statement_catch(static_cast<token_id *>(t.root().data())->get_id(), context, raw.front().back());
	}

	statement_base *method_throw::translate(const context_t &context, const std::deque<std::deque<token_base *>> &raw)
	{
		return new statement_throw(static_cast<token_expr *>(raw.front().at(1))->get_tree(), context,
		                           raw.front().back());
	}
} // namespace cs
