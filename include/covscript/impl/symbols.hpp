#pragma once
/*
* Covariant Script Symbols
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
* Copyright (C) 2017-2020 Michael Lee(李登淳)
*
* This software is registered with the National Copyright Administration
* of the People's Republic of China(Registration Number: 2020SR0408026)
* and is protected by the Copyright Law of the People's Republic of China.
*
* Email:   lee@covariant.cn, mikecovlee@163.com
* Github:  https://github.com/mikecovlee
* Website: http://covscript.org.cn
*/
#include <covscript/impl/type_ext.hpp>

namespace cs {
	enum class token_types {
		null,
		endline,
		action,
		signal,
		id,
		vargs,
		expand,
		value,
		literal,
		sblist,
		mblist,
		lblist,
		expr,
		arglist,
		array,
		parallel
	};
	enum class action_types {
		import_,
		as_,
		package_,
		using_,
		namespace_,
		struct_,
		extends_,
		block_,
		endblock_,
		var_,
		constant_,
		do_,
		if_,
		else_,
		switch_,
		case_,
		default_,
		while_,
		loop_,
		until_,
		for_,
		foreach_,
		in_,
		break_,
		continue_,
		function_,
		override_,
		return_,
		try_,
		catch_,
		throw_
	};
	enum class signal_types {
		endline_,
		add_,
		addasi_,
		sub_,
		subasi_,
		mul_,
		mulasi_,
		div_,
		divasi_,
		mod_,
		modasi_,
		pow_,
		powasi_,
		com_,
		dot_,
		und_,
		abo_,
		asi_,
		equ_,
		ueq_,
		aeq_,
		neq_,
		and_,
		or_,
		not_,
		inc_,
		dec_,
		pair_,
		choice_,
		slb_,
		srb_,
		mlb_,
		mrb_,
		llb_,
		lrb_,
		esb_,
		emb_,
		elb_,
		fcall_,
		access_,
		typeid_,
		new_,
		gcnew_,
		arrow_,
		lambda_,
		escape_,
		minus_,
		vardef_,
		varchk_,
		varprt_,
		vargs_,
		bind_,
		error_
	};
	enum class constant_values {
		local_namepace, global_namespace
	};
	/*
	* Grammar Types
	* Null: Do not use!
	* Single: Single Line Statement.
	* Block: Statement with code block.
	* Jit Command: Statement whitch execute in compile time.
	*/
	enum class method_types {
		null, single, block, jit_command
	};

	template<typename Key, typename T>
	class mapping final {
		std::map<Key, T> mDat;
	public:
		mapping(std::initializer_list<std::pair<const Key, T>> l) : mDat(l) {}

		bool exist(const Key &k) const
		{
			return mDat.count(k) > 0;
		}

		const T &match(const Key &k) const
		{
			if (!exist(k))
				throw runtime_error("Undefined Mapping.");
			return mDat.at(k);
		}
	};

	class token_base {
	protected:
		std::size_t line_num = 1;
	public:
		static garbage_collector<token_base> gc;

		static void *operator new(std::size_t size)
		{
			void *ptr = ::operator new(size);
			gc.add(ptr);
			return ptr;
		}

		static void operator delete(void *ptr)
		{
			gc.remove(ptr);
			::operator delete(ptr);
		}

		token_base() = default;

		token_base(const token_base &) = default;

		explicit token_base(std::size_t line) : line_num(line) {}

		virtual ~token_base() = default;

		virtual token_types get_type() const noexcept = 0;

		virtual std::size_t get_line_num() const noexcept final
		{
			return line_num;
		}

		virtual bool dump(std::ostream &o) const
		{
			o << "< BasicToken >";
			return false;
		}
	};

	class token_endline final : public token_base {
	public:
		token_endline() = default;

		explicit token_endline(std::size_t line) : token_base(line) {}

		token_types get_type() const noexcept override
		{
			return token_types::endline;
		}

		bool dump(std::ostream &o) const override
		{
			o << "< Endline >";
			return false;
		}
	};

	class token_action final : public token_base {
		action_types mType;
	public:
		token_action() = delete;

		explicit token_action(action_types t) : mType(t) {}

		token_types get_type() const noexcept override
		{
			return token_types::action;
		}

		action_types get_action() const noexcept
		{
			return this->mType;
		}

		bool dump(std::ostream &o) const override
		{
			o << "< Action >";
			return false;
		}
	};

	class token_signal final : public token_base {
		signal_types mType;
	public:
		token_signal() = delete;

