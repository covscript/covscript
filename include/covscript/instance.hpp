#pragma once
/*
* Covariant Script Instance
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
* Copyright (C) 2018 Michael Lee(李登淳)
* Email: mikecovlee@163.com
* Github: https://github.com/mikecovlee
*/
#include <covscript/symbols.hpp>
#include <covscript/runtime.hpp>

namespace cs {
	class translator_type final {
	public:
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

	private:
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
	};

	constexpr std::size_t fcall_stack_size = 1024;

	class instance_type final : public runtime_type {
		friend class repl;

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
			{"constant",     action_types::constant_},
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
			'+', '-', '*', '/', '%', '^', ',', '.', '>', '<', '=', '&', '|', '!', '(', ')', '[', ']', '{', '}', ':', '?'
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
			{signal_types::com_,    -1},
			{signal_types::dot_,    15},
			{signal_types::arrow_,  15},
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
		// Constant Pool
		std::deque<var> constant_pool;

		void mark_constant()
		{
			for (auto &it:constant_pool)
				it.constant();
		}

		// Translator
		translator_type translator;

		// Statements
		std::deque<statement_base *> statements;

		// Initializations
		void init_grammar();

		void init_runtime();

	public:
		// Constants
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

		// Context
		context_t context;

		// Status
		bool inside_lambda = false;
		bool no_optimize = false;
		bool return_fcall = false;
		bool break_block = false;
		bool continue_block = false;
		// Refers
		std::forward_list<instance_type> refers;
		// Function Stack
		cov::static_stack<var, fcall_stack_size> fcall_stack;
		// Var definition
		struct define_var_profile {
			std::string id;
			cov::tree<token_base *> expr;
		};

		void parse_define_var(cov::tree<token_base *> &, define_var_profile &);

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

		// Parser and Code Generator
		void kill_action(std::deque<std::deque<token_base *>>, std::deque<statement_base *> &, bool raw = false);

		void translate_into_statements(std::deque<token_base *> &, std::deque<statement_base *> &);

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

		void optimize_expression(cov::tree<token_base *> &tree)
		{
			if (!no_optimize)
				opt_expr(tree, tree.root());
		}

		void opt_expr(cov::tree<token_base *> &, cov::tree<token_base *>::iterator);

		// Constructor and destructor
		instance_type() : context(std::make_shared<context_type>(this))
		{
			struct_builder::reset_counter();
			init_grammar();
			init_runtime();
		}

		instance_type(const instance_type &) = delete;

		~instance_type() = default;

		// Wrapped Method
		extension_t import(const std::string &, const std::string &);

		void compile(const std::string &);

		void interpret();

		static void dump_expr(cov::tree<token_base *>::const_iterator, std::ostream &);

		void dump_ast(std::ostream &);
	};

// Repl
	class repl final {
		std::deque<std::deque<token_base *>> tmp;
		method_base *method = nullptr;
		std::size_t line_num = 0;
		bool multi_line = false;
		string line_buff;
		string cmd_buff;
		int level = 0;

		void run(const string &);

	public:
		context_t context;

		repl() = delete;

		explicit repl(context_t c) : context(std::move(c)) {}

		repl(const repl &) = delete;

		void exec(const string &);

		void reset_status()
		{
			context_t __context = context;
			std::size_t __line_num = line_num;
			this->~repl();
			::new(this) repl(__context);
			line_num = __line_num;
		}

		int get_level() const
		{
			return level;
		}
	};

// Guarder
	class scope_guard final {
		context_t context;
	public:
		scope_guard() = delete;

		explicit scope_guard(context_t c) : context(std::move(std::move(c)))
		{
			context->instance->storage.add_domain();
		}

		~scope_guard()
		{
			context->instance->storage.remove_domain();
		}

		domain_t get() const
		{
			return context->instance->storage.get_domain();
		}

		void clear() const
		{
			context->instance->storage.clear_domain();
		}
	};

	class fcall_guard final {
		context_t context;
	public:
		fcall_guard() = delete;

		explicit fcall_guard(context_t c) : context(std::move(std::move(c)))
		{
			context->instance->fcall_stack.push(null_pointer);
		}

		~fcall_guard()
		{
			context->instance->fcall_stack.pop();
		}

		var get() const
		{
			return context->instance->fcall_stack.top();
		}
	};
}
