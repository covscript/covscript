#pragma once
/*
* Covariant Script Flat Executor
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
* Copyright (C) 2017-2021 Michael Lee(李登淳)
*
* This software is registered with the National Copyright Administration
* of the People's Republic of China(Registration Number: 2020SR0408026)
* and is protected by the Copyright Law of the People's Republic of China.
*
* Email:   lee@covariant.cn, mikecovlee@163.com
* Github:  https://github.com/mikecovlee
* Website: http://covscript.org.cn
*/
#include <covscript/impl/impl.hpp>

namespace cs {
	class flat_executor final {
	public:
		struct scope {
			std::size_t scope_intro = 0;
			scope_type type = scope_type::normal;
			std::vector<std::size_t *> scope_exit;

			scope(std::size_t pc, scope_type t) : scope_intro(pc), type(t) {}
		};

		struct stack_frame {
			scope_type type = scope_type::normal;
			std::size_t pc = 0;

			explicit stack_frame(scope_type t) : type(t) {}
		};

		struct iterate_helper {
			var begin, end;
			std::function<void()> next;
			std::function<var()> iterator;
		};
	private:
		instance_type *instance = nullptr;
		std::vector<instruct_base *> irs;
		stack_type <scope> scope_stack;
		stack_type <stack_frame> stack;
		stack_type <iterate_helper> it;

		inline static bool same_scope(scope_type a, scope_type b)
		{
			return static_cast<unsigned char>(a) & static_cast<unsigned char>(b);
		}
	public:
		std::size_t pc = 0;

		flat_executor() = delete;

		explicit flat_executor(instance_type *ptr) : instance(ptr) {}

		~flat_executor()
		{
			for (auto &it:irs)
				delete it;
		}

		// Code Generating
		template<typename T, typename...ArgsT>
		void push_ir(ArgsT &&...args)
		{
			irs.push_back(new T(this, std::forward<ArgsT>(args)...));
			irs.back()->cur_pc = pc++;
		}

		void push_scope(scope_type type = scope_type::normal)
		{
			scope_stack.push(pc, type);
		}

		instruct_base *get_current_ir()
		{
			return irs.back();
		}

		std::size_t get_scope_intro(scope_type type = scope_type::all)
		{
			for (auto &it:scope_stack)
				if (same_scope(it.type, type))
					return it.scope_intro;
			throw internal_error("No matching scope.");
		}

		void expect_scope_exit(std::size_t *tag, scope_type type = scope_type::all)
		{
			for (auto &it:scope_stack) {
				if (same_scope(it.type, type)) {
					it.scope_exit.push_back(tag);
					break;
				}
			}
		}

		void pop_scope()
		{
			for (auto &it:scope_stack.top().scope_exit)
				*it = pc - 1;
			scope_stack.pop_no_return();
		}

		template<typename T>
		bool has_instruct_in_scope()
		{
			for (std::size_t i = get_scope_intro(); i < irs.size(); ++i)
				if (irs[i]->type() == typeid(T))
					return true;
			return false;
		}

		void print_irs(std::ostream &o)
		{
			for (auto &it:irs) {
				o << it->cur_pc << ": ";
				it->dump(o);
			}
		}

		// Code Execution

		instance_type* get_instance() const
		{
			return instance;
		}

		void push_frame(scope_type = scope_type::normal);

		void pop_frame();

		// Don't use it for normal frame pop
		void stack_rewind(scope_type, bool = true);

		void recover_register()
		{
			pc = stack.top().pc;
		}

		iterate_helper& begin_iteration()
		{
			it.push();
			return it.top();
		}

		iterate_helper& current_iteration()
		{
			return it.top();
		}

		void end_iteration()
		{
			it.pop_no_return();
		}

		void print_exec(std::ostream &o)
		{
			for (; pc < irs.size(); ++pc) {
				o << irs[pc]->cur_pc << ": ";
				irs[pc]->dump(o);
				irs[pc]->exec(this);
			}
		}

		void exec()
		{
			for (; pc < irs.size(); ++pc)
				irs[pc]->exec(this);
		}

		void end_exec()
		{
			pc = irs.size() - 1;
		}

		void info(std::ostream &o)
		{
			o << "\n## Flat Executor Info ##" << std::endl;
			o << "\n## Stack Info ##" << std::endl;
			o << "Scope   Stack: " << scope_stack.size() << std::endl;
			o << "Frame   Stack: " << stack.size() << std::endl;
			o << "Iterate Stack: " << it.size() << std::endl;
			o << "\n## Register Info ##\n" << std::endl;
			o << "Program Count: " << pc << std::endl;
		}
	};

	class task_scheduler final {
		struct task_context {
			process_context t_context;
			instance_type  t_instance;
			flat_executor  t_executor;
			// task_context(flat_executor *fe) : t_instance(*fe->get_instance()), t_executor(*fe) {}
		};
	};

	class instruct_internal final : public instruct_base {
		std::function<void(flat_executor *)> m_exec;
		std::string m_info;
	public:
		template<typename T>
		instruct_internal(flat_executor *fe, const std::string &info, T &&func) : m_exec(std::forward<T>(func)),
			m_info(info) {}

		void exec(flat_executor *fe) override
		{
			m_exec(fe);
		}

		void dump(std::ostream &o) const override
		{
			o << "<" << m_info << ">\n";
		}
	};

