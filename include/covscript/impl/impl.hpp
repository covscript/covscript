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
 * Copyright (C) 2017-2026 Michael Lee(李登淳)
 *
 * This software is registered with the National Copyright Administration
 * of the People's Republic of China(Registration Number: 2020SR0408026)
 * and is protected by the Copyright Law of the People's Republic of China.
 *
 * Email:   mikecovlee@163.com
 * Github:  https://github.com/mikecovlee
 * Website: http://covscript.org.cn
 */
#include <covscript/impl/runtime.hpp>
#include <optional>

namespace cs
{
	context_t create_context(const array &, std::size_t stack_size = 0);

	context_t create_subcontext(context_type *);

	inline context_t create_subcontext(const context_t &parent)
	{
		return create_subcontext(parent.get());
	}

	class instance_type final : public runtime_type
	{
		friend class repl;

		friend context_t cs::create_context(const array &, std::size_t stack_size);

		friend context_t cs::create_subcontext(context_type *);

		// Statements
		std::deque<statement_base *> statements;

		// Token arena for the current program; freed when the program is released.
		std::shared_ptr<compile_unit> m_unit;

		// Fiber Stack Pointer
		stack_pointer fiber_sp = nullptr;
		stack_pointer &fiber_stack;

		// Drop the arena reference (lambdas live in the runtime's store, not in
		// tokens, so there is no arena <-> function cycle to break).
		void release_unit()
		{
			m_unit.reset();
		}

	   public:
		// Status
		bool return_fcall = false;
		bool break_block = false;
		bool continue_block = false;
		// Context
		context_type *context;

		// Constructor and destructor
		instance_type() = delete;

		explicit instance_type(context_type *c)
		    : context(c), runtime_type(fiber_sp), fiber_stack(fiber_sp) {}

		instance_type(context_type *c, stack_pointer &fsp)
		    : context(c), runtime_type(fsp), fiber_stack(fsp) {}

		instance_type(context_type *c, std::size_t stack_size)
		    : context(c), runtime_type(fiber_sp, stack_size), fiber_stack(fiber_sp) {}

		instance_type(context_type *c, stack_pointer &fsp, std::size_t stack_size)
		    : context(c), runtime_type(fsp, stack_size), fiber_stack(fsp) {}

		instance_type(const instance_type &) = delete;

		// The compiled program is owned by this instance: statements are freed
		// when the instance dies or the next compile replaces them.
		~instance_type()
		{
			statement_base::delete_children(statements);
			// current_unit is already destroyed here (it dies before the instance
			// during teardown); release_statements() handles the recompile path.
			m_unit.reset();
		}

		// Release the current program (used by compile() and instance teardown).
		void release_statements()
		{
			statement_base::delete_children(statements);
			if (context && context->current_unit == m_unit)
				context->current_unit = nullptr;
			release_unit();
		}

		// The token arena of the current program (tests / embedders).
		const std::shared_ptr<compile_unit> &get_current_unit() const
		{
			return m_unit;
		}

		// Wrapped Method
		namespace_t import(const std::string &, const std::string &);

		namespace_t source_import(const std::string &);

		void compile(const std::string &);

		void compile(std::istream &);

		void interpret();

		void dump_ast(std::ostream &);

		// Parse variable definition
		void check_declar_var(tree_type<token_base *>::iterator, bool = false);

		void check_define_var(tree_type<token_base *>::iterator, bool = false, bool = false);

		// A constant's RHS is usually folded to a token_value; a lambda or a
		// callable-containing value is left un-folded and evaluated at runtime.
		var fold_constant(tree_type<token_base *>::iterator, bool constant);

		void parse_define_var(tree_type<token_base *>::iterator, bool = false, bool = false);

		void check_define_structured_binding(tree_type<token_base *>::iterator, bool = false);

		void parse_define_structured_binding(tree_type<token_base *>::iterator, bool = false, bool = false);

		// Parse using statement
		void parse_using(tree_type<token_base *>::iterator, bool = false);

		// Coroutines
		void swap_context(stack_type<domain_type> *stack)
		{
			fiber_stack = stack;
		}

