/*
* Covariant Script Lexer
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
#include <iostream>

namespace cs {
	void compiler_type::process_char_buff(const std::deque<char> &buff, std::deque<token_base *> &tokens)
	{
		if (buff.empty())
			throw runtime_error("Received empty character buffer.");
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
						throw runtime_error("Do not allow empty character.");
					if (tmp.size() > 1)
						throw runtime_error("Char must be a single character.");
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
				throw runtime_error("Uknown character.");
				break;
			case token_types::id:
				if (std::isalnum(buff[i]) || buff[i] == '_') {
					tmp += buff[i];
					++i;
					continue;
				}
				type = token_types::null;
				if (action_map.exist(tmp)) {
					tokens.push_back(new token_action(action_map.match(tmp)));
					tmp.clear();
					break;
				}
				if (reserved_map.exist(tmp)) {
					tokens.push_back(reserved_map.match(tmp)());
					tmp.clear();
					break;
				}
				tokens.push_back(new token_id(tmp));
				tmp.clear();
				break;
			case token_types::signal: {
				if (issignal(buff[i])) {
					tmp += buff[i];
					++i;
					continue;
				}
				type = token_types::null;
				std::string sig;
				for (auto &ch:tmp) {
					if (!signal_map.exist(sig + ch)) {
						tokens.push_back(new token_signal(signal_map.match(sig)));
						sig = ch;
					}
					else
						sig += ch;
				}
				if (!sig.empty())
					tokens.push_back(new token_signal(signal_map.match(sig)));
				tmp.clear();
				break;
			}
			case token_types::value:
				if (std::isdigit(buff[i]) || buff[i] == '.') {
					tmp += buff[i];
					++i;
					continue;
				}
				type = token_types::null;
				tokens.push_back(new_value(parse_number(tmp)));
				tmp.clear();
				break;
			}
		}
		if (inside_char)
			throw runtime_error("Lack of the \'.");
		if (inside_str)
			throw runtime_error("Lack of the \".");
		if (tmp.empty())
			return;
		switch (type) {
		default:
			break;
		case token_types::id:
			if (action_map.exist(tmp)) {
				tokens.push_back(new token_action(action_map.match(tmp)));
				break;
			}
			if (reserved_map.exist(tmp)) {
				tokens.push_back(reserved_map.match(tmp)());
				tmp.clear();
				break;
			}
			tokens.push_back(new token_id(tmp));
			break;
		case token_types::signal: {
			std::string sig;
			for (auto &ch:tmp) {
				if (!signal_map.exist(sig + ch)) {
					tokens.push_back(new token_signal(signal_map.match(sig)));
					sig = ch;
				}
				else
					sig += ch;
			}
			if (!sig.empty())
				tokens.push_back(new token_signal(signal_map.match(sig)));
			break;
		}
		case token_types::value:
			tokens.push_back(new_value(parse_number(tmp)));
			break;
		}
	}

	class compiler_type::preprocessor final {
		std::size_t line_num = 1;
		bool is_annotation = false;
		bool is_command = false;
		bool multi_line = false;
		bool empty_buff = true;
		bool empty_line = true;
		std::deque<char> buff;
		std::string command;
		std::string line;

		void new_empty_line(const context_t &context)
		{
			context->file_buff.emplace_back();
			empty_line = true;
			++line_num;
		}

		void process_endline(const context_t &context, compiler_type &compiler, std::deque<token_base *> &tokens)
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
				compiler.process_char_buff(buff, tokens);
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
		explicit preprocessor(const context_t &context, compiler_type &compiler, const std::deque<char> &char_buff,
		                      std::deque<token_base *> &tokens)
		{
			for (auto &ch:char_buff) {
				if (ch == '\n') {
					process_endline(context, compiler, tokens);
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
			process_endline(context, compiler, tokens);
			if (multi_line)
				throw runtime_error("Lack of the @end command.");
		}
	};

	void compiler_type::translate_into_tokens(const std::deque<char> &char_buff, std::deque<token_base *> &tokens)
	{
		preprocessor(context, *this, char_buff, tokens);
	}

	void compiler_type::process_empty_brackets(std::deque<token_base *> &tokens)
	{
		if (tokens.empty())
			throw runtime_error("Received empty token buffer.");
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
						throw runtime_error("Parentheses do not match.");
					if (blist_stack.front() != 1)
						throw runtime_error("The parentheses type does not match.(Request Small Bracket)");
					blist_stack.pop_front();
					if (empty_bracket) {
						empty_bracket = false;
						tokens.push_back(new token_signal(signal_types::esb_));
						continue;
					}
					break;
				case signal_types::mrb_:
					if (blist_stack.empty())
						throw runtime_error("Parentheses do not match.");
					if (blist_stack.front() != 2)
						throw runtime_error("The parentheses type does not match.(Request Middle Bracket)");
					blist_stack.pop_front();
					if (empty_bracket) {
						empty_bracket = false;
						tokens.push_back(new token_signal(signal_types::emb_));
						continue;
					}
					break;
				case signal_types::lrb_:
					if (blist_stack.empty())
						throw runtime_error("Parentheses do not match.");
					if (blist_stack.front() != 3)
						throw runtime_error("The parentheses type does not match.(Request Large Bracket)");
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
			throw runtime_error("Parentheses do not match.");
	}

	void compiler_type::process_brackets(std::deque<token_base *> &tokens)
	{
		if (tokens.empty())
			throw runtime_error("Received empty token buffer.");
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
						throw runtime_error("Parentheses do not match.");
					if (blist_stack.front() != 1)
						throw runtime_error("The parentheses type does not match.(Request Small Bracket)");
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
						throw runtime_error("Parentheses do not match.");
					if (blist_stack.front() != 2)
						throw runtime_error("The parentheses type does not match.(Request Middle Bracket)");
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
						throw runtime_error("Parentheses do not match.");
					if (blist_stack.front() != 3)
						throw runtime_error("The parentheses type does not match.(Request Large Bracket)");
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
			throw runtime_error("Parentheses do not match.");
	}
}
