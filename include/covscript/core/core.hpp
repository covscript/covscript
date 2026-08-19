#pragma once
/*
 * Covariant Script Programming Language
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
 *
 * Namespaces:
 * cs: Main Namespace
 * cs_impl: Implement Namespace
 */
// Compiler Detection & Optimization
#if defined(__clang__)
#define COVSCRIPT_COMPILER_CLANG
#define COVSCRIPT_COMPILER_SIMPLE_NAME "clang"
#ifdef CS_AGGRESSIVE_OPTIMIZE
#define COVSCRIPT_ALWAYS_INLINE inline __attribute__((always_inline))
#else
#define COVSCRIPT_ALWAYS_INLINE inline
#endif
#elif defined(__GNUC__)
#define COVSCRIPT_COMPILER_GNUC
#define COVSCRIPT_COMPILER_SIMPLE_NAME "gcc"
#ifdef CS_AGGRESSIVE_OPTIMIZE
#define COVSCRIPT_ALWAYS_INLINE inline __attribute__((always_inline))
#else
#define COVSCRIPT_ALWAYS_INLINE inline
#endif
#elif defined(_MSC_VER)
#define COVSCRIPT_COMPILER_MSVC
#define COVSCRIPT_COMPILER_SIMPLE_NAME "msvc"
#ifdef CS_AGGRESSIVE_OPTIMIZE
#define COVSCRIPT_ALWAYS_INLINE __forceinline
#else
#define COVSCRIPT_ALWAYS_INLINE inline
#endif
#else
#define COVSCRIPT_COMPILER_UNKNOWN
#define COVSCRIPT_COMPILER_SIMPLE_NAME "unknown"
#define COVSCRIPT_ALWAYS_INLINE inline
#endif
// Configs
#ifndef COVSCRIPT_STACK_PRESERVE
#define COVSCRIPT_STACK_PRESERVE 64
#endif
#ifndef COVSCRIPT_FIBER_STACK_LIMIT
#define COVSCRIPT_FIBER_STACK_LIMIT (1024 * 1024)
#endif
#ifndef COVSCRIPT_FIBER_BUSY_WAIT_COEF
#define COVSCRIPT_FIBER_BUSY_WAIT_COEF 0.01
#endif
#ifndef COVSCRIPT_FIBER_BUSY_WAIT_MIN
#define COVSCRIPT_FIBER_BUSY_WAIT_MIN 10
#endif
// Hash Map and Set
#ifndef CS_COMPATIBILITY_MODE
#include <parallel_hashmap/phmap.h>
#else
#include <unordered_map>
#include <unordered_set>
#endif
// STL
#include <forward_list>
#include <type_traits>
#include <functional>
#include <typeindex>
#include <algorithm>
#include <limits>
#include <exception>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <istream>
#include <ostream>
#include <utility>
#include <cstring>
#include <cstdio>
#include <atomic>
#include <chrono>
#include <thread>
#include <mutex>
#include <cctype>
#include <string>
#include <vector>
#include <memory>
#include <cmath>
#include <deque>
#include <list>
// CovScript Headers
#include <covscript/core/components.hpp>
#include <covscript/core/definition.hpp>
#include <covscript/core/variable.hpp>
#include <covscript/core/version.hpp>

namespace cs
{
	// Per-execution-path fiber state (chain + schedule tunables), shared through
	// process_context::fiber_cxt (thread-local default; forks inherit the pointer).
	class fiber_context final
	{
	   public:
		stack_type<fiber_t> stack; // fiber chain (was process_context::fiber_stack)
		double busy_wait_coef = COVSCRIPT_FIBER_BUSY_WAIT_COEF;
		std::size_t busy_wait_min = COVSCRIPT_FIBER_BUSY_WAIT_MIN;

		static fiber_context *current()
		{
			thread_local fiber_context cxt;
			return &cxt;
		}
	};

	// Signal arrival is process-global (handlers may run on any thread); the
	// running process's poll_event() takes these flags and dispatches them.
	class signal_control
	{
		std::atomic<bool> m_sigint_pending{false};
		std::atomic<bool> m_exit_pending{false};

	   public:
		void raise_sigint() noexcept
		{
			m_sigint_pending.store(true, std::memory_order_relaxed);
		}

		void raise_exit() noexcept
		{
			m_exit_pending.store(true, std::memory_order_relaxed);
		}

		bool take_sigint() noexcept
		{
			return m_sigint_pending.exchange(false, std::memory_order_relaxed);
		}

		bool take_exit() noexcept
		{
			return m_exit_pending.exchange(false, std::memory_order_relaxed);
		}
	};

	extern signal_control global_signals;

	// Per-struct type identity: nodes live in the defining context's process
	// pool (fibers/imports share or never allocate there, so both are safe);
	// addresses are unique for the process lifetime. Only native bridging of
	// types across unrelated processes can outlive the base process and
	// falsify is_a.
	struct type_node final
	{
		std::string name;
		const type_node *parent = nullptr;
		set_t<const type_node *> ancestors;
	};

	class process_context final : public std::enable_shared_from_this<process_context>
	{
		// Generation chain (keep-alive); null means the parent is the root.
		std::shared_ptr<process_context> m_parent;

	   public:
		// Version
		const std::string version = COVSCRIPT_VERSION_STR;
		const numeric std_version = COVSCRIPT_STD_VERSION;
		// Output Precision
		int output_precision = 8;
		// Exit code
		int exit_code = 0;
		// Import Path
		std::string import_path = ".";
		// Context being destroyed; finalizers borrow it while its members are intact.
		context_type *teardown_ctx = nullptr;
		// Type identity nodes of structs defined while this process is active.
		// Only ever appended; freed when the process dies.
		std::deque<type_node> type_nodes;
		// Stack
		std::size_t stack_size = COVSCRIPT_STACK_PRESERVE;

		stack_type<var> stack;
#ifdef CS_DEBUGGER
		stack_type<std::string> stack_backtrace;
#endif

		// Shared fiber chain for this execution path (thread-local default; forks
		// inherit the pointer). Access via `fiber_context::current()->stack`.
		fiber_context *const fiber_cxt;

