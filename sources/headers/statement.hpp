#pragma once
/*
* Covariant Script Statement
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
#include "./symbols.hpp"

namespace cs {
	class statement_expression final : public statement_base {
		cov::tree<token_base *> mTree;
	public:
		statement_expression() = delete;

		statement_expression(const cov::tree<token_base *> &tree, token_base *ptr) : statement_base(ptr), mTree(tree) {}

		virtual statement_types get_type() const noexcept override
		{
			return statement_types::expression_;
		}

		virtual void run() override;
	};

	class statement_var final : public statement_base {
		define_var_profile mDvp;
	public:
		statement_var() = delete;

		statement_var(const define_var_profile &dvp, token_base *ptr) : statement_base(ptr), mDvp(dvp) {}

		virtual statement_types get_type() const noexcept override
		{
			return statement_types::var_;
		}

		virtual void run() override;
	};

	class statement_break final : public statement_base {
	public:
		statement_break() = default;

		statement_break(token_base *ptr) : statement_base(ptr) {}

		virtual statement_types get_type() const noexcept override
		{
			return statement_types::break_;
		}

		virtual void run() override;
	};

	class statement_continue final : public statement_base {
	public:
		statement_continue() = default;

		statement_continue(token_base *ptr) : statement_base(ptr) {}

		virtual statement_types get_type() const noexcept override
		{
			return statement_types::continue_;
		}

		virtual void run() override;
	};

	class statement_block final : public statement_base {
		std::deque<statement_base *> mBlock;
	public:
		statement_block() = delete;

		statement_block(const std::deque<statement_base *> &block, token_base *ptr) : statement_base(ptr), mBlock(block) {}

		virtual statement_types get_type() const noexcept override
		{
			return statement_types::block_;
		}

		virtual void run() override;
	};

	class statement_namespace final : public statement_base {
		std::string mName;
		std::deque<statement_base *> mBlock;
	public:
		statement_namespace() = delete;

		statement_namespace(token_base *tbp, const std::deque<statement_base *> &block, token_base *ptr) : statement_base(ptr), mName(static_cast<token_id *>(tbp)->get_id()), mBlock(block) {}

		virtual statement_types get_type() const noexcept override
		{
			return statement_types::namespace_;
		}

		virtual void run() override;
	};

	class statement_if final : public statement_base {
		cov::tree<token_base *> mTree;
		std::deque<statement_base *> mBlock;
	public:
		statement_if() = delete;

		statement_if(const cov::tree<token_base *> &tree, const std::deque<statement_base *> &block, token_base *ptr) : statement_base(ptr), mTree(tree), mBlock(block) {}

		virtual statement_types get_type() const noexcept override
		{
			return statement_types::if_;
		}

		virtual void run() override;
	};

	class statement_ifelse final : public statement_base {
		cov::tree<token_base *> mTree;
		std::deque<statement_base *> mBlock;
		std::deque<statement_base *> mElseBlock;
	public:
		statement_ifelse() = delete;

		statement_ifelse(const cov::tree<token_base *> &tree, const std::deque<statement_base *> &btrue, const std::deque<statement_base *> &bfalse, token_base *ptr) : statement_base(ptr), mTree(tree), mBlock(btrue), mElseBlock(bfalse) {}

		virtual statement_types get_type() const noexcept override
		{
			return statement_types::if_;
		}

		virtual void run() override;
	};

	class statement_else final : public statement_base {
	public:
		statement_else() = default;

		virtual statement_types get_type() const noexcept override
		{
			return statement_types::else_;
		}

		virtual void run() override
		{
			throw syntax_error("Do not allowed standalone else statement.");
		}
	};

	class statement_switch final : public statement_base {
		cov::tree<token_base *> mTree;
		statement_block *mDefault = nullptr;
		std::unordered_map<var, statement_block *> mCases;
	public:
		statement_switch() = delete;

		statement_switch(const cov::tree<token_base *> &tree, const std::unordered_map<var, statement_block *> &cases, statement_block *dptr, token_base *ptr) : statement_base(ptr), mTree(tree), mCases(cases), mDefault(dptr) {}

		virtual statement_types get_type() const noexcept override
		{
			return statement_types::switch_;
		}

		virtual void run() override;
	};

	class statement_case final : public statement_base {
		var mTag;
		statement_block *mBlock;
	public:
		statement_case() = delete;

		statement_case(const var &tag, const std::deque<statement_base *> &b, token_base *ptr) : statement_base(ptr), mTag(copy(tag)), mBlock(new statement_block(b, ptr)) {}

		virtual statement_types get_type() const noexcept override
		{
			return statement_types::case_;
		}

		virtual void run() override
		{
			throw syntax_error("Do not allowed standalone case statement.");
		}

		const var &get_tag() const
		{
			return this->mTag;
		}

		statement_block *get_block() const
		{
			return this->mBlock;
		}
	};

	class statement_default final : public statement_base {
		statement_block *mBlock;
	public:
		statement_default() = delete;

		statement_default(const std::deque<statement_base *> &b, token_base *ptr) : statement_base(ptr), mBlock(new statement_block(b, ptr)) {}

		virtual statement_types get_type() const noexcept override
		{
			return statement_types::default_;
		}

		virtual void run() override
		{
			throw syntax_error("Do not allowed standalone default statement.");
		}

		statement_block *get_block() const
		{
			return this->mBlock;
		}
	};

	class statement_while final : public statement_base {
		cov::tree<token_base *> mTree;
		std::deque<statement_base *> mBlock;
	public:
		statement_while() = delete;

		statement_while(const cov::tree<token_base *> &tree, const std::deque<statement_base *> &b, token_base *ptr) : statement_base(ptr), mTree(tree), mBlock(b) {}

		virtual statement_types get_type() const noexcept override
		{
			return statement_types::while_;
		}

		virtual void run() override;
	};

	class statement_until final : public statement_base {
		token_expr *mExpr = nullptr;
	public:
		statement_until() = delete;

		statement_until(token_expr *expr, token_base *ptr) : statement_base(ptr), mExpr(expr) {}

		virtual statement_types get_type() const noexcept override
		{
			return statement_types::until_;
		}

		token_expr *get_expr() const
		{
			return mExpr;
		}

		virtual void run() override
		{
			throw syntax_error("Do not allowed standalone until statement.");
		}
	};

	class statement_loop final : public statement_base {
		token_expr *mExpr = nullptr;
		std::deque<statement_base *> mBlock;
	public:
		statement_loop() = delete;

		statement_loop(token_expr *expr, const std::deque<statement_base *> &b, token_base *ptr) : statement_base(ptr), mExpr(expr), mBlock(b) {}

		virtual statement_types get_type() const noexcept override
		{
			return statement_types::loop_;
		}

		virtual void run() override;
	};

	class statement_for final : public statement_base {
		define_var_profile mDvp;
		cov::tree<token_base *> mEnd;
		cov::tree<token_base *> mStep;
		std::deque<statement_base *> mBlock;
	public:
		statement_for() = delete;

		statement_for(cov::tree<token_base *> &tree0, const cov::tree<token_base *> &tree1, const cov::tree<token_base *> &tree2, const std::deque<statement_base *> &b, token_base *ptr) : statement_base(ptr), mEnd(tree1), mStep(tree2), mBlock(b)
		{
			parse_define_var(tree0, mDvp);
		}

		virtual statement_types get_type() const noexcept override
		{
			return statement_types::for_;
		}

		virtual void run() override;
	};

	class statement_foreach final : public statement_base {
		std::string mIt;
		cov::tree<token_base *> mObj;
		std::deque<statement_base *> mBlock;
	public:
		statement_foreach() = delete;

		statement_foreach(const std::string &it, const cov::tree<token_base *> &tree1, const std::deque<statement_base *> &b, token_base *ptr) : statement_base(ptr), mIt(it), mObj(tree1), mBlock(b) {}

		virtual statement_types get_type() const noexcept override
		{
			return statement_types::foreach_;
		}

		virtual void run() override;
	};

	class statement_struct final : public statement_base {
		std::string mName;
		struct_builder mBuilder;
	public:
		statement_struct() = delete;

		statement_struct(const std::string &name, const std::deque<statement_base *> &method, token_base *ptr) : statement_base(ptr), mName(name), mBuilder(name, method) {}

		virtual statement_types get_type() const noexcept override
		{
			return statement_types::struct_;
		}

		virtual void run() override;
	};

	class statement_function final : public statement_base {
		std::string mName;
		function mFunc;
		bool mIsMemFn = false;
	public:
		statement_function() = delete;

		statement_function(const std::string &name, const std::deque<std::string> &args, const std::deque<statement_base *> &body, token_base *ptr) : statement_base(ptr), mName(name), mFunc(args, body) {}

		virtual statement_types get_type() const noexcept override
		{
			return statement_types::function_;
		}

		void set_mem_fn()
		{
			mFunc.add_this();
			mIsMemFn = true;
		}

		virtual void run() override;
	};

	class statement_return final : public statement_base {
		cov::tree<token_base *> mTree;
	public:
		statement_return() = delete;

		statement_return(const cov::tree<token_base *> &tree, token_base *ptr) : statement_base(ptr), mTree(tree) {}

		virtual statement_types get_type() const noexcept override
		{
			return statement_types::return_;
		}

		virtual void run() override;
	};

	class statement_end final : public statement_base {
	public:
		statement_end() = default;

		virtual statement_types get_type() const noexcept override
		{
			return statement_types::end_;
		}

		virtual void run() override
		{
			throw syntax_error("Do not allowed standalone end statement.");
		}
	};

	class statement_try final : public statement_base {
		std::string mName;
		std::deque<statement_base *> mTryBody;
		std::deque<statement_base *> mCatchBody;
	public:
		statement_try() = delete;

		statement_try(const std::string &name, const std::deque<statement_base *> &tbody, const std::deque<statement_base *> &cbody, token_base *ptr) : statement_base(ptr), mName(name), mTryBody(tbody), mCatchBody(cbody) {}

		virtual statement_types get_type() const noexcept override
		{
			return statement_types::try_;
		}

		virtual void run() override;
	};

	class statement_catch final : public statement_base {
		std::string mName;
	public:
		statement_catch() = delete;

		statement_catch(const std::string &name, token_base *ptr) : statement_base(ptr), mName(name) {}

		virtual statement_types get_type() const noexcept override
		{
			return statement_types::catch_;
		}

		const std::string &get_name() const
		{
			return this->mName;
		}

		virtual void run() override
		{
			throw syntax_error("Do not allowed standalone catch statement.");
		}
	};

	class statement_throw final : public statement_base {
		cov::tree<token_base *> mTree;
	public:
		statement_throw() = delete;

		statement_throw(const cov::tree<token_base *> &tree, token_base *ptr) : statement_base(ptr), mTree(tree) {}

		virtual statement_types get_type() const noexcept override
		{
			return statement_types::throw_;
		}

		virtual void run() override;
	};
}

#include "./optimize.hpp"

namespace cs {
	var function::call(array &args) const
	{
		if (args.size() != this->mArgs.size())
			throw syntax_error("Wrong size of arguments.");
		scope_guard scope(mContext);
		fcall_guard fcall(mContext);
		for (std::size_t i = 0; i < args.size(); ++i)
			mContext->instance->storage.add_var(this->mArgs[i], args[i]);
		for (auto &ptr:this->mBody) {
			try {
				ptr->run();
			}
			catch (const lang_error &le) {
				throw le;
			}
			catch (const cs::exception &e) {
				throw e;
			}
			catch (const std::exception &e) {
				throw exception(ptr->get_line_num(), ptr->get_file_path(), ptr->get_code(), e.what());
			}
			if (return_fcall) {
				return_fcall = false;
				return fcall.get();
			}
		}
		return fcall.get();
	}

	var struct_builder::operator()()
	{
		scope_guard scope(mContext);
		for (auto &ptr:this->mMethod) {
			try {
				ptr->run();
			}
			catch (const lang_error &le) {
				throw le;
			}
			catch (const cs::exception &e) {
				throw e;
			}
			catch (const std::exception &e) {
				throw exception(ptr->get_line_num(), ptr->get_file_path(), ptr->get_code(), e.what());
			}
		}
		return var::make<structure>(this->mHash, this->mName, scope.get());
	}

	void statement_expression::run()
	{
		parse_expr(mTree.root());
	}

	void statement_var::run()
	{
		runtime->storage.add_var(mDvp.id, copy(parse_expr(mDvp.expr.root())));
	}

	void statement_break::run()
	{
		break_block = true;
	}

	void statement_continue::run()
	{
		continue_block = true;
	}

	void statement_block::run()
	{
		runtime->storage.add_domain();
		for (auto &ptr:mBlock) {
			try {
				ptr->run();
			}
			catch (const lang_error &le) {
				runtime->storage.remove_domain();
				throw le;
			}
			catch (const cs::exception &e) {
				throw e;
			}
			catch (const std::exception &e) {
				throw exception(ptr->get_line_num(), ptr->get_file_path(), ptr->get_code(), e.what());
			}
			if (return_fcall || break_block || continue_block)
				break;
		}
		runtime->storage.remove_domain();
	}

	void statement_namespace::run()
	{
		runtime->storage.add_domain();
		for (auto &ptr:mBlock) {
			try {
				ptr->run();
			}
			catch (const lang_error &le) {
				runtime->storage.remove_domain();
				throw le;
			}
			catch (const cs::exception &e) {
				throw e;
			}
			catch (const std::exception &e) {
				throw exception(ptr->get_line_num(), ptr->get_file_path(), ptr->get_code(), e.what());
			}
		}
		domain_t domain = runtime->storage.get_domain();
		runtime->storage.remove_domain();
		runtime->storage.add_var(this->mName, var::make_protect<name_space_t>(std::make_shared<name_space_holder>(domain)));
	}

	void statement_if::run()
	{
		if (parse_expr(mTree.root()).const_val<boolean>()) {
			runtime->storage.add_domain();
			for (auto &ptr:mBlock) {
				try {
					ptr->run();
				}
				catch (const lang_error &le) {
					runtime->storage.remove_domain();
					throw le;
				}
				catch (const cs::exception &e) {
					throw e;
				}
				catch (const std::exception &e) {
					throw exception(ptr->get_line_num(), ptr->get_file_path(), ptr->get_code(), e.what());
				}
				if (return_fcall || break_block || continue_block)
					break;
			}
			runtime->storage.remove_domain();
		}
	}

	void statement_ifelse::run()
	{
		if (parse_expr(mTree.root()).const_val<boolean>()) {
			runtime->storage.add_domain();
			for (auto &ptr:mBlock) {
				try {
					ptr->run();
				}
				catch (const lang_error &le) {
					runtime->storage.remove_domain();
					throw le;
				}
				catch (const cs::exception &e) {
					throw e;
				}
				catch (const std::exception &e) {
					throw exception(ptr->get_line_num(), ptr->get_file_path(), ptr->get_code(), e.what());
				}
				if (return_fcall || break_block || continue_block)
					break;
			}
			runtime->storage.remove_domain();
		}
		else {
			runtime->storage.add_domain();
			for (auto &ptr:mElseBlock) {
				try {
					ptr->run();
				}
				catch (const lang_error &le) {
					runtime->storage.remove_domain();
					throw le;
				}
				catch (const cs::exception &e) {
					throw e;
				}
				catch (const std::exception &e) {
					throw exception(ptr->get_line_num(), ptr->get_file_path(), ptr->get_code(), e.what());
				}
				if (return_fcall || break_block || continue_block)
					break;
			}
			runtime->storage.remove_domain();
		}
	}

	void statement_switch::run()
	{
		var key = parse_expr(mTree.root());
		if (mCases.count(key) > 0)
			mCases[key]->run();
		else if (mDefault != nullptr)
			mDefault->run();
	}

	void statement_while::run()
	{
		if (break_block)
			break_block = false;
		if (continue_block)
			continue_block = false;
		runtime->storage.add_domain();
		while (parse_expr(mTree.root()).const_val<boolean>()) {
			runtime->storage.clear_domain();
			for (auto &ptr:mBlock) {
				try {
					ptr->run();
				}
				catch (const lang_error &le) {
					runtime->storage.remove_domain();
					throw le;
				}
				catch (const cs::exception &e) {
					throw e;
				}
				catch (const std::exception &e) {
					throw exception(ptr->get_line_num(), ptr->get_file_path(), ptr->get_code(), e.what());
				}
				if (return_fcall) {
					runtime->storage.remove_domain();
					return;
				}
				if (break_block) {
					break_block = false;
					runtime->storage.remove_domain();
					return;
				}
				if (continue_block) {
					continue_block = false;
					break;
				}
			}
		}
		runtime->storage.remove_domain();
	}

	void statement_loop::run()
	{
		if (break_block)
			break_block = false;
		if (continue_block)
			continue_block = false;
		runtime->storage.add_domain();
		do {
			runtime->storage.clear_domain();
			for (auto &ptr:mBlock) {
				try {
					ptr->run();
				}
				catch (const lang_error &le) {
					runtime->storage.remove_domain();
					throw le;
				}
				catch (const cs::exception &e) {
					throw e;
				}
				catch (const std::exception &e) {
					throw exception(ptr->get_line_num(), ptr->get_file_path(), ptr->get_code(), e.what());
				}
				if (return_fcall) {
					runtime->storage.remove_domain();
					return;
				}
				if (break_block) {
					break_block = false;
					runtime->storage.remove_domain();
					return;
				}
				if (continue_block) {
					continue_block = false;
					break;
				}
			}
		}
		while (!(mExpr != nullptr && parse_expr(mExpr->get_tree().root()).const_val<boolean>()));
		runtime->storage.remove_domain();
	}

	void statement_for::run()
	{
		if (break_block)
			break_block = false;
		if (continue_block)
			continue_block = false;
		runtime->storage.add_domain();
		var val = copy(parse_expr(mDvp.expr.root()));
		while (val.const_val<number>() <= parse_expr(mEnd.root()).const_val<number>()) {
			runtime->storage.clear_domain();
			runtime->storage.add_var(mDvp.id, val);
			for (auto &ptr:mBlock) {
				try {
					ptr->run();
				}
				catch (const lang_error &le) {
					runtime->storage.remove_domain();
					throw le;
				}
				catch (const cs::exception &e) {
					throw e;
				}
				catch (const std::exception &e) {
					throw exception(ptr->get_line_num(), ptr->get_file_path(), ptr->get_code(), e.what());
				}
				if (return_fcall) {
					runtime->storage.remove_domain();
					return;
				}
				if (break_block) {
					break_block = false;
					runtime->storage.remove_domain();
					return;
				}
				if (continue_block) {
					continue_block = false;
					break;
				}
			}
			val.val<number>(true) += parse_expr(mStep.root()).const_val<number>();
		}
		runtime->storage.remove_domain();
	}

	template<typename T, typename X>
	void foreach_helper(const string &iterator, const var &obj, std::deque<statement_base *> &body)
	{
		if (obj.const_val<T>().empty())
			return;
		if (break_block)
			break_block = false;
		if (continue_block)
			continue_block = false;
		runtime->storage.add_domain();
		for (const X &it:obj.const_val<T>()) {
			runtime->storage.clear_domain();
			runtime->storage.add_var(iterator, it);
			for (auto &ptr:body) {
				try {
					ptr->run();
				}
				catch (const lang_error &le) {
					runtime->storage.remove_domain();
					throw le;
				}
				catch (const cs::exception &e) {
					throw e;
				}
				catch (const std::exception &e) {
					throw exception(ptr->get_line_num(), ptr->get_file_path(), ptr->get_code(), e.what());
				}
				if (return_fcall) {
					runtime->storage.remove_domain();
					return;
				}
				if (break_block) {
					break_block = false;
					runtime->storage.remove_domain();
					return;
				}
				if (continue_block) {
					continue_block = false;
					break;
				}
			}
		}
		runtime->storage.remove_domain();
	}

	void statement_foreach::run()
	{
		const var &obj = parse_expr(this->mObj.root());
		if (obj.type() == typeid(string))
			foreach_helper<string, char>(this->mIt, obj, this->mBlock);
		else if (obj.type() == typeid(list))
			foreach_helper<list, var>(this->mIt, obj, this->mBlock);
		else if (obj.type() == typeid(array))
			foreach_helper<array, var>(this->mIt, obj, this->mBlock);
		else if (obj.type() == typeid(hash_map))
			foreach_helper<hash_map, pair>(this->mIt, obj, this->mBlock);
		else
			throw syntax_error("Unsupported type(foreach)");
	}

	void statement_struct::run()
	{
		runtime->storage.add_struct(this->mName, this->mBuilder);
	}

	void statement_function::run()
	{
		if (this->mIsMemFn)
			runtime->storage.add_var(this->mName, var::make_protect<callable>(this->mFunc, callable::types::member_fn));
		else
			runtime->storage.add_var(this->mName, var::make_protect<callable>(this->mFunc));
	}

	void statement_return::run()
	{
		if (fcall_stack.empty())
			throw syntax_error("Return outside function.");
		fcall_stack.top() = parse_expr(this->mTree.root());
		return_fcall = true;
	}

	void statement_try::run()
	{
		runtime->storage.add_domain();
		for (auto &ptr:mTryBody) {
			try {
				ptr->run();
			}
			catch (const lang_error &le) {
				runtime->storage.clear_domain();
				runtime->storage.add_var(mName, le);
				for (auto &ptr:mCatchBody) {
					try {
						ptr->run();
					}
					catch (const lang_error &le) {
						runtime->storage.remove_domain();
						throw le;
					}
					catch (const cs::exception &e) {
						throw e;
					}
					catch (const std::exception &e) {
						throw exception(ptr->get_line_num(), ptr->get_file_path(), ptr->get_code(), e.what());
					}
					if (return_fcall || break_block || continue_block)
						break;
				}
				runtime->storage.remove_domain();
				return;
			}
			catch (const cs::exception &e) {
				throw e;
			}
			catch (const std::exception &e) {
				throw exception(ptr->get_line_num(), ptr->get_file_path(), ptr->get_code(), e.what());
			}
			if (return_fcall || break_block || continue_block)
				break;
		}
		runtime->storage.remove_domain();
	}

	void statement_throw::run()
	{
		var e = parse_expr(this->mTree.root());
		if (e.type() != typeid(lang_error))
			throw syntax_error("Throwing unsupported exception.");
		else
			throw e.const_val<lang_error>();
	}
}
