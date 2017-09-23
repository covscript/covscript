#pragma once
/*
* Covariant Script Parser
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
* Copyright (C) 2017 Michael Lee(李登淳)
* Email: mikecovlee@163.com
* Github: https://github.com/mikecovlee
*/
#include "./lexer.hpp"

namespace cs {
	class token_expr final : public token_base {
		cov::tree<token_base *> mTree;
	public:
		token_expr() = delete;

		token_expr(const cov::tree<token_base *> &tree) : mTree(tree) {}

		virtual token_types get_type() const noexcept override
		{
			return token_types::expr;
		}

		cov::tree<token_base *> &get_tree() noexcept
		{
			return this->mTree;
		}
	};

	class token_arglist final : public token_base {
		std::deque<cov::tree<token_base *>> mTreeList;
	public:
		token_arglist() = default;

		token_arglist(const std::deque<cov::tree<token_base *>> &tlist) : mTreeList(tlist) {}

		virtual token_types get_type() const noexcept override
		{
			return token_types::arglist;
		}

		std::deque<cov::tree<token_base *>> &get_arglist() noexcept
		{
			return this->mTreeList;
		}
	};

	class token_array final : public token_base {
		std::deque<cov::tree<token_base *>> mTreeList;
	public:
		token_array() = default;

		token_array(const std::deque<cov::tree<token_base *>> &tlist) : mTreeList(tlist) {}

		virtual token_types get_type() const noexcept override
		{
			return token_types::array;
		}

		std::deque<cov::tree<token_base *>> &get_array() noexcept
		{
			return this->mTreeList;
		}
	};

	mapping<signal_types, int> signal_level_map = {
		{signal_types::add_,    10},
		{signal_types::addasi_, 0},
		{signal_types::sub_,    10},
		{signal_types::subasi_, 0},
		{signal_types::mul_,    11},
		{signal_types::mulasi_, 0},
		{signal_types::div_,    11},
		{signal_types::divasi_, 0},
		{signal_types::mod_,    12},
		{signal_types::modasi_, 0},
		{signal_types::pow_,    12},
		{signal_types::powasi_, 0},
		{signal_types::dot_,    17},
		{signal_types::arrow_,  17},
		{signal_types::und_,    9},
		{signal_types::abo_,    9},
		{signal_types::asi_,    0},
		{signal_types::choice_, 2},
		{signal_types::pair_,   3},
		{signal_types::equ_,    9},
		{signal_types::ueq_,    9},
		{signal_types::aeq_,    9},
		{signal_types::neq_,    9},
		{signal_types::lambda_, 1},
		{signal_types::vardef_, 20},
		{signal_types::and_,    7},
		{signal_types::or_,     7},
		{signal_types::not_,    8},
		{signal_types::inc_,    13},
		{signal_types::dec_,    13},
		{signal_types::fcall_,  14},
		{signal_types::emb_,    14},
		{signal_types::access_, 15},
		{signal_types::typeid_, 16},
		{signal_types::new_,    16},
		{signal_types::gcnew_,  16}
	};

	int get_signal_level(token_base *ptr)
	{
		if (ptr == nullptr)
			throw syntax_error("Get the level of null token.");
		if (ptr->get_type() != token_types::signal)
			throw syntax_error("Get the level of non-signal token.");
		return signal_level_map.match(static_cast<token_signal *>(ptr)->get_signal());
	}

	constexpr signal_types signal_left_associative[] = {
		signal_types::asi_, signal_types::addasi_, signal_types::subasi_, signal_types::mulasi_, signal_types::divasi_, signal_types::modasi_, signal_types::powasi_, signal_types::equ_, signal_types::und_, signal_types::abo_, signal_types::ueq_, signal_types::aeq_, signal_types::neq_, signal_types::and_, signal_types::or_
	};

	bool is_left_associative(token_base *ptr)
	{
		if (ptr == nullptr)
			throw syntax_error("Get the level of null token.");
		if (ptr->get_type() != token_types::signal)
			throw syntax_error("Get the level of non-signal token.");
		signal_types s = static_cast<token_signal *>(ptr)->get_signal();
		for (auto &t:signal_left_associative)
			if (t == s)
				return true;
		return false;
	}

	void kill_brackets(std::deque<token_base *> &);

