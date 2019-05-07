/*
* Covariant Script Compiler
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
#include <covscript/impl/compiler.hpp>
#include <covscript/impl/codegen.hpp>

namespace cs {
	bool token_signal::dump(std::ostream &o) const
	{
		o << "< Signal = \"";
		switch (mType) {
		case signal_types::add_:
			o << "+";
			break;
		case signal_types::addasi_:
			o << "+=";
			break;
		case signal_types::sub_:
			o << "-";
			break;
		case signal_types::subasi_:
			o << "-=";
			break;
		case signal_types::mul_:
			o << "*";
			break;
		case signal_types::mulasi_:
			o << "*=";
			break;
		case signal_types::div_:
			o << "/";
			break;
		case signal_types::divasi_:
			o << "/=";
			break;
		case signal_types::mod_:
			o << "%";
			break;
		case signal_types::modasi_:
			o << "%=";
			break;
		case signal_types::pow_:
			o << "^";
			break;
		case signal_types::powasi_:
			o << "^=";
			break;
		case signal_types::com_:
			o << ",";
			break;
		case signal_types::dot_:
			o << ".";
			break;
		case signal_types::und_:
			o << "<";
			break;
		case signal_types::abo_:
			o << ">";
			break;
		case signal_types::asi_:
			o << "=";
			break;
		case signal_types::equ_:
			o << "==";
			break;
		case signal_types::ueq_:
			o << "<=";
			break;
		case signal_types::aeq_:
			o << ">=";
			break;
		case signal_types::neq_:
			o << "!=";
			break;
		case signal_types::and_:
			o << "&&";
			break;
		case signal_types::or_:
			o << "||";
			break;
		case signal_types::not_:
			o << "!";
			break;
		case signal_types::inc_:
			o << "++";
			break;
		case signal_types::dec_:
			o << "--";
			break;
		case signal_types::pair_:
			o << ":";
			break;
		case signal_types::choice_:
			o << "?";
			break;
		case signal_types::slb_:
			o << "(";
			break;
		case signal_types::srb_:
			o << ")";
			break;
		case signal_types::mlb_:
			o << "[";
			break;
		case signal_types::mrb_:
			o << "]";
			break;
		case signal_types::llb_:
			o << "{";
			break;
		case signal_types::lrb_:
			o << "}";
			break;
		case signal_types::esb_:
			o << "()";
			break;
		case signal_types::emb_:
			o << "[]";
			break;
		case signal_types::elb_:
			o << "{}";
			break;
		case signal_types::fcall_:
			o << "[call]";
			break;
		case signal_types::access_:
			o << "[access]";
			break;
		case signal_types::typeid_:
			o << "typeid";
			break;
		case signal_types::new_:
			o << "new";
			break;
		case signal_types::gcnew_:
			o << "gcnew";
			break;
		case signal_types::arrow_:
			o << "->";
			break;
		case signal_types::lambda_:
			o << "[lambda]";
			break;
		case signal_types::escape_:
			o << "*";
			break;
		case signal_types::minus_:
			o << "-";
			break;
		case signal_types::vardef_:
			o << "[vardef]";
			break;
		case signal_types::varprt_:
			o << "[varprt]";
			break;
		case signal_types::vargs_:
			o << "[vargs]";
			break;
		case signal_types::error_:
			o << "[error]";
			break;
		}
		o << "\" >";
		return false;
	}

	bool token_expand::dump(std::ostream &o) const
	{
		o << "< ExpandExpression = ";
		compiler_type::dump_expr(mTree.root(), o);
		o << " >";
		return true;
	}

	bool token_expr::dump(std::ostream &o) const
	{
		o << "< ChildExpression = ";
		compiler_type::dump_expr(mTree.root(), o);
		o << " >";
		return true;
	}

	bool token_arglist::dump(std::ostream &o) const
	{
		o << "< ArgumentList = {";
		for (auto &tree:mTreeList) {
			o << "< ChildExpression: ";
			compiler_type::dump_expr(tree.root(), o);
			o << " >";
		}
		o << "} >";
		return true;
	}

	bool token_array::dump(std::ostream &o) const
	{
		o << "< ArrayList = {";
		for (auto &tree:mTreeList) {
			o << "< ChildExpression: ";
			compiler_type::dump_expr(tree.root(), o);
			o << " >";
		}
		o << "} >";
		return true;
	}

	bool token_parallel::dump(std::ostream &o) const
	{
		o << "< ParallelList = {";
		for (auto &tree:mTreeList) {
			o << "< ChildExpression: ";
			compiler_type::dump_expr(tree.root(), o);
			o << " >";
		}
		o << "} >";
		return true;
	}

	void compiler_type::trim_expr(tree_type<token_base *> &tree, tree_type<token_base *>::iterator it)
	{
		if (!it.usable())
			return;
		token_base *token = it.data();
		if (token == nullptr)
			return;
		switch (token->get_type()) {
		default:
			break;
		case token_types::id: {
			const std::string &id = static_cast<token_id *>(token)->get_id();
			if (!context->instance->storage.exist_record(id) &&
			        context->instance->storage.exist_record_in_struct(id)) {
				it.data() = new token_signal(signal_types::dot_);
				tree.emplace_left_left(it, new token_id("this"));
				tree.emplace_right_right(it, token);
			}
			return;
		}
		case token_types::expr: {
			tree_type<token_base *> &t = static_cast<token_expr *>(it.data())->get_tree();
			trim_expression(t);
			tree.merge(it, t);
			return;
		}
		case token_types::array: {
			for (auto &tree:static_cast<token_array *>(token)->get_array())
				trim_expression(tree);
			return;
		}
		case token_types::signal: {
			switch (static_cast<token_signal *>(token)->get_signal()) {
			default:
				break;
			case signal_types::new_:
				if (it.left().data() != nullptr)
					throw runtime_error("Wrong grammar for new expression.");
				break;
			case signal_types::gcnew_:
				if (it.left().data() != nullptr)
					throw runtime_error("Wrong grammar for gcnew expression.");
				trim_expr(tree, it.right());
				return;
				break;
			case signal_types::typeid_:
				if (it.left().data() != nullptr)
					throw runtime_error("Wrong grammar for typeid expression.");
				break;
			case signal_types::not_:
				if (it.left().data() != nullptr)
					throw runtime_error("Wrong grammar for not expression.");
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
					throw runtime_error("Wrong grammar for assign expression.");
				trim_expr(tree, it.left());
				trim_expr(tree, it.right());
				return;
			case signal_types::com_: {
				trim_expr(tree, it.left());
				trim_expr(tree, it.right());
				token_base *lptr = it.left().data();
				token_base *rptr = it.right().data();
				if (lptr == nullptr && rptr == nullptr)
					it.data() = nullptr;
				else {
					token_parallel *parallel_list = nullptr;
					if (lptr != nullptr && lptr->get_type() == token_types::parallel)
						parallel_list = static_cast<token_parallel *>(lptr);
					else if (lptr != nullptr)
						parallel_list = new token_parallel({tree_type<token_base *>(it.left())});
					else
						parallel_list = new token_parallel();
					if (rptr != nullptr && rptr->get_type() == token_types::parallel)
						for (auto &tree:static_cast<token_parallel *>(rptr)->get_parallel())
							parallel_list->get_parallel().push_back(tree);
					else if (rptr != nullptr)
						parallel_list->get_parallel().emplace_back(it.right());
					it.data() = parallel_list;
				}
				return;
			}
			case signal_types::choice_: {
				token_signal *sig = dynamic_cast<token_signal *>(it.right().data());
				if (sig == nullptr || sig->get_signal() != signal_types::pair_)
					throw runtime_error("Wrong grammar for choice expression.");
				trim_expr(tree, it.left());
				trim_expr(tree, it.right().left());
				trim_expr(tree, it.right().right());
				return;
			}
			case signal_types::vardef_: {
				if (it.left().data() != nullptr)
					throw runtime_error("Wrong grammar for variable definition.");
				token_base *rptr = it.right().data();
				if (rptr == nullptr || rptr->get_type() != token_types::id)
					throw runtime_error("Wrong grammar for variable definition.");
				context->instance->storage.add_record(static_cast<token_id *>(rptr)->get_id());
				it.data() = rptr;
				return;
			}
			case signal_types::varprt_: {
				if (it.left().data() != nullptr)
					throw runtime_error("Wrong grammar for variable definition.");
				token_base *rptr = it.right().data();
				if (rptr == nullptr || rptr->get_type() != token_types::id)
					throw runtime_error("Wrong grammar for variable definition.");
				it.data() = rptr;
				return;
			}
			case signal_types::arrow_:
				if (it.left().data() == nullptr || it.right().data() == nullptr ||
				        it.right().data()->get_type() != token_types::id)
					throw runtime_error("Wrong grammar for arrow expression.");
				trim_expr(tree, it.left());
				return;
			case signal_types::dot_: {
				trim_expr(tree, it.left());
				token_base *rptr = it.right().data();
				if (rptr == nullptr || rptr->get_type() != token_types::id)
					throw runtime_error("Wrong grammar for dot expression.");
				return;
			}
			case signal_types::fcall_: {
				trim_expr(tree, it.left());
				trim_expr(tree, it.right());
				token_base *lptr = it.left().data();
				token_base *rptr = it.right().data();
				if (lptr == nullptr || rptr == nullptr || rptr->get_type() != token_types::arglist)
					throw runtime_error("Wrong syntax for function call.");
				return;
			}
			case signal_types::vargs_: {
				if (it.left().data() == nullptr) {
					token_base *rptr = it.right().data();
					if (rptr == nullptr || rptr->get_type() != token_types::id)
						throw runtime_error("Wrong grammar for vargs expression.");
					it.data() = new token_vargs(static_cast<token_id *>(rptr)->get_id());
				}
				else {
					if (it.right().data() != nullptr)
						throw runtime_error("Wrong grammar for vargs expression.");
					it.data() = new token_expand(tree_type<token_base *>(it.left()));
				}
				return;
			}
			case signal_types::emb_: {
				trim_expr(tree, it.left());
				trim_expr(tree, it.right());
				token_base *lptr = it.left().data();
				token_base *rptr = it.right().data();
				if (!inside_lambda || lptr != nullptr || rptr == nullptr ||
				        rptr->get_type() != token_types::arglist)
					throw runtime_error("Wrong grammar for lambda expression.");
				it.data() = rptr;
				return;
			}
			case signal_types::lambda_: {
				inside_lambda = true;
				trim_expr(tree, it.left());
				inside_lambda = false;
				trim_expr(tree, it.right());
				token_base *lptr = it.left().data();
				token_base *rptr = it.right().data();
				if (lptr == nullptr || rptr == nullptr || lptr->get_type() != token_types::arglist)
					throw runtime_error("Wrong grammar for lambda expression.");
				std::vector<std::string> args;
				bool is_vargs = false;
				for (auto &it:static_cast<token_arglist *>(lptr)->get_arglist()) {
					if (it.root().data() == nullptr)
						throw internal_error("Null pointer accessed.");
					if (it.root().data()->get_type() == token_types::id) {
						const std::string &str = static_cast<token_id *>(it.root().data())->get_id();
						for (auto &it:args)
							if (it == str)
								throw runtime_error("Redefinition of function argument.");
						args.push_back(str);
					}
					else if (it.root().data()->get_type() == token_types::vargs) {
						const std::string &str = static_cast<token_vargs *>(it.root().data())->get_id();
						if (!args.empty())
							throw runtime_error("Redefinition of function argument(Multi-define of vargs).");
						context->instance->storage.add_record(str);
						args.push_back(str);
						is_vargs = true;
					}
					else
						throw runtime_error("Wrong grammar for function definition.");
				}
				statement_base *ret = new statement_return(tree_type<token_base *>(it.right()), context,
				        new token_endline(token->get_line_num()));
#ifdef CS_DEBUGGER
				std::string decl="function [lambda](";
				if(args.size()!=0) {
					for(auto& it:args)
						decl+=it+", ";
					decl.pop_back();
					decl[decl.size()-1]=')';
				}
				else
					decl+=")";
				it.data() = new_value(var::make_protect<callable>(function(context, decl, ret, args, std::deque<statement_base *> {ret}, is_vargs)));
#else
				it.data() = new_value(var::make_protect<callable>(
				                          function(context, args, std::deque<statement_base *> {ret}, is_vargs)));
#endif
				return;
			}
			}
		}
		}
		trim_expr(tree, it.left());
		trim_expr(tree, it.right());
	}

	void compiler_type::opt_expr(tree_type<token_base *> &tree, tree_type<token_base *>::iterator it)
	{
		if (!it.usable())
			return;
		token_base *token = it.data();
		if (token == nullptr)
			return;
		switch (token->get_type()) {
		default:
			break;
		case token_types::id: {
			var value = context->instance->storage.get_var_optimizable(static_cast<token_id *>(token)->get_id());
			if (value.usable() && value.is_protect())
				it.data() = new_value(value);
			return;
		}
		case token_types::expand: {
			tree_type<token_base *> &tree = static_cast<token_expand *>(token)->get_tree();
			opt_expr(tree, tree.root());
			return;
		}
		case token_types::array: {
			token_base *ptr = nullptr;
			for (auto &tree:static_cast<token_array *>(token)->get_array()) {
				ptr = tree.root().data();
				if (ptr != nullptr && ptr->get_type() == token_types::expand) {
					auto &child_tree = static_cast<token_expand *>(ptr)->get_tree();
					optimize_expression(child_tree);
					if (!optimizable(child_tree.root()))
						return;
				}
				else {
					optimize_expression(tree);
					if (!optimizable(tree.root()))
						return;
				}
			}
			token_base *oldt = it.data();
			try {
				array arr;
				for (auto &tree:static_cast<token_array *>(token)->get_array()) {
					ptr = tree.root().data();
					if (ptr != nullptr && ptr->get_type() == token_types::expand) {
						const auto &child_arr = context->instance->parse_expr(
						                            static_cast<token_expand *>(ptr)->get_tree().root()).const_val<array>();
						for (auto &it:child_arr)
							arr.push_back(copy(it));
					}
					else
						arr.push_back(copy(context->instance->parse_expr(tree.root())));
				}
				for (auto &it:arr)
					add_constant(it);
				it.data() = new_value(var::make<array>(std::move(arr)));
			}
			catch (...) {
				it.data() = oldt;
			}
			return;
		}
		case token_types::signal: {
			switch (static_cast<token_signal *>(token)->get_signal()) {
			default:
				break;
			case signal_types::gcnew_:
				if (it.left().data() != nullptr)
					throw runtime_error("Wrong grammar for gcnew expression.");
				opt_expr(tree, it.right());
				return;
			case signal_types::asi_:
				if (it.left().data() == nullptr || it.right().data() == nullptr)
					throw runtime_error("Wrong grammar for assign expression.");
				opt_expr(tree, it.left());
				opt_expr(tree, it.right());
				return;
			case signal_types::choice_: {
				token_signal *sig = dynamic_cast<token_signal *>(it.right().data());
				if (sig == nullptr || sig->get_signal() != signal_types::pair_)
					throw runtime_error("Wrong grammar for choice expression.");
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
						throw runtime_error("Choice condition must be boolean.");
				}
				return;
			}
			case signal_types::arrow_:
				if (it.left().data() == nullptr || it.right().data() == nullptr ||
				        it.right().data()->get_type() != token_types::id)
					throw runtime_error("Wrong grammar for arrow expression.");
				opt_expr(tree, it.left());
				return;
			case signal_types::dot_: {
				opt_expr(tree, it.left());
				token_base *lptr = it.left().data();
				token_base *rptr = it.right().data();
				if (rptr == nullptr || rptr->get_type() != token_types::id)
					throw runtime_error("Wrong grammar for dot expression.");
				if (lptr != nullptr && lptr->get_type() == token_types::value) {
					const var &a = static_cast<token_value *>(lptr)->get_value();
					token_base *orig_ptr = it.data();
					try {
						const var &v = context->instance->parse_dot(a, rptr);
						if (v.is_protect())
							it.data() = new_value(v);
					}
					catch (...) {
						it.data() = orig_ptr;
					}
				}
				return;
			}
			case signal_types::fcall_: {
				opt_expr(tree, it.left());
				opt_expr(tree, it.right());
				token_base *lptr = it.left().data();
				token_base *rptr = it.right().data();
				if (lptr == nullptr || rptr == nullptr || rptr->get_type() != token_types::arglist)
					throw runtime_error("Wrong syntax for function call.");
				if (lptr->get_type() == token_types::value) {
					var &a = static_cast<token_value *>(lptr)->get_value();
					if (a.type() == typeid(callable) && a.const_val<callable>().is_constant()) {
						token_base *ptr = nullptr;
						for (auto &tree:static_cast<token_arglist *>(rptr)->get_arglist()) {
							ptr = tree.root().data();
							if (ptr != nullptr && ptr->get_type() == token_types::expand) {
								if (!optimizable(static_cast<token_expand *>(ptr)->get_tree().root()))
									return;
							}
							else {
								if (!optimizable(tree.root()))
									return;
							}
						}
						token_base *oldt = it.data();
						try {
							vector args;
							args.reserve(static_cast<token_arglist *>(rptr)->get_arglist().size());
							for (auto &tree:static_cast<token_arglist *>(rptr)->get_arglist()) {
								ptr = tree.root().data();
								if (ptr != nullptr && ptr->get_type() == token_types::expand) {
									const auto &arr = context->instance->parse_expr(
									                      static_cast<token_expand *>(ptr)->get_tree().root()).const_val<array>();
									for (auto &it:arr)
										args.push_back(lvalue(it));
								}
								else
									args.push_back(lvalue(context->instance->parse_expr(tree.root())));
							}
							it.data() = new_value(a.const_val<callable>().call(args));
						}
						catch (...) {
							it.data() = oldt;
						}
					}
					else if (a.type() == typeid(object_method) && a.const_val<object_method>().is_constant) {
						token_base *ptr = nullptr;
						for (auto &tree:static_cast<token_arglist *>(rptr)->get_arglist()) {
							ptr = tree.root().data();
							if (ptr != nullptr && ptr->get_type() == token_types::expand) {
								if (!optimizable(static_cast<token_expand *>(ptr)->get_tree().root()))
									return;
							}
							else {
								if (!optimizable(tree.root()))
									return;
							}
						}
						token_base *oldt = it.data();
						try {
							const auto &om = a.const_val<object_method>();
							vector args{om.object};
							args.reserve(static_cast<token_arglist *>(rptr)->get_arglist().size());
							for (auto &tree:static_cast<token_arglist *>(rptr)->get_arglist()) {
								ptr = tree.root().data();
								if (ptr != nullptr && ptr->get_type() == token_types::expand) {
									const auto &arr = context->instance->parse_expr(
									                      static_cast<token_expand *>(ptr)->get_tree().root()).const_val<array>();
									for (auto &it:arr)
										args.push_back(lvalue(it));
								}
								else
									args.push_back(lvalue(context->instance->parse_expr(tree.root())));
							}
							it.data() = new_value(om.callable.const_val<callable>().call(args));
						}
						catch (...) {
							it.data() = oldt;
						}
					}
				}
				return;
			}
			}
		}
		}
		opt_expr(tree, it.left());
		opt_expr(tree, it.right());
		if (optimizable(it.left()) && optimizable(it.right())) {
			token_base *oldt = it.data();
			try {
				token_value *token = new_value(context->instance->parse_expr(it));
				tree.
				erase_left(it);
				tree.
				erase_right(it);
				it.data() = token;
			}
			catch (...) {
				it.data() = oldt;
			}
		}
	}

	void compiler_type::parse_define_var(tree_type<token_base *> &tree, define_var_profile &dvp)
	{
		const auto &it = tree.root();
		token_base *root = it.data();
		if (root == nullptr || root->get_type() != token_types::signal ||
		        static_cast<token_signal *>(root)->get_signal() != signal_types::asi_)
			throw runtime_error("Wrong grammar for variable definition.");
		token_base *left = it.left().data();
		const auto &right = it.right();
		if (left == nullptr || right.data() == nullptr || left->get_type() != token_types::id)
			throw runtime_error("Wrong grammar for variable definition.");
		dvp.id = static_cast<token_id *>(left)->get_id();
		dvp.expr = tree_type<token_base *>(right);
	}

	void compiler_type::dump_expr(tree_type<token_base *>::iterator it, std::ostream &stream)
	{
		if (!it.usable()) {
			stream << "< Empty Expression >";
			return;
		}
		stream << "<";
		bool cutoff = false;
		if (it.data() != nullptr) {
			stream << " Tree Node = ";
			cutoff = it.data()->dump(stream);
		}
		if (!cutoff) {
			if (it.left().usable()) {
				stream << " Left Leaf = ";
				dump_expr(it.left(), stream);
			}
			if (it.right().usable()) {
				stream << " Right Leaf = ";
				dump_expr(it.right(), stream);
			}
		}
		stream << " >";
	}

	void translator_type::match_grammar(const context_t &context, std::deque<token_base*>& raw)
	{
		for(auto& dat:m_data) {
			bool matched = false;
			{
				bool failed = false, skip_useless = false;
				std::size_t i = 0;
				for (auto &it:dat->first) {
					switch (it->get_type()) {
					case token_types::action: {
						if (skip_useless) {
							// Keep looking for id token or action token that have been replaced.
							for (; i < raw.size(); ++i) {
								if (raw[i]->get_type() == token_types::id) {
									auto &id = static_cast<token_id *>(raw[i])->get_id();
									if (context->compiler->action_map.exist(id) &&
									        context->compiler->action_map.match(id) ==
									        static_cast<token_action *>(it)->get_action())
										break;
								}
								else if (raw[i]->get_type() == token_types::action)
									break;
							}
						}
						if (i >= raw.size()) {
							failed = true;
							break;
						}
						// The "matched" condition is satisfied only if the target token is an id and it can match the grammar rule.
						if (raw[i]->get_type() == token_types::id) {
							auto &id = static_cast<token_id *>(raw[i])->get_id();
							if (!context->compiler->action_map.exist(id) ||
							        context->compiler->action_map.match(id) !=
							        static_cast<token_action *>(it)->get_action()) {
								matched = false;
								failed = true;
							}
							else
								matched = true;
						}
						else if (raw[i]->get_type() == token_types::action)
							failed= static_cast<token_action*>(raw[i])->get_action()!=static_cast<token_action *>(it)->get_action();
						else
							failed=true;
						skip_useless = false;
						++i;
						break;
					}
					case token_types::expr:
						skip_useless = true;
						break;
					}
					if (failed)
						break;
				}
				if ((skip_useless && i == raw.size()) || (!skip_useless && i < raw.size()))
					matched = false;
			}
			if (matched) {
				// If matched, find and replace all id token with correspondent action token.
				bool skip_useless = false;
				std::size_t i = 0;
				for (auto &it:dat->first) {
					switch (it->get_type()) {
					case token_types::action: {
						if (skip_useless) {
							// Keep looking for id token or action token that have been replaced.
							for (; i < raw.size(); ++i) {
								if (raw[i]->get_type() == token_types::id) {
									auto &id = static_cast<token_id *>(raw[i])->get_id();
									if (context->compiler->action_map.exist(id) &&
									        context->compiler->action_map.match(id) ==
									        static_cast<token_action *>(it)->get_action())
										break;
								}
								else if(raw[i]->get_type() == token_types::action)
									break;
							}
						}
						if (raw[i]->get_type() == token_types::id) {
							auto &id = static_cast<token_id *>(raw[i])->get_id();
							if (context->compiler->action_map.exist(id) &&
							        context->compiler->action_map.match(id) ==
							        static_cast<token_action *>(it)->get_action())
								raw[i] = it;
						}
						skip_useless = false;
						++i;
						break;
					}
					case token_types::expr:
						skip_useless = true;
						break;
					}
				}
			}
		}
	}

	void translator_type::translate(const context_t &context, const std::deque<std::deque<token_base *>> &lines,
	                                std::deque<statement_base *> &statements, bool raw)
	{
		std::deque<std::deque<token_base *>> tmp;
		method_base *method = nullptr;
		std::size_t method_line_num = 0, line_num = 0;
		int level = 0;
		for (auto &it:lines) {
			std::deque<token_base *> line = it;
			line_num = static_cast<token_endline *>(line.back())->get_line_num();
			try {
				if (raw)
					context->compiler->process_line(line);
				method_base *m = this->match(line);
				switch (m->get_type()) {
				case method_types::null:
					throw runtime_error("Null type of grammar.");
					break;
				case method_types::single: {
					statement_base *sptr = nullptr;
					if (level > 0) {
						if (m->get_target_type() == statement_types::end_) {
							if (raw) {
								context->instance->storage.remove_set();
								context->instance->storage.remove_domain();
							}
							--level;
						}
						if (level == 0) {
							line_num = method_line_num;
							if (m->get_target_type() == statement_types::end_)
								sptr = static_cast<method_end *>(m)->translate_end(method, context, tmp, line);
							else
								sptr = method->translate(context, tmp);
							tmp.clear();
							method = nullptr;
						}
						else {
							if (raw)
								m->preprocess(context, {line});
							tmp.push_back(line);
						}
					}
					else {
						if (m->get_target_type() == statement_types::end_)
							throw runtime_error("Hanging end statement.");
						else {
							if (raw)
								m->preprocess(context, {line});
							sptr = m->translate(context, {line});
						}
					}
					if (sptr != nullptr)
						statements.push_back(sptr);
				}
				break;
				case method_types::block: {
					if (level == 0) {
						method_line_num = static_cast<token_endline *>(line.back())->get_line_num();
						method = m;
					}
					++level;
					if (raw) {
						context->instance->storage.add_domain();
						context->instance->storage.add_set();
						m->preprocess(context, {line});
					}
					tmp.push_back(line);
				}
				break;
				case method_types::jit_command:
					m->translate(context, {line});
					break;
				}
			}
			catch (const cs::exception &e) {
				throw e;
			}
			catch (const std::exception &e) {
				throw exception(line_num, context->file_path, context->file_buff.at(line_num - 1), e.what());
			}
		}
		if (level != 0)
			throw runtime_error("Lack of the \"end\" signal.");
	}
}