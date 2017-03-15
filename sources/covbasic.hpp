#pragma once
#include "./runtime.hpp"
namespace cov_basic {
	bool return_fcall=false;
	bool break_block=false;
	bool continue_block=false;
	std::deque<const function*> fcall_stack;
	cov::any function::call(const array& args) const
	{
		if(args.size()!=this->mArgs.size())
			throw syntax_error("Wrong size of arguments.");
		storage.add_domain();
		fcall_stack.push_front(this);
		for(std::size_t i=0; i<args.size(); ++i)
			storage.add_var(this->mArgs.at(i),args.at(i));
		for(auto& ptr:this->mBody) {
			ptr->run();
			if(this->mRetVal!=nullptr) {
				return_fcall=false;
				storage.remove_domain();
				fcall_stack.pop_front();
				cov::any retval=*this->mRetVal;
				delete this->mRetVal;
				return retval;
			}
		}
		storage.remove_domain();
		fcall_stack.pop_front();
		return number(0);
	}
	void statement_expression::run()
	{
		if(return_fcall)
		{
			return;
		}
		parse_expr(mTree.root());
	}
	void statement_define::run()
	{
		if(return_fcall)
		{
			return;
		}
		define_var=true;
		parse_expr(mTree.root());
		define_var=false;
	}
	void statement_break::run()
	{
		if(return_fcall)
		{
			return;
		}
		break_block=true;
	}
	void statement_continue::run()
	{
		if(return_fcall)
		{
			return;
		}
		continue_block=true;
	}
	void statement_block::run()
	{
		if(return_fcall)
		{
			return;
		}
		storage.add_domain();
		for(auto& ptr:mBlock)
		{
			ptr->run();
			if(return_fcall)
			{
				storage.remove_domain();
				return;
			}
		}
		storage.remove_domain();
	}
	void statement_if::run()
	{
		if(return_fcall)
		{
			return;
		}
		storage.add_domain();
		if(parse_expr(mTree.root()).const_val<boolean>()) {
			if(mBlock!=nullptr) {
				for(auto& ptr:*mBlock) {
					ptr->run();
					if(return_fcall)
					{
						storage.remove_domain();
						return;
					}
					if(break_block||continue_block) {
						storage.remove_domain();
						return;
					}
				}
			} else
				throw syntax_error("Empty If body.");
		} else {
			if(mElseBlock!=nullptr) {
				for(auto& ptr:*mElseBlock) {
					ptr->run();
					if(return_fcall)
					{
						storage.remove_domain();
						return;
					}
					if(break_block||continue_block) {
						storage.remove_domain();
						return;
					}
				}
			}
		}
		storage.remove_domain();
	}
	void statement_while::run()
	{
		if(return_fcall)
		{
			return;
		}
		storage.add_domain();
		while(parse_expr(mTree.root()).const_val<boolean>()) {
			for(auto& ptr:mBlock) {
				ptr->run();
				if(return_fcall)
				{
					storage.remove_domain();
					return;
				}
				if(break_block) {
					break_block=false;
					storage.remove_domain();
					return;
				}
				if(continue_block) {
					continue_block=false;
					break;
				}
			}
		}
		storage.remove_domain();
	}
	void statement_function::run()
	{
		if(return_fcall)
		{
			return_fcall=false;
			return;
		}
		storage.add_var(this->mName,this->mFunc);
	}
	void statement_return::run()
	{
		if(return_fcall)
		{
			return_fcall=false;
			return;
		}
		if(fcall_stack.empty())
			throw syntax_error("Return outside function.");
		fcall_stack.front()->mRetVal=new cov::any(parse_expr(this->mTree.root()));
		return_fcall=true;
	}
	void kill_action(std::deque<std::deque<token_base*>>& lines,std::deque<statement_base*>& statements)
	{
		std::deque<statement_base*>* tmp_statement=nullptr;
		std::deque<std::deque<token_base*>> tmp_lines;
		std::deque<statement_base*> tmp_statements;
		token_action* tmp_action=nullptr;
		token_expr* tmp_expr=nullptr;
		int level=0;
		for(auto& line:lines) {
			if(line.empty()||line.front()==nullptr)
				throw syntax_error("Undefined behaviour.");
			if(level>0) {
				if(line.front()->get_type()==token_types::action) {
					switch(dynamic_cast<token_action*>(line.front())->get_action()) {
					case action_types::block_:
						++level;
						break;
					case action_types::if_:
						++level;
						break;
					case action_types::else_:
						if(level==1) {
							kill_action(tmp_lines,tmp_statements);
							tmp_statement=new std::deque<statement_base*>(tmp_statements);
							tmp_lines.clear();
							tmp_statements.clear();
							continue;
						}
						break;
					case action_types::while_:
						++level;
						break;
					case action_types::function_:
						++level;
						break;
					case action_types::endblock_:
						--level;
						if(level==0) {
							kill_action(tmp_lines,tmp_statements);
							switch(tmp_action->get_action()) {
							case action_types::block_:
								statements.push_back(new statement_block(tmp_statements));
								break;
							case action_types::if_:
								if(tmp_statement==nullptr)
									statements.push_back(new statement_if(tmp_expr->get_tree(),new std::deque<statement_base*>(tmp_statements),nullptr));
								else
									statements.push_back(new statement_if(tmp_expr->get_tree(),tmp_statement,new std::deque<statement_base*>(tmp_statements)));
								break;
							case action_types::while_:
								statements.push_back(new statement_while(tmp_expr->get_tree(),tmp_statements));
								break;
							case action_types::function_: {
								cov::tree<token_base*>& t=tmp_expr->get_tree();
								std::string name=dynamic_cast<token_id*>(t.root().left().data())->get_id();
								std::deque<std::string> args;
								for(auto& it:dynamic_cast<token_arglist*>(t.root().right().data())->get_arglist())
									args.push_back(dynamic_cast<token_id*>(it.root().data())->get_id());
								statements.push_back(new statement_function(name,args,tmp_statements));
								break;
							}
							}
							tmp_lines.clear();
							tmp_statements.clear();
							tmp_expr=nullptr;
							tmp_action=nullptr;
							tmp_statement=nullptr;
							continue;
						}
						break;
					}
				}
				tmp_lines.push_back(line);
				continue;
			}
			switch(line.front()->get_type()) {
			case token_types::expr:
				statements.push_back(new statement_expression(dynamic_cast<token_expr*>(line.front())->get_tree()));
				break;
			case token_types::action:
				tmp_action=dynamic_cast<token_action*>(line.front());
				switch(tmp_action->get_action()) {
				case action_types::block_:
					++level;
					break;
				case action_types::if_:
					++level;
					tmp_expr=dynamic_cast<token_expr*>(line.at(1));
					break;
				case action_types::while_:
					++level;
					tmp_expr=dynamic_cast<token_expr*>(line.at(1));
					break;
				case action_types::function_:
					++level;
					tmp_expr=dynamic_cast<token_expr*>(line.at(1));
					break;
				case action_types::define_:
					statements.push_back(new statement_define(dynamic_cast<token_expr*>(line.at(1))->get_tree()));
					break;
				case action_types::return_:
					statements.push_back(new statement_return(dynamic_cast<token_expr*>(line.at(1))->get_tree()));
					break;
				case action_types::break_:
					statements.push_back(new statement_break);
					break;
				case action_types::continue_:
					statements.push_back(new statement_continue);
					break;
				}
				break;
			}
		}
	}
	void translate_into_statements(std::deque<token_base*>& tokens,std::deque<statement_base*>& statements)
	{
		process_brackets(tokens);
		kill_brackets(tokens);
		kill_expr(tokens);
		std::deque<std::deque<token_base*>> lines;
		{
			std::deque<token_base*> tmp;
			for(auto& ptr:tokens) {
				if(ptr!=nullptr&&ptr->get_type()==token_types::action&&dynamic_cast<token_action*>(ptr)->get_action()==action_types::endline_) {
					if(!tmp.empty()) {
						lines.push_back(tmp);
						tmp.clear();
					}
					continue;
				} else
					tmp.push_back(ptr);
			}
			if(!tmp.empty()) {
				lines.push_back(tmp);
				tmp.clear();
			}
		}
		kill_action(lines,statements);
	}
}