		explicit token_signal(signal_types t) : mType(t)
		{
			if (t == signal_types::error_)
				throw runtime_error("Wrong grammar for signals.");
		}

		token_signal(signal_types t, std::size_t line) : token_base(line), mType(t) {}

		token_types get_type() const noexcept override
		{
			return token_types::signal;
		}

		signal_types get_signal() const noexcept
		{
			return this->mType;
		}

		bool dump(std::ostream &) const override;
	};

	class token_id final : public token_base {
		var_id mId;
	public:
		token_id() = delete;

		explicit token_id(const std::string &id) : mId(id) {}

		token_types get_type() const noexcept override
		{
			return token_types::id;
		}

		const var_id &get_id() const noexcept
		{
			return this->mId;
		}

		bool dump(std::ostream &o) const override
		{
			o << "< ID = \"" << mId.get_id() << "\" >";
			return true;
		}
	};

	class token_vargs final : public token_base {
		std::string mId;
	public:
		token_vargs() = delete;

		explicit token_vargs(std::string id) : mId(std::move(id)) {}

		token_types get_type() const noexcept override
		{
			return token_types::vargs;
		}

		const std::string &get_id() const noexcept
		{
			return this->mId;
		}

		bool dump(std::ostream &o) const override
		{
			o << "< Variable Arguments List, ID = \"" << mId << "\" >";
			return true;
		}
	};

	class token_expand final : public token_base {
		tree_type<token_base *> mTree;
	public:
		token_expand() = delete;

		explicit token_expand(tree_type<token_base *> tree) : mTree(std::move(tree)) {}

		token_types get_type() const noexcept override
		{
			return token_types::expand;
		}

		tree_type<token_base *> &get_tree() noexcept
		{
			return this->mTree;
		}

		bool dump(std::ostream &) const override;
	};

	class token_value final : public token_base {
		static const map_t<char, char> escape_char;
		var mVal;
	public:
		token_value() = delete;

		explicit token_value(var val) : mVal(std::move(val)) {}

		token_types get_type() const noexcept override
		{
			return token_types::value;
		}

		var &get_value() noexcept
		{
			return this->mVal;
		}

		bool dump(std::ostream &) const override;
	};

	class token_literal final : public token_base {
		friend class compiler_type;

		std::string m_data, m_literal;
	public:
		token_literal() = delete;

		token_literal(const std::string &data, const std::string &literal) : m_data(data), m_literal(literal) {}

		token_types get_type() const noexcept override
		{
			return token_types::literal;
		}

		const std::string &get_data() const noexcept
		{
			return m_data;
		}

		const std::string &get_literal() const noexcept
		{
			return m_literal;
		}

		bool dump(std::ostream &o) const override
		{
			o << "< Literal = \"" << m_data << "\"" << m_literal << " >";
			return true;
		}
	};

	class token_sblist final : public token_base {
		std::deque<std::deque<token_base *>> mList;
	public:
		token_sblist() = delete;

		explicit token_sblist(std::deque<std::deque<token_base *>> list) : mList(std::move(list)) {}

		token_types get_type() const noexcept override
		{
			return token_types::sblist;
		}

		std::deque<std::deque<token_base *>> &get_list() noexcept
		{
			return this->mList;
		}

		bool dump(std::ostream &o) const override
		{
			o << "< Small Bracket >";
			return false;
		}
	};

	class token_mblist final : public token_base {
		std::deque<std::deque<token_base *>> mList;
	public:
		token_mblist() = delete;

		explicit token_mblist(std::deque<std::deque<token_base *>> list) : mList(std::move(list)) {}

		token_types get_type() const noexcept override
		{
			return token_types::mblist;
		}

		std::deque<std::deque<token_base *>> &get_list() noexcept
		{
			return this->mList;
		}

		bool dump(std::ostream &o) const override
		{
			o << "< Middle Bracket >";
			return false;
		}
	};

	class token_lblist final : public token_base {
		std::deque<std::deque<token_base *>> mList;
	public:
		token_lblist() = delete;

		explicit token_lblist(std::deque<std::deque<token_base *>> list) : mList(std::move(list)) {}

		token_types get_type() const noexcept override
		{
			return token_types::lblist;
		}

		std::deque<std::deque<token_base *>> &get_list() noexcept
		{
			return this->mList;
		}

		bool dump(std::ostream &o) const override
		{
			o << "< Large Bracket >";
			return false;
		}
	};

	class token_expr final : public token_base {
		tree_type<token_base *> mTree;
	public:
		token_expr() = delete;

		explicit token_expr(tree_type<token_base *> tree) : mTree(std::move(tree)) {}