	class instruct_push_scope final : public instruct_base {
		scope_type t;
	public:
		instruct_push_scope(flat_executor *fe, scope_type type = scope_type::normal) : t(type)
		{
			if (type == scope_type::all)
				throw internal_error("Can't push scope using wildcard \"scope_type::all\".");
			fe->push_scope(t);
		}

		void exec(flat_executor *fe) override
		{
			fe->push_frame(t);
		}

		void dump(std::ostream &o) const override
		{
			switch (t) {
			case scope_type::normal:
				o << "<push scope>\n";
				break;
			case scope_type::loop:
				o << "<push loop>\n";
				break;
			case scope_type::except:
				o << "<push except>\n";
				break;
			case scope_type::task:
				o << "<push task>\n";
				break;
			}
		}
	};

	class instruct_pop_scope final : public instruct_base {
	public:
		instruct_pop_scope(flat_executor *fe)
		{
			fe->pop_scope();
		}

		void exec(flat_executor *fe) override
		{
			fe->pop_frame();
		}

		void dump(std::ostream &o) const override
		{
			o << "<pop scope>\n";
		}
	};

	class instruct_var final : public instruct_base {
		tree_type<token_base *> tree;
	public:
		instruct_var(flat_executor *, const tree_type<token_base *> &t) : tree(t) {}

		void exec(flat_executor *fe) override
		{
			fe->get_instance()->parse_define_var(tree.root());
		}

		void dump(std::ostream &o) const override
		{
			o << "<define var>\n";
		}
	};

	class instruct_eval final : public instruct_base {
		tree_type<token_base *> tree;
	public:
		instruct_eval(flat_executor *, const tree_type<token_base *> &t) : tree(t) {}

		void exec(flat_executor *fe) override
		{
			fe->get_instance()->parse_expr(tree.root());
		}

		void dump(std::ostream &o) const override
		{
			o << "<expression>\n";
		}
	};

	class instruct_jump final : public instruct_base {
	public:
		std::size_t tag = 0;

		instruct_jump(flat_executor *, std::size_t pc) : tag(pc) {}

		explicit instruct_jump(flat_executor *fe, scope_type type = scope_type::all)
		{
			fe->expect_scope_exit(&tag, type);
		}

		void exec(flat_executor *fe) override
		{
			fe->pc = tag;
		}

		void dump(std::ostream &o) const override
		{
			o << "<jump: tag = " << tag + 1 << ">\n";
		}
	};

	class instruct_cond final : public instruct_base {
		tree_type<token_base *> tree;
		bool cond = true;
	public:
		std::size_t tag = 0;

		instruct_cond(flat_executor *, const tree_type<token_base *> &t, std::size_t pc, bool c) : tree(t), tag(pc),
			cond(c) {}

		instruct_cond(flat_executor *fe, const tree_type<token_base *> &t, bool c, scope_type type = scope_type::all) : tree(t), cond(c)
		{
			fe->expect_scope_exit(&tag, type);
		}

		void exec(flat_executor *fe) override
		{
			if (fe->get_instance()->parse_expr(tree.root()).const_val<boolean>() == cond)
				fe->pc = tag;
		}

		void dump(std::ostream &o) const override
		{
			o << "<conditional jump: tag = " << tag + 1 << ">\n";
		}
	};

	class instruct_cond_internal final : public instruct_base {
		std::function<bool(flat_executor *)> cond;
		std::size_t tag = 0;
	public:
		instruct_cond_internal(flat_executor *, std::function<bool(flat_executor *)> t, std::size_t pc) : cond(
			    std::move(t)), tag(pc) {}

		instruct_cond_internal(flat_executor *fe, std::function<bool(flat_executor *)> t, scope_type type = scope_type::all) : cond(std::move(t))
		{
			fe->expect_scope_exit(&tag, type);
		}

		void exec(flat_executor *fe) override
		{
			if (cond(fe))
				fe->pc = tag;
		}

		void dump(std::ostream &o) const override
		{
			o << "<conditional jump: tag = " << tag + 1 << ">\n";
		}
	};

	class executor_proxy final {
		flat_executor *parent = nullptr;
		std::vector<std::string> mArgs;
		bool mIsMemFn = false;
		bool mIsVargs = false;
		std::size_t tag = 0;
	public:
		executor_proxy() = delete;
		executor_proxy(flat_executor *fe, std::vector<std::string> args, bool mem_fn, bool vargs, std::size_t t) : parent(fe), mArgs(std::move(args)), mIsMemFn(mem_fn), mIsVargs(vargs), tag(t) {}
		var operator()(vector &);
	};

	class instruct_function_def final : public instruct_base {
		bool mOverride = false;
		bool mIsMemFn = false;
		executor_proxy mFunc;
		std::string mName;
	public:
		instruct_function_def(flat_executor *, const std::string& name, bool override, bool mem_fn, executor_proxy func) : mName(name), mOverride(override), mIsMemFn(mem_fn), mFunc(std::move(func)) {}

		void exec(flat_executor *fe) override
		{
			if (this->mIsMemFn)
				fe->get_instance()->storage.add_var(this->mName, var::make_protect<callable>(mFunc, callable::types::member_fn), mOverride);
			else
				fe->get_instance()->storage.add_var(this->mName, var::make_protect<callable>(mFunc), mOverride);
		}

		void dump(std::ostream &o) const override
		{
			o << "<function definition: name = " << mName << (mIsMemFn ? ", member function" : ", regular function") << ">\n";
		}
	};
}