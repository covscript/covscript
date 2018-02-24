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
* Copyright (C) 2018 Michael Lee(李登淳)
* Email: mikecovlee@163.com
* Github: https://github.com/mikecovlee
*/
#include <covscript/statement.hpp>
#include <iostream>

namespace cs {
	var function::call(vector &args) const
	{
		if (args.size() != this->mArgs.size())
			throw syntax_error("Wrong size of arguments.Expected " + std::to_string(this->mArgs.size()) + ",provided " +
			                   std::to_string(args.size()));
		scope_guard scope(mContext);
		fcall_guard fcall(mContext);
		for (std::size_t i = 0; i < args.size(); ++i)
			mContext->instance->storage.add_var(this->mArgs[i], args[i]);
		for (auto &ptr:this->mBody) {
			try {
				ptr->run();
			}
			catch (const cs::exception &e) {
				throw e;
			}
			catch (const std::exception &e) {
				throw exception(ptr->get_line_num(), ptr->get_file_path(), ptr->get_raw_code(), e.what());
			}
			if (mContext->instance->return_fcall) {
				mContext->instance->return_fcall = false;
				return fcall.get();
			}
		}
		return fcall.get();
	}

	var struct_builder::operator()()
	{
		scope_guard scope(mContext);
		if (!mParent.empty()) {
			var builder = mContext->instance->storage.get_var(mParent);
			if (builder.type() == typeid(type)) {
				var parent = builder.const_val<type>().constructor();
				if (parent.type() == typeid(structure))
					mContext->instance->storage.involve_domain(parent.const_val<structure>().get_domain());
				else
					throw syntax_error("Target is not a struct.");
			}
			else
				throw syntax_error("Target is not a type.");
		}
		for (auto &ptr:this->mMethod) {
			try {
				ptr->run();
			}
			catch (const cs::exception &e) {
				throw e;
			}
			catch (const std::exception &e) {
				throw exception(ptr->get_line_num(), ptr->get_file_path(), ptr->get_raw_code(), e.what());
			}
		}
		return var::make<structure>(this->mHash, this->mName, scope.get());
	}

	void statement_expression::run()
	{
		context->instance->parse_expr(mTree.root());
	}

	void statement_expression::repl_run()
	{
		const var &result = context->instance->parse_expr(mTree.root());
		try {
			std::cout << result.to_string() << std::endl;
		}
		catch (cov::error &e) {
			if (!std::strcmp(e.what(), "E000D"))
				throw e;
		}
	}

	void statement_involve::run()
	{
		var ns = context->instance->parse_expr(mTree.root());
		if (ns.type() == typeid(name_space_t))
			context->instance->storage.involve_domain(ns.const_val<name_space_t>()->get_domain());
		else
			throw syntax_error("Only support involve namespace.");
	}

	void statement_var::run()
	{
		context->instance->storage.add_var(mDvp.id, copy(context->instance->parse_expr(mDvp.expr.root())));
	}

	void statement_break::run()
	{
		context->instance->break_block = true;
	}

	void statement_continue::run()
	{
		context->instance->continue_block = true;
	}

	void statement_block::run()
	{
		scope_guard scope(context);
		for (auto &ptr:mBlock) {
			try {
				ptr->run();
			}
			catch (const cs::exception &e) {
				throw e;
			}
			catch (const std::exception &e) {
				throw exception(ptr->get_line_num(), ptr->get_file_path(), ptr->get_raw_code(), e.what());
			}
			if (context->instance->return_fcall || context->instance->break_block || context->instance->continue_block)
				break;
		}
	}

	void statement_namespace::run()
	{
		context->instance->storage.add_var(this->mName,
		var::make_protect<name_space_t>(std::make_shared<name_space_holder>([this] {
			scope_guard scope(context);
			for (auto &ptr:mBlock)
			{
				try {
					ptr->run();
				}
				catch (const cs::exception &e) {
					throw e;
				}
				catch (const std::exception &e) {
					throw exception(ptr->get_line_num(), ptr->get_file_path(),
					                ptr->get_raw_code(), e.what());
				}
			}
			return scope.get();
		}())));
	}

