/*
* Covariant Script Statement
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
* Copyright (C) 2017-2022 Michael Lee(李登淳)
*
* This software is registered with the National Copyright Administration
* of the People's Republic of China(Registration Number: 2020SR0408026)
* and is protected by the Copyright Law of the People's Republic of China.
*
* Email:   lee@covariant.cn, mikecovlee@163.com
* Github:  https://github.com/mikecovlee
* Website: http://covscript.org.cn
*/
#include <covscript/impl/statement.hpp>
#include <iostream>

namespace cs {
	var function::call(vector &args) const
	{
		current_process->poll_event();
		if (!mIsVargs && args.size() != this->mArgs.size())
			throw runtime_error(
			    "Wrong size of arguments.Expected " + std::to_string(this->mArgs.size()) + ",provided " +
			    std::to_string(args.size()));
		scope_guard scope(mContext);
#ifdef CS_DEBUGGER
		fcall_guard fcall(mDecl);
		if(mMatch)
			cs_debugger_func_callback(mDecl, mStmt);
#else
		fcall_guard fcall;
#endif
		if (mIsVargs) {
			var arg_list = var::make<cs::array>();
			auto &arr = arg_list.val<cs::array>();
			std::size_t i = 0;
			if (mIsMemFn)
				mContext->instance->storage.add_var("this", args[i++]);
			if (mIsLambda)
				mContext->instance->storage.add_var("self", args[i++]);
			for (; i < args.size(); ++i)
				arr.push_back(args[i]);
			mContext->instance->storage.add_var(this->mArgs.front(), arg_list);
		}
		else {
			for (std::size_t i = 0; i < args.size(); ++i)
				mContext->instance->storage.add_var(this->mArgs[i], args[i]);
		}
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
		if (mParent.root().usable()) {
			var builder = mContext->instance->parse_expr(mParent.root());
			if (builder.type() == typeid(type_t)) {
				const auto &t = builder.const_val<type_t>();
				if (mTypeId == t.id)
					throw runtime_error("Can not inherit itself.");
				var parent = t.constructor();
				if (parent.type() == typeid(structure)) {
					parent.protect();
					mContext->instance->storage.involve_domain(parent.const_val<structure>().get_domain());
					mContext->instance->storage.add_var("parent", parent, true);
				}
				else
					throw runtime_error("Target is not a struct.");
			}
			else
				throw runtime_error("Target is not a type.");
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
		return var::make<structure>(this->mTypeId, this->mName, scope.get());
	}

	void statement_expression::run_impl()
	{
		CS_DEBUGGER_STEP(this);
		context->instance->parse_expr(mTree.root());
	}

	void statement_expression::repl_run_impl()
	{
		const var &result = context->instance->parse_expr(mTree.root());
		std::cout << "Result: " << result.to_string() << std::endl;
	}

	void statement_expression::dump(std::ostream &o) const
	{
		o << "< Expression: ";
		compiler_type::dump_expr(mTree.root(), o);
		o << " >\n";
	}

	void statement_import::run_impl()
	{
		for (auto &val:m_var_list)
			context->instance->storage.add_var(val.first, val.second, true);
	}

	void statement_import::dump(std::ostream &o) const
	{
		for (auto &val:m_var_list)
			o << "< Import: Name = \"" << val.first << "\" >\n";
	}

	void statement_involve::run_impl()
	{
		CS_DEBUGGER_STEP(this);
		context->instance->parse_using(mTree.root(), mOverride);
	}

	void statement_involve::dump(std::ostream &o) const
	{
		o << "< Using: ";
		compiler_type::dump_expr(mTree.root(), o);
		o << " >\n";
	}

	void statement_var::run_impl()
	{
		CS_DEBUGGER_STEP(this);
		context->instance->parse_define_var(mTree.root(), false, link);
	}

	void statement_var::dump(std::ostream &o) const
	{
		o << "< Var: ";
		compiler_type::dump_expr(mTree.root(), o);
		o << (link ? ", type = link" : "") << " >\n";
	}

	void statement_constant::run_impl()
	{
		CS_DEBUGGER_STEP(this);
		context->instance->parse_define_var(mTree.root(), true);
	}

	void statement_constant::dump(std::ostream &o) const
	{
		o << "< Constant: ";
		compiler_type::dump_expr(mTree.root(), o);
		o << " >\n";
	}

	void statement_break::run_impl()
	{
		CS_DEBUGGER_STEP(this);
		context->instance->break_block = true;
	}

	void statement_break::dump(std::ostream &o) const
	{
		o << "< Break >\n";
	}

	void statement_continue::run_impl()
	{
		CS_DEBUGGER_STEP(this);
		context->instance->continue_block = true;
	}

	void statement_continue::dump(std::ostream &o) const
	{
		o << "< Continue >\n";
	}

	void statement_block::run_impl()
	{
		CS_DEBUGGER_STEP(this);
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

	void statement_block::dump(std::ostream &o) const
	{
		o << "< BeginBlock >\n";
		for (auto &ptr:mBlock)
			ptr->dump(o);
		o << "< EndBlock >\n";
	}

	void statement_namespace::run_impl()
	{
		CS_DEBUGGER_STEP(this);
		context->instance->storage.add_var(this->mName,
		make_namespace(make_shared_namespace<name_space>([this] {
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
		}())), true);
	}

	void statement_namespace::dump(std::ostream &o) const
	{
		o << "< BeginNamespace: ID = \"" << mName << "\" >\n";
		for (auto &ptr:mBlock)
			ptr->dump(o);
		o << "< EndNamespace >\n";
	}

	void statement_if::run_impl()
	{
		CS_DEBUGGER_STEP(this);
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

	void statement_if::dump(std::ostream &o) const
	{
		o << "< BeginIf: Condition = ";
		compiler_type::dump_expr(mTree.root(), o);
		o << " >\n";
		for (auto &ptr:mBlock)
			ptr->dump(o);
		o << "< EndIf >\n";
	}

	void statement_ifelse::run_impl()
	{
		CS_DEBUGGER_STEP(this);
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

	void statement_ifelse::dump(std::ostream &o) const
	{
		o << "< BeginIfElse: Condition = ";
		compiler_type::dump_expr(mTree.root(), o);
		o << " >\n";
		for (auto &ptr:mBlock)
			ptr->dump(o);
		o << "< Else >\n";
		for (auto &ptr:mElseBlock)
			ptr->dump(o);
		o << "< EndIfElse >\n";
	}

	void statement_switch::run_impl()
	{
		CS_DEBUGGER_STEP(this);
		var key = context->instance->parse_expr(mTree.root());
		if (mCases.count(key) > 0)
			mCases[key]->run();
		else if (mDefault != nullptr)
			mDefault->run();
	}

	void statement_switch::dump(std::ostream &o) const
	{
		o << "< BeginSwitch: Condition = ";
		compiler_type::dump_expr(mTree.root(), o);
		o << " >\n";
		for (auto &it:mCases) {
			o << "< BeginCase: Tag = \"";
			o << it.first.to_string();
			o << "\" >\n";
			for (auto &ptr:it.second->get_block())
				ptr->dump(o);
			o << "< EndCase >\n";
		}
		if (mDefault != nullptr) {
			o << "< BeginDefaultCase >\n";
			for (auto &ptr:mDefault->get_block())
				ptr->dump(o);
			o << "< EndDefaultCase >\n";
		}
		o << "< EndSwitch >\n";
	}

	void statement_while::run_impl()
	{
		CS_DEBUGGER_STEP(this);
		if (context->instance->break_block)
			context->instance->break_block = false;
		if (context->instance->continue_block)
			context->instance->continue_block = false;
		scope_guard scope(context);
		while (context->instance->parse_expr(mTree.root()).const_val<boolean>()) {
			scope.clear();
			current_process->poll_event();
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

	void statement_while::dump(std::ostream &o) const
	{
		o << "< BeginWhile: Condition = ";
		compiler_type::dump_expr(mTree.root(), o);
		o << " >\n";
		for (auto &ptr:mBlock)
			ptr->dump(o);
		o << "< EndWhile >\n";
	}

	void statement_loop::run_impl()
	{
		CS_DEBUGGER_STEP(this);
		if (context->instance->break_block)
			context->instance->break_block = false;
		if (context->instance->continue_block)
			context->instance->continue_block = false;
		scope_guard scope(context);
		while (true) {
			scope.clear();
			current_process->poll_event();
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

	void statement_loop::dump(std::ostream &o) const
	{
		o << "< BeginLoop >\n";
		for (auto &ptr:mBlock)
			ptr->dump(o);
		o << "< EndLoop >\n";
	}

	void statement_loop_until::run_impl()
	{
		CS_DEBUGGER_STEP(this);
		if (context->instance->break_block)
			context->instance->break_block = false;
		if (context->instance->continue_block)
			context->instance->continue_block = false;
		scope_guard scope(context);
		do {
			scope.clear();
			current_process->poll_event();
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
		while (!context->instance->parse_expr(mExpr.root()).const_val<boolean>());
	}

	void statement_loop_until::dump(std::ostream &o) const
	{
		o << "< BeginLoop >\n";
		for (auto &ptr:mBlock)
			ptr->dump(o);
		o << "< Until: Condition = ";
		compiler_type::dump_expr(mExpr.root(), o);
		o << " >\n";
		o << "< EndLoop >\n";
	}

	void statement_for::run_impl()
	{
		CS_DEBUGGER_STEP(this);
		if (context->instance->break_block)
			context->instance->break_block = false;
		if (context->instance->continue_block)
			context->instance->continue_block = false;
		scope_guard top_scope(context);
		context->instance->parse_define_var(mParallel[0].root());
		scope_guard scope(context);
		while (true) {
			scope.clear();
			current_process->poll_event();
			if (!context->instance->parse_expr(mParallel[1].root()).const_val<boolean>())
				break;
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
			context->instance->parse_expr(mParallel[2].root());
		}
	}

	void statement_for::dump(std::ostream &o) const
	{
		o << "< BeginFor >\n";
		o << "< Iterator = ";
		compiler_type::dump_expr(mParallel[0].root(), o);
		o << " >\n< Condition = ";
		compiler_type::dump_expr(mParallel[1].root(), o);
		o << " >\n< Increment = ";
		compiler_type::dump_expr(mParallel[2].root(), o);
		o << " >\n< Body >\n";
		for (auto &ptr:mBlock)
			ptr->dump(o);
		o << "< EndFor >\n";
	}

	template<typename T, typename X>
	void foreach_helper(const context_t &context, const string &iterator, const var &obj,
	                    std::deque<statement_base *> &body)
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
			current_process->poll_event();
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

	void statement_foreach::run_impl()
	{
		CS_DEBUGGER_STEP(this);
		const var &obj = context->instance->parse_expr(this->mObj.root());
		if (obj.type() == typeid(string))
			foreach_helper<string, char>(context, this->mIt, obj, this->mBlock);
		else if (obj.type() == typeid(list))
			foreach_helper<list, var>(context, this->mIt, obj, this->mBlock);
		else if (obj.type() == typeid(array))
			foreach_helper<array, var>(context, this->mIt, obj, this->mBlock);
		else if (obj.type() == typeid(hash_set))
			foreach_helper<hash_set, var>(context, this->mIt, obj, this->mBlock);
		else if (obj.type() == typeid(hash_map))
			foreach_helper<hash_map, pair>(context, this->mIt, obj, this->mBlock);
		else if (obj.type() == typeid(range_type))
			foreach_helper<range_type, number>(context, this->mIt, obj, this->mBlock);
		else
			throw runtime_error("Unsupported type(foreach)");
	}

	void statement_foreach::dump(std::ostream &o) const
	{
		o << "< BeginForEach >\n";
		o << "< IteratorID = \"" << mIt << "\", TargetValue = ";
		compiler_type::dump_expr(mObj.root(), o);
		o << " >\n< Body >\n";
		for (auto &ptr:mBlock)
			ptr->dump(o);
		o << "< EndForEach >\n";
	}

	void statement_struct::run_impl()
	{
		CS_DEBUGGER_STEP(this);
		context->instance->storage.add_struct(this->mName, this->mBuilder);
	}

	void statement_struct::dump(std::ostream &o) const
	{
		o << "< BeginStruct: ID = \"" << mName << "\"";
		if (mParent.root().usable()) {
			o << ", Parent = ";
			compiler_type::dump_expr(mParent.root(), o);
		}
		o << " >\n";
		for (auto &ptr:mBlock)
			ptr->dump(o);
		o << "< EndStruct >\n";
	}

	void statement_function::run_impl()
	{
		CS_DEBUGGER_STEP(this);
		if (this->mIsMemFn)
			context->instance->storage.add_var(this->mName,
			                                   var::make_protect<callable>(this->mFunc, callable::types::member_fn),
			                                   mOverride);
		else {
			var func = var::make_protect<callable>(this->mFunc);
#ifdef CS_DEBUGGER
			if(context->instance->storage.is_initial())
				cs_debugger_func_breakpoint(this->mName, func);
#endif
			context->instance->storage.add_var(this->mName, func, mOverride);
		}
	}

	void statement_function::dump(std::ostream &o) const
	{
		o << "< BeginFunction: ID = \"" << mName << "\"";
		if (mOverride)
			o << ", Override = True >\n";
		else
			o << ", Override = False >\n";
		o << "< FunctionArgs = {";
		for (auto &name:mArgs)
			o << "< ID = \"" << name << "\" >";
		o << "} >\n< Body >\n";
		for (auto &ptr:mBlock)
			ptr->dump(o);
		o << "< EndFunction >\n";
	}

	void statement_return::run_impl()
	{
		CS_DEBUGGER_STEP(this);
		if (current_process->stack.empty())
			throw runtime_error("Return outside function.");
		current_process->stack.top() = context->instance->parse_expr(this->mTree.root());
		context->instance->return_fcall = true;
	}

	void statement_return::dump(std::ostream &o) const
	{
		o << "< Return: ";
		compiler_type::dump_expr(mTree.root(), o);
		o << " >\n";
	}

	void statement_try::run_impl()
	{
		CS_DEBUGGER_STEP(this);
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

	void statement_try::dump(std::ostream &o) const
	{
		o << "< Try >\n";
		for (auto &ptr:mTryBody)
			ptr->dump(o);
		o << "< Catch: ID = \"" << mName << "\" >\n";
		for (auto &ptr:mCatchBody)
			ptr->dump(o);
		o << "< EndTry >\n";
	}

	void statement_throw::run_impl()
	{
		CS_DEBUGGER_STEP(this);
		var e = context->instance->parse_expr(this->mTree.root());
		if (e.type() != typeid(lang_error))
			throw runtime_error("Throwing unsupported exception.");
		else
			throw e.const_val<lang_error>();
	}

	void statement_throw::dump(std::ostream &o) const
	{
		o << "< Throw: ";
		compiler_type::dump_expr(mTree.root(), o);
		o << " >\n";
	}
}