		// Transitional compatibility aliases into fiber_cxt; DEPRECATED, removed in 3.5.3.
		stack_type<fiber_t> &fiber_stack;
		double &fiber_busy_wait_coef;
		std::size_t &fiber_busy_wait_min;

		// Stack Resize must before any context instance start
		void resize_stack(std::size_t size)
		{
			stack_size = size;
			stack.resize(size);
			fiber_cxt->stack.resize(child_stack_size());
#ifdef CS_DEBUGGER
			stack_backtrace.resize(size);
#endif
		}

		inline std::size_t child_stack_size() const
		{
			if (stack_size >= 1000)
				return stack_size / 10;
			else
				return COVSCRIPT_STACK_PRESERVE;
		}

		// Event Handling
		static void cleanup_context();

		static bool on_process_exit_default_handler(void *);

		// SIGINT carries a null payload; the default just ignores it.
		static bool on_process_sigint_default_handler(void *) { return true; }

		event_type on_process_exit;

		// DO NOT TOUCH THIS EVENT DIRECTLY!!
		event_type on_process_sigint;

		// Dispatch pending global signals to this process's own events.
		inline void poll_event()
		{
			if (global_signals.take_sigint())
				on_process_sigint.touch(nullptr);
			// Ctrl+Break is an exit request dispatched via on_process_exit, not a SIGINT reset.
			if (global_signals.take_exit())
			{
				int code = 0;
				on_process_exit.touch(&code);
			}
		}

		// Exception Handling
		std::exception_ptr eptr = nullptr;
		std::mutex eptr_mutex;

		static void cs_defalt_exception_handler(const lang_error &e)
		{
			throw e;
		}

		static void std_defalt_exception_handler(const std::exception &e)
		{
			throw forward_exception(e.what());
		}

		std_exception_handler std_eh_callback = &std_defalt_exception_handler;
		cs_exception_handler cs_eh_callback = &cs_defalt_exception_handler;

		process_context()
		    : fiber_cxt(fiber_context::current()),
		      fiber_stack(fiber_cxt->stack),
		      fiber_busy_wait_coef(fiber_cxt->busy_wait_coef),
		      fiber_busy_wait_min(fiber_cxt->busy_wait_min),
		      on_process_exit(&on_process_exit_default_handler),
		      on_process_sigint(&on_process_sigint_default_handler)
		{
		}

		explicit process_context(std::size_t ss, fiber_context *cxt)
		    : fiber_cxt(cxt),
		      fiber_stack(fiber_cxt->stack),
		      fiber_busy_wait_coef(fiber_cxt->busy_wait_coef),
		      fiber_busy_wait_min(fiber_cxt->busy_wait_min),
		      on_process_exit(&on_process_exit_default_handler),
		      on_process_sigint(&on_process_sigint_default_handler)
		{
			resize_stack(ss);
		}

		// Fork a child process; parent is current_owner(), null for the root.
		static std::shared_ptr<process_context> fork(const std::shared_ptr<process_context> &parent);

		// Owning process of the current execution, or null for the root.
		static std::shared_ptr<process_context> current_owner();

		// Whether this process is `other`, or an ancestor/descendant of it (same
		// forked family), so a fiber's process relates to its owner.
		bool is_related(const process_context *other) const noexcept;
	};

	// Per-thread process indirection; extensions route through the host accessor.
	class process_context_ref
	{
	   public:
		using accessor_t = process_context **(*) ();

		static process_context **thread_slot() noexcept
		{
			static thread_local process_context *slot = nullptr;
			return &slot;
		}

	   private:
		accessor_t m_access = process_context_ref::thread_slot;

	   public:
		process_context_ref() = default;

		process_context_ref(const process_context_ref &) = default;

		process_context_ref &operator=(const process_context_ref &) = default;

		process_context_ref &operator=(process_context *p) noexcept
		{
			*thread_slot() = p;
			return *this;
		}

		process_context *operator->() const noexcept
		{
			return *m_access();
		}

		process_context &operator*() const noexcept
		{
			return **m_access();
		}

		operator process_context *() const noexcept
		{
			return *m_access();
		}

		explicit operator bool() const noexcept
		{
			return *m_access() != nullptr;
		}

		bool operator==(std::nullptr_t) const noexcept
		{
			return *m_access() == nullptr;
		}

		bool operator!=(std::nullptr_t) const noexcept
		{
			return *m_access() != nullptr;
		}

		// Swap the accessor, returning the previous one for later restore.
		accessor_t set_accessor(accessor_t access) noexcept
		{
			accessor_t old = m_access;
			m_access = access;
			return old;
		}
	};

	extern process_context_ref current_process;

	// Host process accessor handed to extension DLLs.
	process_context **current_process_host_accessor();

	// Context
	class context_type final : public std::enable_shared_from_this<context_type>
	{
	   public:
		// Subcontexts from module imports; owned here to break cycles.
		std::vector<context_t> subcontexts;
		compiler_t compiler = nullptr;
		instance_t instance = nullptr;
		std::deque<string> file_buff;
		string file_path = "<Unknown>";
		string package_name;
		var cmd_args;

		// Process owned by this context (subcontexts share the parent's).
		std::shared_ptr<process_context> process;

		// Current compile's token arena; functions keep it alive via m_unit.
		std::shared_ptr<compile_unit> current_unit;

		// Only the context that created the compiler may clear its module cache;
		// subcontexts (module imports) share it and must leave it intact.
		bool owns_compiler = false;

		context_type() = default;

		context_type(const context_type &) = default;

		// Runs structure finalizers; defined in covscript.cpp.
		~context_type();

		// Safe access to a source line by 1-based line number.
		// Returns an empty string when line_num is 0 or out of range.
		const string &get_file_line(std::size_t line_num) const noexcept
		{
			static const string empty_line;
			if (line_num == 0 || line_num > file_buff.size())
				return empty_line;
			return file_buff[line_num - 1];
		}
	};

