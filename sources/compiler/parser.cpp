/*
* Covariant Script Parser
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

namespace cs {
	void compiler_type::kill_brackets(std::deque<token_base *> &tokens, std::size_t
	                                  line_num)
	{
		std::deque<token_base *> oldt;
		std::swap(tokens, oldt);
		tokens.clear();
		bool insert_vardef = false;
		bool insert_varchk = false;
		bool expected_fdef = false;
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
				case action_types::import_:
					insert_vardef = true;
					tokens.push_back(ptr);
					tokens.push_back(new token_signal(signal_types::vardef_));
					continue;
				case action_types::var_:
					insert_varchk = true;
					tokens.push_back(ptr);
					tokens.push_back(new token_signal(signal_types::varchk_));
					continue;
				case action_types::constant_:
					insert_varchk = true;
					tokens.push_back(ptr);
					tokens.push_back(new token_signal(signal_types::varchk_));
					continue;
				case action_types::for_:
					tokens.push_back(ptr);
					tokens.push_back(new token_signal(signal_types::varprt_));
					continue;
				case action_types::foreach_:
					tokens.push_back(ptr);
					tokens.push_back(new token_signal(signal_types::varprt_));
					continue;
				case action_types::struct_:
					tokens.push_back(ptr);
					tokens.push_back(new token_signal(signal_types::vardef_));
					continue;
				case action_types::function_:
					tokens.push_back(ptr);
					tokens.push_back(new token_signal(signal_types::vardef_));
					expected_fdef = true;
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
					std::deque<tree_type<token_base *>> tlist;
					if (expected_fdef) {
						expected_fdef = false;
						no_optimize = true;
					}
					for (auto &list:static_cast<token_sblist *>(ptr)->get_list()) {
						tree_type<token_base *> tree;
						gen_tree(tree, list, trim_type::no_this_deduce);
						tlist.push_back(tree);
					}
					no_optimize = false;
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
				auto *mbl = static_cast<token_mblist *>(ptr);
				if (mbl->get_list().size() != 1)
					throw runtime_error("There are no more elements in middle bracket.");
				kill_brackets(mbl->get_list().front());
				tree_type<token_base *> tree;
				gen_tree(tree, mbl->get_list().front());
				tokens.push_back(new token_signal(signal_types::access_));
				tokens.push_back(new token_expr(tree));
				expected_fcall = true;
				continue;
			}
			case token_types::lblist: {
				for (auto &list:static_cast<token_lblist *>(ptr)->get_list())
					kill_brackets(list);
				std::deque<tree_type<token_base *>> tlist;
				for (auto &list:static_cast<token_lblist *>(ptr)->get_list()) {
					tree_type<token_base *> tree;
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
				case signal_types::com_:
					if (insert_vardef) {
						tokens.push_back(ptr);
						tokens.push_back(new token_signal(signal_types::vardef_));
						continue;
					}
					else if (insert_varchk) {
						tokens.push_back(ptr);
						tokens.push_back(new token_signal(signal_types::varchk_));
						continue;
					}
					else
						break;
				case signal_types::arrow_:
					if (expected_lambda) {
						tokens.push_back(new token_signal(signal_types::lambda_, line_num));
						expected_lambda = false;
						expected_fcall = false;
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
						throw runtime_error("Do not allow standalone empty small parentheses.");
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
			tokens.
			push_back(ptr);
		}
	}

	void compiler_type::split_token(std::deque<token_base *> &raw, std::deque<token_base *> &signals,
	                                std::deque<token_base *> &objects)
	{
		bool request_signal = false;
		for (auto &ptr:raw) {
			if (ptr->get_type() == token_types::action)
				throw runtime_error("Wrong format of expression.");
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

	void compiler_type::build_tree(tree_type<token_base *> &tree, std::deque<token_base *> &signals,
	                               std::deque<token_base *> &objects)
	{
		if (objects.empty() || signals.empty() || objects.size() != signals.size() + 1)
			throw runtime_error("Wrong grammar.");
		for (auto &obj:objects) {
			if (obj != nullptr && obj->get_type() == token_types::sblist) {
				auto *sbl = static_cast<token_sblist *>(obj);
				std::deque<token_base *> tokens;
				for (auto &list:sbl->get_list()) {
					for (auto &it:list)
						tokens.push_back(it);
					tokens.push_back(new token_signal(signal_types::com_));
				}
				tokens.pop_back();
				tree_type<token_base *> t;
				gen_tree(t, tokens);
				obj = new token_expr(t);
			}
		}
		tree.clear();
		tree.emplace_root_left(tree.root(), signals.front());
		tree.emplace_left_left(tree.root(), objects.front());
		for (std::size_t i = 1; i < signals.size(); ++i) {
			for (typename tree_type<token_base *>::iterator it = tree.root(); it.usable(); it = it.right()) {
				if (!it.right().usable()) {
					tree.emplace_right_right(it, objects.at(i));
					break;
				}
			}
			for (typename tree_type<token_base *>::iterator it = tree.root(); it.usable(); it = it.right()) {
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
		for (typename tree_type<token_base *>::iterator it = tree.root(); it.usable(); it = it.right()) {
			if (!it.right().usable()) {
				tree.emplace_right_right(it, objects.back());
				break;
			}
		}
	}

	void compiler_type::gen_tree(tree_type<token_base *> &tree, std::deque<token_base *> &raw, trim_type do_trim)
	{
		tree.clear();
		if (raw.size() == 1) {
			token_base *obj = raw.front();
			if (obj != nullptr && obj->get_type() == token_types::sblist) {
				auto *sbl = static_cast<token_sblist *>(obj);
				std::deque<token_base *> tokens;
				for (auto &list:sbl->get_list()) {
					for (auto &it:list)
						tokens.push_back(it);
					tokens.push_back(new token_signal(signal_types::com_));
				}
				tokens.pop_back();
				tree_type<token_base *> t;
				gen_tree(t, tokens);
				obj = new token_expr(t);
			}
			tree.emplace_root_left(tree.root(), obj);
		}
		else {
			std::deque<token_base *> signals, objects;
			split_token(raw, signals, objects);
			build_tree(tree, signals, objects);
		}
		optimize_expression(tree, do_trim);
	}

	void compiler_type::kill_expr(std::deque<token_base *> &tokens)
	{
		std::deque<token_base *> oldt, expr;
		std::swap(tokens, oldt);
		tokens.clear();
		for (auto &ptr:oldt) {
			if (ptr->get_type() == token_types::action || ptr->get_type() == token_types::endline) {
				if (!expr.empty()) {
					tree_type<token_base *> tree;
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
}
