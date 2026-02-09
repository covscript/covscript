/*
 * Covariant Script Compiler
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
#include <regex>

namespace cs {
	const map_t<char, char> token_value::escape_char = {
		{'\'', '\''},
		{'\"', '\"'},
		{'\?', '\?'},
		{'\\', '\\'},
		{'\a', 'a'},
		{'\b', 'b'},
		{'\f', 'f'},
		{'\n', 'n'},
		{'\r', 'r'},
		{'\t', 't'},
		{'\v', 'v'}
	};

	bool token_value::dump(std::ostream &o) const
	{
		o << "< Value = ";
		if (mVal.is_type_of<cs::string>()) {
			o << "\"";
			const cs::string &str = mVal.const_val<cs::string>();
			for (auto ch : str) {
				if (escape_char.count(ch) > 0)
					o << '\\' << escape_char.at(ch);
				else
					o << ch;
			}
			o << "\"";
		}
		else if (mVal.is_type_of<char>()) {
			o << "\'";
			char ch = mVal.const_val<char>();
			if (escape_char.count(ch) > 0)
				o << '\\' << escape_char.at(ch);
			else
				o << ch;
			o << "\'";
		}
		else
			o << mVal.to_string();
		o << ">";
		return true;
	}

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
		case signal_types::lnkasi_:
			o << ":=";
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
		case signal_types::addr_:
			o << "&";
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
		case signal_types::varchk_:
			o << "[varchk]";
			break;
		case signal_types::varprt_:
			o << "[varprt]";
			break;
		case signal_types::vargs_:
			o << "[vargs]";
			break;
		case signal_types::bind_:
			o << "=";
			break;
		case signal_types::error_:
			o << "[error]";
			break;
		case signal_types::endline_:
			o << "[endline]";
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
		for (auto &tree : mTreeList) {
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
		for (auto &tree : mTreeList) {
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
		for (auto &tree : mTreeList) {
			o << "< ChildExpression: ";
			compiler_type::dump_expr(tree.root(), o);
			o << " >";
		}
		o << "} >";
		return true;
	}

	const mapping<std::string, signal_types> compiler_type::signal_map = {
		{";", signal_types::endline_},
		{"+", signal_types::add_},
		{"+=", signal_types::addasi_},
		{"-", signal_types::sub_},
		{"-=", signal_types::subasi_},
		{"*", signal_types::mul_},
		{"*=", signal_types::mulasi_},
		{"/", signal_types::div_},
		{"/=", signal_types::divasi_},
		{"%", signal_types::mod_},
		{"%=", signal_types::modasi_},
		{"^", signal_types::pow_},
		{"^=", signal_types::powasi_},
		{">", signal_types::abo_},
		{"<", signal_types::und_},
		{"=", signal_types::asi_},
		{":=", signal_types::lnkasi_},
		{"&", signal_types::addr_},
		{"|", signal_types::error_},
		{"&&", signal_types::and_},
		{"||", signal_types::or_},
		{"!", signal_types::not_},
		{"==", signal_types::equ_},
		{"!=", signal_types::neq_},
		{">=", signal_types::aeq_},
		{"<=", signal_types::ueq_},
		{"(", signal_types::slb_},
		{")", signal_types::srb_},
		{"[", signal_types::mlb_},
		{"]", signal_types::mrb_},
		{"{", signal_types::llb_},
		{"}", signal_types::lrb_},
		{",", signal_types::com_},
		{".", signal_types::dot_},
		{"()", signal_types::esb_},
		{"[]", signal_types::emb_},
		{"{}", signal_types::elb_},
		{"++", signal_types::inc_},
		{"--", signal_types::dec_},
		{":", signal_types::pair_},
		{"?", signal_types::choice_},
		{"->", signal_types::arrow_},
		{"..", signal_types::error_},
		{"...", signal_types::vargs_}
	};

	const mapping<std::string, action_types> compiler_type::action_map = {
		{"import", action_types::import_},
		{"as", action_types::as_},
		{"package", action_types::package_},
		{"using", action_types::using_},
		{"namespace", action_types::namespace_},
		{"struct", action_types::struct_},
		{"class", action_types::struct_},
		{"extends", action_types::extends_},
		{"block", action_types::block_},
		{"end", action_types::endblock_},
		{"var", action_types::var_},
		{"link", action_types::link_},
		{"constant", action_types::constant_},
		{"do", action_types::do_},
		{"if", action_types::if_},
		{"else", action_types::else_},
		{"switch", action_types::switch_},
		{"case", action_types::case_},
		{"default", action_types::default_},
		{"while", action_types::while_},
		{"until", action_types::until_},
		{"loop", action_types::loop_},
		{"for", action_types::for_},
		{"foreach", action_types::foreach_},
		{"in", action_types::in_},
		{"break", action_types::break_},
		{"continue", action_types::continue_},
		{"function", action_types::function_},
		{"override", action_types::override_},
		{"return", action_types::return_},
		{"try", action_types::try_},
		{"catch", action_types::catch_},
		{"throw", action_types::throw_}
	};

	const mapping<std::string, std::function<token_base *()>> compiler_type::reserved_map = {
		{
			"and", []() -> token_base *
			{ return new token_signal(signal_types::and_); }
		},
		{
			"or", []() -> token_base *
			{ return new token_signal(signal_types::or_); }
		},
		{
			"not", []() -> token_base *
			{ return new token_signal(signal_types::not_); }
		},
		{
			"typeid", []() -> token_base *
			{ return new token_signal(signal_types::typeid_); }
		},
		{
			"new", []() -> token_base *
			{ return new token_signal(signal_types::new_); }
		},
		{
			"gcnew", []() -> token_base *
			{ return new token_signal(signal_types::gcnew_); }
		},
		{
			"local", []() -> token_base *
			{
				return new token_value(var::make_constant<constant_values>(constant_values::local_namepace));
			}
		},
		{
			"global", []() -> token_base *
			{
				return new token_value(var::make_constant<constant_values>(constant_values::global_namespace));
			}
		},
		{
			"null", []() -> token_base *
			{ return new token_value(null_pointer); }
		},
		{
			"true", []() -> token_base *
			{ return new token_value(var::make_constant<bool>(true)); }
		},
		{
			"false", []() -> token_base *
			{ return new token_value(var::make_constant<bool>(false)); }
		}
	};

	const mapping<char32_t, char32_t> compiler_type::escape_map = {
		{'a', '\a'},
		{'b', '\b'},
		{'f', '\f'},
		{'n', '\n'},
		{'r', '\r'},
		{'t', '\t'},
		{'v', '\v'},
		{'\\', '\\'},
		{'\'', '\''},
		{'\"', '\"'},
		{'0', '\0'}
	};

	const set_t<char32_t> compiler_type::signals = {
		'+', '-', '*', '/', '%', '^', ',', '.', '>', '<', '=', '&', '|', '!', '(', ')', '[', ']', '{', '}', ':',
		'?', ';'
	};

	const mapping<signal_types, int> compiler_type::signal_level_map = {
		{signal_types::add_, 10},
		{signal_types::addasi_, 1},
		{signal_types::sub_, 10},
		{signal_types::subasi_, 1},
		{signal_types::mul_, 11},
		{signal_types::mulasi_, 1},
		{signal_types::div_, 11},
		{signal_types::divasi_, 1},
		{signal_types::mod_, 12},
		{signal_types::modasi_, 1},
		{signal_types::pow_, 12},
		{signal_types::powasi_, 1},
		{signal_types::com_, 0},
		{signal_types::dot_, 15},
		{signal_types::arrow_, 15},
		{signal_types::und_, 9},
		{signal_types::abo_, 9},
		{signal_types::asi_, 1},
		{signal_types::lnkasi_, 1},
		{signal_types::choice_, 3},
		{signal_types::pair_, 4},
		{signal_types::equ_, 9},
		{signal_types::ueq_, 9},
		{signal_types::aeq_, 9},
		{signal_types::neq_, 9},
		{signal_types::lambda_, 2},
		{signal_types::vardef_, 20},
		{signal_types::varchk_, 20},
		{signal_types::varprt_, 20},
		{signal_types::or_, 6},
		{signal_types::and_, 7},
		{signal_types::not_, 8},
		{signal_types::inc_, 13},
		{signal_types::dec_, 13},
		{signal_types::addr_, 13},
		{signal_types::fcall_, 15},
		{signal_types::emb_, 15},
		{signal_types::access_, 15},
		{signal_types::typeid_, 14},
		{signal_types::new_, 14},
		{signal_types::gcnew_, 14},
		{signal_types::vargs_, 20}
	};

	const set_t<signal_types> compiler_type::signal_left_associative = {
		signal_types::asi_, signal_types::lnkasi_, signal_types::addasi_, signal_types::subasi_,
		signal_types::mulasi_,
		signal_types::divasi_, signal_types::modasi_, signal_types::powasi_
	};

	bool compiler_type::find_id_ref(tree_type<token_base *>::iterator it, const std::string &id)
	{
		if (!it.usable())
			return false;
		token_base *token = it.data();
		if (token == nullptr)
			return false;
		switch (token->get_type()) {
		default:
			break;
		case token_types::id:
			return static_cast<token_id *>(token)->get_id().get_id() == id;
		case token_types::expr:
			return find_id_ref(static_cast<token_expr *>(it.data())->get_tree().root(), id);
		case token_types::array: {
			for (auto &tree : static_cast<token_array *>(token)->get_array()) {
				if (find_id_ref(tree.root(), id))
					return true;
			}
			return false;
		}
		}
		if (find_id_ref(it.left(), id) || find_id_ref(it.right(), id))
			return true;
		else
			return false;
	}

	void
	compiler_type::trim_expr(tree_type<token_base *> &tree, tree_type<token_base *>::iterator it, trim_type do_trim)
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
			if (do_trim != trim_type::no_this_deduce) {
				const std::string &id = static_cast<token_id *>(token)->get_id();
				if (!context->instance->storage.exist_record(id) &&
				        context->instance->storage.exist_record_in_struct(id)) {
					it.data() = new token_signal(signal_types::dot_);
					tree.emplace_left_left(it, token);
					tree.emplace_left_left(it, new token_id("this"));
					tree.emplace_right_right(it, token);
				}
			}
			return;
		}
		case token_types::expr: {
			if (do_trim != trim_type::no_expr_fold) {
				tree_type<token_base *> &t = static_cast<token_expr *>(it.data())->get_tree();
				trim_expression(t, do_trim);
				tree.merge(it, t);
			}
			return;
		}
		case token_types::array: {
			for (auto &tree : static_cast<token_array *>(token)->get_array())
				trim_expression(tree, do_trim);
			return;
		}
		case token_types::signal: {
			switch (static_cast<token_signal *>(token)->get_signal()) {
			default:
				break;
			case signal_types::addr_:
				if (it.left().data() != nullptr)
					throw compile_error("Wrong grammar for addr expression.");
				break;
			case signal_types::new_:
				if (it.left().data() != nullptr)
					throw compile_error("Wrong grammar for new expression.");
				break;
			case signal_types::gcnew_:
				if (it.left().data() != nullptr)
					throw compile_error("Wrong grammar for gcnew expression.");
				trim_expr(tree, it.right(), do_trim);
				return;
				break;
			case signal_types::typeid_:
				if (it.left().data() != nullptr)
					throw compile_error("Wrong grammar for typeid expression.");
				break;
			case signal_types::not_:
				if (it.left().data() != nullptr)
					throw compile_error("Wrong grammar for not expression.");
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
					throw compile_error("Wrong grammar for assign expression.");
				trim_expr(tree, it.left(), do_trim);
				if (it.left().data()->get_type() == token_types::parallel)
					it.data() = new token_signal(signal_types::bind_);
				trim_expr(tree, it.right(), do_trim);
				return;
			case signal_types::com_: {
				trim_expr(tree, it.left(), trim_type::no_expr_fold);
				trim_expr(tree, it.right(), trim_type::no_expr_fold);
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
						for (auto &tree : static_cast<token_parallel *>(rptr)->get_parallel())
							parallel_list->get_parallel().push_back(tree);
					else if (rptr != nullptr)
						parallel_list->get_parallel().emplace_back(it.right());
					for (auto &lst : parallel_list->get_parallel())
						trim_expr(lst, lst.root(), do_trim);
					it.data() = parallel_list;
				}
				return;
			}
			case signal_types::choice_: {
				token_signal *sig = dynamic_cast<token_signal *>(it.right().data());
				if (sig == nullptr || sig->get_signal() != signal_types::pair_)
					throw compile_error("Wrong grammar for choice expression.");
				trim_expr(tree, it.left(), do_trim);
				trim_expr(tree, it.right().left(), do_trim);
				trim_expr(tree, it.right().right(), do_trim);
				return;
			}
			case signal_types::vardef_: {
				if (it.left().data() != nullptr)
					throw compile_error("Wrong grammar for variable declaration.");
				trim_expr(tree, it.right(), trim_type::no_this_deduce);
				token_base *rptr = it.right().data();
				if (rptr == nullptr || rptr->get_type() != token_types::id)
					throw compile_error("Wrong grammar for variable declaration.");
				context->instance->storage.add_record(static_cast<token_id *>(rptr)->get_id().get_id());
				it.data() = rptr;
				return;
			}
			case signal_types::varchk_: {
				if (it.left().data() != nullptr)
					throw compile_error("Wrong grammar for variable declaration.");
				trim_expr(tree, it.right(), trim_type::no_this_deduce);
				context->instance->check_declar_var(it.right(), true);
				it.data() = it.right().data();
				return;
			}
			case signal_types::varprt_: {
				if (it.left().data() != nullptr)
					throw compile_error("Wrong grammar for variable declaration.");
				trim_expr(tree, it.right(), trim_type::no_this_deduce);
				it.data() = it.right().data();
				return;
			}
			case signal_types::arrow_:
				if (it.left().data() == nullptr || it.right().data() == nullptr ||
				        it.right().data()->get_type() != token_types::id)
					throw compile_error("Wrong grammar for arrow expression.");
				trim_expr(tree, it.left(), do_trim);
				return;
			case signal_types::dot_: {
				trim_expr(tree, it.left(), do_trim);
				token_base *rptr = it.right().data();
				if (rptr == nullptr || rptr->get_type() != token_types::id)
					throw compile_error("Wrong grammar for dot expression.");
				return;
			}
			case signal_types::fcall_: {
				trim_expr(tree, it.left(), do_trim);
				trim_expr(tree, it.right(), do_trim);
				token_base *lptr = it.left().data();
				token_base *rptr = it.right().data();
				if (lptr == nullptr || rptr == nullptr || rptr->get_type() != token_types::arglist)
					throw compile_error("Wrong syntax for function call.");
				return;
			}
			case signal_types::vargs_: {
				if (it.left().data() == nullptr) {
					token_base *rptr = it.right().data();
					if (rptr == nullptr || rptr->get_type() != token_types::id)
						throw compile_error("Wrong grammar for vargs expression.");
					it.data() = new token_vargs(static_cast<token_id *>(rptr)->get_id());
				}
				else {
					if (it.right().data() != nullptr)
						throw compile_error("Wrong grammar for vargs expression.");
					trim_expr(tree, it.left(), do_trim);
					it.data() = new token_expand(tree_type<token_base *>(it.left()));
				}
				return;
			}
			case signal_types::emb_: {
				trim_expr(tree, it.left(), do_trim);
				trim_expr(tree, it.right(), do_trim);
				token_base *lptr = it.left().data();
				token_base *rptr = it.right().data();
				if (!inside_lambda || lptr != nullptr || rptr == nullptr ||
				        rptr->get_type() != token_types::arglist)
					throw compile_error("Wrong grammar for lambda expression.");
				it.data() = rptr;
				return;
			}
			case signal_types::lambda_: {
				inside_lambda = true;
				trim_expr(tree, it.left(), do_trim);
				inside_lambda = false;
				token_base *lptr = it.left().data();
				trim_expr(tree, it.right(), trim_type::no_this_deduce);
				token_base *rptr = it.right().data();
				if (lptr == nullptr || rptr == nullptr || lptr->get_type() != token_types::arglist)
					throw compile_error("Wrong grammar for lambda expression.");
				std::vector<std::string> args;
				bool is_vargs = false;
				for (auto &item : static_cast<token_arglist *>(lptr)->get_arglist()) {
					if (item.root().data() == nullptr)
						throw internal_error("Null pointer accessed.");
					try_fix_this_deduction(item.root());
					if (item.root().data()->get_type() == token_types::id) {
						const std::string &str = static_cast<token_id *>(item.root().data())->get_id();
						for (auto &it : args)
							if (it == str)
								throw compile_error("Redefinition of function argument.");
						args.push_back(str);
					}
					else if (item.root().data()->get_type() == token_types::vargs) {
						const std::string &str = static_cast<token_vargs *>(item.root().data())->get_id();
						if (!args.empty())
							throw compile_error("Redefinition of function argument(Multi-define of vargs).");
						args.push_back(str);
						is_vargs = true;
					}
					else
						throw compile_error("Unexpected element in function argument list.");
				}
				bool find_self_ref = find_id_ref(it.right(), "self");
				if (!is_vargs && find_self_ref) {
					std::vector<std::string> new_args{"self"};
					new_args.reserve(args.size());
					for (auto &name : args) {
						if (name != "self")
							new_args.emplace_back(std::move(name));
						else
							throw runtime_error("Overwrite the default argument \"self\".");
					}
					std::swap(new_args, args);
				}
				statement_base *ret = new statement_return(tree_type<token_base *>(it.right()), context,
				    new token_endline(token->get_line_num()));
#ifdef CS_DEBUGGER
				std::string decl = "function [lambda](";
				if (args.size() != 0) {
					for (auto &it : args)
						decl += it + ", ";
					decl.pop_back();
					decl[decl.size() - 1] = ')';
				}
				else
					decl += ")";
				function func(context, decl, ret, args, std::deque<statement_base *> {ret}, is_vargs, true);
#else
				function func(context, args, std::deque<statement_base *> {ret}, is_vargs, true);
#endif
				if (find_self_ref) {
					var lambda = var::make<object_method>(var(), var::make_protect<callable>(func));
					lambda.val<object_method>().object = lambda;
					lambda.mark_protect();
					it.data() = new_value(lambda);
				}
				else
					it.data() = new_value(var::make_protect<callable>(func));
				return;
			}
			}
		}
		}
		trim_expr(tree, it.left(), do_trim);
		trim_expr(tree, it.right(), do_trim);
	}

	void
	compiler_type::opt_expr(tree_type<token_base *> &tree, tree_type<token_base *>::iterator it, optm_type do_optm)
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
			if (value.usable() && value.is_protect()) {
				if (do_optm == optm_type::enable_namespace_optm || !value.is_type_of<namespace_t>() ||
				        !value.const_val<namespace_t>()->get_domain().exist("__PRAGMA_CS_NAMESPACE_DEFINITION__"))
					it.data() = new_value(value);
			}
			return;
		}
		case token_types::literal: {
			token_literal *ptr = static_cast<token_literal *>(token);
			token_base *oldt = it.data();
			try {
				it.data() = new_value(context->instance->get_string_literal(ptr->get_data(), ptr->get_literal()));
			}
			catch (...) {
				it.data() = oldt;
			}
			return;
		}
		case token_types::expand: {
			tree_type<token_base *> &tree = static_cast<token_expand *>(token)->get_tree();
			opt_expr(tree, tree.root(), do_optm);
			return;
		}
		case token_types::array: {
			token_base *ptr = nullptr;
			for (auto &tree : static_cast<token_array *>(token)->get_array()) {
				ptr = tree.root().data();
				if (ptr != nullptr && ptr->get_type() == token_types::expand) {
					auto &child_tree = static_cast<token_expand *>(ptr)->get_tree();
					optimize_expression(child_tree, do_optm);
					if (!optimizable(child_tree.root()))
						return;
				}
				else {
					optimize_expression(tree, do_optm);
					if (!optimizable(tree.root()))
						return;
				}
			}
			token_base *oldt = it.data();
			try {
				array arr;
				for (auto &tree : static_cast<token_array *>(token)->get_array()) {
					ptr = tree.root().data();
					if (ptr != nullptr && ptr->get_type() == token_types::expand) {
						const auto &child_arr = context->instance->parse_expr(
						                            static_cast<token_expand *>(ptr)->get_tree().root())
						                        .const_val<array>();
						for (auto &it : child_arr)
							arr.push_back(copy(it));
					}
					else
						arr.push_back(copy(context->instance->parse_expr(tree.root())));
				}
				for (auto &it : arr)
					add_constant(it);
				it.data() = new_value(var::make<array>(std::move(arr)));
			}
			catch (...) {
				it.data() = oldt;
			}
			return;
		}
		case token_types::parallel: {
			for (auto &tree : static_cast<token_parallel *>(token)->get_parallel())
				optimize_expression(tree, do_optm);
			return;
		}
		case token_types::signal: {
			switch (static_cast<token_signal *>(token)->get_signal()) {
			default:
				break;
			case signal_types::new_:
				// Fix(2020-11-26): Terminate new expression optimization
				if (it.left().data() != nullptr)
					throw compile_error("Wrong grammar for new expression.");
				return;
			case signal_types::gcnew_:
				if (it.left().data() != nullptr)
					throw compile_error("Wrong grammar for gcnew expression.");
				opt_expr(tree, it.right(), do_optm);
				return;
			case signal_types::asi_:
				if (it.left().data() == nullptr || it.right().data() == nullptr)
					throw compile_error("Wrong grammar for assign expression.");
				opt_expr(tree, it.left(), do_optm);
				opt_expr(tree, it.right(), do_optm);
				return;
			case signal_types::choice_: {
				token_signal *sig = dynamic_cast<token_signal *>(it.right().data());
				if (sig == nullptr || sig->get_signal() != signal_types::pair_)
					throw compile_error("Wrong grammar for choice expression.");
				opt_expr(tree, it.left(), do_optm);
				opt_expr(tree, it.right().left(), do_optm);
				opt_expr(tree, it.right().right(), do_optm);
				token_value *val = dynamic_cast<token_value *>(it.left().data());
				if (val != nullptr) {
					if (val->get_value().is_type_of<boolean>()) {
						if (val->get_value().const_val<boolean>())
							tree.reserve_left(it.right());
						else
							tree.reserve_right(it.right());
						tree.reserve_right(it);
					}
					else
						throw compile_error("Choice condition must be boolean.");
				}
				return;
			}
			case signal_types::arrow_:
				if (it.left().data() == nullptr || it.right().data() == nullptr ||
				        it.right().data()->get_type() != token_types::id)
					throw compile_error("Wrong grammar for arrow expression.");
				opt_expr(tree, it.left(), do_optm);
				return;
			case signal_types::dot_: {
				tree_type<token_base *> ltree(it.left());
				opt_expr(tree, it.left(), optm_type::enable_namespace_optm);
				token_base *lptr = it.left().data();
				token_base *rptr = it.right().data();
				if (rptr == nullptr || rptr->get_type() != token_types::id)
					throw compile_error("Wrong grammar for dot expression.");
				if (lptr != nullptr && lptr->get_type() == token_types::value) {
					const var &a = static_cast<token_value *>(lptr)->get_value();
					token_base *orig_ptr = it.data();
					try {
						const var &v = context->instance->parse_dot(a, rptr);
						if (v.is_protect())
							it.data() = new_value(v);
					}
					catch (...) {
						// Fix(2020-12-05): Broken AST structure caused by tree_type::merge
						it.data() = orig_ptr;
						tree.merge(it.left(), ltree);
					}
				}
				return;
			}
			case signal_types::fcall_: {
				opt_expr(tree, it.left(), do_optm);
				opt_expr(tree, it.right(), do_optm);
				token_base *lptr = it.left().data();
				token_base *rptr = it.right().data();
				if (lptr == nullptr || rptr == nullptr || rptr->get_type() != token_types::arglist)
					throw compile_error("Wrong syntax for function call.");
				if (lptr->get_type() == token_types::value) {
					var &a = static_cast<token_value *>(lptr)->get_value();
					if (a.is_type_of<callable>() && a.const_val<callable>().is_request_fold()) {
						token_base *ptr = nullptr;
						for (auto &tree : static_cast<token_arglist *>(rptr)->get_arglist()) {
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
							for (auto &tree : static_cast<token_arglist *>(rptr)->get_arglist()) {
								ptr = tree.root().data();
								if (ptr != nullptr && ptr->get_type() == token_types::expand) {
									const auto &arr = context->instance->parse_expr(
									                      static_cast<token_expand *>(ptr)->get_tree().root())
									                  .const_val<array>();
									for (auto &it : arr)
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
					else if (a.is_type_of<object_method>() &&
					         a.const_val<object_method>().is_request_fold) {
						token_base *ptr = nullptr;
						for (auto &tree : static_cast<token_arglist *>(rptr)->get_arglist()) {
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
							for (auto &tree : static_cast<token_arglist *>(rptr)->get_arglist()) {
								ptr = tree.root().data();
								if (ptr != nullptr && ptr->get_type() == token_types::expand) {
									const auto &arr = context->instance->parse_expr(
									                      static_cast<token_expand *>(ptr)->get_tree().root())
									                  .const_val<array>();
									for (auto &it : arr)
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
		opt_expr(tree, it.left(), do_optm);
		opt_expr(tree, it.right(), do_optm);
		if (optimizable(it.left()) && optimizable(it.right())) {
			token_base *oldt = it.data();
			try {
				token_value *token = new_value(context->instance->parse_expr(it));
				tree.erase_left(it);
				tree.erase_right(it);
				it.data() = token;
			}
			catch (...) {
				it.data() = oldt;
			}
		}
	}

	void compiler_type::try_fix_this_deduction(cs::tree_type<cs::token_base *>::iterator it)
	{
		if (!it.usable())
			return;
		token_signal *sig = dynamic_cast<token_signal *>(it.data());
		if (sig == nullptr || sig->get_signal() != signal_types::dot_)
			return;
		token_id *id = dynamic_cast<token_id *>(it.left().data());
		if (id == nullptr || id->get_id().get_id() != "this")
			return;
		// Check if it was generated by AST trimmer
		if (it.left().left().usable() && it.left().left().data() != nullptr &&
		        it.left().left().data() == it.right().data()) {
			// it.data() = it.right().data();
			throw compile_error(std::string("Symbol collision with structure member \"") +
			                    static_cast<token_id *>(it.right().data())->get_id().get_id() + "\".");
		}
	}

	void compiler_type::import_csym(const std::string &module_path, const std::string &csym_path)
	{
		std::ifstream ifs(csym_path);
		if (!ifs.is_open())
			return;
		csym_info csym;
		std::regex reg("^#\\$cSYM/1\\.0\\(([^\\)]*)\\):(.*)$");
		// Read file
		std::string header, buff;
		bool expect_n = false;
		for (int ch = ifs.get(); ifs; ch = ifs.get()) {
			if (expect_n) {
				expect_n = false;
				if (ch != '\n')
					buff += '\r';
			}
			switch (ch) {
			case '\n':
				if (!header.empty()) {
					csym.codes.emplace_back(buff);
					buff.clear();
				}
				else
					std::swap(header, buff);
				break;
			case '\r':
				expect_n = true;
				break;
			default:
				buff += ch;
				break;
			}
		}
		if (!buff.empty()) {
			csym.codes.emplace_back(buff);
		}
		// Parsing header
		std::smatch match;
		if (!std::regex_match(header, match, reg))
			return;
		csym.file = match.str(1);
		std::string map_str = match.str(2);
		for (auto &ch : map_str) {
			if (ch == ',') {
				if (!buff.empty()) {
					if (buff != "-")
						csym.map.push_back(std::stoull(buff) + 1);
					else
						csym.map.push_back(0);
					buff.clear();
				}
			}
			else
				buff += ch;
		}
		if (!buff.empty()) {
			if (buff != "-")
				csym.map.push_back(std::stoull(buff) + 1);
			else
				csym.map.push_back(0);
			buff.clear();
		}
		csyms.emplace(module_path, std::move(csym));
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

	void translator_type::match_grammar(const context_t &context, std::deque<token_base *> &raw)
	{
		for (auto &dat : m_data) {
			bool matched = false;
			{
				bool failed = false, skip_useless = false;
				std::size_t i = 0;
				for (auto &it : dat->first) {
					switch (it->get_type()) {
					default:
						break;
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
							failed = static_cast<token_action *>(raw[i])->get_action() !=
							         static_cast<token_action *>(it)->get_action();
						else
							failed = true;
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
				for (auto &it : dat->first) {
					switch (it->get_type()) {
					default:
						break;
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
		std::size_t method_line_num = 0, line_num = 0;
		std::deque<std::deque<token_base *>> tmp;
		stack_type<method_base *> methods;
		for (auto &it : lines) {
			std::deque<token_base *> line = it;
			line_num = static_cast<token_endline *>(line.back())->get_line_num();
			try {
				if (raw)
					context->compiler->process_line(line);
				method_base *m = this->match(line);
				switch (m->get_type()) {
				case method_types::null:
					throw compile_error("Null type of grammar.");
					break;
				case method_types::single: {
					statement_base *sptr = nullptr;
					if (!methods.empty()) {
						method_base *expected_method = nullptr;
						if (m->get_target_type() == statement_types::end_) {
							if (raw) {
								context->instance->storage.remove_set();
								domain_type domain = std::move(context->instance->storage.get_domain());
								context->instance->storage.remove_domain();
								methods.top()->postprocess(context, domain);
							}
							expected_method = methods.top();
							methods.pop();
						}
						if (methods.empty()) {
							line_num = method_line_num;
							if (m->get_target_type() == statement_types::end_)
								sptr = static_cast<method_end *>(m)->translate_end(expected_method, context, tmp,
								       line);
							else
								sptr = expected_method->translate(context, tmp);
							tmp.clear();
						}
						else {
							if (raw)
								m->preprocess(context, {line});
							tmp.push_back(line);
						}
					}
					else {
						if (m->get_target_type() == statement_types::end_)
							throw compile_error("Hanging end statement.");
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
					if (methods.empty())
						method_line_num = static_cast<token_endline *>(line.back())->get_line_num();
					methods.push(m);
					if (raw) {
						context->instance->storage.add_domain();
						context->instance->storage.add_set();
						methods.top()->preprocess(context, {line});
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
		if (!methods.empty())
			throw compile_error("Lack of the \"end\" signal.");
	}
} // namespace cs
