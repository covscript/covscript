/*
* Covariant Script Runtime
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
#include <covscript/impl/runtime.hpp>

namespace cs {
	var runtime_type::parse_add(const var &a, const var &b)
	{
		if (a.type() == typeid(number) && b.type() == typeid(number))
			return a.const_val<number>() + b.const_val<number>();
		else if (a.type() == typeid(string))
			return var::make<std::string>(a.const_val<string>() + b.to_string());
		else
			throw runtime_error("Unsupported operator operations(Add).");
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
			throw runtime_error("Unsupported operator operations(Sub).");
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
			throw runtime_error("Unsupported operator operations(Minus).");
	}

	var runtime_type::parse_mul(const var &a, const var &b)
	{
		if (a.type() == typeid(number) && b.type() == typeid(number))
			return a.const_val<number>() * b.const_val<number>();
		else
			throw runtime_error("Unsupported operator operations(Mul).");
	}

	var runtime_type::parse_mulasi(var a, const var &b)
	{
		a.swap(parse_mul(a, b), true);
		return a;
	}

	var runtime_type::parse_escape(const var &b)
	{
		if (b.type() == typeid(pointer)) {
			const auto &ptr = b.const_val<pointer>();
			if (ptr.data.usable())
				return ptr.data;
			else
				throw runtime_error("Escape from null pointer.");
		}
		else
			throw runtime_error("Unsupported operator operations(Escape).");
	}

	var runtime_type::parse_div(const var &a, const var &b)
	{
		if (a.type() == typeid(number) && b.type() == typeid(number))
			return a.const_val<number>() / b.const_val<number>();
		else
			throw runtime_error("Unsupported operator operations(Div).");
	}

	var runtime_type::parse_divasi(var a, const var &b)
	{
		a.swap(parse_div(a, b), true);
		return a;
	}

	var runtime_type::parse_mod(const var &a, const var &b)
	{
		if (a.type() == typeid(number) && b.type() == typeid(number))
			return std::fmod(a.const_val<number>(), b.const_val<number>());
		else
			throw runtime_error("Unsupported operator operations(Mod).");
	}

	var runtime_type::parse_modasi(var a, const var &b)
	{
		a.swap(parse_mod(a, b), true);
		return a;
	}

	var runtime_type::parse_pow(const var &a, const var &b)
	{
		if (a.type() == typeid(number) && b.type() == typeid(number))
			return std::pow(a.const_val<number>(), b.const_val<number>());
		else
			throw runtime_error("Unsupported operator operations(Pow).");
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
			case constant_values::local_namepace:
				return storage.get_var_current(static_cast<token_id *>(b)->get_id());
				break;
			default:
				throw runtime_error("Unsupported operator operations(Dot).");
				break;
			}
		}
		else if (a.type() == typeid(namespace_t))
			return a.val<namespace_t>(true)->get_var(static_cast<token_id *>(b)->get_id());
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

	var runtime_type::parse_arrow(const var &a, token_base *b)
	{
		if (a.type() == typeid(pointer))
			return parse_dot(a.const_val<pointer>().data, b);
		else
			throw runtime_error("Unsupported operator operations(Arraw).");
	}

	var runtime_type::parse_typeid(const var &b)
	{
		if (b.type() == typeid(type))
			return b.const_val<type>().id;
		else if (b.type() == typeid(structure))
			return b.const_val<structure>().get_id();
		else
			return var::make<type_id>(b.type());
	}

	var runtime_type::parse_new(const var &b)
	{
		if (b.type() == typeid(type))
			return b.const_val<type>().constructor();
		else
			throw runtime_error("Unsupported operator operations(New).");
	}

	var runtime_type::parse_gcnew(const var &b)
	{
		if (b.type() == typeid(type))
			return var::make<pointer>(b.const_val<type>().constructor());
		else
			throw runtime_error("Unsupported operator operations(GcNew).");
	}

	var runtime_type::parse_und(const var &a, const var &b)
	{
		if (a.type() == typeid(number) && b.type() == typeid(number))
			return boolean(a.const_val<number>() < b.const_val<number>());
		else
			throw runtime_error("Unsupported operator operations(Und).");
	}

	var runtime_type::parse_abo(const var &a, const var &b)
	{
		if (a.type() == typeid(number) && b.type() == typeid(number))
			return boolean(a.const_val<number>() > b.const_val<number>());
		else
			throw runtime_error("Unsupported operator operations(Abo).");
	}

	var runtime_type::parse_ueq(const var &a, const var &b)
	{
		if (a.type() == typeid(number) && b.type() == typeid(number))
			return boolean(a.const_val<number>() <= b.const_val<number>());
		else
			throw runtime_error("Unsupported operator operations(Ueq).");
	}

	var runtime_type::parse_aeq(const var &a, const var &b)
	{
		if (a.type() == typeid(number) && b.type() == typeid(number))
			return boolean(a.const_val<number>() >= b.const_val<number>());
		else
			throw runtime_error("Unsupported operator operations(Aeq).");
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
			throw runtime_error("Unsupported operator operations(Choice).");
	}

	var runtime_type::parse_pair(const var &a, const var &b)
	{
		if (a.type() != typeid(pair) && b.type() != typeid(pair))
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

	var runtime_type::parse_and(const var &a, const var &b)
	{
		if (a.type() == typeid(boolean) && b.type() == typeid(boolean))
			return boolean(a.const_val<boolean>() && b.const_val<boolean>());
		else
			throw runtime_error("Unsupported operator operations(And).");
	}

	var runtime_type::parse_or(const var &a, const var &b)
	{
		if (a.type() == typeid(boolean) && b.type() == typeid(boolean))
			return boolean(a.const_val<boolean>() || b.const_val<boolean>());
		else
			throw runtime_error("Unsupported operator operations(Or).");
	}

	var runtime_type::parse_not(const var &b)
	{
		if (b.type() == typeid(boolean))
			return boolean(!b.const_val<boolean>());
		else
			throw runtime_error("Unsupported operator operations(Not).");
	}

	var runtime_type::parse_inc(var a, var b)
	{
		if (a.usable()) {
			if (b.usable())
				throw runtime_error("Unsupported operator operations(Inc).");
			else
				return a.val<number>(true)++;
		}
		else {
			if (!b.usable())
				throw runtime_error("Unsupported operator operations(Inc).");
			else
				return ++b.val<number>(true);
		}
	}

	var runtime_type::parse_dec(var a, var b)
	{
		if (a.usable()) {
			if (b.usable())
				throw runtime_error("Unsupported operator operations(Dec).");
			else
				return a.val<number>(true)--;
		}
		else {
			if (!b.usable())
				throw runtime_error("Unsupported operator operations(Dec).");
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
				args.push_back(lvalue(parse_expr(tree.root())));
			return a.const_val<callable>().call(args);
		}
		else if (a.type() == typeid(object_method)) {
			const auto &om = a.const_val<object_method>();
			vector args{om.object};
			args.reserve(static_cast<token_arglist *>(b)->get_arglist().size());
			for (auto &tree:static_cast<token_arglist *>(b)->get_arglist())
				args.push_back(lvalue(parse_expr(tree.root())));
			return om.callable.const_val<callable>().call(args);
		}
		else
			throw runtime_error("Unsupported operator operations(Fcall).");
	}

	var runtime_type::parse_access(var a, const var &b)
	{
		if (a.type() == typeid(array)) {
			if (b.type() != typeid(number))
				throw runtime_error("Index must be a number.");
			const auto &carr = a.const_val<array>();
			std::size_t posit = 0;
			if (b.const_val<number>() >= 0) {
				posit = b.const_val<number>();
				if (posit >= carr.size()) {
					auto &arr = a.val<array>(true);
					for (std::size_t i = posit - arr.size() + 1; i > 0; --i)
						arr.emplace_back(number(0));
				}
			}
			else {
				if (-b.const_val<number>() > carr.size())
					throw runtime_error("Out of range.");
				posit = carr.size() + b.const_val<number>();
			}
			return carr[posit];
		}
		else if (a.type() == typeid(hash_map)) {
			const auto &cmap = a.const_val<hash_map>();
			if (cmap.count(b) == 0)
				a.val<hash_map>(true).emplace(copy(b), number(0));
			return cmap.at(b);
		}
		else if (a.type() == typeid(string)) {
			if (b.type() != typeid(number))
				throw runtime_error("Index must be a number.");
			const auto &cstr = a.const_val<string>();
			if (b.const_val<number>() >= 0)
				return cstr[b.const_val<number>()];
			else
				return cstr[cstr.size() + b.const_val<number>()];
		}
		else
			throw runtime_error("Access non-array or string object.");
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
			array
			arr;
			for (auto &tree:static_cast<token_array *>(token)->get_array())
				arr.push_back(copy(parse_expr(tree.root())));
			return rvalue(var::make<array>(std::move(arr)));
		}
		case token_types::parallel: {
			var result;
			for (auto &tree:static_cast<token_parallel *>(token)->get_parallel())
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
				return rvalue(parse_and(parse_expr(it.left()), parse_expr(it.right())));
				break;
			case signal_types::or_:
				return rvalue(parse_or(parse_expr(it.left()), parse_expr(it.right())));
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

	void instruction_executor::gen_instruction(const cov::tree<token_base *>::iterator &it,
	        std::vector<instruction_base *> &assembly)
	{
		if (!it.usable())
			throw internal_error("The expression tree is not available.");
		token_base *token = it.data();
		if (token == nullptr) {
			assembly.push_back(new instruction_push(var(), runtime));
			return;
		}
		switch (token->get_type()) {
		default:
			break;
		case token_types::id:
			assembly.push_back(new instruction_id(static_cast<token_id *>(token)->get_id(), runtime));
			break;
		case token_types::value:
			assembly.push_back(new instruction_push(static_cast<token_value *>(token)->get_value(), runtime));
			break;
		case token_types::expr:
			gen_instruction(static_cast<token_expr *>(token)->get_tree().root(), assembly);
			break;
		case token_types::array: {
			auto &arr = static_cast<token_array *>(token)->get_array();
			for (auto it = arr.rbegin(); it != arr.rend(); ++it)
				gen_instruction(it->root(), assembly);
			assembly.push_back(new instruction_array(arr.size(), runtime));
			break;
		}
		case token_types::parallel: {
			var result;
			for (auto &tree:static_cast<token_parallel *>(token)->get_parallel()) {
				gen_instruction(tree.root(), assembly);
				assembly.push_back(new instruction_pop(runtime));
			}
			break;
		}
		case token_types::signal: {
			switch (static_cast<token_signal *>(token)->get_signal()) {
			default:
				gen_instruction(it.right(), assembly);
				gen_instruction(it.left(), assembly);
				assembly.push_back(
				    new instruction_signal(static_cast<token_signal *>(token)->get_signal(), runtime));
				break;
			case signal_types::minus_:
				gen_instruction(it.right(), assembly);
				assembly.push_back(
				    new instruction_signal(static_cast<token_signal *>(token)->get_signal(), runtime));
				break;
			case signal_types::escape_:
				gen_instruction(it.right(), assembly);
				assembly.push_back(
				    new instruction_signal(static_cast<token_signal *>(token)->get_signal(), runtime));
				break;
			case signal_types::typeid_:
				gen_instruction(it.right(), assembly);
				assembly.push_back(
				    new instruction_signal(static_cast<token_signal *>(token)->get_signal(), runtime));
				break;
			case signal_types::new_:
				gen_instruction(it.right(), assembly);
				assembly.push_back(
				    new instruction_signal(static_cast<token_signal *>(token)->get_signal(), runtime));
				break;
			case signal_types::gcnew_:
				gen_instruction(it.right(), assembly);
				assembly.push_back(
				    new instruction_signal(static_cast<token_signal *>(token)->get_signal(), runtime));
				break;
			case signal_types::not_:
				gen_instruction(it.right(), assembly);
				assembly.push_back(
				    new instruction_signal(static_cast<token_signal *>(token)->get_signal(), runtime));
				break;
			case signal_types::dot_:
				gen_instruction(it.left(), assembly);
				assembly.push_back(new instruction_sig_dot(it.right().data(), runtime));
				break;
			case signal_types::arrow_:
				gen_instruction(it.left(), assembly);
				assembly.push_back(new instruction_sig_arrow(it.right().data(), runtime));
				break;
			case signal_types::choice_: {
				std::vector<instruction_base *> assembly_true, assembly_false;
				gen_instruction(it.right().right(), assembly_false);
				gen_instruction(it.right().left(), assembly_true);
				gen_instruction(it.left(), assembly);
				assembly.push_back(new instruction_sig_choice(assembly_true, assembly_false, runtime));
				break;
			}
			case signal_types::fcall_: {
				auto &arglist = static_cast<token_arglist *>(it.right().data())->get_arglist();
				for (auto it = arglist.rbegin(); it != arglist.rend(); ++it)
					gen_instruction(it->root(), assembly);
				gen_instruction(it.left(), assembly);
				assembly.push_back(new instruction_sig_fcall(arglist.size(), runtime));
				break;
			}
			}
			break;
		}
		}
	}

	void instruction_id::exec()
	{
		runtime->stack.push(runtime->storage.get_var(m_id));
	}

	void instruction_array::exec()
	{
		array arr;
		for (std::size_t i = 0; i < m_size; ++i)
			arr.push_back(copy(runtime->stack.pop()));
		runtime->stack.push(rvalue(var::make<array>(std::move(arr))));
	}

	void instruction_signal::exec()
	{
		switch (m_signal) {
		default:
			break;
		case signal_types::add_: {
			var left(runtime->stack.pop()), right(runtime->stack.pop());
			runtime->stack.push(rvalue(runtime->parse_add(left, right)));
			break;
		}
		case signal_types::addasi_: {
            var left(runtime->stack.pop()), right(runtime->stack.pop());
			runtime->stack.push(runtime->parse_addasi(left, right));
			break;
		}
		case signal_types::sub_: {
            var left(runtime->stack.pop()), right(runtime->stack.pop());
			return runtime->stack.push(rvalue(runtime->parse_sub(left, right)));
			break;
		}
		case signal_types::subasi_: {
            var left(runtime->stack.pop()), right(runtime->stack.pop());
			runtime->stack.push(runtime->parse_subasi(left, right));
			break;
		}
		case signal_types::minus_:
			runtime->stack.push(rvalue(runtime->parse_minus(runtime->stack.pop())));
			break;
		case signal_types::mul_: {
            var left(runtime->stack.pop()), right(runtime->stack.pop());
			runtime->stack.push(rvalue(runtime->parse_mul(left, right)));
			break;
		}
		case signal_types::mulasi_: {
            var left(runtime->stack.pop()), right(runtime->stack.pop());
			runtime->stack.push(runtime->parse_mulasi(left, right));
			break;
		}
		case signal_types::escape_:
			runtime->stack.push(runtime->parse_escape(runtime->stack.pop()));
			break;
		case signal_types::div_: {
            var left(runtime->stack.pop()), right(runtime->stack.pop());
			runtime->stack.push(rvalue(runtime->parse_div(left, right)));
			break;
		}
		case signal_types::divasi_: {
            var left(runtime->stack.pop()), right(runtime->stack.pop());
			runtime->stack.push(runtime->parse_divasi(left, right));
			break;
		}
		case signal_types::mod_: {
            var left(runtime->stack.pop()), right(runtime->stack.pop());
			runtime->stack.push(rvalue(runtime->parse_mod(left, right)));
			break;
		}
		case signal_types::modasi_: {
            var left(runtime->stack.pop()), right(runtime->stack.pop());
			runtime->stack.push(runtime->parse_modasi(left, right));
			break;
		}
		case signal_types::pow_: {
            var left(runtime->stack.pop()), right(runtime->stack.pop());
			runtime->stack.push(rvalue(runtime->parse_pow(left, right)));
			break;
		}
		case signal_types::powasi_: {
            var left(runtime->stack.pop()), right(runtime->stack.pop());
			runtime->stack.push(runtime->parse_powasi(left, right));
			break;
		}
		case signal_types::typeid_:
			runtime->stack.push(rvalue(runtime->parse_typeid(runtime->stack.pop())));
			break;
		case signal_types::new_:
			runtime->stack.push(rvalue(runtime->parse_new(runtime->stack.pop())));
			break;
		case signal_types::gcnew_:
			runtime->stack.push(rvalue(runtime->parse_gcnew(runtime->stack.pop())));
			break;
		case signal_types::und_: {
            var left(runtime->stack.pop()), right(runtime->stack.pop());
			runtime->stack.push(rvalue(runtime->parse_und(left, right)));
			break;
		}
		case signal_types::abo_: {
            var left(runtime->stack.pop()), right(runtime->stack.pop());
			runtime->stack.push(rvalue(runtime->parse_abo(left, right)));
			break;
		}
		case signal_types::asi_: {
            var left(runtime->stack.pop()), right(runtime->stack.pop());
			runtime->stack.push(runtime->parse_asi(left, right));
			break;
		}
		case signal_types::pair_: {
            var left(runtime->stack.pop()), right(runtime->stack.pop());
			runtime->stack.push(rvalue(runtime->parse_pair(left, right)));
			break;
		}
		case signal_types::equ_: {
            var left(runtime->stack.pop()), right(runtime->stack.pop());
			runtime->stack.push(rvalue(runtime->parse_equ(left, right)));
			break;
		}
		case signal_types::ueq_: {
            var left(runtime->stack.pop()), right(runtime->stack.pop());
			runtime->stack.push(rvalue(runtime->parse_ueq(left, right)));
			break;
		}
		case signal_types::aeq_: {
            var left(runtime->stack.pop()), right(runtime->stack.pop());
			runtime->stack.push(rvalue(runtime->parse_aeq(left, right)));
			break;
		}
		case signal_types::neq_: {
            var left(runtime->stack.pop()), right(runtime->stack.pop());
			runtime->stack.push(rvalue(runtime->parse_neq(left, right)));
			break;
		}
		case signal_types::and_: {
            var left(runtime->stack.pop()), right(runtime->stack.pop());
			runtime->stack.push(rvalue(runtime->parse_and(left, right)));
			break;
		}
		case signal_types::or_: {
            var left(runtime->stack.pop()), right(runtime->stack.pop());
			runtime->stack.push(rvalue(runtime->parse_or(left, right)));
			break;
		}
		case signal_types::not_:
			runtime->stack.push(rvalue(runtime->parse_not(runtime->stack.pop())));
			break;
		case signal_types::inc_: {
            var left(runtime->stack.pop()), right(runtime->stack.pop());
			runtime->stack.push(runtime->parse_inc(left, right));
			break;
		}
		case signal_types::dec_: {
            var left(runtime->stack.pop()), right(runtime->stack.pop());
			runtime->stack.push(runtime->parse_dec(left, right));
			break;
		}
		case signal_types::access_: {
            var left(runtime->stack.pop()), right(runtime->stack.pop());
			runtime->stack.push(runtime->parse_access(left, right));
			break;
		}
		}
	}

	void instruction_sig_dot::exec()
	{
		runtime->stack.push(runtime->parse_dot(runtime->stack.pop(), m_token));
	}

	void instruction_sig_arrow::exec()
	{
		runtime->stack.push(runtime->parse_arrow(runtime->stack.pop(), m_token));
	}

	void instruction_sig_choice::exec()
	{
		var cond(runtime->stack.pop());
		if (cond.type() == typeid(boolean)) {
			if (cond.const_val<boolean>()) {
				for (auto &it:m_assembly_true)
					it->exec();
			}
			else {
				for (auto &it:m_assembly_false)
					it->exec();
			}
		}
		else
			throw runtime_error("Unsupported operator operations(Choice).");
	}

	void instruction_sig_fcall::exec()
	{
		var func(runtime->stack.pop());
		if (func.type() == typeid(callable)) {
			vector args;
			args.reserve(m_size);
			for (std::size_t i = 0; i < m_size; ++i)
				args.push_back(lvalue(runtime->stack.pop()));
			runtime->stack.push(func.const_val<callable>().call(args));
		}
		else if (func.type() == typeid(object_method)) {
			const auto &om = func.const_val<object_method>();
			vector args{om.object};
			args.reserve(m_size);
			for (std::size_t i = 0; i < m_size; ++i)
				args.push_back(lvalue(runtime->stack.pop()));
			runtime->stack.push(om.callable.const_val<callable>().call(args));
		}
		else
			throw runtime_error("Unsupported operator operations(Fcall).");
	}
}
