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
#include <covscript/impl/statement.hpp>
#include <covscript/impl/flat_executor.hpp>

namespace cs {
	void flat_executor::push_frame(scope_type type)
	{
		if (!stack.empty())
			stack.top().pc = pc;
		stack.push(type);
		instance->storage.add_domain();
	}

	void flat_executor::pop_frame()
	{
		stack.pop_no_return();
		instance->storage.remove_domain();
	}

	void flat_executor::stack_rewind(scope_type type, bool delete_cur)
	{
		while (!stack.empty() && stack.top().type < type)
			pop_frame();
		if (stack.size() < 2)
			throw internal_error("No matching frame when stack rewinding.");
		if (delete_cur)
			pop_frame();
	}

	var executor_proxy::operator()(vector &args)
	{
		if (!mIsVargs && args.size() != this->mArgs.size())
			throw runtime_error(
			    "Wrong size of arguments.Expected " + std::to_string(this->mArgs.size()) + ",provided " +
			    std::to_string(args.size()));
		parent->push_frame(scope_type::task);
		fcall_guard fcall;
		if (mIsVargs) {
			var arg_list = var::make<cs::array>();
			auto &arr = arg_list.val<cs::array>();
			std::size_t i = 0;
			if (mIsMemFn)
				parent->get_instance()->storage.add_var("this", args[i++]);
			for (; i < args.size(); ++i)
				arr.push_back(args[i]);
			parent->get_instance()->storage.add_var(this->mArgs.front(), arg_list);
		}
		else {
			for (std::size_t i = 0; i < args.size(); ++i)
				parent->get_instance()->storage.add_var(this->mArgs[i], args[i]);
		}
		parent->pc = tag;
		parent->exec();
		parent->stack_rewind(scope_type::task);
		parent->recover_register();
		return fcall.get();
	}

	void statement_expression::gen_flat_ir(flat_executor *fe)
	{
		fe->push_ir<instruct_eval>(mTree);
	}

	void statement_var::gen_flat_ir(flat_executor *fe)
	{
		fe->push_ir<instruct_var>(mTree);
	}

	void statement_break::gen_flat_ir(flat_executor *fe)
	{
		fe->push_ir<instruct_internal>("break loop", [](flat_executor *fe) {
			fe->stack_rewind(scope_type::loop, false);
		});
		fe->push_ir<instruct_jump>(scope_type::loop);
	}

	void statement_continue::gen_flat_ir(flat_executor *fe)
	{
		fe->push_ir<instruct_internal>("continue loop", [](flat_executor *fe) {
			fe->stack_rewind(scope_type::loop, false);
		});
		fe->push_ir<instruct_jump>(fe->get_scope_intro(scope_type::loop));
	}

	void statement_block::gen_flat_ir(flat_executor *fe)
	{
		fe->push_ir<instruct_push_scope>();
		for (auto &it:mBlock)
			it->gen_flat_ir(fe);
		fe->push_ir<instruct_pop_scope>();
	}

	void statement_if::gen_flat_ir(flat_executor *fe)
	{
		fe->push_ir<instruct_push_scope>();
		fe->push_ir<instruct_cond>(mTree, false);
		for (auto &it:mBlock)
			it->gen_flat_ir(fe);
		fe->push_ir<instruct_pop_scope>();
	}

	void statement_ifelse::gen_flat_ir(flat_executor *fe)
	{
		fe->push_ir<instruct_push_scope>();
		fe->push_ir<instruct_cond>(mTree, 0, false);
		instruct_cond *jmp = static_cast<instruct_cond*>(fe->get_current_ir());
		for (auto &it:mBlock)
			it->gen_flat_ir(fe);
		fe->push_ir<instruct_jump>();
		jmp->tag = fe->pc - 1;
		for (auto &it:mElseBlock)
			it->gen_flat_ir(fe);
		fe->push_ir<instruct_pop_scope>();
	}

	void statement_while::gen_flat_ir(flat_executor *fe)
	{
		fe->push_ir<instruct_push_scope>(scope_type::loop);
		fe->push_ir<instruct_cond>(mTree, false, scope_type::loop);
		for (auto &it:mBlock)
			it->gen_flat_ir(fe);
		if (fe->has_instruct_in_scope<instruct_var>()) {
			fe->push_ir<instruct_internal>("clear scope", [](flat_executor *fe) {
				fe->get_instance()->storage.clear_domain();
			});
		}
		fe->push_ir<instruct_jump>(fe->get_scope_intro(scope_type::loop));
		fe->push_ir<instruct_pop_scope>();
	}

	void statement_loop::gen_flat_ir(flat_executor *fe)
	{
		fe->push_ir<instruct_push_scope>(scope_type::loop);
		for (auto &it:mBlock)
			it->gen_flat_ir(fe);
		if (fe->has_instruct_in_scope<instruct_var>()) {
			fe->push_ir<instruct_internal>("clear scope", [](flat_executor *fe) {
				fe->get_instance()->storage.clear_domain();
			});
		}
		fe->push_ir<instruct_jump>(fe->get_scope_intro(scope_type::loop));
		fe->push_ir<instruct_pop_scope>();
	}

