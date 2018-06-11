/*
* Covariant Script Code Gen
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
*/
#include <covscript/codegen.hpp>

namespace cs {
	statement_base *method_expression::translate(const std::deque<std::deque<token_base *>> &raw)
	{
		return new statement_expression(dynamic_cast<token_expr *>(raw.front().front())->get_tree(), context,
		                                raw.front().back());
	}

	statement_base *method_import::translate(const std::deque<std::deque<token_base *>> &raw)
	{
		token_base *token = dynamic_cast<token_expr *>(raw.front().at(1))->get_tree().root().data();
		if (token == nullptr || token->get_type() != token_types::id)
			throw syntax_error("Wrong grammar for import statement.");
		const std::string &package_name = dynamic_cast<token_id *>(token)->get_id();
		const var &ext = var::make_protect<extension_t>(context->instance->import(import_path, package_name));
		context->instance->storage.add_var(package_name, ext);
		return new statement_constant(package_name, ext, context, raw.front().back());
	}

	statement_base *method_package::translate(const std::deque<std::deque<token_base *>> &raw)
	{
		if (!context->package_name.empty())
			throw syntax_error("Redefinition of package");
		context->package_name = dynamic_cast<token_id *>(dynamic_cast<token_expr *>(raw.front().at(
		                            1))->get_tree().root().data())->get_id();
		return nullptr;
	}

	statement_base *method_involve::translate(const std::deque<std::deque<token_base *>> &raw)
	{
		cov::tree<token_base *> &tree = dynamic_cast<token_expr *>(raw.front().at(1))->get_tree();
		token_value *vptr = dynamic_cast<token_value *>(tree.root().data());
		if (vptr != nullptr) {
			var ns = vptr->get_value();
			if (ns.type() == typeid(name_space_t))
				context->instance->storage.involve_domain(ns.const_val<name_space_t>()->get_domain());
			else
				throw syntax_error("Only support involve namespace.");
			return new statement_involve(tree, true, context, raw.front().back());
		}
		else
			return new statement_involve(tree, false, context, raw.front().back());
	}

	statement_base *method_var::translate(const std::deque<std::deque<token_base *>> &raw)
	{
		cov::tree<token_base *> &tree = dynamic_cast<token_expr *>(raw.front().at(1))->get_tree();
		instance_type::define_var_profile dvp;
		context->instance->parse_define_var(tree, dvp);
		return new statement_var(dvp, context, raw.front().back());
	}

	statement_base *method_constant::translate(const std::deque<std::deque<token_base *>> &raw)
	{
		cov::tree<token_base *> &tree = dynamic_cast<token_expr *>(raw.front().at(2))->get_tree();
		instance_type::define_var_profile dvp;
		context->instance->parse_define_var(tree, dvp);
		if (dvp.expr.root().data()->get_type() != token_types::value)
			throw syntax_error("Constant variable must have an constant value.");
		const var &val = static_cast<token_value *>(dvp.expr.root().data())->get_value();
		context->instance->add_constant(val);
		context->instance->storage.add_var(dvp.id, val);
		return new statement_constant(dvp.id, val, context, raw.front().back());
	}

	statement_base *method_end::translate(const std::deque<std::deque<token_base *>> &raw)
	{
		return new statement_end;
	}

	statement_base *method_block::translate(const std::deque<std::deque<token_base *>> &raw)
	{
		std::deque<statement_base *> body;
		context->instance->kill_action({raw.begin() + 1, raw.end()}, body);
		return new statement_block(body, context, raw.front().back());
	}

	statement_base *method_namespace::translate(const std::deque<std::deque<token_base *>> &raw)
	{
		std::deque<statement_base *> body;
		context->instance->kill_action({raw.begin() + 1, raw.end()}, body);
		for (auto &ptr:body)
			if (ptr->get_type() != statement_types::import_ && ptr->get_type() != statement_types::involve_ &&
			        ptr->get_type() != statement_types::var_ && ptr->get_type() != statement_types::function_ &&
			        ptr->get_type() != statement_types::namespace_ && ptr->get_type() != statement_types::struct_)
				throw syntax_error("Wrong grammar for namespace definition.");
		return new statement_namespace(dynamic_cast<token_expr *>(raw.front().at(1))->get_tree().root().data(), body,
		                               context, raw.front().back());
	}

