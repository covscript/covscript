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
#include <covscript/impl/variant.hpp>

namespace cs {
	class domain_manager {
		std::deque<set_t<string>> m_set;
		std::deque<domain_t> m_data;
	public:
		domain_manager()
		{
			m_set.emplace_front();
			m_data.emplace_front(std::make_shared<map_t<string, var >>());
		}

		domain_manager(const domain_manager &) = delete;

		~domain_manager() = default;

		bool is_initial() const
		{
			return m_data.size() == 1;
		}

		void add_set()
		{
			m_set.emplace_front();
		}

		void add_domain()
		{
			m_data.emplace_front(std::make_shared<map_t<string, var >>());
		}

		domain_t &get_domain()
		{
			return m_data.front();
		}

		domain_t &get_global()
		{
			return m_data.back();
		}

		void remove_set()
		{
			m_set.pop_front();
		}

		void remove_domain()
		{
			m_data.pop_front();
		}

		void clear_set()
		{
			m_set.front().clear();
		}

		void clear_domain()
		{
			m_data.front()->clear();
		}

		bool exist_record(const string &name)
		{
			return m_set.front().count(name) > 0;
		}

		bool exist_record_in_struct(const string &name)
		{
			for (auto &set:m_set) {
				if (set.count("__PRAGMA_CS_STRUCT_DEFINITION__") > 0)
					return set.count(name) > 0;
			}
			return false;
		}

		bool var_exist(const string &name)
		{
			for (auto &domain:m_data)
				if (domain->count(name) > 0)
					return true;
			return false;
		}

		bool var_exist_current(const string &name)
		{
			return m_data.front()->count(name) > 0;
		}

		bool var_exist_global(const string &name)
		{
			return m_data.back()->count(name) > 0;
		}

		var &get_var(const string &name)
		{
			for (auto &domain:m_data)
				if (domain->count(name) > 0)
					return (*domain)[name];
			throw runtime_error("Use of undefined variable \"" + name + "\".");
		}

		var &get_var_current(const string &name)
		{
			if (m_data.front()->count(name) > 0)
				return (*m_data.front())[name];
			throw runtime_error("Use of undefined variable \"" + name + "\" in current domain.");
		}

		var &get_var_global(const string &name)
		{
			if (m_data.back()->count(name) > 0)
				return (*m_data.back())[name];
			throw runtime_error("Use of undefined variable \"" + name + "\" in global domain.");
		}

		domain_manager &add_record(const string &name)
		{
			if (exist_record(name))
				throw runtime_error("Redefinition of variable \"" + name + "\".");
			else
				m_set.front().emplace(name);
			return *this;
		}

		void mark_set_as_struct()
		{
			add_record("__PRAGMA_CS_STRUCT_DEFINITION__");
		}

		domain_manager &add_var(const string &name, const var &val, bool is_override = false)
		{
			if (var_exist_current(name)) {
				if (is_override)
					(*m_data.front())[name] = val;
				else
					throw runtime_error("Target domain exist variable \"" + name + "\".");
			}
			else
				m_data.front()->emplace(name, val);
			return *this;
		}

		domain_manager &add_var_global(const string &name, const var &var)
		{
			if (var_exist_global(name))
				throw runtime_error("Target domain exist variable \"" + name + "\".");
			else
				m_data.back()->emplace(name, var);
			return *this;
		}

		domain_manager &add_buildin_var(const string &name, const var &var)
		{
			add_record(name);
			return add_var_global(name, var);
		}

		domain_manager &add_struct(const std::string &name, const struct_builder &builder)
		{
			return add_var(name, var::make_protect<type>(builder, builder.get_id()));
		}

		domain_manager &add_type(const std::string &name, const std::function<var()> &func, const std::type_index &id)
		{
			return add_var(name, var::make_protect<type>(func, id));
		}

		domain_manager &
		add_type(const std::string &name, const std::function<var()> &func, const std::type_index &id,
		         namespace_t ext)
		{
			return add_var(name, var::make_protect<type>(func, id, ext));
		}

		domain_manager &
		add_buildin_type(const std::string &name, const std::function<var()> &func, const std::type_index &id)
		{
			add_record(name);
			return add_var(name, var::make_protect<type>(func, id));
		}

