/*
* Covariant Script Code Generating
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
*/
#include <covscript/impl/codegen.hpp>

namespace cs {
	statement_base *
	method_expression::translate(const context_t &context, const std::deque<std::deque<token_base *>> &raw)
	{
		return new statement_expression(static_cast<token_expr *>(raw.front().front())->get_tree(), context,
		                                raw.front().back());
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
				throw runtime_error("Wrong grammar for import statement.");
			const var_id &package_name = static_cast<token_id *>(token)->get_id();
			const var &ext = make_namespace(context->instance->import(current_process->import_path, package_name));
			context->compiler->add_constant(ext);
			context->instance->storage.add_var(package_name, ext);
			var_list.emplace_back(package_name, ext);
		};
		if (tree.root().data()->get_type() == token_types::parallel) {
			auto &parallel_list = static_cast<token_parallel *>(tree.root().data())->get_parallel();
			for (auto &t:parallel_list)
				process(t);
		}
		else
			process(tree);
		mResult = new statement_import(var_list, context, raw.front().back());
	}

	statement_base *
	method_import::translate(const context_t &context, const std::deque<std::deque<token_base *>> &raw)
	{
		return mResult;
	}

	statement_base *
	method_package::translate(const context_t &context, const std::deque<std::deque<token_base *>> &raw)
	{
		if (!context->package_name.empty())
			throw runtime_error("Redefinition of package");
		context->package_name = static_cast<token_id *>(static_cast<token_expr *>(raw.front().at(
		                            1))->get_tree().root().data())->get_id();
		return nullptr;
	}

	void method_involve::preprocess(const context_t &context, const std::deque<std::deque<token_base *>> &raw)
	{
		tree_type<token_base *> &tree = static_cast<token_expr *>(raw.front().at(1))->get_tree();
		token_value *vptr = dynamic_cast<token_value *>(tree.root().data());
		if (vptr != nullptr) {
			var ns = vptr->get_value();
			if (ns.type() == typeid(namespace_t)) {
				auto &domain = ns.const_val<namespace_t>()->get_domain();
				for (auto &it:domain) {
					if (domain.get_var_by_id(it.second).is_protect())
						context->instance->storage.add_record(it.first);
				}
				context->instance->storage.involve_domain(domain);
			}
			else
				throw runtime_error("Only support involve namespace.");
			mResult = new statement_involve(tree, true, context, raw.front().back());
		}
		else
			mResult = new statement_involve(tree, false, context, raw.front().back());
	}

	statement_base *
	method_involve::translate(const context_t &context, const std::deque<std::deque<token_base *>> &raw)
	{
		return mResult;
	}

	void method_var::preprocess(const context_t &context, const std::deque<std::deque<token_base *>> &raw)
	{
		context->instance->check_define_var(static_cast<token_expr *>(raw.front().at(1))->get_tree().root());
	}

	statement_base *method_var::translate(const context_t &context, const std::deque<std::deque<token_base *>> &raw)
	{
		return new statement_var(static_cast<token_expr *>(raw.front().at(1))->get_tree(), context, raw.front().back());
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
		                       1))->get_tree().root().data())->get_id();
		context->instance->storage.add_var("__PRAGMA_CS_NAMESPACE_DEFINITION__", name);
	}

	statement_base *
	method_namespace::translate(const context_t &context, const std::deque<std::deque<token_base *>> &raw)
	{
		std::deque<statement_base *> body;
		context->compiler->translate({raw.begin() + 1, raw.end()}, body);
		for (auto &ptr:body)
			if (ptr->get_type() != statement_types::import_ && ptr->get_type() != statement_types::involve_ &&
			        ptr->get_type() != statement_types::var_ && ptr->get_type() != statement_types::function_ &&
			        ptr->get_type() != statement_types::namespace_ && ptr->get_type() != statement_types::struct_)
				throw runtime_error("Wrong grammar for namespace definition.");
		return new statement_namespace(static_cast<token_expr *>(raw.front().at(1))->get_tree().root().data(), body,
		                               context, raw.front().back());
	}

	void method_namespace::postprocess(const context_t &context, const domain_type &domain)
	{
		context->instance->storage.add_var(domain.get_var("__PRAGMA_CS_NAMESPACE_DEFINITION__").const_val<string>(),
		                                   make_namespace(make_shared_namespace<name_space>(domain)));
	}

	statement_base *method_if::translate(const context_t &context, const std::deque<std::deque<token_base *>> &raw)
	{
		bool have_else = false;
		std::deque<statement_base *> body;
		context->compiler->translate({raw.begin() + 1, raw.end()}, body);
		for (auto &ptr:body) {
			if (ptr->get_type() == statement_types::else_) {
				if (!have_else)
					have_else = true;
				else
					throw runtime_error("Multi Else Grammar.");
			}
		}
		tree_type<token_base *> &tree = static_cast<token_expr *>(raw.front().at(1))->get_tree();
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
		for (auto &it:body) {
			try {
				if (it->get_type() == statement_types::case_) {
					auto *scptr = static_cast<statement_case *>(it);
					if (cases.count(scptr->get_tag()) > 0)
						throw runtime_error("Redefinition of case.");
					cases.emplace(scptr->get_tag(), scptr->get_block());
				}
				else if (it->get_type() == statement_types::default_) {
					auto *sdptr = static_cast<statement_default *>(it);
					if (dptr != nullptr)
						throw runtime_error("Redefinition of default case.");
					dptr = sdptr->get_block();
				}
				else
					throw runtime_error("Wrong format of switch statement.");
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
			throw runtime_error("Wrong grammar(loop-until)");
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
			throw runtime_error("Wrong grammar(for)");
		auto &parallel_list = static_cast<token_parallel *>(tree.root().data())->get_parallel();
		if (parallel_list.size() != 3)
			throw runtime_error("Wrong grammar(for)");
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
			throw runtime_error("Wrong grammar(for)");
		auto &parallel_list = static_cast<token_parallel *>(tree.root().data())->get_parallel();
		if (parallel_list.size() != 3)
			throw runtime_error("Wrong grammar(for)");
		context->instance->check_define_var(parallel_list[0].root());
		return new statement_for(parallel_list, {
			new statement_expression(static_cast<token_expr *>(raw.front().at(3))->get_tree(),
			                         context, raw.front().back())
		}, context, raw.front().back());
	}

	void method_foreach::preprocess(const context_t &context, const std::deque<std::deque<token_base *>> &raw)
	{
		tree_type<token_base *> &t = static_cast<token_expr *>(raw.front().at(1))->get_tree();
		if (t.root().data() == nullptr)
			throw internal_error("Null pointer accessed.");
		if (t.root().data()->get_type() != token_types::id)
			throw runtime_error("Wrong grammar(foreach)");
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
			throw runtime_error("Wrong grammar(foreach)");
		const std::string &it = static_cast<token_id *>(t.root().data())->get_id();
		std::deque<statement_base *> body;
		context->compiler->translate({raw.begin() + 1, raw.end()}, body);
		return new statement_foreach(it, static_cast<token_expr *>(raw.front().at(3))->get_tree(), {
			new statement_expression(static_cast<token_expr *>(raw.front().at(5))->get_tree(),
			                         context, raw.front().back())
		}, context,
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
			throw runtime_error("Wrong grammar for function definition.");
		if (t.root().left().data() == nullptr)
			throw internal_error("Null pointer accessed.");
		if (t.root().left().data()->get_type() != token_types::id)
			throw runtime_error("Wrong grammar for function definition.");
		if (t.root().right().data() == nullptr)
			throw internal_error("Null pointer accessed.");
		if (t.root().right().data()->get_type() != token_types::arglist)
			throw runtime_error("Wrong grammar for function definition.");
		std::vector<std::string> args;
		for (auto &it:static_cast<token_arglist *>(t.root().right().data())->get_arglist()) {
			if (it.root().data() == nullptr)
				throw internal_error("Null pointer accessed.");
			context->compiler->try_fix_this_deduction(it.root());
			if (it.root().data()->get_type() == token_types::id) {
				const std::string &str = static_cast<token_id *>(it.root().data())->get_id();
				for (auto &it:args)
					if (it == str)
						throw runtime_error("Redefinition of function argument.");
				context->instance->storage.add_record(str);
				args.push_back(str);
			}
			else if (it.root().data()->get_type() == token_types::vargs) {
				const std::string &str = static_cast<token_vargs *>(it.root().data())->get_id();
				if (!args.empty())
					throw runtime_error("Redefinition of function argument(Multi-define of vargs).");
				context->instance->storage.add_record(str);
				args.push_back(str);
			}
			else
				throw runtime_error("Wrong grammar for function definition.");
		}
	}

	statement_base *
	method_function::translate(const context_t &context, const std::deque<std::deque<token_base *>> &raw)
	{
		tree_type<token_base *> &t = static_cast<token_expr *>(raw.front().at(1))->get_tree();
		std::string name = static_cast<token_id *>(t.root().left().data())->get_id();
		std::vector<std::string> args;
		bool is_vargs = false;
		for (auto &it:static_cast<token_arglist *>(t.root().right().data())->get_arglist()) {
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
		std::string decl="function "+name+"(";
		if(args.size()!=0) {
			for(auto& it:args)
				decl+=it+", ";
			decl.pop_back();
			decl[decl.size()-1]=')';
		}
		else
			decl+=")";
		if(raw.front().size() == 4)
			decl+=" override";
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

	void method_struct::preprocess(const context_t &context, const std::deque<std::deque<token_base *>> &)
	{
		context->instance->storage.mark_set_as_struct();
	}

	statement_base *
	method_struct::translate(const context_t &context, const std::deque<std::deque<token_base *>> &raw)
	{
		tree_type<token_base *> &t = static_cast<token_expr *>(raw.front().at(1))->get_tree();
		if (t.root().data() == nullptr)
			throw internal_error("Null pointer accessed.");
		if (t.root().data()->get_type() != token_types::id)
			throw runtime_error("Wrong grammar for struct definition.");
		std::string name = static_cast<token_id *>(t.root().data())->get_id();
		std::deque<statement_base *> body;
		context->compiler->translate({raw.begin() + 1, raw.end()}, body);
		for (auto &ptr:body) {
			try {
				switch (ptr->get_type()) {
				default:
					throw runtime_error("Wrong grammar for struct definition.");
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
			throw runtime_error("Wrong grammar for try statement.");
		return new statement_try(name, tbody, cbody, context, raw.front().back());
	}

	statement_base *method_catch::translate(const context_t &context, const std::deque<std::deque<token_base *>> &raw)
	{
		tree_type<token_base *> &t = static_cast<token_expr *>(raw.front().at(1))->get_tree();
		if (t.root().data() == nullptr)
			throw internal_error("Null pointer accessed.");
		if (t.root().data()->get_type() != token_types::id)
			throw runtime_error("Wrong grammar for catch statement.");
		return new statement_catch(static_cast<token_id *>(t.root().data())->get_id(), context, raw.front().back());
	}

	statement_base *method_throw::translate(const context_t &context, const std::deque<std::deque<token_base *>> &raw)
	{
		return new statement_throw(static_cast<token_expr *>(raw.front().at(1))->get_tree(), context,
		                           raw.front().back());
	}
}