	// Installs a context's process as current_process. Same-process nesting is
	// transparent; an unrelated active process is rejected.
	class process_run_scope
	{
		process_context *m_prev_process = nullptr;

	   public:
		explicit process_run_scope(const context_t &c)
		    : process_run_scope(c.get()) {}

		explicit process_run_scope(context_type *c)
		{
			if (c == nullptr)
				throw fatal_error("the context is null");
			process_context *p = c->process.get();
			if (p == nullptr)
				throw fatal_error("the context has no process attached");
			if (current_process != nullptr && current_process != p && !current_process->is_related(p))
				throw fatal_error("another Covscript instance is already running on this thread");
			m_prev_process = current_process;
			current_process = p;
		}

		explicit process_run_scope(process_context *p)
		{
			if (p == nullptr)
				throw fatal_error("the process is null");
			if (current_process != nullptr && current_process != p && !current_process->is_related(p))
				throw fatal_error("another Covscript instance is already running on this thread");
			m_prev_process = current_process;
			current_process = p;
		}

		~process_run_scope()
		{
			current_process = m_prev_process;
		}
	};

	// Callable and Function
	class callable final
	{
	   public:
		using function_type = std::function<var(vector &)>;
		enum class types
		{
			normal,
			request_fold,
			member_fn,
			member_visitor,
			force_regular
		};

	   private:
		function_type mFunc;
		types mType = types::normal;

	   public:
		callable() = delete;

		callable(const callable &) = default;

		explicit callable(function_type func, types type = types::normal)
		    : mFunc(std::move(func)), mType(type) {}

		bool is_request_fold() const
		{
			return mType == types::request_fold;
		}

		bool is_member_fn() const
		{
			return mType == types::member_fn;
		}

		types type() const
		{
			return mType;
		}

		var call(vector &args) const
		{
			return mFunc(args);
		}

		// Argument count of the underlying function (script function or CNI);
		// defined in covscript.cpp where function_ptr/cni are complete.
		std::size_t argument_count() const;

		const function_type &get_raw_data() const
		{
			return mFunc;
		}
	};

	class future_type
	{
	   protected:
		future_type() = default;

	   public:
		future_type(const future_type &) = delete;
		future_type &operator=(const future_type &) = delete;

		virtual ~future_type() = default;

		virtual bool wait_for(std::size_t) = 0;

		virtual void wait() = 0;

		virtual var get() = 0;
	};

	enum class fiber_state
	{
		ready,
		running,
		suspended,
		sleeping,
		finished
	};

	class fiber_type
	{
	   public:
		std::chrono::steady_clock::time_point wake_up_time{};
		std::size_t busy_skip_count = 0;

	   protected:
		fiber_type() = default;

	   public:
		fiber_type(const fiber_type &) = delete;
		fiber_type &operator=(const fiber_type &) = delete;

		virtual ~fiber_type() = default;

		virtual fiber_state get_state() const = 0;

		virtual var return_value() const = 0;

		// The process this fiber runs on, or null for native fibers.
		virtual const std::shared_ptr<process_context> &get_process() const noexcept = 0;
	};

	namespace fiber
	{
		enum class schedule_policy
		{
			normal,
			no_backpressure,
		};

		inline fiber_type const *current()
		{
			return cs::fiber_context::current()->stack.empty() ? nullptr
			                                                   : cs::fiber_context::current()->stack.top().get();
		}

		inline bool within()
		{
			return !cs::fiber_context::current()->stack.empty();
		}

		fiber_t create(context_type *, std::function<var()>);

		inline fiber_t create(const context_t &c, std::function<var()> fn)
		{
			return create(c.get(), std::move(fn));
		}

		fiber_t create_native(std::function<var()>);

		future_t get_future(const fiber_t &);

		struct schedule_parameters
		{
			double busy_wait_coef;
			std::size_t busy_wait_min;
		};

		schedule_parameters get_schedule_parameters();
		void set_schedule_parameters(const schedule_parameters &);

		void resume(const fiber_t &, schedule_policy = schedule_policy::normal);

		void sleep_for(std::size_t);

		void yield();
	} // namespace fiber

	class function final
	{
		// Weak back-ref to the defining context; calls lock it.
		std::weak_ptr<context_type> mContext;
#ifdef CS_DEBUGGER
		// Source location for debugger breakpoints (immutable after construction).
		mutable bool mMatch = false;
		std::string mDecl;
		std::string mFile;
		std::size_t mLine = 0;
#endif
		bool mIsMemFn = false;
		bool mIsVargs = false;
		bool mIsLambda = false;
		std::vector<std::string> mArgs;
		std::deque<statement_base *> mBody;
		// Keeps the compiling unit's token arena alive for an escaped function.
		std::shared_ptr<compile_unit> m_unit;

		static var call_rr(const function *, vector &);

		static var call_vv(const function *, vector &);

		static var call_rl(const function *, vector &);

		static var call_el(const function *, vector &);

		var (*call_ptr)(const function *, vector &) = nullptr;

		inline void init_call_ptr() noexcept
		{
			if (!mIsVargs)
			{
				if (mIsLambda)
					call_ptr = mArgs.empty() ? &call_el : &call_rl;
				else
					call_ptr = &call_rr;
			}
			else
				call_ptr = &call_vv;
		}

	   public:
		function() = delete;

		function(const function &) = delete;

		function &operator=(const function &) = delete;

#ifdef CS_DEBUGGER
		function(context_type *c, std::string decl, std::string file, std::size_t line,
		         std::vector<std::string> args, std::deque<statement_base *> body,
		         bool is_vargs = false, bool is_lambda = false)
		    : mContext(c->weak_from_this()), mDecl(std::move(decl)), mFile(std::move(file)), mLine(line), mIsVargs(is_vargs), mIsLambda(is_lambda), mArgs(std::move(args)), mBody(std::move(body)), m_unit(c->current_unit)
		{
			init_call_ptr();
		}
#else

