#pragma once
/*
* Covariant Script Grammar Implement
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
* Copyright (C) 2017 Michael Lee(李登淳)
* Email: mikecovlee@163.com
* Github: https://github.com/mikecovlee
*/
#include "./optimize.hpp"

#define CS_STACK_SIZE 1024
namespace cs {
	bool return_fcall=false;
	bool break_block=false;
	bool continue_block=false;
	cov::static_stack<var,CS_STACK_SIZE> fcall_stack;
	var function::call(array& args) const
	{
		if(args.size()!=this->mArgs.size())
			throw syntax_error("Wrong size of arguments.");
		runtime->storage.add_domain();
		fcall_stack.push(number(0));
		for(std::size_t i=0; i<args.size(); ++i)
			runtime->storage.add_var(this->mArgs[i],args[i]);
		for(auto& ptr:this->mBody) {
			try {
				ptr->run();
			}
			catch(const lang_error& le) {
				runtime->storage.remove_domain();
				throw le;
			}
			catch(const cs::exception& e) {
				throw e;
			}
			catch(const std::exception& e) {
				throw exception(ptr->get_line_num(),ptr->get_file_path(),ptr->get_code(),e.what());
			}
			if(return_fcall) {
				return_fcall=false;
				runtime->storage.remove_domain();
				var retval=fcall_stack.top();
				fcall_stack.pop();
				return retval;
			}
		}
		runtime->storage.remove_domain();
		var retval=fcall_stack.top();
		fcall_stack.pop();
		return retval;
	}
	var struct_builder::operator()()
	{
		runtime->storage.add_domain();
		for(auto& ptr:this->mMethod) {
			try {
				ptr->run();
			}
			catch(const lang_error& le) {
				runtime->storage.remove_domain();
				throw le;
			}
			catch(const cs::exception& e) {
				throw e;
			}
			catch(const std::exception& e) {
				throw exception(ptr->get_line_num(),ptr->get_file_path(),ptr->get_code(),e.what());
			}
		}
		var dat=var::make<structure>(this->mHash,this->mName,runtime->storage.get_domain());
		runtime->storage.remove_domain();
		return dat;
	}
	void statement_expression::run()
	{
		parse_expr(mTree.root());
	}
	void statement_var::run()
	{
		runtime->storage.add_var(mDvp.id,copy(parse_expr(mDvp.expr.root())));
	}
	void statement_break::run()
	{
		break_block=true;
	}
	void statement_continue::run()
	{
		continue_block=true;
	}
	void statement_block::run()
	{
		runtime->storage.add_domain();
		for(auto& ptr:mBlock) {
			try {
				ptr->run();
			}
			catch(const lang_error& le) {
				runtime->storage.remove_domain();
				throw le;
			}
			catch(const cs::exception& e) {
				throw e;
			}
			catch(const std::exception& e) {
				throw exception(ptr->get_line_num(),ptr->get_file_path(),ptr->get_code(),e.what());
			}
			if(return_fcall||break_block||continue_block)
				break;
		}
		runtime->storage.remove_domain();
	}
	void statement_namespace::run()
	{
		runtime->storage.add_domain();
		for(auto& ptr:mBlock) {
			try {
				ptr->run();
			}
			catch(const lang_error& le) {
				runtime->storage.remove_domain();
				throw le;
			}
			catch(const cs::exception& e) {
				throw e;
			}
			catch(const std::exception& e) {
				throw exception(ptr->get_line_num(),ptr->get_file_path(),ptr->get_code(),e.what());
			}
		}
		domain_t domain=runtime->storage.get_domain();
		runtime->storage.remove_domain();
		runtime->storage.add_var(this->mName,var::make_protect<name_space_t>(std::make_shared<name_space_holder>(domain)));
	}
	void statement_if::run()
	{
		if(parse_expr(mTree.root()).const_val<boolean>()) {
			runtime->storage.add_domain();
			for(auto& ptr:mBlock) {
				try {
					ptr->run();
				}
				catch(const lang_error& le) {
					runtime->storage.remove_domain();
					throw le;
				}
				catch(const cs::exception& e) {
					throw e;
				}
				catch(const std::exception& e) {
					throw exception(ptr->get_line_num(),ptr->get_file_path(),ptr->get_code(),e.what());
				}
				if(return_fcall||break_block||continue_block)
					break;
			}
			runtime->storage.remove_domain();
		}
	}
	void statement_ifelse::run()
	{
		if(parse_expr(mTree.root()).const_val<boolean>()) {
			runtime->storage.add_domain();
			for(auto& ptr:mBlock) {
				try {
					ptr->run();
				}
				catch(const lang_error& le) {
					runtime->storage.remove_domain();
					throw le;
				}
				catch(const cs::exception& e) {
					throw e;
				}
				catch(const std::exception& e) {
					throw exception(ptr->get_line_num(),ptr->get_file_path(),ptr->get_code(),e.what());
				}
				if(return_fcall||break_block||continue_block)
					break;
			}
			runtime->storage.remove_domain();
		}
		else {
			runtime->storage.add_domain();
			for(auto& ptr:mElseBlock) {
				try {
					ptr->run();
				}
				catch(const lang_error& le) {
					runtime->storage.remove_domain();
					throw le;
				}
				catch(const cs::exception& e) {
					throw e;
				}
				catch(const std::exception& e) {
					throw exception(ptr->get_line_num(),ptr->get_file_path(),ptr->get_code(),e.what());
				}
				if(return_fcall||break_block||continue_block)
					break;
			}
			runtime->storage.remove_domain();
		}
	}
	void statement_switch::run()
	{
		var key=parse_expr(mTree.root());
		if(mCases.count(key)>0)
			mCases[key]->run();
		else if(mDefault!=nullptr)
			mDefault->run();
	}
	void statement_while::run()
	{
		if(break_block)
			break_block=false;
		if(continue_block)
			continue_block=false;
		runtime->storage.add_domain();
		while(parse_expr(mTree.root()).const_val<boolean>()) {
			runtime->storage.clear_domain();
			for(auto& ptr:mBlock) {
				try {
					ptr->run();
				}
				catch(const lang_error& le) {
					runtime->storage.remove_domain();
					throw le;
				}
				catch(const cs::exception& e) {
					throw e;
				}
				catch(const std::exception& e) {
					throw exception(ptr->get_line_num(),ptr->get_file_path(),ptr->get_code(),e.what());
				}
				if(return_fcall) {
					runtime->storage.remove_domain();
					return;
				}
				if(break_block) {
					break_block=false;
					runtime->storage.remove_domain();
					return;
				}
				if(continue_block) {
					continue_block=false;
					break;
				}
			}
		}
		runtime->storage.remove_domain();
	}
	void statement_loop::run()
	{
		if(break_block)
			break_block=false;
		if(continue_block)
			continue_block=false;
		runtime->storage.add_domain();
		do {
			runtime->storage.clear_domain();
			for(auto& ptr:mBlock) {
				try {
					ptr->run();
				}
				catch(const lang_error& le) {
					runtime->storage.remove_domain();
					throw le;
				}
				catch(const cs::exception& e) {
					throw e;
				}
				catch(const std::exception& e) {
					throw exception(ptr->get_line_num(),ptr->get_file_path(),ptr->get_code(),e.what());
				}
				if(return_fcall) {
					runtime->storage.remove_domain();
					return;
				}
				if(break_block) {
					break_block=false;
					runtime->storage.remove_domain();
					return;
				}
				if(continue_block) {
					continue_block=false;
					break;
				}
			}
		}
		while(!(mExpr!=nullptr&&parse_expr(mExpr->get_tree().root()).const_val<boolean>()));
		runtime->storage.remove_domain();
	}
	void statement_for::run()
	{
		if(break_block)
			break_block=false;
		if(continue_block)
			continue_block=false;
		runtime->storage.add_domain();
		var val=copy(parse_expr(mDvp.expr.root()));
		while(val.const_val<number>()<=parse_expr(mEnd.root()).const_val<number>()) {
			runtime->storage.clear_domain();
			runtime->storage.add_var(mDvp.id,val);
			for(auto& ptr:mBlock) {
				try {
					ptr->run();
				}
				catch(const lang_error& le) {
					runtime->storage.remove_domain();
					throw le;
				}
				catch(const cs::exception& e) {
					throw e;
				}
				catch(const std::exception& e) {
					throw exception(ptr->get_line_num(),ptr->get_file_path(),ptr->get_code(),e.what());
				}
				if(return_fcall) {
					runtime->storage.remove_domain();
					return;
				}
				if(break_block) {
					break_block=false;
					runtime->storage.remove_domain();
					return;
				}
				if(continue_block) {
					continue_block=false;
					break;
				}
			}
			val.val<number>(true)+=parse_expr(mStep.root()).const_val<number>();
		}
		runtime->storage.remove_domain();
	}
	template<typename T,typename X>void foreach_helper(const string& iterator,const var& obj,std::deque<statement_base*>& body)
	{
		if(obj.const_val<T>().empty())
			return;
		if(break_block)
			break_block=false;
		if(continue_block)
			continue_block=false;
		runtime->storage.add_domain();
		for(const X& it:obj.const_val<T>()) {
			runtime->storage.clear_domain();
			runtime->storage.add_var(iterator,it);
			for(auto& ptr:body) {
				try {
					ptr->run();
				}
				catch(const lang_error& le) {
					runtime->storage.remove_domain();
					throw le;
				}
				catch(const cs::exception& e) {
					throw e;
				}
				catch(const std::exception& e) {
					throw exception(ptr->get_line_num(),ptr->get_file_path(),ptr->get_code(),e.what());
				}
				if(return_fcall) {
					runtime->storage.remove_domain();
					return;
				}
				if(break_block) {
					break_block=false;
					runtime->storage.remove_domain();
					return;
				}
				if(continue_block) {
					continue_block=false;
					break;
				}
			}
		}
		runtime->storage.remove_domain();
	}
	void statement_foreach::run()
	{
		const var& obj=parse_expr(this->mObj.root());
		if(obj.type()==typeid(string))
			foreach_helper<string,char>(this->mIt,obj,this->mBlock);
		else if(obj.type()==typeid(list))
			foreach_helper<list,var>(this->mIt,obj,this->mBlock);
		else if(obj.type()==typeid(array))
			foreach_helper<array,var>(this->mIt,obj,this->mBlock);
		else if(obj.type()==typeid(hash_map))
			foreach_helper<hash_map,pair>(this->mIt,obj,this->mBlock);
		else
			throw syntax_error("Unsupported type(foreach)");
	}
	void statement_struct::run()
	{
		runtime->storage.add_struct(this->mName,this->mBuilder);
	}
	void statement_function::run()
	{
		if(this->mIsMemFn)
			runtime->storage.add_var(this->mName,var::make_protect<callable>(this->mFunc,callable::types::member_fn));
		else
			runtime->storage.add_var(this->mName,var::make_protect<callable>(this->mFunc));
	}
	void statement_return::run()
	{
		if(fcall_stack.empty())
			throw syntax_error("Return outside function.");
		fcall_stack.top()=parse_expr(this->mTree.root());
		return_fcall=true;
	}
	void statement_try::run()
	{
		runtime->storage.add_domain();
		for(auto& ptr:mTryBody) {
			try {
				ptr->run();
			}
			catch(const lang_error& le) {
				runtime->storage.clear_domain();
				runtime->storage.add_var(mName,le);
				for(auto& ptr:mCatchBody) {
					try {
						ptr->run();
					}
					catch(const lang_error& le) {
						runtime->storage.remove_domain();
						throw le;
					}
					catch(const cs::exception& e) {
						throw e;
					}
					catch(const std::exception& e) {
						throw exception(ptr->get_line_num(),ptr->get_file_path(),ptr->get_code(),e.what());
					}
					if(return_fcall||break_block||continue_block)
						break;
				}
				runtime->storage.remove_domain();
				return;
			}
			catch(const cs::exception& e) {
				throw e;
			}
			catch(const std::exception& e) {
				throw exception(ptr->get_line_num(),ptr->get_file_path(),ptr->get_code(),e.what());
			}
			if(return_fcall||break_block||continue_block)
				break;
		}
		runtime->storage.remove_domain();
	}
	void statement_throw::run()
	{
		var e=parse_expr(this->mTree.root());
		if(e.type()!=typeid(lang_error))
			throw syntax_error("Throwing unsupported exception.");
		else
			throw e.const_val<lang_error>();
	}
	void kill_action(std::deque<std::deque<token_base*>> lines,std::deque<statement_base*>& statements,bool raw=false)
	{
		std::deque<std::deque<token_base*>> tmp;
		method_type* method=nullptr;
		token_endline* endsig=nullptr;
		int level=0;
		for(auto& line:lines) {
			endsig=dynamic_cast<token_endline*>(line.back());
			try {
				if(raw) {
					process_brackets(line);
					kill_brackets(line);
					kill_expr(line);
				}
				method_type* m=&translator.match(line);
				switch(m->type) {
				case grammar_types::null:
					throw syntax_error("Null type of grammar.");
					break;
				case grammar_types::single: {
					if(level>0) {
						if(m->statement_type==statement_types::end_) {
							runtime->storage.remove_set();
							runtime->storage.remove_domain();
							--level;
						}
						if(level==0) {
							statements.push_back(method->function(tmp));
							tmp.clear();
							method=nullptr;
						}
						else
							tmp.push_back(line);
					}
					else
						statements.push_back(m->function({line}));
				}
				break;
				case grammar_types::block: {
					if(level==0)
						method=m;
					++level;
					runtime->storage.add_domain();
					runtime->storage.add_set();
					m->init({line});
					tmp.push_back(line);
				}
				break;
				case grammar_types::jit_command:
					m->function({line});
					break;
				}
			}
			catch(const cs::exception& e) {
				throw e;
			}
			catch(const std::exception& e) {
				throw exception(endsig->get_num(),endsig->get_file(),endsig->get_code(),e.what());
			}
		}
		if(level!=0)
			throw syntax_error("Lack of the \"end\" signal.");
	}
	void translate_into_statements(std::deque<token_base*>& tokens,std::deque<statement_base*>& statements)
	{
		std::deque<std::deque<token_base*>> lines;
		std::deque<token_base*> tmp;
		for(auto& ptr:tokens) {
			tmp.push_back(ptr);
			if(ptr!=nullptr&&ptr->get_type()==token_types::endline) {
				if(tmp.size()>1)
					lines.push_back(tmp);
				tmp.clear();
			}
		}
		if(tmp.size()>1)
			lines.push_back(tmp);
		tmp.clear();
		kill_action(lines,statements,true);
	}
	void init_grammar()
	{
		// Expression Grammar
		translator.add_method({new token_expr(cov::tree<token_base*>()),new token_endline(0)},method_type {statement_types::expression_,grammar_types::single,[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				return new statement_expression(dynamic_cast<token_expr*>(raw.front().front())->get_tree(),raw.front().back());
			}
		});
		// Import Grammar
		translator.add_method({new token_action(action_types::import_),new token_expr(cov::tree<token_base*>()),new token_endline(0)},method_type {statement_types::import_,grammar_types::jit_command,[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				runtime_t rt=covscript(dynamic_cast<token_value*>(dynamic_cast<token_expr*>(raw.front().at(1))->get_tree().root().data())->get_value().const_val<string>());
				if(rt->package_name.empty())
					throw syntax_error("Only packages are allowed to import.");
				runtime->storage.add_var(rt->package_name,var::make_protect<extension_t>(std::make_shared<extension_holder>(rt->storage.get_global())));
				return nullptr;
			}
		});
		// Package Grammar
		translator.add_method({new token_action(action_types::package_),new token_expr(cov::tree<token_base*>()),new token_endline(0)},method_type {statement_types::package_,grammar_types::jit_command,[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				if(!runtime->package_name.empty())
					throw syntax_error("Redefinition of package");
				runtime->package_name=dynamic_cast<token_id*>(dynamic_cast<token_expr*>(raw.front().at(1))->get_tree().root().data())->get_id();
				runtime->storage.add_var_global(runtime->package_name,var::make_protect<extension_t>(std::make_shared<extension_holder>(runtime->storage.get_global())));
				return nullptr;
			}
		});
		// Var Grammar
		translator.add_method({new token_action(action_types::var_),new token_expr(cov::tree<token_base*>()),new token_endline(0)},method_type {statement_types::var_,grammar_types::single,[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				cov::tree<token_base*>& tree=dynamic_cast<token_expr*>(raw.front().at(1))->get_tree();
				define_var_profile dvp;
				parse_define_var(tree,dvp);
				return new statement_var(dvp,raw.front().back());
			}
		});
		translator.add_method({new token_action(action_types::constant_),new token_action(action_types::var_),new token_expr(cov::tree<token_base*>()),new token_endline(0)},method_type {statement_types::constant_,grammar_types::jit_command,[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				cov::tree<token_base*>& tree=dynamic_cast<token_expr*>(raw.front().at(2))->get_tree();
				define_var_profile dvp;
				parse_define_var(tree,dvp);
				if(dvp.expr.root().data()->get_type()!=token_types::value)
					throw syntax_error("Constant variable must have an constant value.");
				runtime->storage.add_var(dvp.id,static_cast<token_value*>(dvp.expr.root().data())->get_value());
				return nullptr;
			}
		});
		// End Grammar
		translator.add_method({new token_action(action_types::endblock_),new token_endline(0)},method_type {statement_types::end_,grammar_types::single,[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				return new statement_end;
			}
		});
		// Block Grammar
		translator.add_method({new token_action(action_types::block_),new token_endline(0)},method_type {statement_types::block_,grammar_types::block,[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				std::deque<statement_base*> body;
				kill_action({raw.begin()+1,raw.end()},body);
				return new statement_block(body,raw.front().back());
			}
		});
		// Namespace Grammar
		translator.add_method({new token_action(action_types::namespace_),new token_expr(cov::tree<token_base*>()),new token_endline(0)},method_type {statement_types::namespace_,grammar_types::block,[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				std::deque<statement_base*> body;
				kill_action({raw.begin()+1,raw.end()},body);
				for(auto& ptr:body)
					if(ptr->get_type()!=statement_types::var_&&ptr->get_type()!=statement_types::function_&&ptr->get_type()!=statement_types::namespace_&&ptr->get_type()!=statement_types::struct_)
						throw syntax_error("Wrong grammar for namespace definition.");
				return new statement_namespace(dynamic_cast<token_expr*>(raw.front().at(1))->get_tree().root().data(),body,raw.front().back());
			}
		});
		// If Grammar
		translator.add_method({new token_action(action_types::if_),new token_expr(cov::tree<token_base*>()),new token_endline(0)},method_type {statement_types::if_,grammar_types::block,[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				bool have_else=false;
				std::deque<statement_base*> body;
				kill_action({raw.begin()+1,raw.end()},body);
				for(auto& ptr:body)
				{
					if(ptr->get_type()==statement_types::else_) {
						if(!have_else)
							have_else=true;
						else
							throw syntax_error("Multi Else Grammar.");
					}
				}
				if(have_else)
				{
					std::deque<statement_base*> body_true;
					std::deque<statement_base*> body_false;
					bool now_place=true;
					for(auto& ptr:body) {
						if(ptr->get_type()==statement_types::else_) {
							now_place=false;
							continue;
						}
						if(now_place)
							body_true.push_back(ptr);
						else
							body_false.push_back(ptr);
					}
					return new statement_ifelse(dynamic_cast<token_expr*>(raw.front().at(1))->get_tree(),body_true,body_false,raw.front().back());
				}
				else
					return new statement_if(dynamic_cast<token_expr*>(raw.front().at(1))->get_tree(),body,raw.front().back());
			}
		});
		// Else Grammar
		translator.add_method({new token_action(action_types::else_),new token_endline(0)},method_type {statement_types::else_,grammar_types::single,[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				return new statement_else;
			}
		});
		// Switch Grammar
		translator.add_method({new token_action(action_types::switch_),new token_expr(cov::tree<token_base*>()),new token_endline(0)},method_type {statement_types::switch_,grammar_types::block,[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				std::deque<statement_base*> body;
				kill_action({raw.begin()+1,raw.end()},body);
				statement_block* dptr=nullptr;
				std::unordered_map<var,statement_block*> cases;
				for(auto& it:body)
				{
					if(it==nullptr)
						throw internal_error("Access Null Pointer.");
					else if(it->get_type()==statement_types::case_) {
						statement_case* scptr=dynamic_cast<statement_case*>(it);
						if(cases.count(scptr->get_tag())>0)
							throw syntax_error("Redefinition of case.");
						cases.emplace(scptr->get_tag(),scptr->get_block());
					}
					else if(it->get_type()==statement_types::default_) {
						statement_default* sdptr=dynamic_cast<statement_default*>(it);
						if(dptr!=nullptr)
							throw syntax_error("Redefinition of default case.");
						dptr=sdptr->get_block();
					}
					else
						throw syntax_error("Wrong format of switch statement.");
				}
				return new statement_switch(dynamic_cast<token_expr*>(raw.front().at(1))->get_tree(),cases,dptr,raw.front().back());
			}
		});
		// Case Grammar
		translator.add_method({new token_action(action_types::case_),new token_expr(cov::tree<token_base*>()),new token_endline(0)},method_type {statement_types::case_,grammar_types::block,[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				cov::tree<token_base*>& tree=dynamic_cast<token_expr*>(raw.front().at(1))->get_tree();
				if(tree.root().data()->get_type()!=token_types::value)
					throw syntax_error("Case Tag must be a constant value.");
				std::deque<statement_base*> body;
				kill_action({raw.begin()+1,raw.end()},body);
				return new statement_case(dynamic_cast<token_value*>(tree.root().data())->get_value(),body,raw.front().back());
			}
		});
		// Default Grammar
		translator.add_method({new token_action(action_types::default_),new token_endline(0)},method_type {statement_types::default_,grammar_types::block,[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				std::deque<statement_base*> body;
				kill_action({raw.begin()+1,raw.end()},body);
				return new statement_default(body,raw.front().back());
			}
		});
		// While Grammar
		translator.add_method({new token_action(action_types::while_),new token_expr(cov::tree<token_base*>()),new token_endline(0)},method_type {statement_types::while_,grammar_types::block,[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				std::deque<statement_base*> body;
				kill_action({raw.begin()+1,raw.end()},body);
				return new statement_while(dynamic_cast<token_expr*>(raw.front().at(1))->get_tree(),body,raw.front().back());
			}
		});
		// Until Grammar
		translator.add_method({new token_action(action_types::until_),new token_expr(cov::tree<token_base*>()),new token_endline(0)},method_type {statement_types::until_,grammar_types::single,[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				return new statement_until(dynamic_cast<token_expr*>(raw.front().at(1)),raw.front().back());
			}
		});
		// Loop Grammar
		translator.add_method({new token_action(action_types::loop_),new token_endline(0)},method_type {statement_types::loop_,grammar_types::block,[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				std::deque<statement_base*> body;
				kill_action({raw.begin()+1,raw.end()},body);
				if(!body.empty()&&body.back()->get_type()==statement_types::until_)
				{
					token_expr* expr=dynamic_cast<statement_until*>(body.back())->get_expr();
					body.pop_back();
					return new statement_loop(expr,body,raw.front().back());
				}
				else
					return new statement_loop(nullptr,body,raw.front().back());
			}
		});
		// For Grammar
		translator.add_method({new token_action(action_types::for_),new token_expr(cov::tree<token_base*>()),new token_action(action_types::to_),new token_expr(cov::tree<token_base*>()),new token_action(action_types::step_),new token_expr(cov::tree<token_base*>()),new token_endline(0)},method_type {statement_types::for_,grammar_types::block,[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				cov::tree<token_base*>& tree=dynamic_cast<token_expr*>(raw.front().at(1))->get_tree();
				define_var_profile dvp;
				parse_define_var(tree,dvp);
				runtime->storage.add_record(dvp.id);
				return nullptr;
			},[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				std::deque<statement_base*> body;
				kill_action({raw.begin()+1,raw.end()},body);
				return new statement_for(dynamic_cast<token_expr*>(raw.front().at(1))->get_tree(),dynamic_cast<token_expr*>(raw.front().at(3))->get_tree(),dynamic_cast<token_expr*>(raw.front().at(5))->get_tree(),body,raw.front().back());
			}
		});
		translator.add_method({new token_action(action_types::for_),new token_expr(cov::tree<token_base*>()),new token_action(action_types::to_),new token_expr(cov::tree<token_base*>()),new token_endline(0)},method_type {statement_types::for_,grammar_types::block,[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				cov::tree<token_base*>& tree=dynamic_cast<token_expr*>(raw.front().at(1))->get_tree();
				define_var_profile dvp;
				parse_define_var(tree,dvp);
				runtime->storage.add_record(dvp.id);
				return nullptr;
			},[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				std::deque<statement_base*> body;
				kill_action({raw.begin()+1,raw.end()},body);
				cov::tree<token_base*> tree_step;
				tree_step.emplace_root_left(tree_step.root(),new token_value(number(1)));
				return new statement_for(dynamic_cast<token_expr*>(raw.front().at(1))->get_tree(),dynamic_cast<token_expr*>(raw.front().at(3))->get_tree(),tree_step,body,raw.front().back());
			}
		});
		translator.add_method({new token_action(action_types::for_),new token_expr(cov::tree<token_base*>()),new token_action(action_types::iterate_),new token_expr(cov::tree<token_base*>()),new token_endline(0)},method_type {statement_types::foreach_,grammar_types::block,[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				cov::tree<token_base*>& t=dynamic_cast<token_expr*>(raw.front().at(1))->get_tree();
				if(t.root().data()==nullptr)
					throw internal_error("Null pointer accessed.");
				if(t.root().data()->get_type()!=token_types::id)
					throw syntax_error("Wrong grammar(foreach)");
				runtime->storage.add_record(dynamic_cast<token_id*>(t.root().data())->get_id());
				return nullptr;
			},[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				cov::tree<token_base*>& t=dynamic_cast<token_expr*>(raw.front().at(1))->get_tree();
				if(t.root().data()==nullptr)
					throw internal_error("Null pointer accessed.");
				if(t.root().data()->get_type()!=token_types::id)
					throw syntax_error("Wrong grammar(foreach)");
				const std::string& it=dynamic_cast<token_id*>(t.root().data())->get_id();
				std::deque<statement_base*> body;
				kill_action({raw.begin()+1,raw.end()},body);
				return new statement_foreach(it,dynamic_cast<token_expr*>(raw.front().at(3))->get_tree(),body,raw.front().back());
			}
		});
		// Break Grammar
		translator.add_method({new token_action(action_types::break_),new token_endline(0)},method_type {statement_types::break_,grammar_types::single,[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				return new statement_break(raw.front().back());
			}
		});
		// Continue Grammar
		translator.add_method({new token_action(action_types::continue_),new token_endline(0)},method_type {statement_types::continue_,grammar_types::single,[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				return new statement_continue(raw.front().back());
			}
		});
		// Function Grammar
		translator.add_method({new token_action(action_types::function_),new token_expr(cov::tree<token_base*>()),new token_endline(0)},method_type {statement_types::function_,grammar_types::block,[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				cov::tree<token_base*>& t=dynamic_cast<token_expr*>(raw.front().at(1))->get_tree();
				if(t.root().data()==nullptr)
					throw internal_error("Null pointer accessed.");
				if(t.root().data()->get_type()!=token_types::signal||dynamic_cast<token_signal*>(t.root().data())->get_signal()!=signal_types::fcall_)
					throw syntax_error("Wrong grammar for function definition.");
				if(t.root().left().data()==nullptr)
					throw internal_error("Null pointer accessed.");
				if(t.root().left().data()->get_type()!=token_types::id)
					throw syntax_error("Wrong grammar for function definition.");
				if(t.root().right().data()==nullptr)
					throw internal_error("Null pointer accessed.");
				if(t.root().right().data()->get_type()!=token_types::arglist)
					throw syntax_error("Wrong grammar for function definition.");
				std::string name=dynamic_cast<token_id*>(t.root().left().data())->get_id();
				std::deque<std::string> args;
				for(auto& it:dynamic_cast<token_arglist*>(t.root().right().data())->get_arglist())
				{
					if(it.root().data()==nullptr)
						throw internal_error("Null pointer accessed.");
					if(it.root().data()->get_type()!=token_types::id)
						throw syntax_error("Wrong grammar for function definition.");
					const std::string& str=dynamic_cast<token_id*>(it.root().data())->get_id();
					for(auto& it:args)
						if(it==str)
							throw syntax_error("Redefinition of function argument.");
					args.push_back(str);
				}
				std::deque<statement_base*> body;
				kill_action({raw.begin()+1,raw.end()},body);
				return new statement_function(name,args,body,raw.front().back());
			}
		});
		// Return Grammar
		translator.add_method({new token_action(action_types::return_),new token_expr(cov::tree<token_base*>()),new token_endline(0)},method_type {statement_types::return_,grammar_types::single,[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				return new statement_return(dynamic_cast<token_expr*>(raw.front().at(1))->get_tree(),raw.front().back());
			}
		});
		translator.add_method({new token_action(action_types::return_),new token_endline(0)},method_type {statement_types::return_,grammar_types::single,[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				cov::tree<token_base*> tree;
				tree.emplace_root_left(tree.root(),new token_value(number(0)));
				return new statement_return(tree,raw.front().back());
			}
		});
		// Struct Grammar
		translator.add_method({new token_action(action_types::struct_),new token_expr(cov::tree<token_base*>()),new token_endline(0)},method_type {statement_types::struct_,grammar_types::block,[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				cov::tree<token_base*>& t=dynamic_cast<token_expr*>(raw.front().at(1))->get_tree();
				if(t.root().data()==nullptr)
					throw internal_error("Null pointer accessed.");
				if(t.root().data()->get_type()!=token_types::id)
					throw syntax_error("Wrong grammar for struct definition.");
				std::string name=dynamic_cast<token_id*>(t.root().data())->get_id();
				std::deque<statement_base*> body;
				kill_action({raw.begin()+1,raw.end()},body);
				for(auto& ptr:body)
				{
					switch(ptr->get_type()) {
					default:
						throw syntax_error("Wrong grammar for struct definition.");
					case statement_types::var_:
						break;
					case statement_types::function_:
						static_cast<statement_function*>(ptr)->set_mem_fn();
						break;
					}
				}
				return new statement_struct(name,body,raw.front().back());
			}
		});
		// Try Grammar
		translator.add_method({new token_action(action_types::try_),new token_endline(0)},method_type {statement_types::try_,grammar_types::block,[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				std::deque<statement_base*> body;
				kill_action({raw.begin()+1,raw.end()},body);
				std::string name;
				std::deque<statement_base*> tbody,cbody;
				bool founded=false;
				for(auto& ptr:body)
				{
					if(ptr->get_type()==statement_types::catch_) {
						name=static_cast<statement_catch*>(ptr)->get_name();
						founded=true;
						continue;
					}
					if(founded)
						cbody.push_back(ptr);
					else
						tbody.push_back(ptr);
				}
				if(!founded)
					throw syntax_error("Wrong grammar for try statement.");
				return new statement_try(name,tbody,cbody,raw.front().back());
			}
		});
		// Catch Grammar
		translator.add_method({new token_action(action_types::catch_),new token_expr(cov::tree<token_base*>()),new token_endline(0)},method_type {statement_types::catch_,grammar_types::single,[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				cov::tree<token_base*>& t=dynamic_cast<token_expr*>(raw.front().at(1))->get_tree();
				if(t.root().data()==nullptr)
					throw internal_error("Null pointer accessed.");
				if(t.root().data()->get_type()!=token_types::id)
					throw syntax_error("Wrong grammar for catch statement.");
				return new statement_catch(dynamic_cast<token_id*>(t.root().data())->get_id(),raw.front().back());
			}
		});
		// Throw Grammar
		translator.add_method({new token_action(action_types::throw_),new token_expr(cov::tree<token_base*>()),new token_endline(0)},method_type {statement_types::throw_,grammar_types::single,[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				return new statement_throw(dynamic_cast<token_expr*>(raw.front().at(1))->get_tree(),raw.front().back());
			}
		});
	}
}