	void statement_loop_until::gen_flat_ir(flat_executor *fe)
	{
		fe->push_ir<instruct_push_scope>(scope_type::loop);
		for (auto &it:mBlock)
			it->gen_flat_ir(fe);
		if (fe->has_instruct_in_scope<instruct_var>()) {
			fe->push_ir<instruct_internal>("clear scope", [](flat_executor *fe) {
				fe->get_instance()->storage.clear_domain();
			});
		}
		fe->push_ir<instruct_cond>(mExpr, fe->get_scope_intro(scope_type::loop), false);
		fe->push_ir<instruct_pop_scope>();
	}

	void statement_for::gen_flat_ir(flat_executor *fe)
	{
		fe->push_ir<instruct_push_scope>();
		fe->push_ir<instruct_var>(mParallel[0]);
		fe->push_ir<instruct_push_scope>(scope_type::loop);
		fe->push_ir<instruct_cond>(mParallel[1], false, scope_type::loop);
		for (auto &it:mBlock)
			it->gen_flat_ir(fe);
		fe->push_ir<instruct_eval>(mParallel[2]);
		if (fe->has_instruct_in_scope<instruct_var>()) {
			fe->push_ir<instruct_internal>("clear scope", [](flat_executor *fe) {
				fe->get_instance()->storage.clear_domain();
			});
		}
		fe->push_ir<instruct_jump>(fe->get_scope_intro(scope_type::loop));
		fe->push_ir<instruct_pop_scope>();
		fe->push_ir<instruct_pop_scope>();
	}
}

namespace cs_impl {
	template<typename type, typename iterator_t, typename data_t>
	void init_foreach(const cs::var &obj, cs::flat_executor *fe)
	{
		using namespace cs;
		flat_executor::iterate_helper &it = fe->begin_iteration();
		it.begin = var::make<iterator_t>(obj.const_val<type>().begin());
		it.end = var::make<iterator_t>(obj.const_val<type>().end());
		it.next = [&it]() {
			++it.begin.val<iterator_t>();
		};
		it.iterator = [&it]() -> var {
			return static_cast<data_t>(*it.begin.val<iterator_t>());
		};
	}
}

namespace cs {
	void statement_foreach::gen_flat_ir(flat_executor *fe)
	{
		fe->push_ir<instruct_internal>("foreach begin", [this](flat_executor *fe) {
			const var &obj = context->instance->parse_expr(this->mObj.root());
			if (obj.type() == typeid(string))
				cs_impl::init_foreach<string, string::const_iterator, char>(obj, fe);
			else if (obj.type() == typeid(list))
				cs_impl::init_foreach<list, list::const_iterator, var>(obj, fe);
			else if (obj.type() == typeid(array))
				cs_impl::init_foreach<array, array::const_iterator, var>(obj, fe);
			else if (obj.type() == typeid(hash_map))
				cs_impl::init_foreach<hash_map, hash_map::const_iterator, pair>(obj, fe);
			else if (obj.type() == typeid(range_type))
				cs_impl::init_foreach<range_type, range_iterator, number>(obj, fe);
			else
				throw runtime_error("Unsupported type(foreach)");
		});
		fe->push_ir<instruct_push_scope>(scope_type::loop);
		fe->push_ir<instruct_cond_internal>([](flat_executor *fe) {
			return fe->current_iteration().begin == fe->current_iteration().end;
		}, scope_type::loop);
		fe->push_ir<instruct_internal>("foreach intro", [this](flat_executor *fe) {
			fe->get_instance()->storage.add_var(mIt, fe->current_iteration().iterator(), true);
		});
		for (auto &it:mBlock)
			it->gen_flat_ir(fe);
		fe->push_ir<instruct_internal>("foreach iterate", [](flat_executor *fe) {
			fe->current_iteration().next();
		});
		if (fe->has_instruct_in_scope<instruct_var>()) {
			fe->push_ir<instruct_internal>("clear scope", [](flat_executor *fe) {
				fe->get_instance()->storage.clear_domain();
			});
		}
		fe->push_ir<instruct_jump>(fe->get_scope_intro(scope_type::loop));
		fe->push_ir<instruct_pop_scope>();
		fe->push_ir<instruct_internal>("foreach end", [](flat_executor *fe) {
			fe->end_iteration();
		});
	}

	void statement_function::gen_flat_ir(flat_executor *fe)
	{
		fe->push_scope();
		fe->push_ir<instruct_function_def>(mName, mOverride, mIsMemFn, executor_proxy(fe, mArgs, mIsMemFn, mIsVargs, fe->pc + 2));
		fe->push_ir<instruct_jump>();
		for (auto& it:mBlock)
			it->gen_flat_ir(fe);
		fe->push_ir<instruct_internal>("end function", [](flat_executor *fe) {
			fe->end_exec();
		});
		fe->pop_scope();
	}

	void statement_return::gen_flat_ir(flat_executor *fe)
	{
		fe->push_ir<instruct_internal>("return", [this](flat_executor *fe) {
			current_process->stack.top() = fe->get_instance()->parse_expr(this->mTree.root());
			fe->end_exec();
		});
	}
}