		function(context_type *c, std::vector<std::string> args, std::deque<statement_base *> body, bool is_vargs = false, bool is_lambda = false)
		    : mContext(c->weak_from_this()), mIsVargs(is_vargs), mIsLambda(is_lambda), mArgs(std::move(args)), mBody(std::move(body)), m_unit(c->current_unit)
		{
			init_call_ptr();
		}

#endif

		// Owns the function body; deletes the statements recursively. Defined in
		// statement.cpp where statement_base is complete.
		~function();

		// The function body statements, owned by this function.
		const std::deque<statement_base *> &get_body() const
		{
			return mBody;
		}

		var call(vector &args) const
		{
			return call_ptr(this, args);
		}

		var operator()(vector &args) const
		{
			return call_ptr(this, args);
		}

		// Locks the defining context (caller must keep it alive).
		std::shared_ptr<context_type> get_context() const
		{
			return mContext.lock();
		}

		bool is_el_func() const
		{
			return call_ptr == &call_el;
		}

		void add_reserve_var(std::string_view reserve, bool is_mem_fn = false)
		{
			mIsMemFn = is_mem_fn;
			if (!mIsVargs)
			{
				std::vector<std::string> args;
				args.reserve(mArgs.size() + 1);
				args.emplace_back(reserve);
				for (auto &name : mArgs)
				{
					if (name != reserve)
						args.emplace_back(std::move(name));
					else
						throw runtime_error("Overwrite the default argument \"" + std::string(reserve) + "\".");
				}
				std::swap(mArgs, args);
			}
#ifdef CS_DEBUGGER
			std::string prefix, suffix;
			auto lpos = mDecl.find('(') + 1;
			auto rpos = mDecl.rfind(')');
			prefix = mDecl.substr(0, lpos);
			suffix = mDecl.substr(rpos);
			if (mArgs.size() > 1 || mIsVargs)
				mDecl = prefix + "this, " + mDecl.substr(lpos, rpos - lpos) + suffix;
			else
				mDecl = prefix + "this" + suffix;
#endif
		}

		std::size_t argument_count() const noexcept
		{
			return mArgs.size();
		}

#ifdef CS_DEBUGGER
		const std::string &get_declaration() const
		{
			return mDecl;
		}

		const std::string &get_debug_file() const
		{
			return mFile;
		}

		std::size_t get_debug_line() const noexcept
		{
			return mLine;
		}

		void set_debugger_state(bool match) const
		{
			mMatch = match;
		}
#endif
	};

	struct function_ptr final
	{
		function *fptr = nullptr;
		// Keeps the function alive while any callable references it.
		std::shared_ptr<function> owner;

		function_ptr() = default;

		function_ptr(function *f, std::shared_ptr<function> owner_ref)
		    : fptr(f), owner(std::move(owner_ref)) {}

		var operator()(vector &args) const
		{
			return fptr->call(args);
		}
	};

	struct object_method final
	{
		var_borrower object;
		var callable;
		bool is_request_fold = false;

		object_method() = delete;

		object_method(var_borrower obj, var func, bool request_fold = false)
		    : object(std::move(obj)), callable(std::move(func)), is_request_fold(request_fold) {}

		~object_method() = default;
		// Self-referencing lambdas borrow their proxy; the original must outlive copies.
	};

	// Copy
	void copy_no_return(var &);

	var copy(var);

	// Move Semantics
	var lvalue(const var &);

	var rvalue(const var &);

	var try_move(const var &);

	// Invoke through the dispatched call pipeline (prep_call + fcall), so any
	// callable type (callable, object_method, structure's op_call, ...) works.
	// Mirrors runtime_type::parse_fcall: prep_call first so a receiver (e.g. an
	// object_method's `self`) lands at the front of the argument list.
	template <typename... ArgsT>
	static var invoke(const var &func, ArgsT &&..._args)
	{
		vector args;
		func.prep_call(args);
		args.reserve(args.size() + sizeof...(ArgsT));
		(args.push_back(std::forward<ArgsT>(_args)), ...);
		return func.fcall(args);
	}

	// Type and struct
	struct pointer final
	{
		var data;

		pointer() = default;

		explicit pointer(var v)
		    : data(std::move(v)) {}

		bool operator==(const pointer &ptr) const
		{
			return data.is_same(ptr.data);
		}
	};

	static const pointer null_pointer = {};

	type_node *alloc_type_node(process_context *p);

	struct type_id final
	{
		std::type_index type_idx;
		const type_node *node = nullptr;

		type_id() = delete;

		type_id(const std::type_index &id, const type_node *n = nullptr)
		    : type_idx(id), node(n) {}

		inline bool is_a(const type_id &id) const
		{
			if (node && id.node)
				return node == id.node || node->ancestors.count(id.node) > 0;
			else
				return type_idx == id.type_idx;
		}

		inline bool compare(const type_id &id) const
		{
			if (node && id.node)
				return node == id.node;
			else
				return type_idx == id.type_idx;
		}

		inline bool operator==(const type_id &id) const
		{
			return compare(id);
		}

		inline bool operator!=(const type_id &id) const
		{
			return !compare(id);
		}
	};

	struct domain_ref final
	{
		domain_type *domain = nullptr;

		domain_ref(domain_type *ptr)
		    : domain(ptr) {}
	};

	class var_id final
	{
		friend class domain_type;

		friend class domain_manager;

		mutable std::size_t m_domain_id = 0, m_slot_id = 0;
		mutable std::shared_ptr<domain_ref> m_ref;
		std::string m_id;

	   public:
		var_id() = delete;

		explicit var_id(std::string_view name)
		    : m_id(name) {}

		var_id(const var_id &) = default;

		var_id(var_id &&) noexcept = default;

		var_id &operator=(const var_id &) = default;

		var_id &operator=(var_id &&) = default;

		inline void set_id(std::string_view id)
		{
			m_id = id;
		}

		inline const std::string &get_id() const noexcept
		{
			return m_id;
		}

		inline operator std::string &() noexcept
		{
			return m_id;
		}

		inline operator const std::string &() const noexcept
		{
			return m_id;
		}
	};

