#pragma once
#include "./runtime.hpp"
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
			storage.add_this(this->mData);
			storage.add_domain(this->mData);
		}
		storage.add_domain();
		fcall_stack.push_front(this);
		for(std::size_t i=0; i<args.size(); ++i)
			storage.add_var(this->mArgs.at(i),args.at(i));
		for(auto& ptr:this->mBody) {
			try {
				ptr->run();
			} catch(const syntax_error& se) {
				throw syntax_error(ptr->get_line_num(),se.what());
			} catch(const lang_error& le) {
				throw lang_error(ptr->get_line_num(),le.what());
			}
			if(this->mRetVal.usable()) {
				return_fcall=false;
				cov::any retval=this->mRetVal;
				this->mRetVal=cov::any();
				if(this->mData.get()!=nullptr) {
					storage.remove_this();
					storage.remove_domain();
				}
				storage.remove_domain();
				fcall_stack.pop_front();
				return retval;
			}
		}
		if(this->mData.get()!=nullptr) {
			storage.remove_this();
			storage.remove_domain();
		}
		storage.remove_domain();
		fcall_stack.pop_front();
		return number(0);
	}
	cov::any struct_builder::operator()()
	{
		storage.add_domain();
		inside_struct=true;
		for(auto& ptr:this->mMethod) {
			try {
				ptr->run();
			} catch(const syntax_error& se) {
				throw syntax_error(ptr->get_line_num(),se.what());
			} catch(const lang_error& le) {
				throw lang_error(ptr->get_line_num(),le.what());
			}
		}
		inside_struct=false;
		structure dat(storage.get_domain());
		storage.remove_domain();
		return dat;
	}
	void statement_expression::run()
	{
		parse_expr(mTree.root());
	}
	void statement_define::run()
	{
		define_var=true;
		cov::any var=parse_expr(mTree.root());
		define_var=false;
		if(mType!=nullptr)
			var.assign(storage.get_var_type(mType->get_id()),true);
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
		storage.add_domain();
		for(auto& ptr:mBlock) {
			try {
				ptr->run();
			} catch(const syntax_error& se) {
				throw syntax_error(ptr->get_line_num(),se.what());
			} catch(const lang_error& le) {
				throw lang_error(ptr->get_line_num(),le.what());
			}
			if(return_fcall) {
				storage.remove_domain();
				return;
			}
		}
		storage.remove_domain();
	}
	void statement_if::run()
	{
		storage.add_domain();
		if(parse_expr(mTree.root()).const_val<boolean>()) {
			if(mBlock!=nullptr) {
				for(auto& ptr:*mBlock) {
					try {
						ptr->run();
					} catch(const syntax_error& se) {
						throw syntax_error(ptr->get_line_num(),se.what());
					} catch(const lang_error& le) {
						throw lang_error(ptr->get_line_num(),le.what());
					}
					if(return_fcall) {
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
					try {
						ptr->run();
					} catch(const syntax_error& se) {
						throw syntax_error(ptr->get_line_num(),se.what());
					} catch(const lang_error& le) {
						throw lang_error(ptr->get_line_num(),le.what());
					}
					if(return_fcall) {
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
		storage.add_domain();
		while(parse_expr(mTree.root()).const_val<boolean>()) {
			for(auto& ptr:mBlock) {
				try {
					ptr->run();
				} catch(const syntax_error& se) {
					throw syntax_error(ptr->get_line_num(),se.what());
				} catch(const lang_error& le) {
					throw lang_error(ptr->get_line_num(),le.what());
				}
				if(return_fcall) {
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
	void statement_struct::run()
	{
		storage.add_type(this->mName,this->mBuilder);
	}
	void statement_function::run()
	{
		if(inside_struct)
			this->mFunc.set_data(storage.get_domain());
		storage.add_var(this->mName,this->mFunc);
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
			method_type* m=nullptr;
			try {
				m=&translator.match(line);
			} catch(const syntax_error& se) {
				throw syntax_error(dynamic_cast<token_endline*>(line.back())->get_num(),se.what());
			}
			if(m->type==grammar_type::single) {
				if(level>0) {
					if(m->function({line})->get_type()==statement_types::end_)
						--level;
					if(level==0) {
						statements.push_back(method->function(tmp));
						tmp.clear();
						method=nullptr;
					} else
						tmp.push_back(line);
				} else
					statements.push_back(m->function({line}));
			} else if(m->type==grammar_type::block) {
				if(level==0)
					method=m;
				++level;
				tmp.push_back(line);
			} else
				throw syntax_error("Null type of grammar.");
		}
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
						} catch(const syntax_error& se) {
							throw syntax_error(dynamic_cast<token_endline*>(ptr)->get_num(),se.what());
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
}
