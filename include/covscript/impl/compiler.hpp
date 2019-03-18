#pragma once
/*
* Covariant Script Compiler
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
#include <covscript/impl/symbols.hpp>

namespace cs {
	class translator_type final {
		using data_type=std::pair<std::deque<token_base *>, method_base *>;

		static bool compare(const token_base *a, const token_base *b)
		{
			if (a == nullptr)
				return b == nullptr;
			if (b == nullptr)
				return a == nullptr;
			if (a->get_type() != b->get_type())
				return false;
			return !(a->get_type() == token_types::action) || static_cast<const token_action *>(a)->get_action() ==
			       static_cast<const token_action *>(b)->get_action();
		}

		std::list<std::shared_ptr<data_type>> m_data;
	public:
		translator_type() = default;

		translator_type(const translator_type &) = delete;

		~translator_type() = default;

		translator_type &add_method(const std::deque<token_base *> &grammar, method_base *method)
		{
			m_data.emplace_back(std::make_shared<data_type>(grammar, method));
			return *this;
		}

		method_base *match(const std::deque<token_base *> &raw)
		{
			if (raw.size() <= 1)
				throw runtime_error("Grammar error.");
			std::list<std::shared_ptr<data_type>> stack;
			for (auto &it:m_data)
				if (cs::translator_type::compare(it->first.front(), raw.front()))
					stack.push_back(it);
			stack.remove_if([&](const std::shared_ptr<data_type> &dat) {
				return dat->first.size() != raw.size();
			});
			stack.remove_if([&](const std::shared_ptr<data_type> &dat) {
				for (std::size_t i = 1; i < raw.size() - 1; ++i) {
					if (!compare(raw.at(i), dat->first.at(i)))
						return true;
				}
				return false;
			});
			if (stack.empty())
				throw runtime_error("Uknow grammar.");
			if (stack.size() > 1)
				throw runtime_error("Ambiguous grammar.");
			return stack.front()->second;
		}

		void translate(const context_t &, const std::deque<std::deque<token_base *>> &, std::deque<statement_base *> &,
		               bool);
	};

	class compiler_type final {
		// Symbol Table
		mapping<std::string, signal_types> signal_map = {
			{"+",  signal_types::add_},
			{"+=", signal_types::addasi_},
			{"-",  signal_types::sub_},
			{"-=", signal_types::subasi_},
			{"*",  signal_types::mul_},
			{"*=", signal_types::mulasi_},
			{"/",  signal_types::div_},
			{"/=", signal_types::divasi_},
			{"%",  signal_types::mod_},
			{"%=", signal_types::modasi_},
			{"^",  signal_types::pow_},
			{"^=", signal_types::powasi_},
			{">",  signal_types::abo_},
			{"<",  signal_types::und_},
			{"=",  signal_types::asi_},
			{"&&", signal_types::and_},
			{"||", signal_types::or_},
			{"!",  signal_types::not_},
			{"==", signal_types::equ_},
			{"!=", signal_types::neq_},
			{">=", signal_types::aeq_},
			{"<=", signal_types::ueq_},
			{"(",  signal_types::slb_},
			{")",  signal_types::srb_},
			{"[",  signal_types::mlb_},
			{"]",  signal_types::mrb_},
			{"{",  signal_types::llb_},
			{"}",  signal_types::lrb_},
			{",",  signal_types::com_},
			{".",  signal_types::dot_},
			{"()", signal_types::esb_},
			{"[]", signal_types::emb_},
			{"{}", signal_types::elb_},
			{"++", signal_types::inc_},
			{"--", signal_types::dec_},
			{":",  signal_types::pair_},
			{"?",  signal_types::choice_},
			{"->", signal_types::arrow_}
		};
		mapping<std::string, action_types> action_map = {
			{"import",    action_types::import_},
			{"package",   action_types::package_},
			{"using",     action_types::using_},
			{"namespace", action_types::namespace_},
			{"struct",    action_types::struct_},
			{"class",     action_types::struct_},
			{"extends",   action_types::extends_},
			{"block",     action_types::block_},
			{"end",       action_types::endblock_},
			{"var",       action_types::var_},
			{"constant",  action_types::constant_},
			{"do",        action_types::do_},
			{"if",        action_types::if_},
			{"else",      action_types::else_},
			{"switch",    action_types::switch_},
			{"case",      action_types::case_},
			{"default",   action_types::default_},
			{"while",     action_types::while_},
			{"until",     action_types::until_},
			{"loop",      action_types::loop_},
			{"for",       action_types::for_},
			{"foreach",   action_types::foreach_},
			{"break",     action_types::break_},
			{"continue",  action_types::continue_},
			{"function",  action_types::function_},
			{"override",  action_types::override_},
			{"return",    action_types::return_},
			{"try",       action_types::try_},
			{"catch",     action_types::catch_},
			{"throw",     action_types::throw_}
		};
		mapping<std::string, std::function<token_base *()>> reserved_map = {
			{"and",    []() -> token_base * { return new token_signal(signal_types::and_); }},
			{"or",     []() -> token_base * { return new token_signal(signal_types::or_); }},
			{"not",    []() -> token_base * { return new token_signal(signal_types::not_); }},
			{"typeid", []() -> token_base * { return new token_signal(signal_types::typeid_); }},
			{"new",    []() -> token_base * { return new token_signal(signal_types::new_); }},
			{"gcnew",  []() -> token_base * { return new token_signal(signal_types::gcnew_); }},
			{
				"local",  []() -> token_base * {
					return new token_value(var::make_constant<constant_values>(constant_values::local_namepace));
				}
			},
			{
				"global", []() -> token_base * {
					return new token_value(var::make_constant<constant_values>(constant_values::global_namespace));
				}
			},
			{"null",   []() -> token_base * { return new token_value(null_pointer); }},
			{"true",   []() -> token_base * { return new token_value(var::make_constant<bool>(true)); }},
			{"false",  []() -> token_base * { return new token_value(var::make_constant<bool>(false)); }}
		};
		mapping<char, char> escape_map = {
			{'a',  '\a'},
			{'b',  '\b'},
			{'f',  '\f'},
			{'n',  '\n'},
			{'r',  '\r'},
			{'t',  '\t'},
			{'v',  '\v'},
			{'\\', '\\'},
			{'\'', '\''},
			{'\"', '\"'},
			{'0',  '\0'}
		};
		std::deque<char> signals = {
			'+', '-', '*', '/', '%', '^', ',', '.', '>', '<', '=', '&', '|', '!', '(', ')', '[', ']', '{', '}', ':',
			'?'
		};
		mapping<signal_types, int> signal_level_map = {
			{signal_types::add_,    10},
			{signal_types::addasi_, 1},
			{signal_types::sub_,    10},
			{signal_types::subasi_, 1},
			{signal_types::mul_,    11},
			{signal_types::mulasi_, 1},
			{signal_types::div_,    11},
			{signal_types::divasi_, 1},
			{signal_types::mod_,    12},
			{signal_types::modasi_, 1},
			{signal_types::pow_,    12},
			{signal_types::powasi_, 1},
			{signal_types::com_,    0},
			{signal_types::dot_,    15},
			{signal_types::arrow_,  15},
			{signal_types::und_,    9},
			{signal_types::abo_,    9},
			{signal_types::asi_,    1},
			{signal_types::choice_, 3},
			{signal_types::pair_,   4},
			{signal_types::equ_,    9},
			{signal_types::ueq_,    9},
			{signal_types::aeq_,    9},
			{signal_types::neq_,    9},
			{signal_types::lambda_, 2},
			{signal_types::vardef_, 20},
			{signal_types::varprt_, 20},
			{signal_types::or_,     6},
			{signal_types::and_,    7},
			{signal_types::not_,    8},
			{signal_types::inc_,    13},
			{signal_types::dec_,    13},
			{signal_types::fcall_,  15},
			{signal_types::emb_,    15},
			{signal_types::access_, 15},
			{signal_types::typeid_, 14},
			{signal_types::new_,    14},
			{signal_types::gcnew_,  14}
		};
		std::deque<signal_types> signal_left_associative = {
			signal_types::asi_, signal_types::addasi_, signal_types::subasi_, signal_types::mulasi_,
			signal_types::divasi_, signal_types::modasi_, signal_types::powasi_, signal_types::equ_,
			signal_types::und_, signal_types::abo_, signal_types::ueq_, signal_types::aeq_, signal_types::neq_,
			signal_types::and_, signal_types::or_
		};
		// Constants Pool
		std::vector<var> constant_pool;
		// Status
		bool inside_lambda = false;
		bool no_optimize = false;
		// Context
		context_t context;
		// Translator
		translator_type translator;

		// Preprocessor
		class preprocessor;

		// Lexer
		bool issignal(char ch)
		{
			for (auto &c:signals)
				if (c == ch)
					return true;
			return false;
		}

		void process_char_buff(const std::deque<char> &, std::deque<token_base *> &);

		void translate_into_tokens(const std::deque<char> &, std::deque<token_base *> &);

		void process_empty_brackets(std::deque<token_base *> &);

		void process_brackets(std::deque<token_base *> &);

		// AST Builder
		int get_signal_level(token_base *ptr)
		{
			if (ptr == nullptr)
				throw runtime_error("Get the level of null token.");
			if (ptr->get_type() != token_types::signal)
				throw runtime_error("Get the level of non-signal token.");
			return signal_level_map.match(static_cast<token_signal *>(ptr)->get_signal());
		}

		bool is_left_associative(token_base *ptr)
		{
			if (ptr == nullptr)
				throw runtime_error("Get the level of null token.");
			if (ptr->get_type() != token_types::signal)
				throw runtime_error("Get the level of non-signal token.");
			signal_types s = static_cast<token_signal *>(ptr)->get_signal();
			for (auto &t:signal_left_associative)
				if (t == s)
					return true;
			return false;
		}

		void kill_brackets(std::deque<token_base *> &, std::size_t= 1);

		void split_token(std::deque<token_base *> &raw, std::deque<token_base *> &, std::deque<token_base *> &);

		void build_tree(cov::tree<token_base *> &, std::deque<token_base *> &, std::deque<token_base *> &);

		void gen_tree(cov::tree<token_base *> &, std::deque<token_base *> &);

		void kill_expr(std::deque<token_base *> &);

		// Optimizer
		bool optimizable(const cov::tree<token_base *>::iterator &it)
		{
			if (!it.usable())
				return false;
			token_base *token = it.data();
			if (token == nullptr)
				return true;
			switch (token->get_type()) {
			default:
				break;
			case token_types::value:
				return true;
				break;
			}
			return false;
		}

		void trim_expression(cov::tree<token_base *> &tree)
		{
			trim_expr(tree, tree.root());
		}

		void optimize_expression(cov::tree<token_base *> &tree)
		{
			trim_expr(tree, tree.root());
			if (!disable_optimizer && !no_optimize)
				opt_expr(tree, tree.root());
		}

		void trim_expr(cov::tree<token_base *> &, cov::tree<token_base *>::iterator);

		void opt_expr(cov::tree<token_base *> &, cov::tree<token_base *>::iterator);

	public:
		compiler_type() = delete;

		explicit compiler_type(context_t c) : context(std::move(c)) {}

		compiler_type(const compiler_type &) = delete;

		~compiler_type() = default;

		// Settings
		bool disable_optimizer = false;

		// Context
		context_t swap_context(context_t cxt)
		{
			std::swap(context, cxt);
			return cxt;
		}

		// Metadata
		void clear_metadata()
		{
			constant_pool.clear();
		}

		void utilize_metadata()
		{
			for (auto &it:constant_pool)
				it.constant();
		}

		void add_constant(const var &val)
		{
			if (!val.is_protect()) {
				constant_pool.push_back(val);
				constant_pool.back().protect();
				constant_pool.back().mark_as_rvalue(false);
			}
		}

		token_value *new_value(const var &val)
		{
			add_constant(val);
			return new token_value(val);
		}

		// Var definition
		struct define_var_profile {
			std::string id;
			cov::tree<token_base *> expr;
		};

		void parse_define_var(cov::tree<token_base *> &, define_var_profile &);

		// Wrapped Method
		void build_expr(const std::deque<char> &buff, cov::tree<token_base *> &tree)
		{
			std::deque<token_base *> tokens;
			process_char_buff(buff, tokens);
			process_brackets(tokens);
			kill_brackets(tokens);
			gen_tree(tree, tokens);
		}

		void process_line(std::deque<token_base *> &line)
		{
			process_brackets(line);
			kill_brackets(line, static_cast<token_endline *>(line.back())->get_line_num());
			kill_expr(line);
		}

		void build_line(const std::deque<char> &buff, std::deque<token_base *> &line, std::size_t line_num = 1)
		{
			process_char_buff(buff, line);
			line.push_back(new token_endline(line_num));
			process_line(line);
		}

		void build_ast(const std::deque<char> &buff, std::deque<std::deque<token_base *>> &ast)
		{
			std::deque<token_base *> tokens, tmp;
			translate_into_tokens(buff, tokens);
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

		compiler_type &add_method(const std::deque<token_base *> &grammar, method_base *method)
		{
			translator.add_method(grammar, method);
			return *this;
		}

		method_base *match_method(const std::deque<token_base *> &raw)
		{
			return translator.match(raw);
		}

		void translate(const std::deque<std::deque<token_base *>> &ast, std::deque<statement_base *> &code)
		{
			translator.translate(context, ast, code, false);
		}

		void code_gen(const std::deque<std::deque<token_base *>> &ast, std::deque<statement_base *> &code)
		{
			translator.translate(context, ast, code, true);
		}

		// AST Debugger
		static void dump_expr(cov::tree<token_base *>::const_iterator, std::ostream &);
	};
}