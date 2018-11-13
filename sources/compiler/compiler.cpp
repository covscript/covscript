/*
* Covariant Script Compiler
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
#include <covscript/compiler.hpp>
#include <covscript/statement.hpp>

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
		}
		o << "\" >";
		return false;
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

	void compiler_type::trim_expr(cov::tree<token_base *> &tree, cov::tree<token_base *>::iterator it)
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
			if (!runtime->storage.exist_record(id) && runtime->storage.exist_record_in_struct(id)) {
				it.data() = new token_signal(signal_types::dot_);
				tree.emplace_left_left(it, new token_id("this"));
				tree.emplace_right_right(it, token);
			}
			return;
			break;
		}
		case token_types::expr: {
			cov::tree<token_base *> &t = static_cast<token_expr *>(it.data())->get_tree();
			trim_expression(t);
			tree.merge(it, t);
			return;
			break;
		}
		case token_types::array: {
			for (auto &tree:static_cast<token_array *>(token)->get_array())
				trim_expression(tree);
			return;
			break;
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
				break;
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
						parallel_list = new token_parallel({cov::tree<token_base *>(it.left())});
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
				break;
			}
			case signal_types::choice_: {
				token_signal *sig = dynamic_cast<token_signal *>(it.right().data());
				if (sig == nullptr || sig->get_signal() != signal_types::pair_)
					throw runtime_error("Wrong grammar for choice expression.");
				trim_expr(tree, it.left());
				trim_expr(tree, it.right().left());
				trim_expr(tree, it.right().right());
				return;
				break;
			}
			case signal_types::vardef_: {
				if (it.left().data() != nullptr)
					throw runtime_error("Wrong grammar for variable definition.");
				token_base *rptr = it.right().data();
				if (rptr == nullptr || rptr->get_type() != token_types::id)
					throw runtime_error("Wrong grammar for variable definition.");
				runtime->storage.add_record(static_cast<token_id *>(rptr)->get_id());
				it.data() = rptr;
				return;
				break;
			}
			case signal_types::varprt_: {
				if (it.left().data() != nullptr)
					throw runtime_error("Wrong grammar for variable definition.");
				token_base *rptr = it.right().data();
				if (rptr == nullptr || rptr->get_type() != token_types::id)
					throw runtime_error("Wrong grammar for variable definition.");
				it.data() = rptr;
				return;
				break;
			}
			case signal_types::arrow_:
				if (it.left().data() == nullptr || it.right().data() == nullptr ||
				        it.right().data()->get_type() != token_types::id)
					throw runtime_error("Wrong grammar for arrow expression.");
				trim_expr(tree, it.left());
				return;
				break;
			case signal_types::dot_: {
				trim_expr(tree, it.left());
				token_base *rptr = it.right().data();
				if (rptr == nullptr || rptr->get_type() != token_types::id)
					throw runtime_error("Wrong grammar for dot expression.");
				return;
				break;
			}
			case signal_types::fcall_: {
				trim_expr(tree, it.left());
				trim_expr(tree, it.right());
				token_base *lptr = it.left().data();
				token_base *rptr = it.right().data();
				if (lptr == nullptr || rptr == nullptr || rptr->get_type() != token_types::arglist)
					throw runtime_error("Wrong syntax for function call.");
				return;
				break;
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
				break;
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
				for (auto &it:static_cast<token_arglist *>(lptr)->get_arglist()) {
					if (it.root().data() == nullptr)
						throw internal_error("Null pointer accessed.");
					if (it.root().data()->get_type() != token_types::id)
						throw runtime_error("Wrong grammar for function definition.");
					const std::string &str = static_cast<token_id *>(it.root().data())->get_id();
					for (auto &it:args)
						if (it == str)
							throw runtime_error("Redefinition of function argument.");
					args.push_back(str);
				}
				it.data() = new_value(var::make_protect<callable>(function(context, args,
				std::deque<statement_base *> {
					new statement_return(
					    cov::tree<
					token_base *>{
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
		trim_expr(tree, it.left());
		trim_expr(tree, it.right());
	}

	void compiler_type::opt_expr(cov::tree<token_base *> &tree, cov::tree<token_base *>::iterator it)
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
			if (runtime->storage.exist_record(id)) {
				if (runtime->storage.var_exist_current(id) && runtime->storage.get_var_current(id).is_protect())
					it.data() = new_value(runtime->storage.get_var(id));
			}
			else if (!runtime->storage.exist_record_in_struct(id) && runtime->storage.var_exist(id) &&
			         runtime->storage.get_var(id).is_protect())
				it.data() = new_value(runtime->storage.get_var(id));
			return;
			break;
		}
		case token_types::array: {
			bool is_optimizable = true;
			for (auto &tree:static_cast<token_array *>(token)->get_array()) {
				optimize_expression(tree);
				if (is_optimizable && !optimizable(tree.root()))
					is_optimizable = false;
			}
			if (is_optimizable) {
				array
				arr;
				for (auto &tree:static_cast<token_array *>(token)->get_array())
					arr.push_back((new_value(copy(runtime->parse_expr(tree.root()))))->get_value());
				it.data() = new_value(var::make<array>(std::move(arr)));
			}
			return;
			break;
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
				break;
			case signal_types::asi_:
				if (it.left().data() == nullptr || it.right().data() == nullptr)
					throw runtime_error("Wrong grammar for assign expression.");
				opt_expr(tree, it.left());
				opt_expr(tree, it.right());
				return;
				break;
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
				break;
			}
			case signal_types::arrow_:
				if (it.left().data() == nullptr || it.right().data() == nullptr ||
				        it.right().data()->get_type() != token_types::id)
					throw runtime_error("Wrong grammar for arrow expression.");
				opt_expr(tree, it.left());
				return;
				break;
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
						const var &v = runtime->parse_dot(a, rptr);
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
					throw runtime_error("Wrong syntax for function call.");
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
								args.push_back(lvalue(runtime->parse_expr(tree.root())));
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
							const auto &om = a.const_val<object_method>();
							vector args{om.object};
							args.reserve(static_cast<token_arglist *>(rptr)->get_arglist().size());
							for (auto &tree:static_cast<token_arglist *>(rptr)->get_arglist())
								args.push_back(lvalue(runtime->parse_expr(tree.root())));
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
			}
		}
		}
		opt_expr(tree, it.left());
		opt_expr(tree, it.right());
		if (optimizable(it.left()) && optimizable(it.right())) {
			token_base *oldt = it.data();
			try {
				token_value *token = new_value(runtime->parse_expr(it));
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

	void compiler_type::parse_define_var(cov::tree<token_base *> &tree, define_var_profile &dvp)
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
		dvp.expr = cov::tree<token_base *>(right);
	}

	void compiler_type::dump_expr(cov::tree<token_base *>::const_iterator it, std::ostream &stream)
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
}