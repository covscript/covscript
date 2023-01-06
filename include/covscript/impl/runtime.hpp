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
#include <covscript/impl/compiler.hpp>

namespace cs {
	class domain_manager {
		stack_type<set_t<string>> m_set;
		stack_type<domain_type> m_data;
		set_t<string> buildin_symbols;
	public:
		domain_manager()
		{
			m_set.push();
			m_data.push();
		}

		explicit domain_manager(std::size_t size)
		{
			m_set.push();
			m_data.resize(size);
			m_data.push();
		}

		domain_manager(const domain_manager &) = delete;

		~domain_manager() = default;

		void clear_all_data()
		{
			while (!m_set.empty())
				m_set.pop_no_return();
			while (!m_data.empty())
				m_data.pop_no_return();
		}

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
			m_data.push();
		}

		domain_type &get_domain() const
		{
			return m_data.top();
		}

		domain_type &get_global() const
		{
			return m_data.bottom();
		}

		namespace_t get_namespace() const
		{
			namespace_t nm = std::make_shared<name_space>();
			const domain_type &global = m_data.bottom();
			for (auto &it : global) {
				if (buildin_symbols.count(it.first) == 0)
					nm->add_var(it.first, global.get_var_by_id(it.second));
			}
			return nm;
		}

		void remove_set()
		{
			m_set.pop_no_return();
		}

		void remove_domain()
		{
			m_data.pop_no_return();
		}

		void clear_set()
		{
			m_set.top().clear();
		}

		void clear_domain()
		{
			m_data.top().clear();
		}

		void next_domain()
		{
			m_data.top().next();
		}

		bool exist_record(const string &name)
		{
			return m_set.top().count(name) > 0;
		}

		bool exist_record_in_struct(const string &name)
		{
			for (auto &set:m_set) {
				if (set.count(name) > 0)
					return set.count("__PRAGMA_CS_STRUCT_DEFINITION__") > 0;
			}
			return false;
		}

		inline bool var_exist_current(const string &name) noexcept
		{
			return m_data.top().exist(name);
		}

		inline bool var_exist_global(const string &name) noexcept
		{
			return m_data.bottom().exist(name);
		}

		inline var &get_var(const std::string &name)
		{
			for (auto &domain:m_data)
				if (domain.exist(name))
					return domain.get_var_no_check(name);
			throw runtime_error("Use of undefined variable \"" + name + "\".");
		}

		inline var &get_var(const var_id &id)
		{
			if (id.m_domain_id < m_data.size() && m_data[id.m_domain_id].consistence(id))
				return m_data[id.m_domain_id].get_var_by_id(id.m_slot_id);
			for (std::size_t i = 0, size = m_data.size(); i < size; ++i)
				if (m_data[i].exist(id))
					return m_data[i].get_var_no_check(id, i);
			throw runtime_error("Use of undefined variable \"" + id.get_id() + "\".");
		}

		template<typename T>
		var &get_var_current(T &&name)
		{
			return m_data.top().get_var(name);
		}

		template<typename T>
		var &get_var_global(T &&name)
		{
			return m_data.bottom().get_var(name);
		}

		template<typename T>
		var get_var_optimizable(T &&name)
		{
			if (m_data.size() == m_set.size()) {
				for (std::size_t i = 0, size = m_data.size(); i < size; ++i) {
					if (m_set[i].count(name) > 0) {
						if (m_data[i].exist(name))
							return m_data[i].get_var_no_check(name);
						else
							break;
					}
				}
			}
			return var();
		}

		domain_manager &add_record(const string &name)
		{
			if (exist_record(name))
				throw runtime_error("Redefinition of variable \"" + name + "\".");
			else
				m_set.top().emplace(name);
			return *this;
		}

		void mark_set_as_struct(bool inherited = false)
		{
			add_record("__PRAGMA_CS_STRUCT_DEFINITION__");
			if (inherited)
				add_record("parent");
		}

		template<typename T>
		domain_manager &add_var(T &&name, const var &val)
		{
			if (!m_data.top().add_var_optimal(name, val))
				throw runtime_error("Target domain exist variable \"" + std::string(name) + "\".");
			return *this;
		}

		template<typename T>
		domain_manager &add_var(T &&name, const var &val, bool is_override)
		{
			if (!m_data.top().add_var_optimal(name, val, is_override))
				throw runtime_error("Target domain exist variable \"" + std::string(name) + "\".");
			return *this;
		}

		template<typename T>
		void add_var_no_return(T &&name, const var &val)
		{
			if (!m_data.top().add_var_optimal(name, val))
				throw runtime_error("Target domain exist variable \"" + std::string(name) + "\".");
		}

		template<typename T>
		void add_var_no_return(T &&name, const var &val, bool is_override)
		{
			if (!m_data.top().add_var_optimal(name, val, is_override))
				throw runtime_error("Target domain exist variable \"" + std::string(name) + "\".");
		}

		template<typename T>
		domain_manager &add_var_global(T &&name, const var &var)
		{
			if (var_exist_global(name))
				throw runtime_error("Target domain exist variable \"" + std::string(name) + "\".");
			else
				m_data.bottom().add_var(name, var);
			return *this;
		}

		template<typename T>
		domain_manager &add_buildin_var(T &&name, const var &var)
		{
			add_record(name);
			buildin_symbols.emplace(name);
			return add_var_global(name, var);
		}

		template<typename T>
		domain_manager &add_struct(T &&name, const struct_builder &builder)
		{
			return add_var(name, var::make_protect<type_t>(builder, builder.get_id()));
		}

		template<typename T>
		domain_manager &add_type(T &&name, const std::function<var()> &func, const std::type_index &id)
		{
			return add_var(name, var::make_protect<type_t>(func, id));
		}

		template<typename T>
		domain_manager &
		add_type(T &&name, const std::function<var()> &func, const std::type_index &id, namespace_t ext)
		{
			return add_var(name, var::make_protect<type_t>(func, id, ext));
		}

		template<typename T>
		domain_manager &add_buildin_type(T &&name, const std::function<var()> &func, const std::type_index &id)
		{
			add_record(name);
			buildin_symbols.emplace(name);
			return add_var(name, var::make_protect<type_t>(func, id));
		}

		template<typename T>
		domain_manager &
		add_buildin_type(T &&name, const std::function<var()> &func, const std::type_index &id, namespace_t ext)
		{
			add_record(name);
			buildin_symbols.emplace(name);
			return add_var(name, var::make_protect<type_t>(func, id, ext));
		}

		void involve_domain(const domain_type &domain, bool is_override = false)
		{
			for (auto &it:domain)
				add_var(it.first, domain.get_var_by_id(it.second), is_override);
		}
	};

	class runtime_type {
		map_t<std::string, callable> literals;
	public:
		domain_manager storage;

		runtime_type() = default;

		explicit runtime_type(std::size_t size) : storage(size) {}

		void add_string_literal(const std::string &literal, const callable &func)
		{
			if (literals.count(literal) > 0)
				throw runtime_error("Duplicated String Literal.");
			else
				literals.emplace(literal, func);
		}

		var get_string_literal(const std::string &data, const std::string &literal)
		{
			if (literals.count(literal) > 0) {
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

		var parse_expr(const tree_type<token_base *>::iterator &, bool= false);
	};
}
