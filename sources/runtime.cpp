/*
* Covariant Script Runtime
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
#include <covscript/runtime.hpp>

namespace cs {
	var runtime_type::parse_add(const var &a, const var &b)
	{
		if (a.type() == typeid(number) && b.type() == typeid(number))
			return a.const_val<number>() + b.const_val<number>();
		else if (a.type() == typeid(string))
			return var::make<std::string>(a.const_val<string>() + b.to_string());
		else
			throw syntax_error("Unsupported operator operations(Add).");
	}

	var runtime_type::parse_addasi(var a, const var &b)
	{
		a.swap(parse_add(a, b), true);
		return a;
	}

	var runtime_type::parse_sub(const var &a, const var &b)
	{
		if (a.type() == typeid(number) && b.type() == typeid(number))
			return a.const_val<number>() - b.const_val<number>();
		else
			throw syntax_error("Unsupported operator operations(Sub).");
	}

	var runtime_type::parse_subasi(var a, const var &b)
	{
		a.swap(parse_sub(a, b), true);
		return a;
	}

	var runtime_type::parse_minus(const var &b)
	{
		if (b.type() == typeid(number))
			return -b.const_val<number>();
		else
			throw syntax_error("Unsupported operator operations(Minus).");
	}

	var runtime_type::parse_mul(const var &a, const var &b)
	{
		if (a.type() == typeid(number) && b.type() == typeid(number))
			return a.const_val<number>() * b.const_val<number>();
		else
			throw syntax_error("Unsupported operator operations(Mul).");
	}

	var runtime_type::parse_mulasi(var a, const var &b)
	{
		a.swap(parse_mul(a, b), true);
		return a;
	}

	var runtime_type::parse_escape(const var &b)
	{
		if (b.type() == typeid(pointer)) {
			const pointer &ptr = b.const_val<pointer>();
			if (ptr.data.usable())
				return ptr.data;
			else
				throw syntax_error("Escape from null pointer.");
		}
		else
			throw syntax_error("Unsupported operator operations(Escape).");
	}

	var runtime_type::parse_div(const var &a, const var &b)
	{
		if (a.type() == typeid(number) && b.type() == typeid(number))
			return a.const_val<number>() / b.const_val<number>();
		else
			throw syntax_error("Unsupported operator operations(Div).");
	}

	var runtime_type::parse_divasi(var a, const var &b)
	{
		a.swap(parse_div(a, b), true);
		return a;
	}

	var runtime_type::parse_mod(const var &a, const var &b)
	{
		if (a.type() == typeid(number) && b.type() == typeid(number))
			return number(std::fmod(a.const_val<number>(), b.const_val<number>()));
		else
			throw syntax_error("Unsupported operator operations(Mod).");
	}

	var runtime_type::parse_modasi(var a, const var &b)
	{
		a.swap(parse_mod(a, b), true);
		return a;
	}

	var runtime_type::parse_pow(const var &a, const var &b)
	{
		if (a.type() == typeid(number) && b.type() == typeid(number))
			return number(std::pow(a.const_val<number>(), b.const_val<number>()));
		else
			throw syntax_error("Unsupported operator operations(Pow).");
	}

	var runtime_type::parse_powasi(var a, const var &b)
	{
		a.swap(parse_pow(a, b), true);
		return a;
	}

	var runtime_type::parse_dot(const var &a, token_base *b)
	{
		if (a.type() == typeid(constant_values)) {
			switch (a.const_val<constant_values>()) {
			case constant_values::global_namespace:
				return storage.get_var_global(static_cast<token_id *>(b)->get_id());
				break;
			case constant_values::current_namespace:
				return storage.get_var_current(static_cast<token_id *>(b)->get_id());
				break;
			default:
				throw syntax_error("Unsupported operator operations(Dot).");
				break;
			}
		}
		else if (a.type() == typeid(extension_t))
			return a.val<extension_t>(true)->get_var(static_cast<token_id *>(b)->get_id());
		else if (a.type() == typeid(type))
			return a.val<type>(true).get_var(static_cast<token_id *>(b)->get_id());
		else if (a.type() == typeid(structure)) {
			var &val = a.val<structure>(true).get_var(static_cast<token_id *>(b)->get_id());
			if (val.type() == typeid(callable) && val.const_val<callable>().is_member_fn())
				return var::make_protect<object_method>(a, val);
			else
				return val;
		}
		else {
			var &val = a.get_ext()->get_var(static_cast<token_id *>(b)->get_id());
			if (val.type() == typeid(callable))
				return var::make_protect<object_method>(a, val, val.const_val<callable>().is_constant());
			else
				return val;
		}
	}

	var runtime_type::parse_arraw(const var &a, token_base *b)
	{
		if (a.type() == typeid(pointer))
			return parse_dot(a.const_val<pointer>().data, b);
		else
			throw syntax_error("Unsupported operator operations(Arraw).");
	}

	var runtime_type::parse_typeid(const var &b)
	{
		if (b.type() == typeid(type))
			return b.const_val<type>().id;
		else if (b.type() == typeid(structure))
			return b.const_val<structure>().get_hash();
		else
			return cs_impl::hash<std::string>(b.type().name());
	}

	var runtime_type::parse_new(const var &b)
	{
		if (b.type() == typeid(type))
			return b.const_val<type>().constructor();
		else
			throw syntax_error("Unsupported operator operations(New).");
	}

	var runtime_type::parse_gcnew(const var &b)
	{
		if (b.type() == typeid(type))
			return var::make<pointer>(b.const_val<type>().constructor());
		else
			throw syntax_error("Unsupported operator operations(GcNew).");
	}

	var runtime_type::parse_und(const var &a, const var &b)
	{
		if (a.type() == typeid(number) && b.type() == typeid(number))
			return boolean(a.const_val<number>() < b.const_val<number>());
		else
			throw syntax_error("Unsupported operator operations(Und).");
	}

	var runtime_type::parse_abo(const var &a, const var &b)
	{
		if (a.type() == typeid(number) && b.type() == typeid(number))
			return boolean(a.const_val<number>() > b.const_val<number>());
		else
			throw syntax_error("Unsupported operator operations(Abo).");
	}

	var runtime_type::parse_ueq(const var &a, const var &b)
	{
		if (a.type() == typeid(number) && b.type() == typeid(number))
			return boolean(a.const_val<number>() <= b.const_val<number>());
		else
			throw syntax_error("Unsupported operator operations(Ueq).");
	}

	var runtime_type::parse_aeq(const var &a, const var &b)
	{
		if (a.type() == typeid(number) && b.type() == typeid(number))
			return boolean(a.const_val<number>() >= b.const_val<number>());
		else
			throw syntax_error("Unsupported operator operations(Aeq).");
	}

	var runtime_type::parse_asi(var a, const var &b)
	{
		a.swap(copy(b), true);
		return a;
	}

	var runtime_type::parse_choice(const var &a, const cov::tree<token_base *>::iterator &b)
	{
		if (a.type() == typeid(boolean)) {
			if (a.const_val<boolean>())
				return parse_expr(b.left());
			else
				return parse_expr(b.right());
		}
		else
			throw syntax_error("Unsupported operator operations(Choice).");
	}

	var runtime_type::parse_pair(const var &a, const var &b)
	{
		if (a.type() != typeid(pair) && b.type() != typeid(pair))
			return var::make<pair>(copy(a), copy(b));
		else
			throw syntax_error("Unsupported operator operations(Pair).");
	}

	var runtime_type::parse_equ(const var &a, const var &b)
	{
		return boolean(a.compare(b));
	}

	var runtime_type::parse_neq(const var &a, const var &b)
	{
		return boolean(!a.compare(b));
	}

	var runtime_type::parse_and(const var &a, const var &b)
	{
		if (a.type() == typeid(boolean) && b.type() == typeid(boolean))
			return boolean(a.const_val<boolean>() && b.const_val<boolean>());
		else
			throw syntax_error("Unsupported operator operations(And).");
	}

	var runtime_type::parse_or(const var &a, const var &b)
	{
		if (a.type() == typeid(boolean) && b.type() == typeid(boolean))
			return boolean(a.const_val<boolean>() || b.const_val<boolean>());
		else
			throw syntax_error("Unsupported operator operations(Or).");
	}

	var runtime_type::parse_not(const var &b)
	{
		if (b.type() == typeid(boolean))
			return boolean(!b.const_val<boolean>());
		else
			throw syntax_error("Unsupported operator operations(Not).");
	}

	var runtime_type::parse_inc(var a, var b)
	{
		if (a.usable()) {
			if (b.usable())
				throw syntax_error("Unsupported operator operations(Inc).");
			else
				return a.val<number>(true)++;
		}
		else {
			if (!b.usable())
				throw syntax_error("Unsupported operator operations(Inc).");
			else
				return ++b.val<number>(true);
		}
	}

	var runtime_type::parse_dec(var a, var b)
	{
		if (a.usable()) {
			if (b.usable())
				throw syntax_error("Unsupported operator operations(Dec).");
			else
				return a.val<number>(true)--;
		}
		else {
			if (!b.usable())
				throw syntax_error("Unsupported operator operations(Dec).");
			else
				return --b.val<number>(true);
		}
	}

	var runtime_type::parse_fcall(const var &a, token_base *b)
	{
		if (a.type() == typeid(callable)) {
			vector args;
			args.reserve(static_cast<token_arglist *>(b)->get_arglist().size());
			for (auto &tree:static_cast<token_arglist *>(b)->get_arglist())
				args.push_back(parse_expr(tree.root()));
			return a.const_val<callable>().call(args);
		}
		else if (a.type() == typeid(object_method)) {
			const object_method &om = a.const_val<object_method>();
			vector args{om.object};
			args.reserve(static_cast<token_arglist *>(b)->get_arglist().size());
			for (auto &tree:static_cast<token_arglist *>(b)->get_arglist())
				args.push_back(parse_expr(tree.root()));
			return om.callable.const_val<callable>().call(args);
		}
		else
			throw syntax_error("Unsupported operator operations(Fcall).");
	}

	var runtime_type::parse_access(var a, const var &b)
	{
		if (a.type() == typeid(array)) {
			if (b.type() != typeid(number))
				throw syntax_error("Index must be a number.");
			if (b.const_val<number>() < 0)
				throw syntax_error("Index must above zero.");
			const array &carr = a.const_val<array>();
			std::size_t posit = b.const_val<number>();
			if (posit >= carr.size()) {
				array & arr = a.val<array>(true);
				for (std::size_t i = posit - arr.size() + 1; i > 0; --i)
					arr.emplace_back(number(0));
			}
			return carr.at(posit);
		}
		else if (a.type() == typeid(hash_map)) {
			const hash_map &cmap = a.const_val<hash_map>();
			if (cmap.count(b) == 0)
				a.val<hash_map>(true).emplace(copy(b), number(0));
			return cmap.at(b);
		}
		else if (a.type() == typeid(string)) {
			if (b.type() != typeid(number))
				throw syntax_error("Index must be a number.");
			return a.const_val<string>().at(b.const_val<number>());
		}
		else
			throw syntax_error("Access non-array or string object.");
	}

	var runtime_type::parse_expr(const cov::tree<token_base *>::iterator &it)
	{
		if (!it.usable())
			throw internal_error("The expression tree is not available.");
		token_base *token = it.data();
		if (token == nullptr)
			return var();
		switch (token->get_type()) {
		default:
			break;
		case token_types::id:
			return storage.get_var(static_cast<token_id *>(token)->get_id());
			break;
		case token_types::value:
			return static_cast<token_value *>(token)->get_value();
			break;
		case token_types::expr:
			return parse_expr(static_cast<token_expr *>(token)->get_tree().root());
			break;
		case token_types::array: {
			array arr;
			bool is_map = true;
			for (auto &tree:static_cast<token_array *>(token)->get_array()) {
				const var &val = parse_expr(tree.root());
				if (is_map && val.type() != typeid(pair))
					is_map = false;
				arr.push_back(copy(val));
			}
			if (arr.empty())
				is_map = false;
			if (is_map) {
				hash_map map;
				for (auto &it:arr) {
					pair &p = it.val<pair>(true);
					if (map.count(p.first) == 0)
						map.emplace(p.first, p.second);
					else
						map[p.first] = p.second;
				}
				return var::make<hash_map>(std::move(map));
			}
			else
				return var::make<array>(std::move(arr));
		}
		case token_types::signal: {
			switch (static_cast<token_signal *>(token)->get_signal()) {
			default:
				break;
			case signal_types::add_:
				return parse_add(parse_expr(it.left()), parse_expr(it.right()));
				break;
			case signal_types::addasi_:
				return parse_addasi(parse_expr(it.left()), parse_expr(it.right()));
				break;
			case signal_types::sub_:
				return parse_sub(parse_expr(it.left()), parse_expr(it.right()));
				break;
			case signal_types::subasi_:
				return parse_subasi(parse_expr(it.left()), parse_expr(it.right()));
				break;
			case signal_types::minus_:
				return parse_minus(parse_expr(it.right()));
				break;
			case signal_types::mul_:
				return parse_mul(parse_expr(it.left()), parse_expr(it.right()));
				break;
			case signal_types::mulasi_:
				return parse_mulasi(parse_expr(it.left()), parse_expr(it.right()));
				break;
			case signal_types::escape_:
				return parse_escape(parse_expr(it.right()));
				break;
			case signal_types::div_:
				return parse_div(parse_expr(it.left()), parse_expr(it.right()));
				break;
			case signal_types::divasi_:
				return parse_divasi(parse_expr(it.left()), parse_expr(it.right()));
				break;
			case signal_types::mod_:
				return parse_mod(parse_expr(it.left()), parse_expr(it.right()));
				break;
			case signal_types::modasi_:
				return parse_modasi(parse_expr(it.left()), parse_expr(it.right()));
				break;
			case signal_types::pow_:
				return parse_pow(parse_expr(it.left()), parse_expr(it.right()));
				break;
			case signal_types::powasi_:
				return parse_powasi(parse_expr(it.left()), parse_expr(it.right()));
				break;
			case signal_types::dot_:
				return parse_dot(parse_expr(it.left()), it.right().data());
				break;
			case signal_types::arrow_:
				return parse_arraw(parse_expr(it.left()), it.right().data());
				break;
			case signal_types::typeid_:
				return parse_typeid(parse_expr(it.right()));
				break;
			case signal_types::new_:
				return parse_new(parse_expr(it.right()));
				break;
			case signal_types::gcnew_:
				return parse_gcnew(parse_expr(it.right()));
				break;
			case signal_types::und_:
				return parse_und(parse_expr(it.left()), parse_expr(it.right()));
				break;
			case signal_types::abo_:
				return parse_abo(parse_expr(it.left()), parse_expr(it.right()));
				break;
			case signal_types::asi_:
				return parse_asi(parse_expr(it.left()), parse_expr(it.right()));
				break;
			case signal_types::choice_:
				return parse_choice(parse_expr(it.left()), it.right());
				break;
			case signal_types::pair_:
				return parse_pair(parse_expr(it.left()), parse_expr(it.right()));
				break;
			case signal_types::equ_:
				return parse_equ(parse_expr(it.left()), parse_expr(it.right()));
				break;
			case signal_types::ueq_:
				return parse_ueq(parse_expr(it.left()), parse_expr(it.right()));
				break;
			case signal_types::aeq_:
				return parse_aeq(parse_expr(it.left()), parse_expr(it.right()));
				break;
			case signal_types::neq_:
				return parse_neq(parse_expr(it.left()), parse_expr(it.right()));
				break;
			case signal_types::and_:
				return parse_and(parse_expr(it.left()), parse_expr(it.right()));
				break;
			case signal_types::or_:
				return parse_or(parse_expr(it.left()), parse_expr(it.right()));
				break;
			case signal_types::not_:
				return parse_not(parse_expr(it.right()));
				break;
			case signal_types::inc_:
				return parse_inc(parse_expr(it.left()), parse_expr(it.right()));
				break;
			case signal_types::dec_:
				return parse_dec(parse_expr(it.left()), parse_expr(it.right()));
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
}