	class domain_type final
	{
		// Owning keys, so a domain survives recompilation (the arena and
		// statements that owned the original names are released).
		map_t<std::string, std::size_t> m_reflect;
		std::shared_ptr<domain_ref> m_ref;
		std::vector<var> m_slot;
		bool optimize = false;

		// phmap keys std::string with a transparent hash (zero-copy string_view
		// lookup); std::unordered_map needs a materialized std::string.
		inline auto reflect_find(std::string_view name) const
		{
#ifdef CS_COMPATIBILITY_MODE
			return m_reflect.find(std::string(name));
#else
			return m_reflect.find(name);
#endif
		}

		inline std::size_t get_slot_id(std::string_view name) const
		{
			auto it = reflect_find(name);
			if (it != m_reflect.end())
				return it->second;
			else
				throw runtime_error("Use of undefined variable \"" + std::string(name) + "\".");
		}

	   public:
		domain_type()
		    : m_ref(std::make_shared<domain_ref>(this)) {}

		domain_type(const domain_type &domain)
		    : m_reflect(domain.m_reflect), m_ref(std::make_shared<domain_ref>(this)), m_slot(domain.m_slot) {}

		domain_type(domain_type &&domain) noexcept
		    : m_ref(std::make_shared<domain_ref>(this))
		{
			std::swap(m_reflect, domain.m_reflect);
			std::swap(m_slot, domain.m_slot);
		}

		~domain_type()
		{
			m_ref->domain = nullptr;
		}

		void clear();

		void safe_rewind();

		inline void next() noexcept
		{
			optimize = true;
		}

		inline bool consistence(const var_id &id) const noexcept
		{
			return id.m_ref == m_ref;
		}

		inline bool exist(std::string_view name) const noexcept
		{
			return reflect_find(name) != m_reflect.end();
		}

		inline bool exist(const var_id &id) const noexcept
		{
			return m_reflect.find(id.m_id) != m_reflect.end();
		}

		domain_type &add_var(const char *name, const var &val)
		{
			auto it = reflect_find(name);
			if (it == m_reflect.end())
			{
				m_slot.push_back(val);
				m_reflect.emplace(name, m_slot.size() - 1);
			}
			else
				m_slot[it->second] = val;
			return *this;
		}

		domain_type &add_var(const var_id &id, const var &val)
		{
			auto it = m_reflect.find(id.m_id);
			if (it == m_reflect.end())
			{
				m_slot.push_back(val);
				m_reflect.emplace(id.m_id, m_slot.size() - 1);
				id.m_slot_id = m_slot.size() - 1;
				id.m_ref = m_ref;
			}
			else
			{
				if (id.m_ref != m_ref)
				{
					id.m_slot_id = it->second;
					id.m_ref = m_ref;
				}
				m_slot[id.m_slot_id] = val;
			}
			return *this;
		}

		bool add_var_optimal(const char *name, const var &val, bool override = false)
		{
			auto it = reflect_find(name);
			if (it != m_reflect.end())
			{
				if (optimize)
				{
					m_slot[it->second] = val;
					return true;
				}
				else if (override)
				{
					m_slot[it->second] = val;
					return true;
				}
				else
					return false;
			}
			else
			{
				m_slot.push_back(val);
				m_reflect.emplace(name, m_slot.size() - 1);
				return true;
			}
		}

		bool add_var_optimal(const var_id &id, const var &val, bool override = false)
		{
			if (id.m_ref == m_ref)
			{
				if (optimize)
				{
					m_slot[id.m_slot_id] = val;
					return true;
				}
				else if (override)
				{
					add_var(id, val);
					return true;
				}
				else
					return false;
			}
			else
			{
				add_var(id, val);
				return true;
			}
		}

		var &get_var(const var_id &id)
		{
			if (id.m_ref != m_ref)
			{
				id.m_slot_id = get_slot_id(id.m_id);
				id.m_ref = m_ref;
			}
			return m_slot[id.m_slot_id];
		}

		const var &get_var(const var_id &id) const
		{
			if (id.m_ref != m_ref)
			{
				id.m_slot_id = get_slot_id(id.m_id);
				id.m_ref = m_ref;
			}
			return m_slot[id.m_slot_id];
		}

		var &get_var(std::string_view name)
		{
			auto it = reflect_find(name);
			if (it != m_reflect.end())
				return m_slot[it->second];
			else
				throw runtime_error("Use of undefined variable \"" + std::string(name) + "\".");
		}

		const var &get_var(std::string_view name) const
		{
			auto it = reflect_find(name);
			if (it != m_reflect.end())
				return m_slot[it->second];
			else
				throw runtime_error("Use of undefined variable \"" + std::string(name) + "\".");
		}

		var *get_var_opt(const var_id &id)
		{
			if (id.m_ref != m_ref)
			{
				auto it = m_reflect.find(id.m_id);
				if (it == m_reflect.end())
					return nullptr;
				id.m_slot_id = it->second;
				id.m_ref = m_ref;
			}
			return &m_slot[id.m_slot_id];
		}

		const var *get_var_opt(const var_id &id) const
		{
			if (id.m_ref != m_ref)
			{
				auto it = m_reflect.find(id.m_id);
				if (it == m_reflect.end())
					return nullptr;
				id.m_slot_id = it->second;
				id.m_ref = m_ref;
			}
			return &m_slot[id.m_slot_id];
		}

		var *get_var_opt(std::string_view name)
		{
			auto it = reflect_find(name);
			if (it != m_reflect.end())
				return &m_slot[it->second];
			else
				return nullptr;
		}

		const var *get_var_opt(std::string_view name) const
		{
			auto it = reflect_find(name);
			if (it != m_reflect.end())
				return &m_slot[it->second];
			else
				return nullptr;
		}

		var &get_var_no_check(const var_id &id)
		{
			if (id.m_ref != m_ref)
			{
				id.m_slot_id = m_reflect.at(id.m_id);
				id.m_ref = m_ref;
			}
			return m_slot[id.m_slot_id];
		}

		const var &get_var_no_check(const var_id &id) const
		{
			if (id.m_ref != m_ref)
			{
				id.m_slot_id = m_reflect.at(id.m_id);
				id.m_ref = m_ref;
			}
			return m_slot[id.m_slot_id];
		}

