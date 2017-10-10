/*
* Covariant Script Lexer
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
#include "headers/instance.hpp"

namespace cs {
	void instance_type::process_char_buff(const std::deque<char> &buff, std::deque<token_base *> &tokens)
	{
		if (buff.empty())
			throw syntax_error("Received empty character buffer.");
		std::string tmp;
		token_types type = token_types::null;
		bool inside_char = false;
		bool inside_str = false;
		bool escape = false;
		for (std::size_t i = 0; i < buff.size();) {
			if (inside_char) {
				if (escape) {
					tmp += escape_map.match(buff[i]);
					escape = false;
				}
				else if (buff[i] == '\\') {
					escape = true;
				}
				else if (buff[i] == '\'') {
					if (tmp.empty())
						throw syntax_error("Do not allow empty character.");
					if (tmp.size() > 1)
						throw syntax_error("Char must be a single character.");
					tokens.push_back(new_value(tmp[0]));
					tmp.clear();
					inside_char = false;
				}
				else {
					tmp += buff[i];
				}
				++i;
				continue;
			}
			if (inside_str) {
				if (escape) {
					tmp += escape_map.match(buff[i]);
					escape = false;
				}
				else if (buff[i] == '\\') {
					escape = true;
				}
				else if (buff[i] == '\"') {
					tokens.push_back(new_value(tmp));
					tmp.clear();
					inside_str = false;
				}
				else {
					tmp += buff[i];
				}
				++i;
				continue;
			}
			switch (type) {
			default:
				break;
			case token_types::null:
				if (buff[i] == '\"') {
					inside_str = true;
					++i;
					continue;
				}
				if (buff[i] == '\'') {
					inside_char = true;
					++i;
					continue;
				}
				if (std::isspace(buff[i])) {
					++i;
					continue;
				}
				if (issignal(buff[i])) {
					type = token_types::signal;
					continue;
				}
				if (std::isdigit(buff[i])) {
					type = token_types::value;
					continue;
				}
				if (std::isalpha(buff[i]) || buff[i] == '_') {
					type = token_types::id;
					continue;
				}
				throw syntax_error("Uknown character.");
				break;
			case token_types::id:
				if (std::isalnum(buff[i]) || buff[i] == '_') {
					tmp += buff[i];
					++i;
					continue;
				}
				type = token_types::null;
				if (action_map.exsist(tmp)) {
					tokens.push_back(new token_action(action_map.match(tmp)));
					tmp.clear();
					break;
				}
				if (reserved_map.exsist(tmp)) {
					tokens.push_back(reserved_map.match(tmp)());
					tmp.clear();
					break;
				}
				tokens.push_back(new token_id(tmp));
				tmp.clear();
				break;
			case token_types::signal:
				if (issignal(buff[i])) {
					if (tmp.size() == 1) {
						if (signal_map.exsist(tmp + buff[i])) {
							tokens.push_back(new token_signal(signal_map.match(tmp + buff[i])));
							tmp.clear();
						}
						else {
							if (signal_map.exsist(tmp)) {
								tokens.push_back(new token_signal(signal_map.match(tmp)));
								tmp.clear();
							}
							tmp += buff[i];
						}
					}
					else
						tmp += buff[i];
					++i;
					continue;
				}
				type = token_types::null;
				if (!tmp.empty())
					tokens.push_back(new token_signal(signal_map.match(tmp)));
				tmp.clear();
				break;
			case token_types::value:
				if (std::isdigit(buff[i]) || buff[i] == '.') {
					tmp += buff[i];
					++i;
					continue;
				}
				type = token_types::null;
				tokens.push_back(new_value(std::stold(tmp)));
				tmp.clear();
				break;
			}
		}
		if (inside_char)
			throw syntax_error("Lack of the \'.");
		if (inside_str)
			throw syntax_error("Lack of the \".");
		if (tmp.empty())
			return;
		switch (type) {
		default:
			break;
		case token_types::id:
			if (action_map.exsist(tmp)) {
				tokens.push_back(new token_action(action_map.match(tmp)));
				break;
			}
			if (reserved_map.exsist(tmp)) {
				tokens.push_back(reserved_map.match(tmp)());
				tmp.clear();
				break;
			}
			tokens.push_back(new token_id(tmp));
			break;
		case token_types::signal:
			tokens.push_back(new token_signal(signal_map.match(tmp)));
			break;
		case token_types::value:
			tokens.push_back(new_value(std::stold(tmp)));
			break;
		}
	}

	class preprocessor final {
		std::size_t line_num = 1;
		bool is_annotation = false;
		bool is_command = false;
		bool multi_line = false;
		bool empty_buff = true;
		bool empty_line = true;
		std::deque<char> buff;
		std::string command;
		std::string line;

		void new_empty_line(context_t context)
		{
			context->file_buff.emplace_back();
			empty_line = true;
			++line_num;
		}

		void process_endline(context_t context, std::deque<token_base *> &tokens)
		{
			if (is_annotation) {
				is_annotation = false;
				new_empty_line(context);
				return;
			}
			if (is_command) {
				is_command = false;
				if (command == "begin" && !multi_line)
					multi_line = true;
				else if (command == "end" && multi_line)
					multi_line = false;
				else
					throw exception(line_num, context->file_path, command, "Wrong grammar for preprocessor command.");
				command.clear();
			}
			if (multi_line || empty_buff) {
				new_empty_line(context);
				return;
			}
			try {
				context->instance->process_char_buff(buff, tokens);
			}
			catch (const cs::exception &e) {
				throw e;
			}
			catch (const std::exception &e) {
				throw exception(line_num, context->file_path, line, e.what());
			}
			tokens.push_back(new token_endline(line_num));
			context->file_buff.emplace_back(line);
			++line_num;
			buff.clear();
			line.clear();
			empty_buff = true;
			empty_line = true;
		}

	public:
		explicit preprocessor(context_t context, const std::deque<char> &char_buff, std::deque<token_base *> &tokens)
		{
			for (auto &ch:char_buff) {
				if (ch == '\n') {
					process_endline(context, tokens);
					continue;
				}
				if (is_annotation)
					continue;
				if (is_command) {
					if (!std::isspace(ch))
						command.push_back(ch);
					continue;
				}
				if (empty_line && !std::isspace(ch)) {
					switch (ch) {
					case '#':
						is_annotation = true;
						continue;
					case '@':
						is_command = true;
						continue;
					default:
						empty_buff = false;
						empty_line = false;
						break;
					}
				}
				if (!empty_buff) {
					buff.push_back(ch);
					line.push_back(ch);
				}
			}
			process_endline(context, tokens);
			if (multi_line)
				throw syntax_error("Lack of the @end command.");
		}
	};

	void instance_type::translate_into_tokens(const std::deque<char> &char_buff, std::deque<token_base *> &tokens)
	{
		preprocessor(context, char_buff, tokens);
	}

	void instance_type::process_empty_brackets(std::deque<token_base *> &tokens)
	{
		if (tokens.empty())
			throw syntax_error("Received empty token buffer.");
		std::deque<token_base *> oldt;
		std::swap(tokens, oldt);
		tokens.clear();
		std::deque<int> blist_stack;
		bool empty_bracket = false;
		auto insert_bracket = [&]() {
			switch (blist_stack.front()) {
			case 1:
				tokens.push_back(new token_signal(signal_types::slb_));
				break;
			case 2:
				tokens.push_back(new token_signal(signal_types::mlb_));
				break;
			case 3:
				tokens.push_back(new token_signal(signal_types::llb_));
				break;
			}
		};
		for (auto &ptr:oldt) {
			if (ptr->get_type() == token_types::signal) {
				switch (static_cast<token_signal *>(ptr)->get_signal()) {
				default:
					break;
				case signal_types::slb_:
					if (empty_bracket)
						insert_bracket();
					blist_stack.push_front(1);
					empty_bracket = true;
					continue;
				case signal_types::mlb_:
					if (empty_bracket)
						insert_bracket();
					blist_stack.push_front(2);
					empty_bracket = true;
					continue;
				case signal_types::llb_:
					if (empty_bracket)
						insert_bracket();
					blist_stack.push_front(3);
					empty_bracket = true;
					continue;
				case signal_types::srb_:
					if (blist_stack.empty())
						throw syntax_error("Parentheses do not match.");
					if (blist_stack.front() != 1)
						throw syntax_error("The parentheses type does not match.(Request Small Bracket)");
					blist_stack.pop_front();
					if (empty_bracket) {
						empty_bracket = false;
						tokens.push_back(new token_signal(signal_types::esb_));
						continue;
					}
					break;
				case signal_types::mrb_:
					if (blist_stack.empty())
						throw syntax_error("Parentheses do not match.");
					if (blist_stack.front() != 2)
						throw syntax_error("The parentheses type does not match.(Request Middle Bracket)");
					blist_stack.pop_front();
					if (empty_bracket) {
						empty_bracket = false;
						tokens.push_back(new token_signal(signal_types::emb_));
						continue;
					}
					break;
				case signal_types::lrb_:
					if (blist_stack.empty())
						throw syntax_error("Parentheses do not match.");
					if (blist_stack.front() != 3)
						throw syntax_error("The parentheses type does not match.(Request Large Bracket)");
					blist_stack.pop_front();
					if (empty_bracket) {
						empty_bracket = false;
						tokens.push_back(new token_signal(signal_types::elb_));
						continue;
					}
					break;
				}
			}
			if (empty_bracket && !blist_stack.empty()) {
				empty_bracket = false;
				insert_bracket();
			}
			tokens.push_back(ptr);
		}
		if (!blist_stack.empty())
			throw syntax_error("Parentheses do not match.");
	}

	void instance_type::process_brackets(std::deque<token_base *> &tokens)
	{
		if (tokens.empty())
			throw syntax_error("Received empty token buffer.");
		process_empty_brackets(tokens);
		std::deque<token_base *> oldt;
		std::swap(tokens, oldt);
		tokens.clear();
		std::deque<std::deque<token_base *>> blist;
		std::deque<token_base *> btokens;
		std::deque<int> blist_stack;
		for (auto &ptr:oldt) {
			if (ptr->get_type() == token_types::signal) {
				switch (static_cast<token_signal *>(ptr)->get_signal()) {
				default:
					break;
				case signal_types::slb_:
					blist_stack.push_front(1);
					if (blist_stack.size() == 1)
						continue;
					break;
				case signal_types::mlb_:
					blist_stack.push_front(2);
					if (blist_stack.size() == 1)
						continue;
					break;
				case signal_types::llb_:
					blist_stack.push_front(3);
					if (blist_stack.size() == 1)
						continue;
					break;
				case signal_types::srb_:
					if (blist_stack.empty())
						throw syntax_error("Parentheses do not match.");
					if (blist_stack.front() != 1)
						throw syntax_error("The parentheses type does not match.(Request Small Bracket)");
					blist_stack.pop_front();
					if (blist_stack.empty()) {
						process_brackets(btokens);
						blist.push_back(btokens);
						tokens.push_back(new token_sblist(blist));
						blist.clear();
						btokens.clear();
						continue;
					}
					break;
				case signal_types::mrb_:
					if (blist_stack.empty())
						throw syntax_error("Parentheses do not match.");
					if (blist_stack.front() != 2)
						throw syntax_error("The parentheses type does not match.(Request Middle Bracket)");
					blist_stack.pop_front();
					if (blist_stack.empty()) {
						process_brackets(btokens);
						blist.push_back(btokens);
						tokens.push_back(new token_mblist(blist));
						blist.clear();
						btokens.clear();
						continue;
					}
					break;
				case signal_types::lrb_:
					if (blist_stack.empty())
						throw syntax_error("Parentheses do not match.");
					if (blist_stack.front() != 3)
						throw syntax_error("The parentheses type does not match.(Request Large Bracket)");
					blist_stack.pop_front();
					if (blist_stack.empty()) {
						process_brackets(btokens);
						blist.push_back(btokens);
						tokens.push_back(new token_lblist(blist));
						blist.clear();
						btokens.clear();
						continue;
					}
					break;
				case signal_types::com_:
					if (blist_stack.size() == 1) {
						process_brackets(btokens);
						blist.push_back(btokens);
						btokens.clear();
						continue;
					}
					break;
				}
			}
			if (blist_stack.empty())
				tokens.push_back(ptr);
			else
				btokens.push_back(ptr);
		}
		if (!blist_stack.empty())
			throw syntax_error("Parentheses do not match.");
	}
}