	void statement_if::run()
	{
		if (context->instance->parse_expr(mTree.root()).const_val<boolean>()) {
			scope_guard scope(context);
			for (auto &ptr:mBlock) {
				try {
					ptr->run();
				}
				catch (const cs::exception &e) {
					throw e;
				}
				catch (const std::exception &e) {
					throw exception(ptr->get_line_num(), ptr->get_file_path(), ptr->get_raw_code(), e.what());
				}
				if (context->instance->return_fcall || context->instance->break_block ||
				        context->instance->continue_block)
					break;
			}
		}
	}

	void statement_ifelse::run()
	{
		if (context->instance->parse_expr(mTree.root()).const_val<boolean>()) {
			scope_guard scope(context);
			for (auto &ptr:mBlock) {
				try {
					ptr->run();
				}
				catch (const cs::exception &e) {
					throw e;
				}
				catch (const std::exception &e) {
					throw exception(ptr->get_line_num(), ptr->get_file_path(), ptr->get_raw_code(), e.what());
				}
				if (context->instance->return_fcall || context->instance->break_block ||
				        context->instance->continue_block)
					break;
			}
		}
		else {
			scope_guard scope(context);
			for (auto &ptr:mElseBlock) {
				try {
					ptr->run();
				}
				catch (const cs::exception &e) {
					throw e;
				}
				catch (const std::exception &e) {
					throw exception(ptr->get_line_num(), ptr->get_file_path(), ptr->get_raw_code(), e.what());
				}
				if (context->instance->return_fcall || context->instance->break_block ||
				        context->instance->continue_block)
					break;
			}
		}
	}

	void statement_switch::run()
	{
		var key = context->instance->parse_expr(mTree.root());
		if (mCases.count(key) > 0)
			mCases[key]->run();
		else if (mDefault != nullptr)
			mDefault->run();
	}

	void statement_while::run()
	{
		if (context->instance->break_block)
			context->instance->break_block = false;
		if (context->instance->continue_block)
			context->instance->continue_block = false;
		scope_guard scope(context);
		while (context->instance->parse_expr(mTree.root()).const_val<boolean>()) {
			scope.clear();
			for (auto &ptr:mBlock) {
				try {
					ptr->run();
				}
				catch (const cs::exception &e) {
					throw e;
				}
				catch (const std::exception &e) {
					throw exception(ptr->get_line_num(), ptr->get_file_path(), ptr->get_raw_code(), e.what());
				}
				if (context->instance->return_fcall) {
					return;
				}
				if (context->instance->break_block) {
					context->instance->break_block = false;
					return;
				}
				if (context->instance->continue_block) {
					context->instance->continue_block = false;
					break;
				}
			}
		}
	}

	void statement_loop::run()
	{
		if (context->instance->break_block)
			context->instance->break_block = false;
		if (context->instance->continue_block)
			context->instance->continue_block = false;
		scope_guard scope(context);
		do {
			scope.clear();
			for (auto &ptr:mBlock) {
				try {
					ptr->run();
				}
				catch (const cs::exception &e) {
					throw e;
				}
				catch (const std::exception &e) {
					throw exception(ptr->get_line_num(), ptr->get_file_path(), ptr->get_raw_code(), e.what());
				}
				if (context->instance->return_fcall) {
					return;
				}
				if (context->instance->break_block) {
					context->instance->break_block = false;
					return;
				}
				if (context->instance->continue_block) {
					context->instance->continue_block = false;
					break;
				}
			}
		}
		while (!(mExpr != nullptr && context->instance->parse_expr(mExpr->get_tree().root()).const_val<boolean>()));
	}

	void statement_for::run()
	{
		if (context->instance->break_block)
			context->instance->break_block = false;
		if (context->instance->continue_block)
			context->instance->continue_block = false;
		scope_guard scope(context);
		var val = copy(context->instance->context->instance->parse_expr(mDvp.expr.root()));
		while (val.const_val<number>() <= context->instance->parse_expr(mEnd.root()).const_val<number>()) {
			scope.clear();
			context->instance->storage.add_var(mDvp.id, val);
			for (auto &ptr:mBlock) {
				try {
					ptr->run();
				}
				catch (const cs::exception &e) {
					throw e;
				}
				catch (const std::exception &e) {
					throw exception(ptr->get_line_num(), ptr->get_file_path(), ptr->get_raw_code(), e.what());
				}
				if (context->instance->return_fcall) {
					return;
				}
				if (context->instance->break_block) {
					context->instance->break_block = false;
					return;
				}
				if (context->instance->continue_block) {
					context->instance->continue_block = false;
					break;
				}
			}
			val.val<number>(true) += context->instance->parse_expr(mStep.root()).const_val<number>();
		}
	}