		var &get_var_no_check(const var_id &id, std::size_t domain_id)
		{
			id.m_domain_id = domain_id;
			if (id.m_ref != m_ref)
			{
				id.m_slot_id = m_reflect.at(id.m_id);
				id.m_ref = m_ref;
			}
			return m_slot[id.m_slot_id];
		}

		inline auto begin() const
		{
			return m_reflect.cbegin();
		}

		inline auto end() const
		{
			return m_reflect.cend();
		}

		// Caution! Only use for traverse!
		inline var &get_var_by_id(std::size_t id)
		{
			return m_slot[id];
		}

		inline const var &get_var_by_id(std::size_t id) const
		{
			return m_slot[id];
		}
	};

	struct type_t final
	{
		std::function<var()> constructor;
		namespace_t extensions;
		type_id id;

		type_t() = delete;

		type_t(std::function<var()> c, const type_id &i)
		    : constructor(std::move(c)), id(i) {}

		type_t(std::function<var()> c, const type_id &i, namespace_t ext)
		    : constructor(std::move(c)), id(i), extensions(std::move(ext)) {}

		template <typename T>
		var &get_var(T &&) const;
	};

	class range_iterator final
	{
		numeric m_step, m_index;

	   public:
		range_iterator() = delete;

		explicit range_iterator(numeric step, numeric index)
		    : m_step(std::move(step)), m_index(std::move(index)) {}

		range_iterator(const range_iterator &) = default;

		range_iterator(range_iterator &&) noexcept = default;

		range_iterator &operator=(const range_iterator &) = default;

		bool operator==(const range_iterator &it) const
		{
			return m_index == it.m_index;
		}

		bool operator!=(const range_iterator &it) const
		{
			// Iterate while index is on the step's side of end (asc: <, desc: >).
			if (m_step > 0)
				return m_index < it.m_index;
			if (m_step < 0)
				return m_index > it.m_index;
			return false; // Step 0 is already rejected at range() construction
		}

		range_iterator &operator++()
		{
			// Clamp on overflow so a wrapped index can't spin forever.
			if (m_step.is_integer() && m_index.is_integer())
			{
				const numeric_integer step = m_step.as_integer();
				const numeric_integer idx = m_index.as_integer();
				if ((step > 0 && idx > (std::numeric_limits<numeric_integer>::max)() - step) ||
				    (step < 0 && idx < (std::numeric_limits<numeric_integer>::min)() - step))
				{
					m_index = step > 0 ? (std::numeric_limits<numeric_integer>::max)()
					                   : (std::numeric_limits<numeric_integer>::min)();
					return *this;
				}
			}
			m_index = m_index + m_step;
			return *this;
		}

		numeric operator*() const
		{
			return m_index;
		}
	};

	class range_type final
	{
		numeric m_start, m_stop, m_step;

	   public:
		range_type() = delete;

		range_type(numeric start, numeric stop, numeric step)
		    : m_start(std::move(start)), m_stop(std::move(stop)), m_step(std::move(step)) {}

		range_type(const range_type &) = default;

		range_type(range_type &&) noexcept = default;

		range_type &operator=(const range_type &) = default;

		range_iterator begin() const
		{
			return range_iterator(m_step, m_start);
		}

		range_iterator end() const
		{
			return range_iterator(m_step, m_stop);
		}

		bool empty() const
		{
			return m_start == m_stop;
		}
	};

	class structure final
	{
		// Pins its owning process so the type node and members outlive the
		// context; only calling a method (function::mContext back-ref) needs it.
		bool m_shadow = false;
		// Set once finalize has run, so an explicit pre-clear finalization (the
		// global domain runs finalizers before releasing its symbol table) does
		// not run the finalizer a second time from the destructor.
		mutable bool m_finalized = false;
		std::string m_name;
		domain_t m_data;
		type_id m_id;
		// Owning process; finalizers run with it activated.
		std::shared_ptr<process_context> m_process;

	   public:
		structure() = delete;

		structure(const type_id &id, std::string name, const domain_type &data)
		    : m_id(id),
		      m_name(std::move(name)),
		      m_data(std::make_shared<domain_type>(data)),
		      m_process(current_process ? current_process->shared_from_this() : nullptr)
		{
			if (m_data->exist("initialize"))
				invoke(m_data->get_var("initialize"), var::make<structure>(this));
		}

		structure(structure &&s) noexcept
		    : m_shadow(s.m_shadow), m_data(nullptr), m_id(typeid(void))
		{
			s.m_shadow = true;
			std::swap(m_name, s.m_name);
			std::swap(m_data, s.m_data);
			std::swap(m_id, s.m_id);
			std::swap(m_process, s.m_process);
			std::swap(m_finalized, s.m_finalized);
		}

		structure(const structure &s)
		    : m_id(s.m_id), m_name(s.m_name), m_data(std::make_shared<domain_type>()), m_process(s.m_process)
		{
			if (s.m_data->exist("parent"))
			{
				var &_p = s.m_data->get_var("parent");
				auto &_parent = _p.val<structure>();
				var p = copy(_p);
				auto &parent = p.val<structure>();
				m_data->add_var("parent", p);
				for (auto &it : *parent.m_data)
				{
					// Handle overriding
					const var &v = s.m_data->get_var(it.first);
					if (!_parent.m_data->get_var(it.first).is_same(v))
						m_data->add_var(it.first.data(), copy(v));
					else
						m_data->add_var(it.first.data(), parent.m_data->get_var_by_id(it.second));
				}
			}
			for (auto &it : *s.m_data)
				if (!m_data->exist(it.first))
					m_data->add_var(it.first.data(), copy(s.m_data->get_var_by_id(it.second)));
			if (m_data->exist("duplicate"))
				invoke(m_data->get_var("duplicate"), var::make<structure>(this), var::make<structure>(&s));
		}

		explicit structure(const structure *s)
		    : m_shadow(true), m_id(s->m_id), m_name(s->m_name), m_data(s->m_data), m_process(s->m_process) {}

