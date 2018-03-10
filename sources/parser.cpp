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
* Copyright (C) 2018 Michael Lee(李登淳)
* Email: mikecovlee@163.com
* Github: https://github.com/mikecovlee
*/
#include <covscript/instance.hpp>

namespace cs {
	void instance_type::kill_brackets(std::deque<token_base *> &tokens, std::size_t
	                                  line_num)
	{
		std::deque<token_base *> oldt;
		std::swap(tokens, oldt
		         );
		tokens.

		clear();

		bool expected_fdef = false;
		bool expected_fcall = false;
		bool expected_lambda = false;
		for (
		    auto &ptr:
		    oldt) {
			switch (ptr->

			        get_type()

			       ) {
			default:
				break;
			case
					token_types::action:
				expected_fcall = false;
				switch (static_cast<token_action *>(ptr)->

				        get_action()

				       ) {
				default:
					break;
				case
						action_types::import_:
					tokens
					.
					push_back(ptr);
					tokens.push_back(new
					                 token_signal(signal_types::vardef_)
					                );
					continue;
				case
						action_types::var_:
					tokens
					.
					push_back(ptr);
					tokens.push_back(new
					                 token_signal(signal_types::vardef_)
					                );
					continue;
				case
						action_types::struct_:
					tokens
					.
					push_back(ptr);
					tokens.push_back(new
					                 token_signal(signal_types::vardef_)
					                );
					continue;
				case
						action_types::function_:
					tokens
					.
					push_back(ptr);
					tokens.push_back(new
					                 token_signal(signal_types::vardef_)
					                );
					expected_fdef = true;
					continue;
				case
						action_types::namespace_:
					tokens
					.
					push_back(ptr);
					tokens.push_back(new
					                 token_signal(signal_types::vardef_)
					                );
					continue;
				case
						action_types::catch_:
					tokens
					.
					push_back(ptr);
					tokens.push_back(new
					                 token_signal(signal_types::vardef_)
					                );
					continue;
				}
				break;
			case
					token_types::id:
				expected_fcall = true;
				break;
			case
					token_types::value:
				expected_fcall = false;
				break;
			case

					token_types::sblist: {
					for (auto &list:static_cast<token_sblist *>(ptr)->get_list())
						kill_brackets(list);
					if (expected_fcall) {
						std::deque<cov::tree<token_base *>> tlist;
						if (expected_fdef) {
							expected_fdef = false;
							no_optimize = true;
						}
						for (auto &list:static_cast<token_sblist *>(ptr)->get_list()) {
							cov::tree<token_base *> tree;
							gen_tree(tree, list);
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

			case

					token_types::mblist: {
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

			case

					token_types::lblist: {
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

			case

					token_types::signal: {
					switch (static_cast<token_signal *>(ptr)->get_signal()) {
					default:
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
			tokens.
			push_back(ptr);
		}
	}

	void instance_type::split_token(std::deque<token_base *> &raw, std::deque<token_base *> &signals,
	                                std::deque<token_base *> &objects)
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

	void instance_type::build_tree(cov::tree<token_base *> &tree, std::deque<token_base *> &signals,
	                               std::deque<token_base *> &objects)
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

	void instance_type::gen_tree(cov::tree<token_base *> &tree, std::deque<token_base *> &raw)
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

	void instance_type::kill_expr(std::deque<token_base *> &tokens)
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

	void
	instance_type::kill_action(std::deque<std::deque<token_base *>> lines, std::deque<statement_base *> &statements,
	                           bool raw)
	{
		std::deque<std::deque<token_base *>> tmp;
		method_base *method = nullptr;
		token_endline *endsig = nullptr;
		int level = 0;
		for (auto &line:lines) {
			endsig = dynamic_cast<token_endline *>(line.back());
			try {
				if (raw) {
					process_brackets(line);
					kill_brackets(line, endsig->get_line_num());
					kill_expr(line);
				}
				method_base *m = translator.match(line);
				switch (m->get_type()) {
				case method_types::null:
					throw syntax_error("Null type of grammar.");
					break;
				case method_types::single: {
					statement_base *sptr = nullptr;
					if (level > 0) {
						if (m->get_target_type() == statement_types::end_) {
							storage.remove_set();
							storage.remove_domain();
							--level;
						}
						if (level == 0) {
							sptr = method->translate(tmp);
							tmp.clear();
							method = nullptr;
						}
						else {
							m->preprocess({line});
							tmp.push_back(line);
						}
					}
					else {
						if (m->get_target_type() == statement_types::end_)
							throw syntax_error("Hanging end statement.");
						else
							sptr = m->translate({line});
					}
					if (sptr != nullptr)
						statements.push_back(sptr);
				}
				break;
				case method_types::block: {
					if (level == 0)
						method = m;
					++level;
					storage.add_domain();
					storage.add_set();
					m->preprocess({line});
					tmp.push_back(line);
				}
				break;
				case method_types::jit_command:
					m->translate({line});
					break;
				}
			}
			catch (const cs::exception &e) {
				throw e;
			}
			catch (const std::exception &e) {
				throw exception(endsig->get_line_num(), context->file_path,
				                context->file_buff.at(endsig->get_line_num() - 1), e.what());
			}
		}
		if (level != 0)
			throw syntax_error("Lack of the \"end\" signal.");
	}

	void instance_type::translate_into_statements(std::deque<token_base *> &tokens,
	        std::deque<statement_base *> &statements)
	{
		std::deque<std::deque<token_base *>> lines;
		std::deque<token_base *> tmp;
		for (auto &ptr:tokens) {
			tmp.push_back(ptr);
			if (ptr != nullptr && ptr->get_type() == token_types::endline) {
				if (tmp.size() > 1)
					lines.push_back(tmp);
				tmp.clear();
			}
		}
		if (tmp.size() > 1)
			lines.push_back(tmp);
		tmp.clear();
		kill_action(lines, statements, true);
	}

}