	void gen_tree(cov::tree<token_base *> &, std::deque<token_base *> &);

	void kill_expr(std::deque<token_base *> &);

	void optimize_expression(cov::tree<token_base *> &);

	void kill_brackets(std::deque<token_base *> &tokens)
	{
		std::deque<token_base *> oldt;
		std::swap(tokens, oldt);
		tokens.clear();
		bool expected_fcall = false;
		bool expected_lambda = false;
		for (auto &ptr:oldt) {
			switch (ptr->get_type()) {
			default:
				break;
			case token_types::action:
				expected_fcall = false;
				switch (static_cast<token_action *>(ptr)->get_action()) {
				default:
					break;
				case action_types::var_:
					tokens.push_back(ptr);
					tokens.push_back(new token_signal(signal_types::vardef_));
					continue;
				case action_types::struct_:
					tokens.push_back(ptr);
					tokens.push_back(new token_signal(signal_types::vardef_));
					continue;
				case action_types::function_:
					tokens.push_back(ptr);
					tokens.push_back(new token_signal(signal_types::vardef_));
					continue;
				case action_types::namespace_:
					tokens.push_back(ptr);
					tokens.push_back(new token_signal(signal_types::vardef_));
					continue;
				case action_types::catch_:
					tokens.push_back(ptr);
					tokens.push_back(new token_signal(signal_types::vardef_));
					continue;
				}
				break;
			case token_types::id:
				expected_fcall = true;
				break;
			case token_types::value:
				expected_fcall = false;
				break;
			case token_types::sblist: {
				for (auto &list:static_cast<token_sblist *>(ptr)->get_list())
					kill_brackets(list);
				if (expected_fcall) {
					std::deque<cov::tree<token_base *>> tlist;
					for (auto &list:static_cast<token_sblist *>(ptr)->get_list()) {
						cov::tree<token_base *> tree;
						gen_tree(tree, list);
						tlist.push_back(tree);
					}
					if (!expected_lambda)
						tokens.push_back(new token_signal(signal_types::fcall_));
					tokens.push_back(new token_arglist(tlist));
					continue;
				}
				else {
					expected_fcall = true;
					break;
				}
			}
			case token_types::mblist: {
				token_mblist *mbl = static_cast<token_mblist *>(ptr);
				if (mbl == nullptr)
					throw syntax_error("Internal Error(Nullptr Access).");
				if (mbl->get_list().size() != 1)
					throw syntax_error("There are no more elements in middle bracket.");
				kill_brackets(mbl->get_list().front());
				cov::tree<token_base *> tree;
				gen_tree(tree, mbl->get_list().front());
				tokens.push_back(new token_signal(signal_types::access_));
				tokens.push_back(new token_expr(tree));
				expected_fcall = true;
				continue;
			}
			case token_types::lblist: {
				for (auto &list:static_cast<token_lblist *>(ptr)->get_list())
					kill_brackets(list);
				std::deque<cov::tree<token_base *>> tlist;
				for (auto &list:static_cast<token_lblist *>(ptr)->get_list()) {
					cov::tree<token_base *> tree;
					gen_tree(tree, list);
					tlist.push_back(tree);
				}
				tokens.push_back(new token_array(tlist));
				expected_fcall = false;
				continue;
			}
			case token_types::signal: {
				switch (static_cast<token_signal *>(ptr)->get_signal()) {
				default:
					break;
				case signal_types::arrow_:
					if (expected_lambda) {
						tokens.push_back(new token_signal(signal_types::lambda_));
						expected_lambda = false;
						continue;
					}
					else
						break;
				case signal_types::esb_:
					if (expected_fcall) {
						if (!expected_lambda)
							tokens.push_back(new token_signal(signal_types::fcall_));
						tokens.push_back(new token_arglist());
					}
					else
						throw syntax_error("Do not allow standalone empty small parentheses.");
					expected_fcall = false;
					continue;
				case signal_types::emb_:
					expected_fcall = true;
					expected_lambda = true;
					tokens.push_back(ptr);
					continue;
				case signal_types::elb_:
					tokens.push_back(new token_array());
					expected_fcall = false;
					continue;
				}
				expected_lambda = false;
				expected_fcall = false;
				break;
			}
			}
			tokens.push_back(ptr);
		}
	}