		domain_manager &
		add_buildin_type(const std::string &name, const std::function<var()> &func, const std::type_index &id,
		                 namespace_t ext)
		{
			add_record(name);
			return add_var(name, var::make_protect<type>(func, id, ext));
		}

		void involve_domain(const domain_t &domain, bool is_override = false)
		{
			for (auto &it:*domain)
				add_var(it.first, it.second, is_override);
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

		var parse_choice(const var &, const cov::tree<token_base *>::iterator &);

		var parse_pair(const var &, const var &);

		var parse_equ(const var &, const var &);

		var parse_neq(const var &, const var &);

		var parse_and(const var &, const var &);

		var parse_or(const var &, const var &);

		var parse_not(const var &);

		var parse_inc(var, var);

		var parse_dec(var, var);

		var parse_fcall(const var &, token_base *);

		var parse_access(var, const var &);

		var parse_expr(const cov::tree<token_base *>::iterator &);

		cov::static_stack<var, 1024> stack;
	};

	class instruction_executor final {
		std::vector<instruction_base *> m_assembly;

		void gen_instruction(const cov::tree<token_base *>::iterator &, std::vector<instruction_base *> &);

		void gen_instruction(const cov::tree<token_base *>::iterator &it)
		{
			gen_instruction(it, m_assembly);
		}

		runtime_type *runtime = nullptr;
	public:
		instruction_executor() = delete;

		instruction_executor(runtime_type *rt, const cov::tree<token_base *>::iterator &it) : runtime(rt)
		{
			gen_instruction(it);
		}

		var operator()()
		{
			for (auto &it:m_assembly)
				it->exec();
			var result = runtime->stack.top();
			runtime->stack.pop();
			return result;
		}
	};

	class instruction_push : public instruction_base {
		var m_value;
	public:
		instruction_push() = delete;

		instruction_push(const var &val, runtime_type *rt) : instruction_base(rt), m_value(val) {}

		void exec() override
		{
			runtime->stack.push(m_value);
		}
	};

	class instruction_pop : public instruction_base {
	public:
		instruction_pop() = delete;

		instruction_pop(runtime_type *rt) : instruction_base(rt) {}

		void exec() override
		{
			runtime->stack.pop();
		}
	};

	class instruction_id : public instruction_base {
		std::string m_id;
	public:
		instruction_id() = delete;

		instruction_id(const std::string &id, runtime_type *rt) : instruction_base(rt), m_id(id) {}

		void exec() override;
	};

	class instruction_array : public instruction_base {
		std::size_t m_size;
	public:
		instruction_array() = delete;

		instruction_array(std::size_t size, runtime_type *rt) : instruction_base(rt), m_size(size) {}

		void exec() override;
	};

	class instruction_signal : public instruction_base {
		signal_types m_signal;
	public:
		instruction_signal() = delete;

		instruction_signal(signal_types signal, runtime_type *rt) : instruction_base(rt), m_signal(signal) {}

		void exec() override;
	};

	class instruction_sig_dot : public instruction_base {
		token_base *m_token = nullptr;
	public:
		instruction_sig_dot() = delete;

		instruction_sig_dot(token_base *token, runtime_type *rt) : instruction_base(rt), m_token(token) {}

		void exec() override;
	};

	class instruction_sig_arrow : public instruction_base {
		token_base *m_token = nullptr;
	public:
		instruction_sig_arrow() = delete;

		instruction_sig_arrow(token_base *token, runtime_type *rt) : instruction_base(rt), m_token(token) {}

		void exec() override;
	};

	class instruction_sig_choice : public instruction_base {
		std::vector<instruction_base *> m_assembly_true, m_assembly_false;
	public:
		instruction_sig_choice() = delete;

		instruction_sig_choice(std::vector<instruction_base *> assembly_true,
		                       std::vector<instruction_base *> assembly_false, runtime_type *rt) : instruction_base(rt),
			m_assembly_true(
			    std::move(
			        assembly_true)),
			m_assembly_false(
			    std::move(
			        assembly_false)) {}

		void exec() override;
	};

	class instruction_sig_fcall : public instruction_base {
		std::size_t m_size;
	public:
		instruction_sig_fcall() = delete;

		instruction_sig_fcall(size_t size, runtime_type *rt) : instruction_base(rt), m_size(size) {}

		void exec() override;
	};
}
