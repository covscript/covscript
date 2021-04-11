/*
* Covariant Script Lexer
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
* Copyright (C) 2017-2021 Michael Lee(李登淳)
*
* This software is registered with the National Copyright Administration
* of the People's Republic of China(Registration Number: 2020SR0408026)
* and is protected by the Copyright Law of the People's Republic of China.
*
* Email:   lee@covariant.cn, mikecovlee@163.com
* Github:  https://github.com/mikecovlee
* Website: http://covscript.org.cn
*/
#include <covscript/impl/compiler.hpp>
#include <codecvt>
#include <cwctype>
#include <climits>

namespace cs {
	namespace codecvt {
		class charset {
		public:
			virtual ~charset() = default;

			virtual std::u32string local2wide(const std::deque<char> &) = 0;

			virtual std::string wide2local(const std::u32string &) = 0;

			virtual bool is_identifier(char32_t) = 0;
		};

		class ascii final : public charset {
		public:
			std::u32string local2wide(const std::deque<char> &local) override
			{
				return std::u32string(local.begin(), local.end());
			}

			std::string wide2local(const std::u32string &str) override
			{
				std::string local;
				local.reserve(str.size());
				for (auto ch:str)
					local.push_back(ch);
				return std::move(local);
			}

			bool is_identifier(char32_t ch) override
			{
				return ch == '_' || std::iswalnum(ch);
			}
		};

		class utf8 final : public charset {
			std::wstring_convert<std::codecvt_utf8<char32_t>, char32_t> cvt;

			static constexpr std::uint32_t ascii_max = 0x7F;
		public:
			std::u32string local2wide(const std::deque<char> &local) override
			{
				std::string str(local.begin(), local.end());
				return cvt.from_bytes(str);
			}

			std::string wide2local(const std::u32string &str) override
			{
				return cvt.to_bytes(str);
			}

			bool is_identifier(char32_t ch) override
			{
				if (compiler_type::issignal(ch))
					return false;
				/**
				 * Chinese Character in Unicode Charset
				 * Basic:    0x4E00 - 0x9FA5
				 * Extended: 0x9FA6 - 0x9FEF
				 * Special:  0x3007
				 */
				if (ch > ascii_max)
					return (ch >= 0x4E00 && ch <= 0x9FA5) || (ch >= 0x9FA6 && ch <= 0x9FEF) || ch == 0x3007;
				else
					return ch == '_' || std::iswalnum(ch);
			}
		};

		class gbk final : public charset {
			static inline char32_t set_zero(char32_t ch)
			{
				return ch & 0x0000ffff;
			}

			static constexpr std::uint8_t u8_blck_begin = 0x80;
			static constexpr std::uint32_t u32_blck_begin = 0x8000;
		public:
			std::u32string local2wide(const std::deque<char> &local) override
			{
				std::u32string wide;
				uint32_t head = 0;
				bool read_next = true;
				for (auto it = local.begin(); it != local.end();) {
					if (read_next) {
						head = *(it++);
						if (head & u8_blck_begin)
							read_next = false;
						else
							wide.push_back(set_zero(head));
					}
					else {
						std::uint8_t tail = *(it++);
						wide.push_back(set_zero(head << 8 | tail));
						read_next = true;
					}
				}
				if (!read_next)
					throw compile_error("Codecvt: Bad encoding.");
				return std::move(wide);
			}

			std::string wide2local(const std::u32string &wide) override
			{
				std::string local;
				for (auto &ch:wide) {
					if (ch & u32_blck_begin)
						local.push_back(ch >> 8);
					local.push_back(ch);
				}
				return std::move(local);
			}