	void split_token(std::deque<token_base *> &raw, std::deque<token_base *> &signals, std::deque<token_base *> &objects)
	{
		bool request_signal = false;
		for (auto &ptr:raw) {
			if (ptr->get_type() == token_types::action)
				throw syntax_error("Wrong format of expression.");
			if (ptr->get_type() == token_types::signal) {
				if (!request_signal)
					objects.push_back(nullptr);
				signals.push_back(ptr);
				request_signal = false;
			}
			else {
				objects.push_back(ptr);
				request_signal = true;
			}
		}
		if (!request_signal)
			objects.push_back(nullptr);
	}

	void build_tree(cov::tree<token_base *> &tree, std::deque<token_base *> &signals, std::deque<token_base *> &objects)
	{
		if (objects.empty() || signals.empty() || objects.size() != signals.size() + 1)
			throw syntax_error("Symbols do not match the object.");
		for (auto &obj:objects) {
			if (obj != nullptr && obj->get_type() == token_types::sblist) {
				token_sblist *sbl = static_cast<token_sblist *>(obj);
				if (sbl->get_list().size() != 1)
					throw syntax_error("There are no more elements in small bracket.");
				cov::tree<token_base *> t;
				gen_tree(t, sbl->get_list().front());
				obj = new token_expr(t);
			}
		}
		tree.clear();
		tree.emplace_root_left(tree.root(), signals.front());
		tree.emplace_left_left(tree.root(), objects.front());
		for (std::size_t i = 1; i < signals.size(); ++i) {
			for (typename cov::tree<token_base *>::iterator it = tree.root(); it.usable(); it = it.right()) {
				if (!it.right().usable()) {
					tree.emplace_right_right(it, objects.at(i));
					break;
				}
			}
			for (typename cov::tree<token_base *>::iterator it = tree.root(); it.usable(); it = it.right()) {
				if (!it.right().usable()) {
					tree.emplace_root_left(it, signals.at(i));
					break;
				}
				if (get_signal_level(it.data()) == get_signal_level(signals.at(i))) {
					if (is_left_associative(it.data()))
						tree.emplace_right_left(it, signals.at(i));
					else
						tree.emplace_root_left(it, signals.at(i));
					break;
				}
				if (get_signal_level(it.data()) > get_signal_level(signals.at(i))) {
					tree.emplace_root_left(it, signals.at(i));
					break;
				}
			}
		}
		for (typename cov::tree<token_base *>::iterator it = tree.root(); it.usable(); it = it.right()) {
			if (!it.right().usable()) {
				tree.emplace_right_right(it, objects.back());
				break;
			}
		}
	}

	void gen_tree(cov::tree<token_base *> &tree, std::deque<token_base *> &raw)
	{
		tree.clear();
		if (raw.size() == 1) {
			token_base *obj = raw.front();
			if (obj != nullptr && obj->get_type() == token_types::sblist) {
				token_sblist *sbl = static_cast<token_sblist *>(obj);
				if (sbl->get_list().size() != 1)
					throw syntax_error("There are no more elements in small bracket.");
				cov::tree<token_base *> t;
				gen_tree(t, sbl->get_list().front());
				obj = new token_expr(t);
			}
			tree.emplace_root_left(tree.root(), obj);
		}
		else {
			std::deque<token_base *> signals, objects;
			split_token(raw, signals, objects);
			build_tree(tree, signals, objects);
		}
		optimize_expression(tree);
	}

	void kill_expr(std::deque<token_base *> &tokens)
	{
		std::deque<token_base *> oldt, expr;
		std::swap(tokens, oldt);
		tokens.clear();
		for (auto &ptr:oldt) {
			if (ptr->get_type() == token_types::action || ptr->get_type() == token_types::endline) {
				if (!expr.empty()) {
					cov::tree<token_base *> tree;
					gen_tree(tree, expr);
					tokens.push_back(new token_expr(tree));
					expr.clear();
				}
				tokens.push_back(ptr);
			}
			else
				expr.push_back(ptr);
		}
	}

	void kill_action(std::deque<std::deque<token_base *>>, std::deque<statement_base *> &, bool raw = false);

	void translate_into_statements(std::deque<token_base *> &tokens, std::deque<statement_base *> &statements);
}