		token_types get_type() const noexcept override
		{
			return token_types::expr;
		}

		tree_type<token_base *> &get_tree() noexcept
		{
			return this->mTree;
		}

		bool dump(std::ostream &) const override;
	};

	class token_arglist final : public token_base {
		std::deque<tree_type<token_base * >> mTreeList;
	public:
		token_arglist() = default;

		explicit token_arglist(std::deque<tree_type<token_base * >>

		                       tlist) :

			mTreeList(std::move(tlist)) {}

		token_types get_type() const noexcept override
		{
			return token_types::arglist;
		}

		std::deque<tree_type<token_base * >> &

		                                  get_arglist() noexcept
		{
			return this->mTreeList;
		}

		bool dump(std::ostream &) const override;
	};

	class token_array final : public token_base {
		std::deque<tree_type<token_base * >> mTreeList;
	public:
		token_array() = default;

		explicit token_array(std::deque<tree_type<token_base * >>

		                     tlist) :

			mTreeList(std::move(tlist)) {}

		token_types get_type() const noexcept override
		{
			return token_types::array;
		}

		std::deque<tree_type<token_base * >> &

		                                  get_array() noexcept
		{
			return this->mTreeList;
		}

		bool dump(std::ostream &) const override;
	};

	class token_parallel final : public token_base {
		std::deque<tree_type<token_base * >> mTreeList;
	public:
		token_parallel() = default;

		explicit token_parallel(std::deque<tree_type<token_base * >>

		                        tlist) :

			mTreeList(std::move(tlist)) {}

		token_types get_type() const noexcept override
		{
			return token_types::parallel;
		}

		std::deque<tree_type<token_base * >> &

		                                  get_parallel() noexcept
		{
			return this->mTreeList;
		}

		bool dump(std::ostream &) const override;
	};

	enum class statement_types {
		null,
		expression_,
		import_,
		package_,
		involve_,
		block_,
		namespace_,
		var_,
		constant_,
		if_,
		else_,
		switch_,
		case_,
		default_,
		while_,
		until_,
		loop_,
		for_,
		foreach_,
		break_,
		continue_,
		struct_,
		function_,
		return_,
		end_,
		try_,
		catch_,
		throw_
	};

	class flat_executor;

	class instruct_base {
	public:
		std::size_t cur_pc = 1;

		virtual void exec(flat_executor *) = 0;

		virtual ~instruct_base() = default;

		virtual void dump(std::ostream &o) const
		{
			o << "<instruct>\n";
		}

		const std::type_info& type() const noexcept
		{
			return typeid(*this);
		}
	};

	enum class expect_tag {
		scope_exit, except_handler
	};

	enum class scope_type {
		all, normal, except, loop
	};

	class flat_executor final {
		std::vector<flat_executor *> child;
		bool resume = false;
	public:
		struct scope {
			std::size_t scope_intro = 0;
			scope_type type = scope_type::normal;
			std::vector<std::size_t *> scope_exit;
			std::vector<std::size_t *> except_handler;

			scope(std::size_t pc, scope_type t) : scope_intro(pc), type(t) {}
		};

		instance_type *instance = nullptr;
		task_context *this_task = nullptr;
		std::vector<instruct_base *> irs;
		stack_type <scope> scope_stack;

		flat_executor() = default;

		~flat_executor()
		{
			for (auto &it:irs)
				delete it;
			for (auto &it:child)
				delete it;
		}

		flat_executor* gen_child()
		{
			child.emplace_back(new flat_executor);
			child.back()->instance = instance;
			return child.back();
		}

		instruct_base *get_current_ir()
		{
			return irs.back();
		}

		template<typename T, typename...ArgsT>
		void push_ir(ArgsT &&...args)
		{
			irs.push_back(new T(this, std::forward<ArgsT>(args)...));
			irs.back()->cur_pc = this_task->pc;
			++this_task->pc;
		}

		void push_scope(scope_type type = scope_type::normal)
		{
			scope_stack.push(this_task->pc, type);
		}

		std::size_t get_scope_intro(scope_type type = scope_type::all)
		{
			for (auto &it:scope_stack)
				if (type == scope_type::all || it.type == type)
					return it.scope_intro;
			throw runtime_error("No matching scope.");
		}

		void expect_scope_exit(std::size_t *tag, scope_type type = scope_type::normal)
		{
			for (auto &it:scope_stack) {
				if (type == scope_type::all || it.type == type) {
					it.scope_exit.push_back(tag);
					break;
				}
			}
		}