	statement_base *method_if::translate(const std::deque<std::deque<token_base *>> &raw)
	{
		bool have_else = false;
		std::deque<statement_base *> body;
		context->instance->kill_action({raw.begin() + 1, raw.end()}, body);
		for (auto &ptr:body) {
			if (ptr->get_type() == statement_types::else_) {
				if (!have_else)
					have_else = true;
				else
					throw syntax_error("Multi Else Grammar.");
			}
		}
		cov::tree<token_base *> &tree = dynamic_cast<token_expr *>(raw.front().at(1))->get_tree();
		token_base *ptr = tree.root().data();
		if (have_else) {
			std::deque<statement_base *> body_true;
			std::deque<statement_base *> body_false;
			bool now_place = true;
			for (auto &ptr:body) {
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

	void method_else::preprocess(const std::deque<std::deque<token_base *>> &)
	{
		context->instance->storage.clear_domain();
		context->instance->storage.clear_set();
	}

	statement_base *method_else::translate(const std::deque<std::deque<token_base *>> &raw)
	{
		return new statement_else;
	}

	statement_base *method_switch::translate(const std::deque<std::deque<token_base *>> &raw)
	{
		std::deque<statement_base *> body;
		context->instance->kill_action({raw.begin() + 1, raw.end()}, body);
		statement_block *dptr = nullptr;
		spp::sparse_hash_map<var, statement_block *> cases;
		for (auto &it:body) {
			try {
				if (it->get_type() == statement_types::case_) {
					statement_case *scptr = dynamic_cast<statement_case *>(it);
					if (cases.count(scptr->get_tag()) > 0)
						throw syntax_error("Redefinition of case.");
					cases.emplace(scptr->get_tag(), scptr->get_block());
				}
				else if (it->get_type() == statement_types::default_) {
					statement_default *sdptr = dynamic_cast<statement_default *>(it);
					if (dptr != nullptr)
						throw syntax_error("Redefinition of default case.");
					dptr = sdptr->get_block();
				}
				else
					throw syntax_error("Wrong format of switch statement.");
			}
			catch (const cs::exception &e) {
				throw e;
			}
			catch (const std::exception &e) {
				throw exception(it->get_line_num(), it->get_file_path(), it->get_raw_code(), e.what());
			}
		}
		return new statement_switch(dynamic_cast<token_expr *>(raw.front().at(1))->get_tree(), cases, dptr, context,
		                            raw.front().back());
	}

	statement_base *method_case::translate(const std::deque<std::deque<token_base *>> &raw)
	{
		cov::tree<token_base *> &tree = dynamic_cast<token_expr *>(raw.front().at(1))->get_tree();
		if (tree.root().data()->get_type() != token_types::value) {
			std::size_t line_num = static_cast<token_endline *>(raw.front().back())->get_line_num();
			const char *what = "Case Tag must be a constant value.";
			throw exception(line_num, context->file_path, context->file_buff.at(line_num - 1), what);
		}
		std::deque<statement_base *> body;
		context->instance->kill_action({raw.begin() + 1, raw.end()}, body);
		return new statement_case(dynamic_cast<token_value *>(tree.root().data())->get_value(), body, context,
		                          raw.front().back());
	}

	statement_base *method_default::translate(const std::deque<std::deque<token_base *>> &raw)
	{
		std::deque<statement_base *> body;
		context->instance->kill_action({raw.begin() + 1, raw.end()}, body);
		return new statement_default(body, context, raw.front().back());
	}

	statement_base *method_while::translate(const std::deque<std::deque<token_base *>> &raw)
	{
		std::deque<statement_base *> body;
		context->instance->kill_action({raw.begin() + 1, raw.end()}, body);
		cov::tree<token_base *> &tree = dynamic_cast<token_expr *>(raw.front().at(1))->get_tree();
		token_base *ptr = tree.root().data();
		if (ptr != nullptr && ptr->get_type() == token_types::value) {
			if (static_cast<token_value *>(ptr)->get_value().const_val<bool>())
				return new statement_loop(nullptr, body, context, raw.front().back());
			else
				return nullptr;
		}
		else
			return new statement_while(dynamic_cast<token_expr *>(raw.front().at(1))->get_tree(), body, context,
			                           raw.front().back());
	}

	statement_base *method_until::translate(const std::deque<std::deque<token_base *>> &raw)
	{
		return new statement_until(dynamic_cast<token_expr *>(raw.front().at(1)), context, raw.front().back());
	}

	statement_base *method_loop::translate(const std::deque<std::deque<token_base *>> &raw)
	{
		std::deque<statement_base *> body;
		context->instance->kill_action({raw.begin() + 1, raw.end()}, body);
		if (!body.empty() && body.back()->get_type() == statement_types::until_) {
			token_expr *expr = dynamic_cast<statement_until *>(body.back())->get_expr();
			body.pop_back();
			cov::tree<token_base *> &tree = dynamic_cast<token_expr *>(expr)->get_tree();
			token_base *ptr = tree.root().data();
			if (ptr != nullptr && ptr->get_type() == token_types::value) {
				if (static_cast<token_value *>(ptr)->get_value().const_val<bool>())
					return new statement_block(body, context, raw.front().back());
				else
					return new statement_loop(nullptr, body, context, raw.front().back());
			}
			else
				return new statement_loop(expr, body, context, raw.front().back());
		}
		else
			return new statement_loop(nullptr, body, context, raw.front().back());
	}

	void method_for_step::preprocess(const std::deque<std::deque<token_base *>> &raw)
	{
		cov::tree<token_base *> &tree = dynamic_cast<token_expr *>(raw.front().at(1))->get_tree();
		instance_type::define_var_profile dvp;
		context->instance->parse_define_var(tree, dvp);
		context->instance->storage.add_record(dvp.id);
	}

	statement_base *method_for_step::translate(const std::deque<std::deque<token_base *>> &raw)
	{
		std::deque<statement_base *> body;
		context->instance->kill_action({raw.begin() + 1, raw.end()}, body);
		return new statement_for(dynamic_cast<token_expr *>(raw.front().at(1))->get_tree(),
		                         dynamic_cast<token_expr *>(raw.front().at(3))->get_tree(),
		                         dynamic_cast<token_expr *>(raw.front().at(5))->get_tree(), body, context,
		                         raw.front().back());
	}

	void method_for::preprocess(const std::deque<std::deque<token_base *>> &raw)
	{
		cov::tree<token_base *> &tree = dynamic_cast<token_expr *>(raw.front().at(1))->get_tree();
		instance_type::define_var_profile dvp;
		context->instance->parse_define_var(tree, dvp);
		context->instance->storage.add_record(dvp.id);
	}

	statement_base *method_for::translate(const std::deque<std::deque<token_base *>> &raw)
	{
		std::deque<statement_base *> body;
		context->instance->kill_action({raw.begin() + 1, raw.end()}, body);
		cov::tree<token_base *> tree_step;
		tree_step.emplace_root_left(tree_step.root(), context->instance->new_value(number(1)));
		return new statement_for(dynamic_cast<token_expr *>(raw.front().at(1))->get_tree(),
		                         dynamic_cast<token_expr *>(raw.front().at(3))->get_tree(), tree_step, body, context,
		                         raw.front().back());
	}

	void method_foreach::preprocess(const std::deque<std::deque<token_base *>> &raw)
	{
		cov::tree<token_base *> &t = dynamic_cast<token_expr *>(raw.front().at(1))->get_tree();
		if (t.root().data() == nullptr)
			throw internal_error("Null pointer accessed.");
		if (t.root().data()->get_type() != token_types::id)
			throw syntax_error("Wrong grammar(foreach)");
		context->instance->storage.add_record(dynamic_cast<token_id *>(t.root().data())->get_id());
	}

	statement_base *method_foreach::translate(const std::deque<std::deque<token_base *>> &raw)
	{
		cov::tree<token_base *> &t = dynamic_cast<token_expr *>(raw.front().at(1))->get_tree();
		if (t.root().data() == nullptr)
			throw internal_error("Null pointer accessed.");
		if (t.root().data()->get_type() != token_types::id)
			throw syntax_error("Wrong grammar(foreach)");
		const std::string &it = dynamic_cast<token_id *>(t.root().data())->get_id();
		std::deque<statement_base *> body;
		context->instance->kill_action({raw.begin() + 1, raw.end()}, body);
		return new statement_foreach(it, dynamic_cast<token_expr *>(raw.front().at(3))->get_tree(), body, context,
		                             raw.front().back());
	}

	statement_base *method_break::translate(const std::deque<std::deque<token_base *>> &raw)
	{
		return new statement_break(context, raw.front().back());
	}

	statement_base *method_continue::translate(const std::deque<std::deque<token_base *>> &raw)
	{
		return new statement_continue(context, raw.front().back());
	}

	statement_base *method_function::translate(const std::deque<std::deque<token_base *>> &raw)
	{
		cov::tree<token_base *> &t = dynamic_cast<token_expr *>(raw.front().at(1))->get_tree();
		if (t.root().data() == nullptr)
			throw internal_error("Null pointer accessed.");
		if (t.root().data()->get_type() != token_types::signal ||
		        dynamic_cast<token_signal *>(t.root().data())->get_signal() != signal_types::fcall_)
			throw syntax_error("Wrong grammar for function definition.");
		if (t.root().left().data() == nullptr)
			throw internal_error("Null pointer accessed.");
		if (t.root().left().data()->get_type() != token_types::id)
			throw syntax_error("Wrong grammar for function definition.");
		if (t.root().right().data() == nullptr)
			throw internal_error("Null pointer accessed.");
		if (t.root().right().data()->get_type() != token_types::arglist)
			throw syntax_error("Wrong grammar for function definition.");
		std::string name = dynamic_cast<token_id *>(t.root().left().data())->get_id();
		std::vector<std::string> args;
		for (auto &it:dynamic_cast<token_arglist *>(t.root().right().data())->get_arglist()) {
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
		std::deque<statement_base *> body;
		context->instance->kill_action({raw.begin() + 1, raw.end()}, body);
		return new statement_function(name, args, body, raw.front().size() == 4, context, raw.front().back());
	}

	statement_base *method_return::translate(const std::deque<std::deque<token_base *>> &raw)
	{
		return new statement_return(dynamic_cast<token_expr *>(raw.front().at(1))->get_tree(), context,
		                            raw.front().back());
	}

	statement_base *method_return_no_value::translate(const std::deque<std::deque<token_base *>> &raw)
	{
		cov::tree<token_base *> tree;
		tree.emplace_root_left(tree.root(), new token_value(null_pointer));
		return new statement_return(tree, context, raw.front().back());
	}

	void method_struct::preprocess(const std::deque<std::deque<token_base *>> &)
	{
		context->instance->storage.mark_set_as_struct();
	}

	statement_base *method_struct::translate(const std::deque<std::deque<token_base *>> &raw)
	{
		cov::tree<token_base *> &t = dynamic_cast<token_expr *>(raw.front().at(1))->get_tree();
		if (t.root().data() == nullptr)
			throw internal_error("Null pointer accessed.");
		if (t.root().data()->get_type() != token_types::id)
			throw syntax_error("Wrong grammar for struct definition.");
		std::string name = dynamic_cast<token_id *>(t.root().data())->get_id();
		std::deque<statement_base *> body;
		context->instance->kill_action({raw.begin() + 1, raw.end()}, body);
		for (auto &ptr:body) {
			try {
				switch (ptr->get_type()) {
				default:
					throw syntax_error("Wrong grammar for struct definition.");
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
			return new statement_struct(name, dynamic_cast<token_expr *>(raw.front().at(3))->get_tree(), body, context,
			                            raw.front().back());
		else
			return new statement_struct(name, cov::tree<token_base *>(), body, context, raw.front().back());
	}

	statement_base *method_try::translate(const std::deque<std::deque<token_base *>> &raw)
	{
		std::deque<statement_base *> body;
		context->instance->kill_action({raw.begin() + 1, raw.end()}, body);
		std::string name;
		std::deque<statement_base *> tbody, cbody;
		bool founded = false;
		for (auto &ptr:body) {
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
			throw syntax_error("Wrong grammar for try statement.");
		return new statement_try(name, tbody, cbody, context, raw.front().back());
	}

	statement_base *method_catch::translate(const std::deque<std::deque<token_base *>> &raw)
	{
		cov::tree<token_base *> &t = dynamic_cast<token_expr *>(raw.front().at(1))->get_tree();
		if (t.root().data() == nullptr)
			throw internal_error("Null pointer accessed.");
		if (t.root().data()->get_type() != token_types::id)
			throw syntax_error("Wrong grammar for catch statement.");
		return new statement_catch(dynamic_cast<token_id *>(t.root().data())->get_id(), context, raw.front().back());
	}

	statement_base *method_throw::translate(const std::deque<std::deque<token_base *>> &raw)
	{
		return new statement_throw(dynamic_cast<token_expr *>(raw.front().at(1))->get_tree(), context,
		                           raw.front().back());
	}
}