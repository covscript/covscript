#pragma once
/*
* Covariant Script Implementation
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
* Copyright (C) 2017-2023 Michael Lee(李登淳)
*
* This software is registered with the National Copyright Administration
* of the People's Republic of China(Registration Number: 2020SR0408026)
* and is protected by the Copyright Law of the People's Republic of China.
*
* Email:   lee@covariant.cn, mikecovlee@163.com
* Github:  https://github.com/mikecovlee
* Website: http://covscript.org.cn
*/
#include <covscript/impl/runtime.hpp>

namespace cs {
	context_t create_context(const array &);

	context_t create_subcontext(const context_t &);

	class instance_type final : public runtime_type {
		friend class repl;
		friend context_t cs::create_context(const array &);
		friend context_t cs::create_subcontext(const context_t &);

		// Statements
		std::deque<statement_base *> statements;

		// Fiber Stack Pointer
		stack_pointer fiber_sp = nullptr;
		stack_pointer &fiber_stack;
	public:
		// Status
		bool return_fcall = false;
		bool break_block = false;
		bool continue_block = false;
		// Context
		context_t context;

		// Constructor and destructor
		instance_type() = delete;

		explicit instance_type(context_t c) : context(std::move(c)), runtime_type(fiber_sp), fiber_stack(fiber_sp) {}

		instance_type(context_t c, stack_pointer &fsp) : context(std::move(c)), runtime_type(fsp), fiber_stack(fsp) {}

		instance_type(context_t c, std::size_t stack_size) : context(std::move(c)), runtime_type(fiber_sp, stack_size), fiber_stack(fiber_sp) {}

		instance_type(context_t c, stack_pointer &fsp, std::size_t stack_size) : context(std::move(c)), runtime_type(fsp, stack_size), fiber_stack(fsp) {}

		instance_type(const instance_type &) = delete;

		~instance_type() = default;

		// Wrapped Method
		namespace_t import(const std::string &, const std::string &);

		namespace_t source_import(const std::string &);

		void compile(const std::string &);

		void compile(std::istream &);

		void interpret();

		void dump_ast(std::ostream &);

		// Parse variable definition
		void check_declar_var(tree_type<token_base *>::iterator, bool= false);

		void check_define_var(tree_type<token_base *>::iterator, bool= false, bool= false);

		void parse_define_var(tree_type<token_base *>::iterator, bool= false, bool= false);

		void check_define_structured_binding(tree_type<token_base *>::iterator, bool= false);

		void parse_define_structured_binding(tree_type<token_base *>::iterator, bool= false, bool= false);

		// Parse using statement
		void parse_using(tree_type<token_base *>::iterator, bool= false);

		// Coroutines
		void swap_context(stack_type<domain_type> *stack)
		{
			fiber_stack = stack;
		}

		void clear_context()
		{
			if (fiber_stack != nullptr) {
				while (!fiber_stack->empty())
					fiber_stack->pop_no_return();
				fiber_stack = nullptr;
			}
		}
	};

// Repl
	class repl final {
		std::deque<std::deque<token_base *>> tmp;
		stack_type<method_base *> methods;
		charset encoding = charset::utf8;
		std::size_t line_num = 0;
		bool multi_line = false;
		string line_buff;
		string cmd_buff;

		void interpret(const string &, std::deque<token_base *> &);

		void run(const string &);

	public:
		context_t context;

		bool echo = true;

		repl() = delete;

		explicit repl(context_t c) : context(std::move(c))
		{
			context->file_path = "<REPL_ENV>";
			context->compiler->fold_expr = false;
		}

		repl(const repl &) = delete;

		void exec(const string &);

		void reset_status()
		{
			context_t _context = context;
			charset _encoding = encoding;
			std::size_t _line_num = line_num;
			this->~repl();
			::new(this) repl(_context);
			encoding = _encoding;
			line_num = _line_num;
			context->compiler->utilize_metadata();
			context->instance->storage.clear_set();
		}

		int get_level() const
		{
			return methods.size();
		}
	};

// Guarder
	class scope_guard final {
		const context_t &context;
	public:
		scope_guard() = delete;

		explicit scope_guard(const context_t &c) : context(c)
		{
			context->instance->storage.add_domain();
		}

		~scope_guard()
		{
			context->instance->storage.remove_domain();
		}

		const domain_type &get() const
		{
			return context->instance->storage.get_domain();
		}

		void clear() const
		{
			context->instance->storage.next_domain();
		}

		void reset() const
		{
			context->instance->storage.clear_domain();
		}
	};

	class fcall_guard final {
	public:
#ifdef CS_DEBUGGER
		fcall_guard() = delete;

		explicit fcall_guard(const std::string &decl)
		{
			current_process->stack.push(null_pointer);
			current_process->stack_backtrace.push(decl);
		}

		~fcall_guard()
		{
			current_process->stack.pop_no_return();
			current_process->stack_backtrace.pop_no_return();
		}
#else

		fcall_guard()
		{
			current_process->stack.push(null_pointer);
		}

		~fcall_guard()
		{
			current_process->stack.pop_no_return();
		}

#endif

		var &get() const
		{
			return current_process->stack.top();
		}
	};

	std::string get_sdk_path();
}