		void clear_context()
		{
			if (fiber_stack != nullptr)
			{
				while (!fiber_stack->empty())
					fiber_stack->pop_no_return();
				fiber_stack = nullptr;
			}
		}
	};

	// Repl
	class repl final
	{
		std::deque<std::deque<token_base *>> tmp;
		stack_type<method_base *> methods;
		charset encoding = charset::utf8;
		std::size_t line_num = 0;
		bool multi_line = false;
		std::size_t m_run_depth = 0;
		string line_buff;
		string cmd_buff;
		// Token arenas, one per in-flight top-level statement (spans multi-line
		// blocks). A stack rather than a single arena so a re-entrant exec()
		// can restore the outer statement's arena after the nested one finishes;
		// otherwise the outer statement resumes evaluating freed tokens.
		std::vector<std::shared_ptr<compile_unit>> m_units;
		std::vector<std::shared_ptr<compile_unit>> m_saved_units;
		// Compile-time storage snapshot; restored on failure, discarded on success.
		std::optional<domain_manager::domain_snapshot> m_snap;
		// True after interpret() commits; guards snapshot from runtime failure restore (B1).
		bool m_committed = false;

		// Drop the current statement's arena and restore the enclosing one (if
		// any) as the context's current unit (lambdas live in the runtime's
		// store, so there is no arena <-> function cycle to break).
		void pop_unit()
		{
			if (!m_units.empty())
			{
				m_units.pop_back();
				std::shared_ptr<compile_unit> saved_unit = m_saved_units.back();
				m_saved_units.pop_back();
				context->current_unit = m_units.empty() ? saved_unit : m_units.back();
			}
		}

		void interpret(const string &, std::deque<token_base *> &);

		void run(const string &);

	   public:
		context_t context;

		bool echo = true;

		repl() = delete;

		explicit repl(context_t);

		repl(const repl &) = delete;

		~repl()
		{
			if (!methods.empty() || !m_units.empty())
				reset_status();
			// Run global finalizers while the session (process) is still active,
			// not during context teardown when the process is already dying.
			context->instance->storage.clear_global();
			m_units.clear();
			m_saved_units.clear();
		}

		void exec(const string &);

		void reset_status()
		{
			// Each open block pushed a domain/set pair; pop them so a failed
			// REPL line does not leave the storage stacks permanently misaligned.
			std::size_t depth = methods.size();
			tmp.clear();
			while (!methods.empty())
				methods.pop_no_return();
			multi_line = false;
			line_buff.clear();
			cmd_buff.clear();
			context->compiler->utilize_metadata();
			context->compiler->loop_depth = 0;
			context->compiler->end_import_scope();
			// Drop the aborted statement's token arena.
			pop_unit();
			while (depth-- > 0)
			{
				context->instance->storage.remove_set();
				context->instance->storage.remove_domain();
			}
			context->instance->storage.clear_set();
			// Roll back only this statement's slots. Nested committed statements
			// keep their original indices.
			context->instance->functions.rollback_transaction();
		}

		std::size_t get_level() const
		{
			return methods.size();
		}
	};

	// Guarder
	class scope_guard final
	{
		context_type *context;

	   public:
		scope_guard() = delete;

		explicit scope_guard(const context_t &c)
		    : context(c.get())
		{
			context->instance->storage.add_domain();
		}

		explicit scope_guard(context_type *c)
		    : context(c)
		{
			context->instance->storage.add_domain();
		}

		~scope_guard()
		{
			if (context != nullptr)
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

		var &return_fcall()
		{
			// Clean up the current domain before returning the top of the stack
			context->instance->storage.remove_domain();
			context = nullptr;
			// Return the top of the stack and mark it as movable
			var &ret = current_process->stack.top();
			ret.try_move();
			return ret;
		}
	};

	class fcall_guard final
	{
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

	// RAII: bind the shared compiler's context for the scope, restore on exit.
	class context_swap_guard final
	{
		compiler_type *compiler;
		context_type *restore;

	   public:
		context_swap_guard() = delete;

		context_swap_guard(compiler_type &comp, context_type *target)
		    : compiler(&comp), restore(comp.swap_context(target)) {}

		~context_swap_guard()
		{
			compiler->swap_context(restore);
		}
	};

	std::string get_sdk_path();
} // namespace cs
