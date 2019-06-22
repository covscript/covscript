#pragma once
/*
* Covariant Script Runtime
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
	class domain_manager {
		stack_type <set_t<string>> m_set;
		stack_type <domain_type> m_data;
		bool m_cache_refresh = false;
	public:
		domain_manager()
		{
			m_set.push();
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
			m_data.push();
			m_cache_refresh = true;
		}

		const domain_type &get_domain() const
		{
			return m_data.top();
		}

		const domain_type &get_global() const
		{
			return m_data.top();
		}

		void remove_set()
		{
			m_set.pop_no_return();
		}

		void remove_domain()
		{
			m_data.pop_no_return();
			m_cache_refresh = true;
		}

		void clear_set()
		{
			m_set.top().clear();
		}

		void clear_domain()
		{
			m_data.top().clear();
		}

		bool exist_record(const string &name)
		{
			return m_set.top().count(name) > 0;
		}

		bool exist_record_in_struct(const string &name)
		{
			for (auto &set:m_set) {
				if (set.count("__PRAGMA_CS_STRUCT_DEFINITION__") > 0)
					return set.count(name) > 0;
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
			if (!m_cache_refresh && id.m_domain_id < m_data.size() && m_data[id.m_domain_id].consistence(id))
				return m_data[id.m_domain_id].get_var_by_id(id.m_slot_id);
			if (m_cache_refresh)
				m_cache_refresh = false;
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

		void mark_set_as_struct()
		{
			add_record("__PRAGMA_CS_STRUCT_DEFINITION__");
		}

		template<typename T>
		domain_manager &add_var(T &&name, const var &val, bool is_override = false)
		{
			if (var_exist_current(name)) {
				if (is_override)
					m_data.top().get_var_no_check(name) = val;
				else
					throw runtime_error("Target domain exist variable \"" + std::string(name) + "\".");
			}
			else
				m_data.top().add_var(name, val);
			return *this;
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
			return add_var(name, var::make_protect<type_t>(func, id));
		}

		template<typename T>
		domain_manager &
		add_buildin_type(T &&name, const std::function<var()> &func, const std::type_index &id, namespace_t ext)
		{
			add_record(name);
			return add_var(name, var::make_protect<type_t>(func, id, ext));
		}

		void involve_domain(const domain_type &domain, bool is_override = false)
		{
			for (auto &it:domain)
				add_var(it.first, domain.get_var_by_id(it.second), is_override);
		}
	};

	class runtime_type {
	public:
		domain_manager storage;

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

		var parse_bind(token_base *, const var &);

		var parse_choice(const var &, const tree_type<token_base *>::iterator &);

		var parse_pair(const var &, const var &);

		var parse_equ(const var &, const var &);

		var parse_neq(const var &, const var &);

		var parse_and(const tree_type<token_base *>::iterator &, const tree_type<token_base *>::iterator &);

		var parse_or(const tree_type<token_base *>::iterator &, const tree_type<token_base *>::iterator &);

		var parse_not(const var &);

		var parse_inc(var, var);

		var parse_dec(var, var);

		var parse_fcall(const var &, token_base *);

		var parse_access(var, const var &);

		var parse_expr(const tree_type<token_base *>::iterator &);
	};
}