		// Runs the structure's `finalize` method (if any) exactly once, while
		// the runtime is still usable. Failures never propagate (finalize runs
		// inside implicitly noexcept destructor paths); they are reported through
		// cs_impl::debug_guard, i.e. governed by the COVSCRIPT_DEBUG level.
		void run_finalize() const
		{
			if (!m_shadow && !m_finalized && m_data->exist("finalize"))
			{
				m_finalized = true;
				// Stack buffer, not std::string: this runs on destructor paths
				// (possibly mid-unwind), so the handlers must not allocate.
				const auto report = [](const char *what)
				{
					char msg[512];
					std::snprintf(msg, sizeof(msg), "[finalize] structure finalizer failed: %.400s", what);
					cs_impl::debug_guard(msg);
				};
				// Cooperative exit/signal sentinels are control flow, not failures.
				const auto sentinel = [](const std::string &msg)
				{
					return msg == "CS_EXIT" || msg == "CS_SIGINT" || msg == "CS_DEBUGGER_EXIT";
				};
				try
				{
					process_run_scope scope(m_process.get());
					invoke(m_data->get_var("finalize"), var::make<structure>(this));
				}
				catch (const exception &e)
				{
					if (!sentinel(e.message()))
						report(e.what());
				}
				catch (const fatal_error &e)
				{
					if (!sentinel(e.message()))
						report(e.what());
				}
				catch (const lang_error &e)
				{
					report(e.what());
				}
				catch (const std::exception &e)
				{
					report(e.what());
				}
				catch (...)
				{
					report("unknown error");
				}
			}
		}

		~structure()
		{
			run_finalize();
		}

		structure &operator=(structure &&s) noexcept
		{
			std::swap(m_shadow, s.m_shadow);
			std::swap(m_name, s.m_name);
			std::swap(m_data, s.m_data);
			std::swap(m_id, s.m_id);
			std::swap(m_process, s.m_process);
			std::swap(m_finalized, s.m_finalized);
			return *this;
		}

		bool operator==(const structure &s) const
		{
			if (s.m_id != m_id)
				return false;
			if (!m_shadow && m_data->exist("equal"))
				return invoke(m_data->get_var("equal"), var::make<structure>(this),
				              var::make<structure>(&s))
				    .const_val<bool>();
			else
			{
				for (auto &it : *m_data)
					if (it.first != "parent" && s.m_data->get_var(it.first) != m_data->get_var_by_id(it.second))
						return false;
				return true;
			}
		}

		const std::string &type_name() const
		{
			return m_name;
		}

		const domain_type &get_domain() const
		{
			return *m_data;
		}

		const type_id &get_id() const
		{
			return m_id;
		}

		template <typename T>
		var &get_var(T &&name) const
		{
			var *ptr = m_data->get_var_opt(name);
			if (ptr != nullptr)
				return *ptr;
			else
				throw runtime_error("Struct \"" + m_name + "\" have no member called \"" + std::string(name) + "\".");
		}
	};

	// Defined here (after `structure`) so finalizers can run during slot destruction.
	inline void domain_type::clear()
	{
		m_reflect.clear();
		// Invalidate cached var_ids before destroying vars.
		m_ref = std::make_shared<domain_ref>(this);
		m_slot.clear();
		optimize = false;
	}

	// Reverse-order destruction: each slot's reflect entry is removed before
	// its var is destroyed. Cached var_ids are invalidated up front.
	inline void domain_type::safe_rewind()
	{
		m_ref = std::make_shared<domain_ref>(this);
		while (!m_slot.empty())
		{
			std::size_t idx = m_slot.size() - 1;
			for (auto it = m_reflect.begin(); it != m_reflect.end();)
				it = (it->second == idx) ? m_reflect.erase(it) : std::next(it);
			m_slot.pop_back();
		}
		optimize = false;
	}

	class struct_builder final
	{
		// Weak back-ref to the defining context.
		std::weak_ptr<context_type> mContext;
		type_node *mNode;
		// Pins the owning process so the type node pool outlives the builder.
		std::shared_ptr<process_context> m_process;
		type_id mTypeId;
		std::string mName;
		tree_type<token_base *> mParent;
		// Statements owned by the shared control block's deleter.
		struct method_storage
		{
			std::deque<statement_base *> methods;
			explicit method_storage(std::deque<statement_base *> m)
			    : methods(std::move(m)) {}
			~method_storage();
		};
		std::shared_ptr<method_storage> mMethod;
		// Keeps member-definition arenas alive across recompilation.
		std::shared_ptr<compile_unit> m_unit;

	   public:
		struct_builder() = delete;

		struct_builder(context_type *c, std::string name, tree_type<token_base *> parent,
		               std::deque<statement_base *> method)
		    : mContext(c->weak_from_this()),
		      mNode(alloc_type_node(c->process.get())),
		      m_process(c->process),
		      mTypeId(typeid(structure), mNode),
		      mName(std::move(name)),
		      mParent(std::move(parent)),
		      mMethod(std::make_shared<method_storage>(std::move(method))),
		      m_unit(c->current_unit)
		{
			mNode->name = mName;
		}

		struct_builder(const struct_builder &) = default;

		struct_builder &operator=(const struct_builder &other)
		{
			if (this != &other)
				struct_builder(other).swap(*this);
			return *this;
		}

		~struct_builder() = default;

		// The method body statements, owned by this builder.
		const std::deque<statement_base *> &get_methods() const
		{
			return mMethod->methods;
		}

		const type_id &get_id() const
		{
			return mTypeId;
		}

		void swap(struct_builder &other) noexcept
		{
			mContext.swap(other.mContext);
			std::swap(mNode, other.mNode);
			m_process.swap(other.m_process);
			std::swap(mTypeId, other.mTypeId);
			mName.swap(other.mName);
			mParent.swap(other.mParent);
			mMethod.swap(other.mMethod);
			m_unit.swap(other.m_unit);
		}

		void do_inherit();

		var operator()();
	};