			bool is_identifier(char32_t ch) override
			{
				if (compiler_type::issignal(ch))
					return false;
				/**
				 * Chinese Character in GBK Charset
				 * GBK/2: 0xB0A1 - 0xF7FE
				 * GBK/3: 0x8140 - 0xA0FE
				 * GBK/4: 0xAA40 - 0xFEA0
				 * GBK/5: 0xA996
				 */
				if (ch & u32_blck_begin)
					return (ch >= 0xB0A1 && ch <= 0xF7FE) || (ch >= 0x8140 && ch <= 0xA0FE) ||
					       (ch >= 0xAA40 && ch <= 0xFEA0) || ch == 0xA996;
				else
					return ch == '_' || std::iswalnum(ch);
			}
		};
	}

	void compiler_type::process_char_buff(const std::deque<char> &raw_buff, std::deque<token_base *> &tokens,
	                                      charset encoding)
	{
		if (raw_buff.empty())
			throw compile_error("Received empty character buffer.");
		std::unique_ptr<codecvt::charset> cvt = nullptr;
		switch (encoding) {
		case charset::ascii:
			cvt = std::make_unique<codecvt::ascii>();
			break;
		case charset::utf8:
			cvt = std::make_unique<codecvt::utf8>();
			break;
		case charset::gbk:
			cvt = std::make_unique<codecvt::gbk>();
			break;
		}
		std::u32string buff = cvt->local2wide(raw_buff);
		std::u32string tmp;
		token_types type = token_types::null;
		bool inside_char = false;
		bool inside_str = false;
		bool escape = false;
		for (auto it = buff.begin(); it != buff.end();) {
			if (inside_char) {
				if (escape) {
					tmp += escape_map.match(*it);
					escape = false;
				}
				else if (*it == '\\') {
					escape = true;
				}
				else if (*it == '\'') {
					if (tmp.empty())
						throw compile_error("Do not allow empty character.");
					if (tmp.size() > 1)
						throw compile_error("Char must be a single character.");
					if (tmp[0] > CHAR_MAX)
						throw compile_error("Do not support unicode character. Please using string instead.");
					tokens.push_back(new_value((char) tmp[0]));
					tmp.clear();
					inside_char = false;
				}
				else {
					tmp += *it;
				}
				++it;
				continue;
			}
			if (inside_str) {
				if (escape) {
					tmp += escape_map.match(*it);
					escape = false;
				}
				else if (*it == '\\') {
					escape = true;
				}
				else if (*it == '\"') {
					inside_str = false;
					if (cvt->is_identifier(*(it + 1))) {
						tokens.push_back(new token_literal(cvt->wide2local(tmp), ""));
						type = token_types::literal;
					}
					else
						tokens.push_back(new_value(cvt->wide2local(tmp)));
					tmp.clear();
				}
				else {
					tmp += *it;
				}
				++it;
				continue;
			}
			if (*it == '#')
				break;
			switch (type) {
			default:
				break;
			case token_types::null:
				if (*it == '\"') {
					inside_str = true;
					++it;
					continue;
				}
				if (*it == '\'') {
					inside_char = true;
					++it;
					continue;
				}
				if (std::iswspace(*it)) {
					++it;
					continue;
				}
				if (issignal(*it)) {
					type = token_types::signal;
					continue;
				}
				if (std::iswdigit(*it)) {
					type = token_types::value;
					continue;
				}
				if (cvt->is_identifier(*it)) {
					type = token_types::id;
					continue;
				}
				throw compile_error(std::string("Uknown character: " + cvt->wide2local(std::u32string(1, *it))));
				break;
			case token_types::id:
				if (cvt->is_identifier(*it)) {
					tmp += *it;
					++it;
					continue;
				}
				type = token_types::null;
				if (reserved_map.exist(cvt->wide2local(tmp))) {
					tokens.push_back(reserved_map.match(cvt->wide2local(tmp))());
					tmp.clear();
					break;
				}
				tokens.push_back(new token_id(cvt->wide2local(tmp)));
				tmp.clear();
				break;
			case token_types::literal:
				if (cvt->is_identifier(*it)) {
					tmp += *it;
					++it;
					continue;
				}
				type = token_types::null;
				static_cast<token_literal *>(tokens.back())->m_literal = cvt->wide2local(tmp);
				tmp.clear();
				break;
			case token_types::signal: {
				if (issignal(*it)) {
					tmp += *it;
					++it;
					continue;
				}
				type = token_types::null;
				std::u32string sig;
				for (auto &ch:tmp) {
					if (!signal_map.exist(cvt->wide2local(sig + ch))) {
						tokens.push_back(new token_signal(signal_map.match(cvt->wide2local(sig))));
						sig = ch;
					}
					else
						sig += ch;
				}
				if (!sig.empty())
					tokens.push_back(new token_signal(signal_map.match(cvt->wide2local(sig))));
				tmp.clear();
				break;
			}
			case token_types::value:
				if (std::iswdigit(*it) || *it == '.') {
					tmp += *it;
					++it;
					continue;
				}
				type = token_types::null;
				tokens.push_back(new_value(parse_number(cvt->wide2local(tmp))));
				tmp.clear();
				break;
			}
		}
		if (inside_char)
			throw compile_error("Lack of the \'.");
		if (inside_str)
			throw compile_error("Lack of the \".");
		if (tmp.empty())
			return;
		switch (type) {
		default:
			break;
		case token_types::id:
			if (reserved_map.exist(cvt->wide2local(tmp))) {
				tokens.push_back(reserved_map.match(cvt->wide2local(tmp))());
				tmp.clear();
				break;
			}
			tokens.push_back(new token_id(cvt->wide2local(tmp)));
			break;
		case token_types::literal:
			static_cast<token_literal *>(tokens.back())->m_literal = cvt->wide2local(tmp);
			break;
		case token_types::signal: {
			std::u32string sig;
			for (auto &ch:tmp) {
				if (!signal_map.exist(cvt->wide2local(sig + ch))) {
					tokens.push_back(new token_signal(signal_map.match(cvt->wide2local(sig))));
					sig = ch;
				}
				else
					sig += ch;
			}
			if (!sig.empty())
				tokens.push_back(new token_signal(signal_map.match(cvt->wide2local(sig))));
			break;
		}
		case token_types::value:
			tokens.push_back(new_value(parse_number(cvt->wide2local(tmp))));
			break;
		}
	}

