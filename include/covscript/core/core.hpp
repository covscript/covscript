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
 * Copyright (C) 2017-2025 Michael Lee(李登淳)
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
#include <exception>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <istream>
#include <ostream>
#include <utility>
#include <cstring>
#include <atomic>
#include <cctype>
#include <string>
#include <vector>
#include <memory>
#include <cmath>
#include <deque>
#include <list>
#include <map>
// CovScript Headers
#include <covscript/core/components.hpp>
#include <covscript/core/definition.hpp>
#include <covscript/core/variable.hpp>
#include <covscript/core/version.hpp>

namespace cs {
// Process Context
	class process_context final {
		std::atomic<bool> is_sigint_raised{};

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
		// Stack
		std::size_t stack_size = COVSCRIPT_STACK_PRESERVE;

		stack_type<var> stack;
#ifdef CS_DEBUGGER
		stack_type<std::string> stack_backtrace;
#endif
		stack_type<fiber_t> fiber_stack;

		// Stack Resize must before any context instance start
		void resize_stack(std::size_t size)
		{
			stack_size = size;
			stack.resize(size);
			fiber_stack.resize(child_stack_size());
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

		event_type on_process_exit;

		// DO NOT TOUCH THIS EVENT DIRECTLY!!
		event_type on_process_sigint;

		inline void poll_event()
		{
			if (is_sigint_raised) {
				is_sigint_raised = false;
				on_process_sigint.touch(nullptr);
			}
		}

		inline void raise_sigint()
		{
			is_sigint_raised = true;
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

		process_context() : on_process_exit(&on_process_exit_default_handler), on_process_sigint(&on_process_exit_default_handler)
		{
			is_sigint_raised = false;
		}

		explicit process_context(std::size_t ss) : on_process_exit(&on_process_exit_default_handler), on_process_sigint(&on_process_exit_default_handler)
		{
			resize_stack(ss);
			is_sigint_raised = false;
		}

		std::unique_ptr<process_context> fork();
	};

	extern process_context this_process;
	extern process_context *current_process;

// Context
	class context_type final {
	public:
		compiler_t compiler = nullptr;
		instance_t instance = nullptr;
		std::deque<string> file_buff;
		string file_path = "<Unknown>";
		string package_name;
		var cmd_args;

		context_type() = default;

		context_type(const context_type &) = default;

		~context_type() = default;
	};

// Callable and Function
	class callable final {
	public:
		using function_type = std::function<var(vector &)>;
		enum class types {
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

		explicit callable(function_type func, types type = types::normal) : mFunc(std::move(func)), mType(type) {}

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

		const function_type &get_raw_data() const
		{
			return mFunc;
		}
	};

	enum class fiber_state {
		ready,
		running,
		suspended,
		finished
	};

	class fiber_type {
	protected:
		fiber_type() = default;

	public:
		fiber_type(const fiber_type &) = delete;
		fiber_type &operator=(const fiber_type &) = delete;

		virtual ~fiber_type() = default;

		virtual fiber_state get_state() const = 0;

		virtual var return_value() const = 0;
	};

	namespace fiber {
		fiber_t create(const context_t &, std::function<var()>);

		void resume(const fiber_t &);

		void yield();
	} // namespace fiber

	class function final {
		context_t mContext;
#ifdef CS_DEBUGGER
		// Debug Information
		mutable bool mMatch = false;
		std::string mDecl;
		statement_base *mStmt;
#endif
		bool mIsMemFn = false;
		bool mIsVargs = false;
		bool mIsLambda = false;
		std::vector<std::string> mArgs;
		std::deque<statement_base *> mBody;

		static var call_rr(const function *, vector &);

		static var call_vv(const function *, vector &);

		static var call_rl(const function *, vector &);

		static var call_el(const function *, vector &);

		var (*call_ptr)(const function *, vector &) = nullptr;

		inline void init_call_ptr() noexcept
		{
			if (!mIsVargs) {
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

		function(const function &) = default;

#ifdef CS_DEBUGGER
		function(context_t c, std::string decl, statement_base *stmt, std::vector<std::string> args, std::deque<statement_base *> body, bool is_vargs = false, bool is_lambda = false) : mContext(std::move(c)), mDecl(std::move(decl)), mStmt(stmt), mIsVargs(is_vargs), mIsLambda(is_lambda), mArgs(std::move(args)), mBody(std::move(body))
		{
			init_call_ptr();
		}
#else

		function(context_t c, std::vector<std::string> args, std::deque<statement_base *> body, bool is_vargs = false, bool is_lambda = false) : mContext(std::move(c)), mIsVargs(is_vargs), mIsLambda(is_lambda), mArgs(std::move(args)), mBody(std::move(body))
		{
			init_call_ptr();
		}

#endif

		~function() = default;

		var call(vector &args) const
		{
			return call_ptr(this, args);
		}

		var operator()(vector &args) const
		{
			return call_ptr(this, args);
		}

		const context_t &get_context() const
		{
			return mContext;
		}

		bool is_el_func() const
		{
			return call_ptr == &call_el;
		}

		void add_reserve_var(std::string_view reserve, bool is_mem_fn = false)
		{
			mIsMemFn = is_mem_fn;
			if (!mIsVargs) {
				std::vector<std::string> args{std::string(reserve)};
				args.reserve(mArgs.size());
				for (auto &name : mArgs) {
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
				mDecl = prefix + "this" + (mIsVargs ? ", ..." : ", ") + mDecl.substr(lpos, rpos - lpos) + suffix;
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

		statement_base *get_raw_statement() const
		{
			return mStmt;
		}

		void set_debugger_state(bool match) const
		{
			mMatch = match;
		}
#endif
	};

	struct function_ptr final {
		function *fptr = nullptr;
		var operator()(vector &args) const
		{
			return fptr->call(args);
		}
	};

	struct object_method final {
		var object;
		var callable;
		bool is_request_fold = false;

		object_method() = delete;

		object_method(var obj, var func, bool request_fold = false) : object(std::move(obj)), callable(std::move(func)), is_request_fold(request_fold) {}

		~object_method() = default;
	};

// Copy
	void copy_no_return(var &);

	var copy(var);

// Move Semantics
	var lvalue(const var &);

	var rvalue(const var &);

	var try_move(const var &);

// Invoke
	template <typename... ArgsT>
	static var invoke(const var &func, ArgsT &&..._args)
	{
		if (func.is_type_of<callable>()) {
			vector args{std::forward<ArgsT>(_args)...};
			return func.const_val<callable>().call(args);
		}
		else if (func.is_type_of<object_method>()) {
			const auto &om = func.const_val<object_method>();
			vector args{om.object, std::forward<ArgsT>(_args)...};
			return om.callable.const_val<callable>().call(args);
		}
		else
			throw runtime_error("Invoke non-callable object.");
	}

// Type and struct
	struct pointer final {
		var data;

		pointer() = default;

		explicit pointer(var v) : data(std::move(v)) {}

		bool operator==(const pointer &ptr) const
		{
			return data.is_same(ptr.data);
		}
	};

	static const pointer null_pointer = {};

	struct type_id final {
		static map_t<std::size_t, set_t<std::size_t>> inherit_map;
		std::type_index type_idx;
		std::size_t type_hash;

		type_id() = delete;

		type_id(const std::type_index &id, std::size_t hash = 0) : type_idx(id), type_hash(hash) {}

		inline bool is_a(const type_id &id) const {
			if (&id == this)
				return true;
			if (type_hash && id.type_hash)
				return inherit_map.count(id.type_hash) > 0 && inherit_map[id.type_hash].count(type_hash) > 0;
			else
				return type_idx == id.type_idx;
		}

		inline bool compare(const type_id &id) const
		{
			if (&id == this)
				return true;
			if (type_hash)
				return type_hash == id.type_hash;
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

	struct domain_ref final {
		domain_type *domain = nullptr;

		domain_ref(domain_type *ptr) : domain(ptr) {}
	};

	class var_id final {
		friend class domain_type;

		friend class domain_manager;

		mutable std::size_t m_domain_id = 0, m_slot_id = 0;
		mutable std::shared_ptr<domain_ref> m_ref;
		std::string m_id;

	public:
		var_id() = delete;

		explicit var_id(std::string_view name) : m_id(name) {}

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

	class domain_type final {
		map_t<std::string_view, std::size_t> m_reflect;
		std::shared_ptr<domain_ref> m_ref;
		std::vector<var> m_slot;
		bool optimize = false;

		inline std::size_t get_slot_id(std::string_view name) const
		{
			auto it = m_reflect.find(name);
			if (it != m_reflect.end())
				return it->second;
			else
				throw runtime_error("Use of undefined variable \"" + std::string(name) + "\".");
		}

	public:
		domain_type() : m_ref(std::make_shared<domain_ref>(this)) {}

		domain_type(const domain_type &domain) : m_reflect(domain.m_reflect), m_ref(std::make_shared<domain_ref>(this)), m_slot(domain.m_slot) {}

		domain_type(domain_type &&domain) noexcept : m_ref(std::make_shared<domain_ref>(this))
		{
			std::swap(m_reflect, domain.m_reflect);
			std::swap(m_slot, domain.m_slot);
		}

		~domain_type()
		{
			m_ref->domain = nullptr;
		}

		void clear()
		{
			m_reflect.clear();
			m_slot.clear();
			optimize = false;
		}

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
			return m_reflect.find(name) != m_reflect.end();
		}

		inline bool exist(const var_id &id) const noexcept
		{
			return m_reflect.find(id.m_id) != m_reflect.end();
		}

		domain_type &add_var(const char *name, const var &val)
		{
			auto it = m_reflect.find(name);
			if (it == m_reflect.end()) {
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
			if (it == m_reflect.end()) {
				m_slot.push_back(val);
				m_reflect.emplace(id.m_id, m_slot.size() - 1);
				id.m_slot_id = m_slot.size() - 1;
				id.m_ref = m_ref;
			}
			else {
				if (id.m_ref != m_ref) {
					id.m_slot_id = it->second;
					id.m_ref = m_ref;
				}
				m_slot[id.m_slot_id] = val;
			}
			return *this;
		}

		bool add_var_optimal(const char *name, const var &val, bool override = false)
		{
			auto it = m_reflect.find(name);
			if (it != m_reflect.end()) {
				if (optimize) {
					m_slot[it->second] = val;
					return true;
				}
				else if (override) {
					m_slot[it->second] = val;
					return true;
				}
				else
					return false;
			}
			else {
				m_slot.push_back(val);
				m_reflect.emplace(name, m_slot.size() - 1);
				return true;
			}
		}

		bool add_var_optimal(const var_id &id, const var &val, bool override = false)
		{
			if (id.m_ref == m_ref) {
				if (optimize) {
					m_slot[id.m_slot_id] = val;
					return true;
				}
				else if (override) {
					add_var(id, val);
					return true;
				}
				else
					return false;
			}
			else {
				add_var(id, val);
				return true;
			}
		}

		var &get_var(const var_id &id)
		{
			if (id.m_ref != m_ref) {
				id.m_slot_id = get_slot_id(id.m_id);
				id.m_ref = m_ref;
			}
			return m_slot[id.m_slot_id];
		}

		const var &get_var(const var_id &id) const
		{
			if (id.m_ref != m_ref) {
				id.m_slot_id = get_slot_id(id.m_id);
				id.m_ref = m_ref;
			}
			return m_slot[id.m_slot_id];
		}

		var &get_var(std::string_view name)
		{
			auto it = m_reflect.find(name);
			if (it != m_reflect.end())
				return m_slot[it->second];
			else
				throw runtime_error("Use of undefined variable \"" + std::string(name) + "\".");
		}

		const var &get_var(std::string_view name) const
		{
			auto it = m_reflect.find(name);
			if (it != m_reflect.end())
				return m_slot[it->second];
			else
				throw runtime_error("Use of undefined variable \"" + std::string(name) + "\".");
		}

		var *get_var_opt(const var_id &id)
		{
			if (id.m_ref != m_ref) {
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
			if (id.m_ref != m_ref) {
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
			auto it = m_reflect.find(name);
			if (it != m_reflect.end())
				return &m_slot[it->second];
			else
				return nullptr;
		}

		const var *get_var_opt(std::string_view name) const
		{
			auto it = m_reflect.find(name);
			if (it != m_reflect.end())
				return &m_slot[it->second];
			else
				return nullptr;
		}

		var &get_var_no_check(const var_id &id) noexcept
		{
			if (id.m_ref != m_ref) {
				id.m_slot_id = m_reflect.at(id.m_id);
				id.m_ref = m_ref;
			}
			return m_slot[id.m_slot_id];
		}

		const var &get_var_no_check(const var_id &id) const noexcept
		{
			if (id.m_ref != m_ref) {
				id.m_slot_id = m_reflect.at(id.m_id);
				id.m_ref = m_ref;
			}
			return m_slot[id.m_slot_id];
		}

		var &get_var_no_check(const var_id &id, std::size_t domain_id) noexcept
		{
			id.m_domain_id = domain_id;
			if (id.m_ref != m_ref) {
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

	struct type_t final {
		std::function<var()> constructor;
		namespace_t extensions;
		type_id id;

		type_t() = delete;

		type_t(std::function<var()> c, const type_id &i) : constructor(std::move(c)), id(i) {}

		type_t(std::function<var()> c, const type_id &i, namespace_t ext) : constructor(std::move(c)), id(i), extensions(std::move(ext)) {}

		template <typename T>
		var &get_var(T &&) const;
	};

	class range_iterator final {
		numeric m_step, m_index;

	public:
		range_iterator() = delete;

		explicit range_iterator(numeric step, numeric index) : m_step(std::move(step)), m_index(std::move(index)) {}

		range_iterator(const range_iterator &) = default;

		range_iterator(range_iterator &&) noexcept = default;

		range_iterator &operator=(const range_iterator &) = default;

		bool operator==(const range_iterator &it) const
		{
			return m_index == it.m_index;
		}

		bool operator!=(const range_iterator &it) const
		{
			return m_index < it.m_index;
		}

		range_iterator &operator++()
		{
			m_index = m_index + m_step;
			return *this;
		}

		numeric operator*() const
		{
			return m_index;
		}
	};

	class range_type final {
		numeric m_start, m_stop, m_step;

	public:
		range_type() = delete;

		range_type(numeric start, numeric stop, numeric step) : m_start(std::move(start)), m_stop(std::move(stop)), m_step(std::move(step)) {}

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

	class structure final {
		bool m_shadow = false;
		std::string m_name;
		domain_t m_data;
		type_id m_id;

	public:
		structure() = delete;

		structure(const type_id &id, std::string name, const domain_type &data) : m_id(id),
			m_name(std::move(name)),
			m_data(std::make_shared<domain_type>(data))
		{
			if (m_data->exist("initialize"))
				invoke(m_data->get_var("initialize"), var::make<structure>(this));
		}

		structure(structure &&s) noexcept : m_shadow(s.m_shadow), m_data(nullptr), m_id(typeid(void))
		{
			s.m_shadow = true;
			std::swap(m_name, s.m_name);
			std::swap(m_data, s.m_data);
			std::swap(m_id, s.m_id);
		}

		structure(const structure &s) : m_id(s.m_id), m_name(s.m_name), m_data(std::make_shared<domain_type>())
		{
			if (s.m_data->exist("parent")) {
				var &_p = s.m_data->get_var("parent");
				auto &_parent = _p.val<structure>();
				var p = copy(_p);
				auto &parent = p.val<structure>();
				m_data->add_var("parent", p);
				for (auto &it : *parent.m_data) {
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

		explicit structure(const structure *s) : m_shadow(true), m_id(s->m_id), m_name(s->m_name), m_data(s->m_data) {}

		~structure()
		{
			if (!m_shadow && m_data->exist("finalize"))
				invoke(m_data->get_var("finalize"), var::make<structure>(this));
		}

		structure &operator=(structure &&s) noexcept
		{
			std::swap(m_shadow, s.m_shadow);
			std::swap(m_name, s.m_name);
			std::swap(m_data, s.m_data);
			std::swap(m_id, s.m_id);
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
			else {
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

	class struct_builder final {
		static map_t<std::size_t, std::size_t> mParentMap;
		static std::size_t mCount;
		context_t mContext;
		type_id mTypeId;
		std::string mName;
		tree_type<token_base *> mParent;
		std::deque<statement_base *> mMethod;

	public:
		struct_builder() = delete;

		struct_builder(context_t c, std::string name, tree_type<token_base *> parent,
		               std::deque<statement_base *> method) : mContext(std::move(c)),
			mTypeId(typeid(structure), ++mCount),
			mName(std::move(name)),
			mParent(std::move(parent)),
			mMethod(std::move(method)) {}

		struct_builder(const struct_builder &) = default;

		~struct_builder() = default;

		static void reset_counter()
		{
			mParentMap.clear();
			mCount = 0;
		}

		const type_id &get_id() const
		{
			return mTypeId;
		}

		void do_inherit();

		var operator()();
	};

// Namespace and extensions
	class name_space {
		domain_type *m_data = nullptr;
		bool is_ref = false;

	public:
		name_space() : m_data(new domain_type) {}

		name_space(const name_space &ns) : m_data(new domain_type)
		{
			copy_namespace(ns);
		}

		explicit name_space(domain_type dat) : m_data(new domain_type(std::move(dat))) {}

		explicit name_space(domain_type *dat) : m_data(dat), is_ref(true) {}

		virtual ~name_space()
		{
			if (!is_ref)
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
			if (&ns != this) {
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

// Internal Garbage Collection
	template <typename T>
	class garbage_collector final {
		set_t<T *> table;

	public:
		garbage_collector() = default;

		garbage_collector(const garbage_collector &) = delete;

		~garbage_collector()
		{
			collect();
		}

		void collect()
		{
			for (auto &ptr : table)
				::operator delete(ptr);
			table.clear();
		}

		void add(void *ptr)
		{
			table.emplace(static_cast<T *>(ptr));
		}

		void remove(void *ptr)
		{
			table.erase(static_cast<T *>(ptr));
		}
	};

	namespace dll {
		constexpr char compatible_check[] = "__CS_ABI_COMPATIBLE__";
		constexpr char main_entrance[] = "__CS_EXTENSION_MAIN__";

		typedef int (*compatible_check_t)();

		typedef void (*main_entrance_t)(name_space *, process_context *);

		void *open(std::string_view);

		void *find_symbol(void *, std::string_view);

		void close(void *);
	} // namespace dll

	class extension final : public name_space {
		void *mHandle;

	public:
		extension() = delete;

		extension(const extension &) = delete;

		virtual ~extension()
		{
			dll::close(mHandle);
		}

		static inline int truncate(int n, int m)
		{
			return n == 0 ? 0 : n / int(std::pow(10, (std::max) (int(std::log10(std::abs(n))) - (std::max) (m, 0) + 1, 0)));
		}

		explicit extension(std::string_view path)
		{
			mHandle = dll::open(path);
			dll::compatible_check_t dll_check = reinterpret_cast<dll::compatible_check_t>(dll::find_symbol(mHandle, dll::compatible_check));
			if (dll_check == nullptr || truncate(dll_check(), 4) != truncate(COVSCRIPT_ABI_VERSION, 4))
				throw runtime_error("Incompatible Covariant Script Extension.(Target: " + std::to_string(dll_check()) +
				                    ", Current: " + std::to_string(COVSCRIPT_ABI_VERSION) + ")");
			dll::main_entrance_t dll_main = reinterpret_cast<dll::main_entrance_t>(dll::find_symbol(mHandle, dll::main_entrance));
			if (dll_main != nullptr) {
				dll_main(this, current_process);
			}
			else
				throw runtime_error("Broken Covariant Script Extension.");
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
