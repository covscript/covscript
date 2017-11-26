#pragma once
/*
* Covariant Script Symbols
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Affero General Public License as published
* by the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Affero General Public License for more details.
*
* You should have received a copy of the GNU Affero General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
* Copyright (C) 2017 Michael Lee(李登淳)
* Email: mikecovlee@163.com
* Github: https://github.com/mikecovlee
*/
#include <covscript/core.hpp>

namespace cs {
	enum class token_types {
		null, endline, action, signal, id, value, sblist, mblist, lblist, expr, arglist, array
	};
	enum class action_types {
		import_,
		package_,
		involve_,
		namespace_,
		struct_,
		block_,
		endblock_,
		var_,
		constant_,
		if_,
		else_,
		switch_,
		case_,
		default_,
		while_,
		loop_,
		until_,
		for_,
		to_,
		step_,
		iterate_,
		break_,
		continue_,
		function_,
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
		vardef_
	};
	enum class constant_values {
		current_namespace, global_namespace
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

		bool exsist(const Key &k)
		{
			return mDat.count(k) > 0;
		}

		T match(const Key &k)
		{
			if (!exsist(k))
				throw syntax_error("Undefined Mapping.");
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

		explicit token_base(std::size_t line):line_num(line) {}

		virtual ~token_base() = default;

		virtual token_types get_type() const noexcept=0;

		virtual std::size_t get_line_num() const noexcept final
		{
			return line_num;
		}
	};

	class token_endline final : public token_base {
	public:
		token_endline() = default;

		token_endline(std::size_t line) : token_base(line) {}

		virtual token_types get_type() const noexcept override
		{
			return token_types::endline;
		}
	};

	class token_action final : public token_base {
		action_types mType;
	public:
		token_action() = delete;

		token_action(action_types t) : mType(t) {}

		virtual token_types get_type() const noexcept override
		{
			return token_types::action;
		}

		action_types get_action() const noexcept
		{
			return this->mType;
		}
	};

	class token_signal final : public token_base {
		signal_types mType;
	public:
		token_signal() = delete;

		token_signal(signal_types t) : mType(t) {}

		token_signal(signal_types t,std::size_t line) : token_base(line),mType(t) {}

		virtual token_types get_type() const noexcept override
		{
			return token_types::signal;
		}

		signal_types get_signal() const noexcept
		{
			return this->mType;
		}
	};

	class token_id final : public token_base {
		std::string mId;
	public:
		token_id() = delete;

		token_id(const std::string &id) : mId(id) {}

		virtual token_types get_type() const noexcept override
		{
			return token_types::id;
		}

		const std::string &get_id() const noexcept
		{
			return this->mId;
		}
	};

	class token_value final : public token_base {
		var mVal;
	public:
		token_value() = delete;

		token_value(const var &val) : mVal(val) {}

		virtual token_types get_type() const noexcept override
		{
			return token_types::value;
		}

		var &get_value() noexcept
		{
			return this->mVal;
		}
	};

	class token_sblist final : public token_base {
		std::deque<std::deque<token_base *>> mList;
	public:
		token_sblist() = delete;

		token_sblist(const std::deque<std::deque<token_base *>> &list) : mList(list) {}

		virtual token_types get_type() const noexcept override
		{
			return token_types::sblist;
		}

		std::deque<std::deque<token_base *>> &get_list() noexcept
		{
			return this->mList;
		}
	};

	class token_mblist final : public token_base {
		std::deque<std::deque<token_base *>> mList;
	public:
		token_mblist() = delete;

		token_mblist(const std::deque<std::deque<token_base *>> &list) : mList(list) {}

		virtual token_types get_type() const noexcept override
		{
			return token_types::mblist;
		}

		std::deque<std::deque<token_base *>> &get_list() noexcept
		{
			return this->mList;
		}
	};

	class token_lblist final : public token_base {
		std::deque<std::deque<token_base *>> mList;
	public:
		token_lblist() = delete;

		token_lblist(const std::deque<std::deque<token_base *>> &list) : mList(list) {}

		virtual token_types get_type() const noexcept override
		{
			return token_types::lblist;
		}

		std::deque<std::deque<token_base *>> &get_list() noexcept
		{
			return this->mList;
		}
	};

	class token_expr final : public token_base {
		cov::tree<token_base *> mTree;
	public:
		token_expr() = delete;

		token_expr(const cov::tree<token_base *> &tree) : mTree(tree) {}

		virtual token_types get_type() const noexcept override
		{
			return token_types::expr;
		}

		cov::tree<token_base *> &get_tree() noexcept
		{
			return this->mTree;
		}
	};

	class token_arglist final : public token_base {
		std::deque<cov::tree<token_base *>> mTreeList;
	public:
		token_arglist() = default;

		token_arglist(const std::deque<cov::tree<token_base *>> &tlist) : mTreeList(tlist) {}

		virtual token_types get_type() const noexcept override
		{
			return token_types::arglist;
		}

		std::deque<cov::tree<token_base *>> &get_arglist() noexcept
		{
			return this->mTreeList;
		}
	};

	class token_array final : public token_base {
		std::deque<cov::tree<token_base *>> mTreeList;
	public:
		token_array() = default;

		token_array(const std::deque<cov::tree<token_base *>> &tlist) : mTreeList(tlist) {}

		virtual token_types get_type() const noexcept override
		{
			return token_types::array;
		}

		std::deque<cov::tree<token_base *>> &get_array() noexcept
		{
			return this->mTreeList;
		}
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

		statement_base(context_t c, token_base *eptr) : context(c),
			line_num(static_cast<token_endline *>(eptr)->get_line_num()) {}

		virtual ~statement_base() = default;

		std::size_t get_line_num() const noexcept
		{
			return line_num;
		}

		const std::string &get_file_path() const noexcept;

		const std::string &get_package_name() const noexcept;

		const std::string &get_raw_code() const noexcept;

		virtual statement_types get_type() const noexcept=0;

		virtual void run()=0;
	};

	class method_base {
		static garbage_collector<method_base> gc;
	protected:
		context_t context;
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

		method_base(context_t c) : context(c) {}

		virtual ~method_base() = default;

		virtual method_types get_type() const noexcept=0;

		virtual statement_types get_target_type() const noexcept=0;

		virtual void preprocess(const std::deque<std::deque<token_base *>> &) {}

		virtual statement_base *translate(const std::deque<std::deque<token_base *>> &)=0;
	};

	garbage_collector<token_base> token_base::gc;

	garbage_collector<statement_base> statement_base::gc;

	garbage_collector<method_base> method_base::gc;
}