	class compiler_type::preprocessor final {
		std::size_t last_line_num = 1, line_num = 1;
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

		void process_endline(const context_t &context, compiler_type &compiler, std::deque<token_base *> &tokens,
		                     charset &encoding)
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
				else if (command == "end" && multi_line) {
					tokens.push_back(new token_endline(last_line_num));
					multi_line = false;
				}
				else if (command == "charset:ascii")
					encoding = charset::ascii;
				else if (command == "charset:utf8")
					encoding = charset::utf8;
				else if (command == "charset:gbk")
					encoding = charset::gbk;
				else
					throw exception(line_num, context->file_path, command, "Wrong grammar for preprocessor command.");
				command.clear();
			}
			if (empty_buff) {
				new_empty_line(context);
				return;
			}
			try {
				compiler.process_char_buff(buff, tokens, encoding);
			}
			catch (const cs::exception &e) {
				throw e;
			}
			catch (const std::exception &e) {
				throw exception(line_num, context->file_path, line, e.what());
			}
			for (auto it = tokens.rbegin(); it != tokens.rend(); ++it) {
				if (*it != nullptr) {
					token_base *ptr = *it;
					if (ptr->get_type() == token_types::signal &&
					        static_cast<token_signal *>(ptr)->get_signal() == signal_types::endline_) {
						ptr->line_num = line_num;
						break;
					}
					else if (ptr->get_type() == token_types::endline)
						break;
					else
						ptr->line_num = line_num;
				}
			}
			if (!multi_line)
				tokens.push_back(new token_endline(line_num));
			context->file_buff.emplace_back(line);
			last_line_num = line_num++;
			buff.clear();
			line.clear();
			empty_buff = true;
			empty_line = true;
		}

	public:
		explicit preprocessor(const context_t &context, compiler_type &compiler, const std::deque<char> &char_buff,
		                      std::deque<token_base *> &tokens, charset encoding)
		{
			for (auto &ch:char_buff) {
				if (ch == '\n') {
					process_endline(context, compiler, tokens, encoding);
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
			process_endline(context, compiler, tokens, encoding);
			if (multi_line)
				throw compile_error("Lack of the @end command.");
		}
	};

	void
	compiler_type::process_token_buff(std::deque<token_base *> &tokens, std::deque<std::deque<token_base *>> &ast)
	{
		std::deque<token_base *> oldt, expr;
		std::swap(tokens, oldt);
		tokens.clear();
		for (auto &ptr:oldt) {
			if (ptr->get_type() == token_types::signal &&
			        static_cast<token_signal *>(ptr)->get_signal() == signal_types::endline_)
				ptr = new token_endline(ptr->get_line_num());
			if (ptr->get_type() == token_types::action || ptr->get_type() == token_types::endline) {
				if (!expr.empty()) {
					translator.match_grammar(context, expr);
					for (auto &it:expr)
						tokens.push_back(it);
					expr.clear();
				}
				tokens.push_back(ptr);
			}
			else
				expr.push_back(ptr);
		}
		std::deque<token_base *> tmp;
		for (auto &ptr:tokens) {
			tmp.push_back(ptr);
			if (ptr != nullptr && ptr->get_type() == token_types::endline) {
				if (tmp.size() > 1)
					ast.push_back(tmp);
				tmp.clear();
			}
		}
		if (tmp.size() > 1)
			ast.push_back(tmp);
	}

	void compiler_type::translate_into_tokens(const std::deque<char> &char_buff, std::deque<token_base *> &tokens,
	        charset encoding)
	{
		preprocessor(context, *this, char_buff, tokens, encoding);
	}

	void compiler_type::process_empty_brackets(std::deque<token_base *> &tokens)
	{
		if (tokens.empty())
			throw compile_error("Received empty token buffer.");
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
						throw compile_error("Parentheses do not match.");
					if (blist_stack.front() != 1)
						throw compile_error("The parentheses type does not match.(Request Small Bracket)");
					blist_stack.pop_front();
					if (empty_bracket) {
						empty_bracket = false;
						tokens.push_back(new token_signal(signal_types::esb_));
						continue;
					}
					break;
				case signal_types::mrb_:
					if (blist_stack.empty())
						throw compile_error("Parentheses do not match.");
					if (blist_stack.front() != 2)
						throw compile_error("The parentheses type does not match.(Request Middle Bracket)");
					blist_stack.pop_front();
					if (empty_bracket) {
						empty_bracket = false;
						tokens.push_back(new token_signal(signal_types::emb_));
						continue;
					}
					break;
				case signal_types::lrb_:
					if (blist_stack.empty())
						throw compile_error("Parentheses do not match.");
					if (blist_stack.front() != 3)
						throw compile_error("The parentheses type does not match.(Request Large Bracket)");
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
			throw compile_error("Parentheses do not match.");
	}

	void compiler_type::process_brackets(std::deque<token_base *> &tokens)
	{
		if (tokens.empty())
			throw compile_error("Received empty token buffer.");
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
						throw compile_error("Parentheses do not match.");
					if (blist_stack.front() != 1)
						throw compile_error("The parentheses type does not match.(Request Small Bracket)");
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
						throw compile_error("Parentheses do not match.");
					if (blist_stack.front() != 2)
						throw compile_error("The parentheses type does not match.(Request Middle Bracket)");
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
						throw compile_error("Parentheses do not match.");
					if (blist_stack.front() != 3)
						throw compile_error("The parentheses type does not match.(Request Large Bracket)");
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
			throw compile_error("Parentheses do not match.");
	}
}
