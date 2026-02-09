/*
 * Covariant Script Runtime
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
#include <covscript/impl/runtime.hpp>

namespace cs {
	var runtime_type::parse_add(const var &a, const var &b)
	{
		return a + b;
	}

	var runtime_type::parse_addasi(var a, const var &b)
	{
		a.swap(parse_add(a, b), true);
		return a;
	}

	var runtime_type::parse_sub(const var &a, const var &b)
	{
		return a - b;
	}

	var runtime_type::parse_subasi(var a, const var &b)
	{
		a.swap(parse_sub(a, b), true);
		return a;
	}

	var runtime_type::parse_minus(const var &b)
	{
		return -b;
	}

	var runtime_type::parse_mul(const var &a, const var &b)
	{
		return a * b;
	}

	var runtime_type::parse_mulasi(var a, const var &b)
	{
		a.swap(parse_mul(a, b), true);
		return a;
	}

	var runtime_type::parse_escape(const var &b)
	{
		return *b;
	}

	var runtime_type::parse_div(const var &a, const var &b)
	{
		return a / b;
	}

	var runtime_type::parse_divasi(var a, const var &b)
	{
		a.swap(parse_div(a, b), true);
		return a;
	}

	var runtime_type::parse_mod(const var &a, const var &b)
	{
		return a % b;
	}

	var runtime_type::parse_modasi(var a, const var &b)
	{
		a.swap(parse_mod(a, b), true);
		return a;
	}

	var runtime_type::parse_pow(const var &a, const var &b)
	{
		return a ^ b;
	}

	var runtime_type::parse_powasi(var a, const var &b)
	{
		a.swap(parse_pow(a, b), true);
		return a;
	}

	var &runtime_type::parse_dot_lhs(const var &a, token_base *b)
	{
		if (a.is_type_of<constant_values>()) {
			switch (a.const_val<constant_values>()) {
			case constant_values::global_namespace:
				return storage.get_var_global(static_cast<token_id *>(b)->get_id());
			case constant_values::local_namepace:
				return storage.get_var_current(static_cast<token_id *>(b)->get_id());
			default:
				throw runtime_error("Unknown scope tag.");
			}
		}
		else if (a.is_type_of<namespace_t>())
			return a.val<namespace_t>()->get_var(static_cast<token_id *>(b)->get_id());
		else if (a.is_type_of<type_t>())
			return a.const_val<type_t>().get_var(static_cast<token_id *>(b)->get_id());
		else if (a.is_type_of<structure>()) {
			var &val = a.val<structure>().get_var(static_cast<token_id *>(b)->get_id());
			if (!val.is_type_of<callable>() || !val.const_val<callable>().is_member_fn())
				return val;
			else
				throw runtime_error("Can not visit member function as lvalue.");
		}
		else {
			try {
				return a.get_ext()->get_var(static_cast<token_id *>(b)->get_id());
			}
			catch (...) {
				return a.access_ref(static_cast<token_id *>(b)->get_id());
			}
		}
	}

	var runtime_type::parse_dot(const var &a, token_base *b)
	{
		if (a.is_type_of<constant_values>()) {
			switch (a.const_val<constant_values>()) {
			case constant_values::global_namespace:
				return storage.get_var_global(static_cast<token_id *>(b)->get_id());
			case constant_values::local_namepace:
				return storage.get_var_current(static_cast<token_id *>(b)->get_id());
			default:
				throw runtime_error("Unknown scope tag.");
			}
		}
		else if (a.is_type_of<namespace_t>())
			return a.val<namespace_t>()->get_var(static_cast<token_id *>(b)->get_id());
		else if (a.is_type_of<type_t>())
			return a.const_val<type_t>().get_var(static_cast<token_id *>(b)->get_id());
		else if (a.is_type_of<structure>()) {
			var &val = a.val<structure>().get_var(static_cast<token_id *>(b)->get_id());
			if (val.is_type_of<callable>() && val.const_val<callable>().is_member_fn())
				return var::make_protect<object_method>(a, val);
			else
				return val;
		}
		else {
			try {
				var &val = a.get_ext()->get_var(static_cast<token_id *>(b)->get_id());
				if (val.is_type_of<callable>()) {
					const callable &func = val.const_val<callable>();
					switch (func.type()) {
					case callable::types::member_visitor: {
						vector args{a};
						return func.call(args);
					}
					case callable::types::force_regular:
						return val;
					default:
						return var::make_protect<object_method>(a, val, func.is_request_fold());
					}
				}
				else
					return val;
			}
			catch (...) {
				return a.access(static_cast<token_id *>(b)->get_id());
			}
		}
	}

	var runtime_type::parse_arrow(const var &a, token_base *b)
	{
		return parse_dot(*a, b);
	}

	var runtime_type::parse_typeid(const var &b)
	{
		if (b.is_type_of<type_t>())
			return b.const_val<type_t>().id;
		else if (b.is_type_of<structure>())
			return b.const_val<structure>().get_id();
		else
			return var::make<type_id>(b.type());
	}

	var runtime_type::parse_new(const var &b)
	{
		if (b.is_type_of<type_t>())
			return b.const_val<type_t>().constructor();
		else
			throw runtime_error("Unsupported operator operations(New).");
	}

	var runtime_type::parse_gcnew(const var &b)
	{
		if (b.is_type_of<type_t>())
			return var::make<pointer>(b.const_val<type_t>().constructor());
		else
			throw runtime_error("Unsupported operator operations(GcNew).");
	}

	var runtime_type::parse_und(const var &a, const var &b)
	{
		return a < b;
	}

	var runtime_type::parse_abo(const var &a, const var &b)
	{
		return a > b;
	}

	var runtime_type::parse_ueq(const var &a, const var &b)
	{
		return a <= b;
	}

	var runtime_type::parse_aeq(const var &a, const var &b)
	{
		return a >= b;
	}

	var runtime_type::parse_asi(var a, const var &b)
	{
		a.swap(copy(b), true);
		return a;
	}

	var runtime_type::parse_lnkasi(var &a, const var &b)
	{
		a = b;
		return a;
	}

	var runtime_type::parse_lnkasi(tree_type<token_base *>::iterator a, const var &b)
	{
		token_base *token = a.data();
		if (token->get_type() == token_types::id)
			return parse_lnkasi(storage.get_var(static_cast<token_id *>(token)->get_id()), b);
		else if (token->get_type() == token_types::signal) {
			switch (static_cast<token_signal *>(token)->get_signal()) {
			case signal_types::dot_:
				return parse_lnkasi(parse_dot_lhs(parse_expr(a.left()), a.right().data()), b);
			case signal_types::access_:
				return parse_lnkasi(parse_access_lhs(parse_expr(a.left()), parse_expr(a.right())), b);
			default:
				throw runtime_error("Unexpected left operand in link assign expression.");
			}
		}
		else
			throw runtime_error("Unexpected left operand in link assign expression.");
	}

	var runtime_type::parse_bind(token_base *a, const var &b)
	{
		if (!b.is_type_of<array>())
			throw runtime_error("Only support structured binding with array.");
		auto &pl = static_cast<token_parallel *>(a)->get_parallel();
		auto &arr = b.const_val<array>();
		if (pl.size() != arr.size())
			throw runtime_error("Unmatched structured binding.");
		for (std::size_t i = 0; i < pl.size(); ++i) {
			if (pl[i].root().data()->get_type() == token_types::parallel)
				parse_bind(pl[i].root().data(), arr[i]);
			else
				parse_asi(parse_expr(pl[i].root()), arr[i]);
		}
		return b;
	}

	var runtime_type::parse_choice(const var &a, const tree_type<token_base *>::iterator &b)
	{
		if (a.is_type_of<boolean>()) {
			if (a.const_val<boolean>())
				return parse_expr(b.left());
			else
				return parse_expr(b.right());
		}
		else
			throw runtime_error("Unsupported operator operations(Choice).");
	}

	var runtime_type::parse_pair(const var &a, const var &b)
	{
		if (!a.is_type_of<pair>() && !b.is_type_of<pair>())
			return var::make<pair>(copy(a), copy(b));
		else
			throw runtime_error("Unsupported operator operations(Pair).");
	}

	var runtime_type::parse_equ(const var &a, const var &b)
	{
		return boolean(a.compare(b));
	}

	var runtime_type::parse_neq(const var &a, const var &b)
	{
		return boolean(!a.compare(b));
	}

	var runtime_type::parse_and(const tree_type<token_base *>::iterator &a, const tree_type<token_base *>::iterator &b)
	{
		return var::make<boolean>(parse_expr(a).const_val<boolean>() && parse_expr(b).const_val<boolean>());
	}

	var runtime_type::parse_or(const tree_type<token_base *>::iterator &a, const tree_type<token_base *>::iterator &b)
	{
		return var::make<boolean>(parse_expr(a).const_val<boolean>() || parse_expr(b).const_val<boolean>());
	}

	var runtime_type::parse_not(const var &b)
	{
		if (b.is_type_of<boolean>())
			return boolean(!b.const_val<boolean>());
		else
			throw runtime_error("Unsupported operator operations(Not).");
	}

	var runtime_type::parse_inc(const var &a, const var &b)
	{
		if (a.usable()) {
			if (b.usable())
				throw runtime_error("Unsupported operator operations(Inc).");
			else if (a.is_type_of<numeric>())
				return a.val<numeric>()++;
			else {
				var oldt = copy(a);
				++a;
				return oldt;
			}
		}
		else {
			if (!b.usable())
				throw runtime_error("Unsupported operator operations(Inc).");
			else
				return ++b;
		}
	}

	var runtime_type::parse_dec(const var &a, const var &b)
	{
		if (a.usable()) {
			if (b.usable())
				throw runtime_error("Unsupported operator operations(Dec).");
			else if (a.is_type_of<numeric>())
				return a.val<numeric>()--;
			else {
				var oldt = copy(a);
				--a;
				return oldt;
			}
		}
		else {
			if (!b.usable())
				throw runtime_error("Unsupported operator operations(Dec).");
			else
				return --b;
		}
	}

	var runtime_type::parse_addr(const var &b)
	{
		return var::make<pointer>(b);
	}

	var runtime_type::parse_fcall(const var &a, token_base *b)
	{
		vector args;
		token_base *ptr = nullptr;
		args.reserve(static_cast<token_arglist *>(b)->get_arglist().size());
		a.prep_call(args);
		for (auto &tree : static_cast<token_arglist *>(b)->get_arglist()) {
			ptr = tree.root().data();
			if (ptr != nullptr && ptr->get_type() == token_types::expand) {
				var val = parse_expr(static_cast<token_expand *>(ptr)->get_tree().root());
				const auto &arr = val.const_val<array>();
				for (auto &it : arr)
					args.push_back(lvalue(it));
			}
			else
				args.push_back(lvalue(parse_expr(tree.root())));
		}
		return a.fcall(args);
	}

	var &runtime_type::parse_access_lhs(const var &a, const var &b)
	{
		return a.index_ref(b);
	}

	var runtime_type::parse_access(const var &a, const var &b)
	{
		return a.index(b);
	}

	var runtime_type::parse_expr(const tree_type<token_base *>::iterator &it, bool disable_parallel)
	{
		if (!it.usable())
			throw internal_error("The expression tree is not available.");
		token_base *token = it.data();
		if (token == nullptr)
			return var();
		switch (token->get_type()) {
		default:
			break;
		case token_types::vargs:
			throw runtime_error("Wrong declaration of variable argument list.");
			break;
		case token_types::expand:
			throw runtime_error("Wrong expanding position.");
			break;
		case token_types::id:
			return storage.get_var(static_cast<token_id *>(token)->get_id());
			break;
		case token_types::literal: {
			token_literal *ptr = static_cast<token_literal *>(token);
			return get_string_literal(ptr->get_data(), ptr->get_literal());
		}
		case token_types::value:
			return static_cast<token_value *>(token)->get_value();
			break;
		case token_types::expr:
			return parse_expr(static_cast<token_expr *>(token)->get_tree().root());
			break;
		case token_types::array: {
			array arr;
			token_base *ptr = nullptr;
			for (auto &tree : static_cast<token_array *>(token)->get_array()) {
				ptr = tree.root().data();
				if (ptr != nullptr && ptr->get_type() == token_types::expand) {
					var val = parse_expr(static_cast<token_expand *>(ptr)->get_tree().root());
					const auto &child_arr = val.const_val<array>();
					for (auto &it : child_arr)
						arr.push_back(copy(it));
				}
				else
					arr.push_back(copy(parse_expr(tree.root())));
			}
			return rvalue(var::make<array>(std::move(arr)));
		}
		case token_types::parallel: {
			if (disable_parallel)
				throw runtime_error("Do not allowed parallel list.");
			var result;
			for (auto &tree : static_cast<token_parallel *>(token)->get_parallel())
				result = parse_expr(tree.root());
			return result;
		}
		case token_types::signal: {
			switch (static_cast<token_signal *>(token)->get_signal()) {
			default:
				break;
			case signal_types::add_:
				return rvalue(parse_add(parse_expr(it.left()), parse_expr(it.right())));
				break;
			case signal_types::addasi_:
				return parse_addasi(parse_expr(it.left()), parse_expr(it.right()));
				break;
			case signal_types::sub_:
				return rvalue(parse_sub(parse_expr(it.left()), parse_expr(it.right())));
				break;
			case signal_types::subasi_:
				return parse_subasi(parse_expr(it.left()), parse_expr(it.right()));
				break;
			case signal_types::minus_:
				return rvalue(parse_minus(parse_expr(it.right())));
				break;
			case signal_types::mul_:
				return rvalue(parse_mul(parse_expr(it.left()), parse_expr(it.right())));
				break;
			case signal_types::mulasi_:
				return parse_mulasi(parse_expr(it.left()), parse_expr(it.right()));
				break;
			case signal_types::escape_:
				return parse_escape(parse_expr(it.right()));
				break;
			case signal_types::div_:
				return rvalue(parse_div(parse_expr(it.left()), parse_expr(it.right())));
				break;
			case signal_types::divasi_:
				return parse_divasi(parse_expr(it.left()), parse_expr(it.right()));
				break;
			case signal_types::mod_:
				return rvalue(parse_mod(parse_expr(it.left()), parse_expr(it.right())));
				break;
			case signal_types::modasi_:
				return parse_modasi(parse_expr(it.left()), parse_expr(it.right()));
				break;
			case signal_types::pow_:
				return rvalue(parse_pow(parse_expr(it.left()), parse_expr(it.right())));
				break;
			case signal_types::powasi_:
				return parse_powasi(parse_expr(it.left()), parse_expr(it.right()));
				break;
			case signal_types::dot_:
				return parse_dot(parse_expr(it.left()), it.right().data());
				break;
			case signal_types::arrow_:
				return parse_arrow(parse_expr(it.left()), it.right().data());
				break;
			case signal_types::typeid_:
				return rvalue(parse_typeid(parse_expr(it.right())));
				break;
			case signal_types::new_:
				return rvalue(parse_new(parse_expr(it.right())));
				break;
			case signal_types::gcnew_:
				return rvalue(parse_gcnew(parse_expr(it.right())));
				break;
			case signal_types::und_:
				return rvalue(parse_und(parse_expr(it.left()), parse_expr(it.right())));
				break;
			case signal_types::abo_:
				return rvalue(parse_abo(parse_expr(it.left()), parse_expr(it.right())));
				break;
			case signal_types::asi_:
				return parse_asi(parse_expr(it.left()), parse_expr(it.right()));
				break;
			case signal_types::lnkasi_:
				return parse_lnkasi(it.left(), parse_expr(it.right()));
				break;
			case signal_types::bind_:
				return parse_bind(it.left().data(), parse_expr(it.right()));
				break;
			case signal_types::choice_:
				return parse_choice(parse_expr(it.left()), it.right());
				break;
			case signal_types::pair_:
				return rvalue(parse_pair(parse_expr(it.left()), parse_expr(it.right())));
				break;
			case signal_types::equ_:
				return rvalue(parse_equ(parse_expr(it.left()), parse_expr(it.right())));
				break;
			case signal_types::ueq_:
				return rvalue(parse_ueq(parse_expr(it.left()), parse_expr(it.right())));
				break;
			case signal_types::aeq_:
				return rvalue(parse_aeq(parse_expr(it.left()), parse_expr(it.right())));
				break;
			case signal_types::neq_:
				return rvalue(parse_neq(parse_expr(it.left()), parse_expr(it.right())));
				break;
			case signal_types::and_:
				return rvalue(parse_and(it.left(), it.right()));
				break;
			case signal_types::or_:
				return rvalue(parse_or(it.left(), it.right()));
				break;
			case signal_types::not_:
				return rvalue(parse_not(parse_expr(it.right())));
				break;
			case signal_types::inc_:
				return parse_inc(parse_expr(it.left()), parse_expr(it.right()));
				break;
			case signal_types::dec_:
				return parse_dec(parse_expr(it.left()), parse_expr(it.right()));
				break;
			case signal_types::addr_:
				return rvalue(parse_addr(parse_expr(it.right())));
				break;
			case signal_types::fcall_:
				return parse_fcall(parse_expr(it.left()), it.right().data());
				break;
			case signal_types::access_:
				return parse_access(parse_expr(it.left()), parse_expr(it.right()));
				break;
			}
		}
		}
		throw internal_error("Unrecognized expression.");
	}
} // namespace cs
