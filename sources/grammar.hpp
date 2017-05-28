#pragma once
#include "./optimize.hpp"
namespace cov_basic {
	bool return_fcall=false;
	bool inside_struct=false;
	bool break_block=false;
	bool continue_block=false;
	std::deque<const function*> fcall_stack;
	cov::any function::call(const array& args) const
	{
		if(args.size()!=this->mArgs.size())
			throw syntax_error("Wrong size of arguments.");
		if(this->mData.get()!=nullptr) {
			runtime->storage.add_this(this->mData);
			runtime->storage.add_domain(this->mData);
		}
		runtime->storage.add_domain();
		fcall_stack.push_front(this);
		for(std::size_t i=0; i<args.size(); ++i)
			runtime->storage.add_var(this->mArgs.at(i),args.at(i));
		for(auto& ptr:this->mBody) {
			try {
				ptr->run();
			}
			catch(const syntax_error& se) {
				throw syntax_error(ptr->get_line_num(),se.what());
			}
			catch(const lang_error& le) {
				throw lang_error(ptr->get_line_num(),le.what());
			}
			catch(const std::exception& e) {
				throw internal_error(ptr->get_line_num(),e.what());
			}
			if(this->mRetVal.usable()) {
				return_fcall=false;
				cov::any retval=this->mRetVal;
				this->mRetVal=cov::any();
				if(this->mData.get()!=nullptr) {
					runtime->storage.remove_this();
					runtime->storage.remove_domain();
				}
				runtime->storage.remove_domain();
				fcall_stack.pop_front();
				return retval;
			}
		}
		if(this->mData.get()!=nullptr) {
			runtime->storage.remove_this();
			runtime->storage.remove_domain();
		}
		runtime->storage.remove_domain();
		fcall_stack.pop_front();
		return number(0);
	}
	cov::any struct_builder::operator()()
	{
		runtime->storage.add_domain();
		inside_struct=true;
		for(auto& ptr:this->mMethod) {
			try {
				ptr->run();
			}
			catch(const syntax_error& se) {
				throw syntax_error(ptr->get_line_num(),se.what());
			}
			catch(const lang_error& le) {
				throw lang_error(ptr->get_line_num(),le.what());
			}
			catch(const std::exception& e) {
				throw internal_error(ptr->get_line_num(),e.what());
			}
		}
		inside_struct=false;
		cov::any dat=cov::any::make<structure>(this->mName,runtime->storage.get_domain());
		runtime->storage.remove_domain();
		return std::move(dat);
	}
	void statement_expression::run()
	{
		parse_expr(mTree.root());
	}
	void statement_import::run()
	{
		for(auto& ptr:statements) {
			try {
				ptr->run();
			}
			catch(const syntax_error& se) {
				throw syntax_error(ptr->get_line_num(),"In file \""+file+"\":"+se.what());
			}
			catch(const lang_error& le) {
				throw lang_error(ptr->get_line_num(),"In file \""+file+"\":"+le.what());
			}
			catch(const std::exception& e) {
				throw internal_error(ptr->get_line_num(),"In file \""+file+"\":"+e.what());
			}
		}
	}
	void statement_define::run()
	{
		define_var=true;
		cov::any var=parse_expr(mTree.root());
		define_var=false;
		if(mType!=nullptr)
			var.assign(runtime->storage.get_var_type(mType->get_id()),true);
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
			catch(const syntax_error& se) {
				throw syntax_error(ptr->get_line_num(),se.what());
			}
			catch(const lang_error& le) {
				throw lang_error(ptr->get_line_num(),le.what());
			}
			catch(const std::exception& e) {
				throw internal_error(ptr->get_line_num(),e.what());
			}
			if(return_fcall) {
				runtime->storage.remove_domain();
				return;
			}
		}
		runtime->storage.remove_domain();
	}
	void statement_if::run()
	{
		runtime->storage.add_domain();
		if(parse_expr(mTree.root()).const_val<boolean>()) {
			if(mBlock!=nullptr) {
				for(auto& ptr:*mBlock) {
					try {
						ptr->run();
					}
					catch(const syntax_error& se) {
						throw syntax_error(ptr->get_line_num(),se.what());
					}
					catch(const lang_error& le) {
						throw lang_error(ptr->get_line_num(),le.what());
					}
					catch(const std::exception& e) {
						throw internal_error(ptr->get_line_num(),e.what());
					}
					if(return_fcall) {
						runtime->storage.remove_domain();
						return;
					}
					if(break_block||continue_block) {
						runtime->storage.remove_domain();
						return;
					}
				}
			}
			else
				throw syntax_error("Empty If body.");
		}
		else {
			if(mElseBlock!=nullptr) {
				for(auto& ptr:*mElseBlock) {
					try {
						ptr->run();
					}
					catch(const syntax_error& se) {
						throw syntax_error(ptr->get_line_num(),se.what());
					}
					catch(const lang_error& le) {
						throw lang_error(ptr->get_line_num(),le.what());
					}
					catch(const std::exception& e) {
						throw internal_error(ptr->get_line_num(),e.what());
					}
					if(return_fcall) {
						runtime->storage.remove_domain();
						return;
					}
					if(break_block||continue_block) {
						runtime->storage.remove_domain();
						return;
					}
				}
			}
		}
		runtime->storage.remove_domain();
	}
	void statement_switch::run()
	{
		cov::any key=parse_expr(mTree.root());
		if(mCases.count(key)>0)
			mCases.at(key)->run();
		else if(mDefault!=nullptr)
			mDefault->run();
	}
	void statement_while::run()
	{
		runtime->storage.add_domain();
		while(parse_expr(mTree.root()).const_val<boolean>()) {
			for(auto& ptr:mBlock) {
				try {
					ptr->run();
				}
				catch(const syntax_error& se) {
					throw syntax_error(ptr->get_line_num(),se.what());
				}
				catch(const lang_error& le) {
					throw lang_error(ptr->get_line_num(),le.what());
				}
				catch(const std::exception& e) {
					throw internal_error(ptr->get_line_num(),e.what());
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
		runtime->storage.add_domain();
		do {
			for(auto& ptr:mBlock) {
				try {
					ptr->run();
				}
				catch(const syntax_error& se) {
					throw syntax_error(ptr->get_line_num(),se.what());
				}
				catch(const lang_error& le) {
					throw lang_error(ptr->get_line_num(),le.what());
				}
				catch(const std::exception& e) {
					throw internal_error(ptr->get_line_num(),e.what());
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
		runtime->storage.add_domain();
		define_var=true;
		cov::any var=parse_expr(mInit.root());
		define_var=false;
		while(var.const_val<number>()<=parse_expr(mEnd.root()).const_val<number>()) {
			for(auto& ptr:mBlock) {
				try {
					ptr->run();
				}
				catch(const syntax_error& se) {
					throw syntax_error(ptr->get_line_num(),se.what());
				}
				catch(const lang_error& le) {
					throw lang_error(ptr->get_line_num(),le.what());
				}
				catch(const std::exception& e) {
					throw internal_error(ptr->get_line_num(),e.what());
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
			var.val<number>(true)+=parse_expr(mStep.root()).const_val<number>();
		}
		runtime->storage.remove_domain();
	}
	void statement_struct::run()
	{
		runtime->storage.add_type(this->mName,this->mBuilder);
	}
	void statement_function::run()
	{
		if(inside_struct)
			this->mFunc.set_data(runtime->storage.get_domain());
		runtime->storage.add_var(this->mName,this->mFunc);
	}
	void statement_return::run()
	{
		if(fcall_stack.empty())
			throw syntax_error("Return outside function.");
		fcall_stack.front()->mRetVal=parse_expr(this->mTree.root());
		return_fcall=true;
	}
	void kill_action(const std::deque<std::deque<token_base*>>& lines,std::deque<statement_base*>& statements)
	{
		std::deque<std::deque<token_base*>> tmp;
		method_type* method=nullptr;
		int level=0;
		for(auto& line:lines) {
			try {
				method_type* m=&translator.match(line);
				if(m->type==grammar_type::single) {
					if(level>0) {
						if(m->function({line})->get_type()==statement_types::end_)
							--level;
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
				else if(m->type==grammar_type::block) {
					if(level==0)
						method=m;
					++level;
					tmp.push_back(line);
				}
				else
					throw syntax_error("Null type of grammar.");
			}
			catch(const syntax_error& se) {
				throw syntax_error(dynamic_cast<token_endline*>(line.back())->get_num(),se.what());
			}
			catch(const std::exception& e) {
				throw internal_error(dynamic_cast<token_endline*>(line.back())->get_num(),e.what());
			}
		}
		if(level!=0)
			throw syntax_error("Lack of the \"end\" signal.");
	}
	void translate_into_statements(std::deque<token_base*>& tokens,std::deque<statement_base*>& statements)
	{
		std::deque<std::deque<token_base*>> lines;
		{
			std::deque<token_base*> tmp;
			for(auto& ptr:tokens) {
				tmp.push_back(ptr);
				if(ptr!=nullptr&&ptr->get_type()==token_types::endline) {
					if(tmp.size()>1) {
						try {
							process_brackets(tmp);
							kill_brackets(tmp);
							kill_expr(tmp);
						}
						catch(const syntax_error& se) {
							throw syntax_error(dynamic_cast<token_endline*>(ptr)->get_num(),se.what());
						}
						catch(const std::exception& e) {
							throw internal_error(dynamic_cast<token_endline*>(ptr)->get_num(),e.what());
						}
						lines.push_back(tmp);
					}
					tmp.clear();
				}
			}
			if(tmp.size()>1)
				lines.push_back(tmp);
			tmp.clear();
		}
		kill_action(lines,statements);
	}
	void init_grammar()
	{
		// Expression Grammar
		translator.add_method({new token_expr(cov::tree<token_base*>()),new token_endline(0)},method_type {grammar_type::single,[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				return new statement_expression(dynamic_cast<token_expr*>(raw.front().front())->get_tree(),raw.front().back());
			}
		});
		// Import Grammar
		translator.add_method({new token_action(action_types::import_),new token_expr(cov::tree<token_base*>()),new token_endline(0)},method_type {grammar_type::single,[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				return new statement_import(dynamic_cast<token_value*>(dynamic_cast<token_expr*>(raw.front().at(1))->get_tree().root().data())->get_value().const_val<string>(),raw.front().back());
			}
		});
		// Define Grammar
		translator.add_method({new token_action(action_types::define_),new token_expr(cov::tree<token_base*>()),new token_endline(0)},method_type {grammar_type::single,[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				return new statement_define(dynamic_cast<token_expr*>(raw.front().at(1))->get_tree(),raw.front().back());
			}
		});
		translator.add_method({new token_action(action_types::define_),new token_expr(cov::tree<token_base*>()),new token_action(action_types::as_),new token_expr(cov::tree<token_base*>()),new token_endline(0)},method_type {grammar_type::single,[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				return new statement_define(dynamic_cast<token_expr*>(raw.front().at(1))->get_tree(),dynamic_cast<token_expr*>(raw.front().at(3))->get_tree().root().data(),raw.front().back());
			}
		});
		// End Grammar
		translator.add_method({new token_action(action_types::endblock_),new token_endline(0)},method_type {grammar_type::single,[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				return new statement_end;
			}
		});
		// Block Grammar
		translator.add_method({new token_action(action_types::block_),new token_endline(0)},method_type {grammar_type::block,[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				std::deque<statement_base*> body;
				kill_action({raw.begin()+1,raw.end()},body);
				return new statement_block(body,raw.front().back());
			}
		});
		// If Grammar
		translator.add_method({new token_action(action_types::if_),new token_expr(cov::tree<token_base*>()),new token_endline(0)},method_type {grammar_type::block,[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				bool have_else=false;
				std::deque<statement_base*>* body=new std::deque<statement_base*>;
				kill_action({raw.begin()+1,raw.end()},*body);
				for(auto& ptr:*body)
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
					std::deque<statement_base*>* body_true=new std::deque<statement_base*>;
					std::deque<statement_base*>* body_false=new std::deque<statement_base*>;
					bool now_place=true;
					for(auto& ptr:*body) {
						if(ptr->get_type()==statement_types::else_) {
							now_place=false;
							continue;
						}
						if(now_place)
							body_true->push_back(ptr);
						else
							body_false->push_back(ptr);
					}
					delete body;
					return new statement_if(dynamic_cast<token_expr*>(raw.front().at(1))->get_tree(),body_true,body_false,raw.front().back());
				}
				else
					return new statement_if(dynamic_cast<token_expr*>(raw.front().at(1))->get_tree(),body,nullptr,raw.front().back());
			}
		});
		// Else Grammar
		translator.add_method({new token_action(action_types::else_),new token_endline(0)},method_type {grammar_type::single,[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				return new statement_else;
			}
		});
		// Switch Grammar
		translator.add_method({new token_action(action_types::switch_),new token_expr(cov::tree<token_base*>()),new token_endline(0)},method_type {grammar_type::block,[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				std::deque<statement_base*> body;
				kill_action({raw.begin()+1,raw.end()},body);
				statement_block* dptr=nullptr;
				std::unordered_map<cov::any,statement_block*> cases;
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
		translator.add_method({new token_action(action_types::case_),new token_expr(cov::tree<token_base*>()),new token_endline(0)},method_type {grammar_type::block,[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				cov::tree<token_base*>& tree=dynamic_cast<token_expr*>(raw.front().at(1))->get_tree();
				if(tree.root().data()->get_type()!=token_types::value)
					throw syntax_error("Case Tag must be a constant value.");
				std::deque<statement_base*> body;
				kill_action({raw.begin()+1,raw.end()},body);
				return new statement_case(dynamic_cast<token_value*>(tree.root().data())->get_value(),body,raw.front().back());
			}
		});
		// Default Grammar
		translator.add_method({new token_action(action_types::default_),new token_endline(0)},method_type {grammar_type::block,[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				std::deque<statement_base*> body;
				kill_action({raw.begin()+1,raw.end()},body);
				return new statement_default(body,raw.front().back());
			}
		});
		// While Grammar
		translator.add_method({new token_action(action_types::while_),new token_expr(cov::tree<token_base*>()),new token_endline(0)},method_type {grammar_type::block,[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				std::deque<statement_base*> body;
				kill_action({raw.begin()+1,raw.end()},body);
				return new statement_while(dynamic_cast<token_expr*>(raw.front().at(1))->get_tree(),body,raw.front().back());
			}
		});
		// Until Grammar
		translator.add_method({new token_action(action_types::until_),new token_expr(cov::tree<token_base*>()),new token_endline(0)},method_type {grammar_type::single,[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				return new statement_until(dynamic_cast<token_expr*>(raw.front().at(1)),raw.front().back());
			}
		});
		// Loop Grammar
		translator.add_method({new token_action(action_types::loop_),new token_endline(0)},method_type {grammar_type::block,[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				std::deque<statement_base*> body;
				kill_action({raw.begin()+1,raw.end()},body);
				if(body.back()->get_type()==statement_types::until_)
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
		translator.add_method({new token_action(action_types::for_),new token_expr(cov::tree<token_base*>()),new token_action(action_types::to_),new token_expr(cov::tree<token_base*>()),new token_action(action_types::step_),new token_expr(cov::tree<token_base*>()),new token_endline(0)},method_type {grammar_type::block,[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				std::deque<statement_base*> body;
				kill_action({raw.begin()+1,raw.end()},body);
				return new statement_for(dynamic_cast<token_expr*>(raw.front().at(1))->get_tree(),dynamic_cast<token_expr*>(raw.front().at(3))->get_tree(),dynamic_cast<token_expr*>(raw.front().at(5))->get_tree(),body,raw.front().back());
			}
		});
		translator.add_method({new token_action(action_types::for_),new token_expr(cov::tree<token_base*>()),new token_action(action_types::to_),new token_expr(cov::tree<token_base*>()),new token_endline(0)},method_type {grammar_type::block,[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				std::deque<statement_base*> body;
				kill_action({raw.begin()+1,raw.end()},body);
				cov::tree<token_base*> tree_step;
				tree_step.emplace_root_left(tree_step.root(),new token_value(number(1)));
				return new statement_for(dynamic_cast<token_expr*>(raw.front().at(1))->get_tree(),dynamic_cast<token_expr*>(raw.front().at(3))->get_tree(),tree_step,body,raw.front().back());
			}
		});
		// Break Grammar
		translator.add_method({new token_action(action_types::break_),new token_endline(0)},method_type {grammar_type::single,[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				return new statement_break(raw.front().back());
			}
		});
		// Continue Grammar
		translator.add_method({new token_action(action_types::continue_),new token_endline(0)},method_type {grammar_type::single,[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				return new statement_continue(raw.front().back());
			}
		});
		// Function Grammar
		translator.add_method({new token_action(action_types::function_),new token_expr(cov::tree<token_base*>()),new token_endline(0)},method_type {grammar_type::block,[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
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
					args.push_back(dynamic_cast<token_id*>(it.root().data())->get_id());
				}
				std::deque<statement_base*> body;
				kill_action({raw.begin()+1,raw.end()},body);
				return new statement_function(name,args,body,raw.front().back());
			}
		});
		// Return Grammar
		translator.add_method({new token_action(action_types::return_),new token_expr(cov::tree<token_base*>()),new token_endline(0)},method_type {grammar_type::single,[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				return new statement_return(dynamic_cast<token_expr*>(raw.front().at(1))->get_tree(),raw.front().back());
			}
		});
		translator.add_method({new token_action(action_types::return_),new token_endline(0)},method_type {grammar_type::single,[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				cov::tree<token_base*> tree;
				tree.emplace_root_left(tree.root(),new token_value(number(0)));
				return new statement_return(tree,raw.front().back());
			}
		});
		// Struct Grammar
		translator.add_method({new token_action(action_types::struct_),new token_expr(cov::tree<token_base*>()),new token_endline(0)},method_type {grammar_type::block,[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				cov::tree<token_base*>& t=dynamic_cast<token_expr*>(raw.front().at(1))->get_tree();
				if(t.root().data()==nullptr)
					throw internal_error("Null pointer accessed.");
				if(t.root().data()->get_type()!=token_types::id)
					throw syntax_error("Wrong grammar for struct definition.");
				std::string name=dynamic_cast<token_id*>(t.root().data())->get_id();
				std::deque<statement_base*> body;
				kill_action({raw.begin()+1,raw.end()},body);
				return new statement_struct(name,body,raw.front().back());
			}
		});
	}
}