	// Namespace and extensions
	class name_space
	{
		domain_type *m_data = nullptr;

	   public:
		name_space()
		    : m_data(new domain_type) {}

		name_space(const name_space &ns)
		    : m_data(new domain_type)
		{
			copy_namespace(ns);
		}

		explicit name_space(domain_type dat)
		    : m_data(new domain_type(std::move(dat))) {}

		virtual ~name_space()
		{
			delete m_data;
		}
		template <typename T>
		name_space &add_var(T &&id, const var &var)
		{
			m_data->add_var(std::forward<T>(id), var);
			return *this;
		}

		var &get_var(std::string_view name)
		{
			return m_data->get_var(name);
		}

		const var &get_var(std::string_view name) const
		{
			return m_data->get_var(name);
		}

		var &get_var(const var_id &id)
		{
			return m_data->get_var(id);
		}

		const var &get_var(const var_id &id) const
		{
			return m_data->get_var(id);
		}

		var *get_var_opt(std::string_view name)
		{
			return m_data->get_var_opt(name);
		}

		const var *get_var_opt(std::string_view name) const
		{
			return m_data->get_var_opt(name);
		}

		var *get_var_opt(const var_id &id)
		{
			return m_data->get_var_opt(id);
		}

		const var *get_var_opt(const var_id &id) const
		{
			return m_data->get_var_opt(id);
		}

		domain_type &get_domain() const
		{
			return *m_data;
		}

		inline void copy_namespace(const name_space &ns)
		{
			copy_domain(*ns.m_data);
		}

		void copy_domain(const domain_type &domain)
		{
			for (auto &it : domain)
				m_data->add_var(it.first.data(), domain.get_var_by_id(it.second));
		}

		name_space &operator=(const name_space &ns)
		{
			if (&ns != this)
			{
				m_data->clear();
				copy_namespace(ns);
			}
			return *this;
		}
	};

	template <typename T>
	var &type_t::get_var(T &&name) const
	{
		if (extensions.get() != nullptr)
			return extensions->get_var(name);
		else
			throw runtime_error("Type doesn't have extension field.");
	}

	// Bump allocator; per-allocation alignment keeps strict-aligned types valid.
	class memory_arena final
	{
		struct chunk
		{
			std::unique_ptr<std::byte[]> data;
			std::size_t size;
			std::size_t used = 0;
		};
		std::vector<chunk> chunks;
		static constexpr std::size_t chunk_capacity = 64 * 1024;

	   public:
		memory_arena() = default;

		memory_arena(const memory_arena &) = delete;

		memory_arena &operator=(const memory_arena &) = delete;

		void *allocate(std::size_t size, std::size_t align)
		{
			std::size_t aligned = chunks.empty() ? 0 : (chunks.back().used + align - 1) & ~(align - 1);
			if (chunks.empty() || aligned + size > chunks.back().size)
			{
				// Oversized allocations get a chunk of their own rather than
				// overflowing a fixed-capacity chunk.
				push_chunk(size > chunk_capacity ? size : chunk_capacity);
				aligned = 0;
			}
			void *ptr = chunks.back().data.get() + aligned;
			chunks.back().used = aligned + size;
			return ptr;
		}

		template <typename T, typename... A>
		T *construct(A &&...a)
		{
			return ::new (allocate(sizeof(T), alignof(T))) T(std::forward<A>(a)...);
		}

	   private:
		void push_chunk(std::size_t size)
		{
			chunks.push_back({std::make_unique<std::byte[]>(size), size, 0});
		}
	};

	namespace dll
	{
		constexpr char compatible_check[] = "__CS_ABI_COMPATIBLE__";
		constexpr char main_entrance[] = "__CS_EXTENSION_MAIN__";

		typedef int (*compatible_check_t)();

		// Extension entry gets an accessor to the host's current_process.
		typedef process_context **(*current_process_accessor_t)();

		typedef void (*main_entrance_t)(name_space *, current_process_accessor_t);

		void *open(std::string_view);

		void *find_symbol(void *, std::string_view);

		void close(void *);
	} // namespace dll

	class extension final : public name_space
	{
		void *mHandle;

	   public:
		extension() = delete;

		extension(const extension &) = delete;

		virtual ~extension() = default;

		static inline int truncate(int n, int m)
		{
			// std::abs(INT_MIN) is UB; widen the magnitude.
			long long mag = n < 0 ? -static_cast<long long>(n) : n;
			return n == 0 ? 0 : n / int(std::pow(10, (std::max) (int(std::log10(static_cast<double>(mag))) - (std::max) (m, 0) + 1, 0)));
		}

		explicit extension(std::string_view path)
		{
			mHandle = dll::open(path);
			try
			{
				dll::compatible_check_t dll_check =
				    reinterpret_cast<dll::compatible_check_t>(dll::find_symbol(mHandle, dll::compatible_check));
				if (dll_check == nullptr)
					throw runtime_error("Incompatible Extension. (Missing ABI check symbol)");
				int target_abi = dll_check(); // Only call after the null check
				if (truncate(target_abi, 4) != truncate(COVSCRIPT_ABI_VERSION, 4))
					throw runtime_error("Incompatible Extension. (Target ABI: " + std::to_string(target_abi) +
					                    ", Current ABI: " + std::to_string(COVSCRIPT_ABI_VERSION) + ")");
				dll::main_entrance_t dll_main =
				    reinterpret_cast<dll::main_entrance_t>(dll::find_symbol(mHandle, dll::main_entrance));
				if (dll_main == nullptr)
					throw runtime_error("Broken Extension.");
				dll_main(this, &current_process_host_accessor);
			}
			catch (...)
			{
				dll::close(mHandle); // Release the handle on every failure path
				mHandle = nullptr;
				throw;
			}
		}
	};

	var make_namespace(const namespace_t &);

	template <typename T, typename... ArgsT>
	static namespace_t make_shared_namespace(ArgsT &&...args)
	{
		return std::make_shared<T>(std::forward<ArgsT>(args)...);
	}

	// Literal format
	numeric parse_number(const std::string &);
} // namespace cs
