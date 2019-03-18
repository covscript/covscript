#pragma once
/*
* Covariant Script Implementation
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
#include <covscript/impl/runtime.hpp>

namespace cs {
	constexpr std::size_t fcall_stack_size = 1024;

	class instance_type final : public runtime_type {
		friend class repl;

		// Statements
		std::deque<statement_base *> statements;
	public:
		// Status
		bool return_fcall = false;
		bool break_block = false;
		bool continue_block = false;
		// Context
		context_t context;
		// Function Stack
		cov::static_stack<var, fcall_stack_size> fcall_stack;
#ifdef CS_DEBUGGER
		cov::static_stack<std::string, fcall_stack_size> stack_backtrace;
#endif

		// Constructor and destructor
		instance_type() = delete;

		explicit instance_type(context_t c) : context(std::move(c))
		{
			struct_builder::reset_counter();
		}

		instance_type(const instance_type &) = delete;

		~instance_type() = default;

		// Wrapped Method
		namespace_t import(const std::string &, const std::string &);

		void compile(const std::string &);

		void interpret();

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

		explicit repl(context_t c) : context(std::move(c))
		{
			context->file_path = "<REPL_ENV>";
		}

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

#ifdef CS_DEBUGGER
		explicit fcall_guard(context_t c, const std::string &decl) : context(std::move(std::move(c)))
		{
			context->instance->fcall_stack.push(null_pointer);
			context->instance->stack_backtrace.push(decl);
		}

		~fcall_guard()
		{
			context->instance->fcall_stack.pop();
			context->instance->stack_backtrace.pop();
		}
#else

		explicit fcall_guard(context_t c) : context(std::move(std::move(c)))
		{
			context->instance->fcall_stack.push(null_pointer);
		}

		~fcall_guard()
		{
			context->instance->fcall_stack.pop();
		}

#endif

		var get() const
		{
			return context->instance->fcall_stack.top();
		}
	};
}