		void expect_except_handler(std::size_t *tag)
		{
			for (auto &it:scope_stack) {
				if (it.type == scope_type::except) {
					it.except_handler.push_back(tag);
					break;
				}
			}
		}

		void pop_scope()
		{
			for (auto &it:scope_stack.top().scope_exit)
				*it = this_task->pc - 1;
			for (auto &it:scope_stack.top().except_handler)
				*it = this_task->pc - 1;
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

		void print(std::ostream &o)
		{
			for (auto &it:irs) {
				o << it->cur_pc << ": ";
				it->dump(o);
			}
		}

		void begin_task();

		void resume_task();

		void end_task()
		{
			current_process->task_stack.pop_no_return();
			this_task = nullptr;
		}

		void print_exec(std::ostream &o)
		{
			begin_task();
			for (; this_task->pc < irs.size(); ++this_task->pc) {
				o << irs[this_task->pc]->cur_pc << ": ";
				irs[this_task->pc]->dump(o);
				irs[this_task->pc]->exec(this);
			}
			end_task();
			resume_task();
		}

		void exec()
		{
			begin_task();
			for (; this_task->pc < irs.size(); ++this_task->pc)
				irs[this_task->pc]->exec(this);
			end_task();
			resume_task();
		}

		void end_exec()
		{
			this_task->pc = irs.size() - 1;
			resume = true;
		}
	};

	class instruct_var;

	class child_executor final {
		bool mHasScope = true;
		bool mIsMemFn = false;
		bool mIsVargs = false;
		flat_executor* child = nullptr;
		std::vector<std::string> mArgs;
	public:
		child_executor(std::vector<std::string>, flat_executor*, bool, bool);
		flat_executor *get_parent() const
		{
			return child;
		}
		var operator()(vector &);
	};

	class statement_base {
	protected:
		context_t context;
		std::size_t line_num = 1;
	public:
		static garbage_collector<statement_base> gc;

		static void *operator new(std::size_t size)
		{
			void *ptr = ::operator new(size);
			gc.add(ptr);
			return ptr;
		}

		static void operator delete(void *ptr)
		{
			gc.remove(ptr);
			::operator delete(ptr);
		}

		statement_base() = default;

		statement_base(const statement_base &) = default;

		statement_base(context_t c, token_base *eptr) : context(std::move(c)),
			line_num(static_cast<token_endline *>(eptr)->get_line_num()) {}

		virtual ~statement_base() = default;

		std::size_t get_line_num() const noexcept
		{
			return line_num;
		}

		const std::string &get_file_path() const noexcept;

		const std::string &get_package_name() const noexcept;

		const std::string &get_raw_code() const noexcept;

		virtual statement_types get_type() const noexcept = 0;

		virtual void run_impl() = 0;

		inline void run()
		{
			current_process->poll_event();
			this->run_impl();
		}

		virtual void repl_run_impl()
		{
			this->run_impl();
		}

		inline void repl_run()
		{
			current_process->poll_event();
			this->repl_run_impl();
		}

		virtual void dump(std::ostream &o) const
		{
			o << "<statement>\n";
		}

		virtual void gen_flat_ir(flat_executor *) {}
	};

	class method_base {
	public:
		static garbage_collector<method_base> gc;

		static void *operator new(std::size_t size)
		{
			void *ptr = ::operator new(size);
			gc.add(ptr);
			return ptr;
		}

		static void operator delete(void *ptr)
		{
			gc.remove(ptr);
			::operator delete(ptr);
		}

		method_base() = default;

		method_base(const method_base &) = default;

		virtual ~method_base() = default;

		virtual method_types get_type() const noexcept = 0;

		virtual statement_types get_target_type() const noexcept = 0;

		virtual void preprocess(const context_t &, const std::deque<std::deque<token_base *>> &) {}

		virtual statement_base *translate(const context_t &, const std::deque<std::deque<token_base *>> &) = 0;

		virtual void postprocess(const context_t &, const domain_type &) {}
	};

	class method_end : public method_base {
	public:
		using method_base::method_base;

		method_types get_type() const noexcept override
		{
			return method_types::single;
		}

		statement_types get_target_type() const noexcept override
		{
			return statement_types::end_;
		}

		virtual statement_base *
		translate_end(method_base *method, const context_t &context, std::deque<std::deque<token_base *>> &raw,
		              std::deque<token_base *> &code)
		{
			return method->translate(context, raw);
		}

		statement_base *translate(const context_t &, const std::deque<std::deque<token_base *>> &) override
		{
			return nullptr;
		}
	};
}
