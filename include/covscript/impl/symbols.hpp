#pragma once
/*
* Covariant Script Symbols
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
#include <covscript/impl/extension.hpp>

namespace cs {
	enum class token_types {
		null, endline, action, signal, id, value, sblist, mblist, lblist, expr, arglist, array, parallel
	};
	enum class action_types {
		import_,
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
		varprt_
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
		static garbage_collector<token_base> gc;
	protected:
		std::size_t line_num = 1;
	public:
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

		explicit token_signal(signal_types t) : mType(t) {}

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
		std::string mId;
	public:
		token_id() = delete;

		explicit token_id(std::string id) : mId(std::move(id)) {}

		token_types get_type() const noexcept override
		{
			return token_types::id;
		}

		const std::string &get_id() const noexcept
		{
			return this->mId;
		}

		bool dump(std::ostream &o) const override
		{
			o << "< ID = \"" << mId << "\" >";
			return true;
		}
	};

	class token_value final : public token_base {
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

		bool dump(std::ostream &o) const override
		{
			o << "< Value = \"";
			try {
				o << mVal.to_string();
			}
			catch (cov::error &e) {
				if (!std::strcmp(e.what(), "E000D"))
					throw e;
				o << "[" << cs_impl::cxx_demangle(mVal.type().name()) << "]";
			}
			o << "\" >";
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
		cov::tree<token_base *> mTree;
	public:
		token_expr() = delete;

		explicit token_expr(cov::tree<token_base *> tree) : mTree(std::move(tree)) {}

		token_types get_type() const noexcept override
		{
			return token_types::expr;
		}

		cov::tree<token_base *> &get_tree() noexcept
		{
			return this->mTree;
		}

		bool dump(std::ostream &) const override;
	};

	class token_arglist final : public token_base {
		std::deque<cov::tree<token_base *>> mTreeList;
	public:
		token_arglist() = default;

		explicit token_arglist(std::deque<cov::tree<token_base *>> tlist) : mTreeList(std::move(tlist)) {}

		token_types get_type() const noexcept override
		{
			return token_types::arglist;
		}

		std::deque<cov::tree<token_base *>> &get_arglist() noexcept
		{
			return this->mTreeList;
		}

		bool dump(std::ostream &) const override;
	};

	class token_array final : public token_base {
		std::deque<cov::tree<token_base *>> mTreeList;
	public:
		token_array() = default;

		explicit token_array(std::deque<cov::tree<token_base *>> tlist) : mTreeList(std::move(tlist)) {}

		token_types get_type() const noexcept override
		{
			return token_types::array;
		}

		std::deque<cov::tree<token_base *>> &get_array() noexcept
		{
			return this->mTreeList;
		}

		bool dump(std::ostream &) const override;
	};

	class token_parallel final : public token_base {
		std::deque<cov::tree<token_base *>> mTreeList;
	public:
		token_parallel() = default;

		explicit token_parallel(std::deque<cov::tree<token_base *>> tlist) : mTreeList(std::move(tlist)) {}

		token_types get_type() const noexcept override
		{
			return token_types::parallel;
		}

		std::deque<cov::tree<token_base *>> &get_parallel() noexcept
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

	class statement_base {
		static garbage_collector<statement_base> gc;
	protected:
		context_t context;
		std::size_t line_num = 1;
	public:
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

		statement_base(context_t c, token_base *eptr) : context(std::move(std::move(c))),
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

		virtual void run() = 0;

		virtual void repl_run()
		{
			this->run();
		}

		virtual void dump(std::ostream &o) const
		{
			o << "<statement>\n";
		}
	};

	class method_base {
		static garbage_collector<method_base> gc;
	public:
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
	};
}