	template<typename T, typename X>
	void foreach_helper(context_t context, const string &iterator, const var &obj, std::deque<statement_base *> &body)
	{
		if (obj.const_val<T>().empty())
			return;
		if (context->instance->break_block)
			context->instance->break_block = false;
		if (context->instance->continue_block)
			context->instance->continue_block = false;
		scope_guard scope(context);
		for (const X &it:obj.const_val<T>()) {
			scope.clear();
			context->instance->storage.add_var(iterator, it);
			for (auto &ptr:body) {
				try {
					ptr->run();
				}
				catch (const cs::exception &e) {
					throw e;
				}
				catch (const std::exception &e) {
					throw exception(ptr->get_line_num(), ptr->get_file_path(), ptr->get_raw_code(), e.what());
				}
				if (context->instance->return_fcall) {
					return;
				}
				if (context->instance->break_block) {
					context->instance->break_block = false;
					return;
				}
				if (context->instance->continue_block) {
					context->instance->continue_block = false;
					break;
				}
			}
		}
	}

	void statement_foreach::run()
	{
		const var &obj = context->instance->parse_expr(this->mObj.root());
		if (obj.type() == typeid(string))
			foreach_helper<string, char>(context, this->mIt, obj, this->mBlock);
		else if (obj.type() == typeid(list))
			foreach_helper<list, var>(context, this->mIt, obj, this->mBlock);
		else if (obj.type() == typeid(array))
			foreach_helper<array, var>(context, this->mIt, obj, this->mBlock);
		else if (obj.type() == typeid(hash_map))
			foreach_helper<hash_map, pair>(context, this->mIt, obj, this->mBlock);
		else
			throw syntax_error("Unsupported type(foreach)");
	}

	void statement_struct::run()
	{
		context->instance->storage.add_struct(this->mName, this->mBuilder);
	}

	void statement_function::run()
	{
		if (this->mIsMemFn)
			context->instance->storage.add_var(this->mName,
			                                   var::make_protect<callable>(this->mFunc, callable::types::member_fn),
			                                   mOverride);
		else
			context->instance->storage.add_var(this->mName, var::make_protect<callable>(this->mFunc), mOverride);
	}

	void statement_return::run()
	{
		if (context->instance->fcall_stack.empty())
			throw syntax_error("Return outside function.");
		context->instance->fcall_stack.top() = context->instance->parse_expr(this->mTree.root());
		context->instance->return_fcall = true;
	}

	void statement_try::run()
	{
		scope_guard scope(context);
		for (auto &ptr:mTryBody) {
			try {
				ptr->run();
			}
			catch (const lang_error &le) {
				scope.clear();
				context->instance->storage.add_var(mName, le);
				for (auto &ptr:mCatchBody) {
					try {
						ptr->run();
					}
					catch (const cs::exception &e) {
						throw e;
					}
					catch (const std::exception &e) {
						throw exception(ptr->get_line_num(), ptr->get_file_path(), ptr->get_raw_code(), e.what());
					}
					if (context->instance->return_fcall || context->instance->break_block ||
					        context->instance->continue_block)
						break;
				}
				return;
			}
			catch (const cs::exception &e) {
				throw e;
			}
			catch (const std::exception &e) {
				throw exception(ptr->get_line_num(), ptr->get_file_path(), ptr->get_raw_code(), e.what());
			}
			if (context->instance->return_fcall || context->instance->break_block || context->instance->continue_block)
				break;
		}
	}

	void statement_throw::run()
	{
		var e = context->instance->parse_expr(this->mTree.root());
		if (e.type() != typeid(lang_error))
			throw syntax_error("Throwing unsupported exception.");
		else
			throw e.const_val<lang_error>();
	}
}
