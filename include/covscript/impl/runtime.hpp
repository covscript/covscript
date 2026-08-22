#pragma once
/*
 * Covariant Script Runtime
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
#include <covscript/impl/compiler.hpp>

namespace cs
{
	using stack_pointer = stack_type<domain_type> *;

	// Owns compiled lambdas (tokens carry an index), preventing arena<->function
	// cycles and keeping a self-referencing lambda's borrowed `self` alive.
	class function_store final
	{
		struct entry
		{
			std::unique_ptr<function> func;
			var callable;
		};

		std::vector<entry> m_entries;
		std::vector<std::vector<std::size_t>> m_transactions;

	   public:
		// Register a function (named or lambda).  Returns a stable function pointer
		// that outlives the entry (heap-allocated via unique_ptr).
		function *add_function(std::unique_ptr<function> func)
		{
			function *raw = func.get();
			m_entries.push_back(entry{std::move(func), var()});
			if (!m_transactions.empty())
				m_transactions.back().push_back(m_entries.size() - 1);
			return raw;
		}

		// Register a lambda: owns the function and stores the callable for
		// token_lambda retrieval.  Returns the lambda index.
		std::size_t add_lambda(std::unique_ptr<function> func, var callable)
		{
			m_entries.push_back(entry{std::move(func), std::move(callable)});
			std::size_t index = m_entries.size() - 1;
			if (!m_transactions.empty())
				m_transactions.back().push_back(index);
			return index;
		}

		std::size_t size() const noexcept
		{
			return m_entries.size();
		}

		void begin_transaction()
		{
			m_transactions.emplace_back();
		}

		void commit_transaction()
		{
			if (!m_transactions.empty())
				m_transactions.pop_back();
		}

		void rollback_transaction()
		{
			if (m_transactions.empty())
				return;
			for (std::size_t index : m_transactions.back())
				if (index < m_entries.size())
				{
					m_entries[index].func.reset();
					m_entries[index].callable = var();
				}
			m_transactions.pop_back();
			// Preserve stable indices for committed nested entries. Only empty
			// slots at the physical end can be reclaimed safely.
			while (!m_entries.empty() && !m_entries.back().func && !m_entries.back().callable.usable())
				m_entries.pop_back();
		}

		std::size_t active_size() const noexcept
		{
			return static_cast<std::size_t>(std::count_if(m_entries.begin(), m_entries.end(),
			                                              [](const entry &e)
			{ return e.func != nullptr; }));
		}

		var get(std::size_t index) const
		{
			return m_entries.at(index).callable;
		}
	};

	class domain_manager
	{
		const stack_pointer &fiber_stack;
		// Owned keys: record names may point into a token arena that dies on
		// recompile, so the set must not hold views into it.
		stack_type<set_t<std::string>> m_set;
		stack_type<domain_type> m_data;
		set_t<std::string_view> buildin_symbols;

	   public:
		explicit domain_manager(const stack_pointer &fiber_sp)
		    : fiber_stack(fiber_sp)
		{
			m_set.push();
			m_data.push();
		}

		domain_manager(const stack_pointer &fiber_sp, std::size_t size)
		    : fiber_stack(fiber_sp)
		{
			m_set.push();
			m_data.resize(size);
			m_data.push();
		}

		domain_manager(const domain_manager &) = delete;

		~domain_manager() = default;

		bool is_initial() const
		{
			return m_data.size() == 1;
		}

		void add_set()
		{
			m_set.push();
		}

		void add_domain()
		{
			if (fiber_stack != nullptr)
				fiber_stack->push();
			else
				m_data.push();
		}

		domain_type &get_domain() const
		{
			if (fiber_stack != nullptr)
				return fiber_stack->top();
			else
				return m_data.top();
		}

		domain_type &get_global() const
		{
			return m_data.bottom();
		}

		// Release globals (running finalizers) while the runtime is usable.
		void clear_global()
		{
			m_data.bottom().safe_rewind();
		}

		namespace_t get_namespace() const
		{
			namespace_t nm = std::make_shared<name_space>();
			const domain_type &global = m_data.bottom();
			for (auto &it : global)
			{
				if (buildin_symbols.count(it.first) == 0)
					nm->add_var(it.first.data(), global.get_var_by_id(it.second));
			}
			return nm;
		}

		void remove_set()
		{
			m_set.pop_no_return();
		}

		void remove_domain()
		{
			if (fiber_stack != nullptr)
			{
				fiber_stack->top().clear();
				fiber_stack->pop_no_return();
			}
			else
			{
				m_data.top().clear();
				m_data.pop_no_return();
			}
		}

		void clear_set()
		{
			m_set.top().clear();
		}

		void clear_domain()
		{
			if (fiber_stack != nullptr)
				fiber_stack->top().clear();
			else
				m_data.top().clear();
		}

		void next_domain()
		{
			if (fiber_stack != nullptr)
				fiber_stack->top().next();
			else
				m_data.top().next();
		}

		bool exist_record(std::string_view name)
		{
			// The set stack can be empty (e.g. debugger expressions after the
			// script finished); no records exist then.
			if (m_set.empty())
				return false;
			return m_set.top().count(std::string(name)) > 0;
		}

		bool exist_record_in_struct(std::string_view name)
		{
			const std::string owned_name(name);
			for (auto &set : m_set)
			{
				if (set.count(owned_name) > 0)
					return set.count("__PRAGMA_CS_STRUCT_DEFINITION__") > 0;
			}
			return false;
		}

		inline var &get_var(const std::string &name)
		{
			if (fiber_stack != nullptr)
			{
				for (auto &domain : *fiber_stack)
				{
					var *ptr = domain.get_var_opt(name);
					if (ptr != nullptr)
						return *ptr;
				}
			}
			for (auto &domain : m_data)
			{
				var *ptr = domain.get_var_opt(name);
				if (ptr != nullptr)
					return *ptr;
			}
			throw runtime_error("Use of undefined variable \"" + name + "\".");
		}

		inline var &get_var(const var_id &id)
		{
			if (fiber_stack != nullptr)
			{
				if (id.m_domain_id < fiber_stack->size() && (*fiber_stack)[id.m_domain_id].consistence(id))
					return (*fiber_stack)[id.m_domain_id].get_var_by_id(id.m_slot_id);
				for (std::size_t i = 0, size = fiber_stack->size(); i < size; ++i)
					if ((*fiber_stack)[i].exist(id))
						return (*fiber_stack)[i].get_var_no_check(id, i);
			}
			if (id.m_domain_id < m_data.size() && m_data[id.m_domain_id].consistence(id))
				return m_data[id.m_domain_id].get_var_by_id(id.m_slot_id);
			for (std::size_t i = 0, size = m_data.size(); i < size; ++i)
				if (m_data[i].exist(id))
					return m_data[i].get_var_no_check(id, i);
			throw runtime_error("Use of undefined variable \"" + id.get_id() + "\".");
		}

		template <typename T>
		var &get_var_current(T &&name)
		{
			if (fiber_stack != nullptr)
				return fiber_stack->top().get_var(name);
			else
				return m_data.top().get_var(name);
		}

		template <typename T>
		var &get_var_global(T &&name)
		{
			return m_data.bottom().get_var(name);
		}

		template <typename T>
		var get_var_optimizable(T &&name)
		{
			if (m_data.size() == m_set.size())
			{
				for (std::size_t i = 0, size = m_data.size(); i < size; ++i)
				{
					auto &current_set = m_set[i];
					if (current_set.find((const std::string &) name) != current_set.end())
					{
						var *ptr = m_data[i].get_var_opt(name);
						if (ptr != nullptr)
							return *ptr;
						else
							break;
					}
				}
			}
			return var();
		}

		domain_manager &add_record(std::string_view name)
		{
			if (exist_record(name))
				throw runtime_error("Redefinition of variable \"" + std::string(name) + "\".");
			else
				m_set.top().emplace(std::string(name));
			return *this;
		}

		void mark_set_as_struct(bool inherited = false)
		{
			add_record("__PRAGMA_CS_STRUCT_DEFINITION__");
			if (inherited)
				add_record("parent");
		}

		template <typename T>
		domain_manager &add_var(T &&name, const var &val)
		{
			if (fiber_stack != nullptr)
			{
				if (!fiber_stack->top().add_var_optimal(name, val))
					throw runtime_error("Target domain exist variable \"" + std::string(name) + "\".");
			}
			else
			{
				if (!m_data.top().add_var_optimal(name, val))
					throw runtime_error("Target domain exist variable \"" + std::string(name) + "\".");
			}
			return *this;
		}

		template <typename T>
		domain_manager &add_var(T &&name, const var &val, bool is_override)
		{
			if (fiber_stack != nullptr)
			{
				if (!fiber_stack->top().add_var_optimal(name, val, is_override))
					throw runtime_error("Target domain exist variable \"" + std::string(name) + "\".");
			}
			else
			{
				if (!m_data.top().add_var_optimal(name, val, is_override))
					throw runtime_error("Target domain exist variable \"" + std::string(name) + "\".");
			}
			return *this;
		}

		template <typename T>
		void add_var_no_return(T &&name, const var &val)
		{
			if (fiber_stack != nullptr)
			{
				if (!fiber_stack->top().add_var_optimal(name, val))
					throw runtime_error("Target domain exist variable \"" + std::string(name) + "\".");
			}
			else
			{
				if (!m_data.top().add_var_optimal(name, val))
					throw runtime_error("Target domain exist variable \"" + std::string(name) + "\".");
			}
		}

		template <typename T>
		void add_var_no_return(T &&name, const var &val, bool is_override)
		{
			if (fiber_stack != nullptr)
			{
				if (!fiber_stack->top().add_var_optimal(name, val, is_override))
					throw runtime_error("Target domain exist variable \"" + std::string(name) + "\".");
			}
			else
			{
				if (!m_data.top().add_var_optimal(name, val, is_override))
					throw runtime_error("Target domain exist variable \"" + std::string(name) + "\".");
			}
		}

		template <typename T>
		domain_manager &add_var_global(T &&name, const var &var)
		{
			if (m_data.bottom().exist(name))
				throw runtime_error("Target domain exist variable \"" + std::string(name) + "\".");
			else
				m_data.bottom().add_var(name, var);
			return *this;
		}

		template <typename T>
		domain_manager &add_buildin_var(T &&name, const var &var)
		{
			add_record(name);
			buildin_symbols.emplace(name);
			return add_var_global(name, var);
		}

		template <typename T>
		domain_manager &add_struct(T &&name, const struct_builder &builder)
		{
			return add_var(name, var::make_protect<type_t>(builder, builder.get_id()));
		}

		template <typename T>
		domain_manager &add_type(T &&name, const std::function<var()> &func, const std::type_index &id)
		{
			return add_var(name, var::make_protect<type_t>(func, id));
		}

		template <typename T>
		domain_manager &
		add_type(T &&name, const std::function<var()> &func, const std::type_index &id, const namespace_t &ext)
		{
			return add_var(name, var::make_protect<type_t>(func, id, ext));
		}

		template <typename T>
		domain_manager &add_buildin_type(T &&name, const std::function<var()> &func, const std::type_index &id)
		{
			add_record(name);
			buildin_symbols.emplace(name);
			return add_var(name, var::make_protect<type_t>(func, id));
		}

		template <typename T>
		domain_manager &
		add_buildin_type(T &&name, const std::function<var()> &func, const std::type_index &id, const namespace_t &ext)
		{
			add_record(name);
			buildin_symbols.emplace(name);
			return add_var(name, var::make_protect<type_t>(func, id, ext));
		}

		void involve_domain(const domain_type &domain, bool is_override = false)
		{
			for (auto &it : domain)
				add_var(it.first.data(), domain.get_var_by_id(it.second), is_override);
		}

		// Compile-time snapshot for storage rollback. Zero runtime cost.
		struct domain_snapshot
		{
			std::vector<domain_type> data;       // bottom → top
			std::vector<set_t<std::string>> set; // bottom → top
		};

		domain_snapshot create_snapshot() const
		{
			domain_snapshot s;
			s.data.reserve(m_data.size());
			for (auto &d : m_data)
				s.data.push_back(d); // top → bottom (reverse iterator)
			std::reverse(s.data.begin(), s.data.end());
			s.set.reserve(m_set.size());
			for (auto &st : m_set)
				s.set.push_back(st);
			std::reverse(s.set.begin(), s.set.end());
			return s;
		}

		void restore_snapshot(const domain_snapshot &snap)
		{
			while (!m_data.empty())
				m_data.pop_no_return();
			while (!m_set.empty())
				m_set.pop_no_return();
			for (auto &d : snap.data)
				m_data.push(d);
			for (auto &st : snap.set)
				m_set.push(st);
		}
	};

	class runtime_type
	{
		map_t<std::string, callable> literals;

	   public:
		domain_manager storage;

		function_store functions;

		explicit runtime_type(const stack_pointer &fiber_sp)
		    : storage(fiber_sp) {}

		runtime_type(const stack_pointer &fiber_sp, std::size_t size)
		    : storage(fiber_sp, size) {}

		void add_string_literal(const std::string &literal, const callable &func)
		{
			if (literals.count(literal) > 0)
				throw runtime_error("Duplicated String Literal.");
			else
				literals.emplace(literal, func);
		}

		var get_string_literal(const std::string &data, const std::string &literal)
		{
			if (literals.count(literal) > 0)
			{
				vector arg{data};
				return literals.at(literal).call(arg);
			}
			else
				throw runtime_error("Undefined String Literal.");
		}

		var parse_add(const var &, const var &);

		var parse_addasi(var, const var &);

		var parse_sub(const var &, const var &);

		var parse_subasi(var, const var &);

		var parse_minus(const var &);

		var parse_mul(const var &, const var &);

		var parse_mulasi(var, const var &);

		var parse_escape(const var &);

		var parse_div(const var &, const var &);

		var parse_divasi(var, const var &);

		var parse_mod(const var &, const var &);

		var parse_modasi(var, const var &);

		var parse_pow(const var &, const var &);

		var parse_powasi(var, const var &);

		var &parse_dot_lhs(const var &, token_base *);

		var parse_dot(const var &, token_base *);

		var parse_arrow(const var &, token_base *);

		var parse_typeid(const var &);

		var parse_new(const var &);

		var parse_gcnew(const var &);

		var parse_und(const var &, const var &);

		var parse_abo(const var &, const var &);

		var parse_ueq(const var &, const var &);

		var parse_aeq(const var &, const var &);

		var parse_asi(var, const var &);

		var parse_lnkasi(var &, const var &);

		var parse_lnkasi(tree_type<token_base *>::iterator, const var &);

		var parse_bind(token_base *, const var &);

		var parse_choice(const var &, const tree_type<token_base *>::iterator &);

		var parse_pair(const var &, const var &);

		var parse_equ(const var &, const var &);

		var parse_neq(const var &, const var &);

		var parse_and(const tree_type<token_base *>::iterator &, const tree_type<token_base *>::iterator &);

		var parse_or(const tree_type<token_base *>::iterator &, const tree_type<token_base *>::iterator &);

		var parse_not(const var &);

		var parse_inc(const var &, const var &);

		var parse_dec(const var &, const var &);

		var parse_addr(const var &);

		var parse_fcall(const var &, token_base *);

		var &parse_access_lhs(const var &, const var &);

		var parse_access(const var &, const var &);

		var parse_expr(const tree_type<token_base *>::iterator &, bool = false);
	};
} // namespace cs
