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
			} catch(const syntax_error& se) {
				throw syntax_error(ptr->get_line_num(),se.what());
			} catch(const lang_error& le) {
				throw lang_error(ptr->get_line_num(),le.what());
			} catch(const std::exception& e) {
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
			} catch(const syntax_error& se) {
				throw syntax_error(ptr->get_line_num(),se.what());
			} catch(const lang_error& le) {
				throw lang_error(ptr->get_line_num(),le.what());
			} catch(const std::exception& e) {
				throw internal_error(ptr->get_line_num(),e.what());
			}
		}
		inside_struct=false;
		structure dat(runtime->storage.get_domain());
		runtime->storage.remove_domain();
		return dat;
	}
	void statement_expression::run()
	{
		parse_expr(mTree.root());
	}
	void statement_import::run()
	{
		for(auto& ptr:mBlock) {
			try {
				ptr->run();
			} catch(const syntax_error& se) {
				throw syntax_error(ptr->get_line_num(),se.what());
			} catch(const lang_error& le) {
				throw lang_error(ptr->get_line_num(),le.what());
			} catch(const std::exception& e) {
				throw internal_error(ptr->get_line_num(),e.what());
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
			} catch(const syntax_error& se) {
				throw syntax_error(ptr->get_line_num(),se.what());
			} catch(const lang_error& le) {
				throw lang_error(ptr->get_line_num(),le.what());
			} catch(const std::exception& e) {
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
					} catch(const syntax_error& se) {
						throw syntax_error(ptr->get_line_num(),se.what());
					} catch(const lang_error& le) {
						throw lang_error(ptr->get_line_num(),le.what());
					} catch(const std::exception& e) {
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
					} catch(const std::exception& e) {
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
	void statement_while::run()
	{
		runtime->storage.add_domain();
		while(parse_expr(mTree.root()).const_val<boolean>()) {
			for(auto& ptr:mBlock) {
				try {
					ptr->run();
				} catch(const syntax_error& se) {
					throw syntax_error(ptr->get_line_num(),se.what());
				} catch(const lang_error& le) {
					throw lang_error(ptr->get_line_num(),le.what());
				} catch(const std::exception& e) {
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
	void statement_until::run()
	{
		runtime->storage.add_domain();
		do {
			for(auto& ptr:mBlock) {
				try {
					ptr->run();
				} catch(const syntax_error& se) {
					throw syntax_error(ptr->get_line_num(),se.what());
				} catch(const lang_error& le) {
					throw lang_error(ptr->get_line_num(),le.what());
				} catch(const std::exception& e) {
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
		} while(parse_expr(mTree.root()).const_val<boolean>());
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
			method_type* m=nullptr;
			try {
				m=&translator.match(line);
			} catch(const syntax_error& se) {
				throw syntax_error(dynamic_cast<token_endline*>(line.back())->get_num(),se.what());
			} catch(const std::exception& e) {
				throw internal_error(dynamic_cast<token_endline*>(line.back())->get_num(),e.what());
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
						} catch(const syntax_error& se) {
							throw syntax_error(dynamic_cast<token_endline*>(ptr)->get_num(),se.what());
						} catch(const std::exception& e) {
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
}
#include "./arglist.hpp"
#include <cmath>
#define add_function(name) cov_basic::runtime->storage.add_var_global(#name,cov_basic::native_interface(name));
#define add_function_name(name,func) cov_basic::runtime->storage.add_var_global(name,cov_basic::native_interface(func));
#include "./system_extension.hpp"
#include "./runtime_extension.hpp"
#ifdef CBS_STRING_EXT
#include "./string_extension.cpp"
#endif
#ifdef CBS_FILE_EXT
#include "./file_extension.cpp"
#endif
#ifdef CBS_DARWIN_EXT
#include "./darwin_extension.cpp"
#endif
namespace cov_basic {
	cov::any _clone(cov::any val)
	{
		val.clone();
		if(val.type()==typeid(array))
			for(cov::any& v:val.val<array>(true))
				v.clone();
		return val;
	}
	cov::any _sizeof(array& args)
	{
		if(args.size()!=1)
			throw syntax_error("Wrong size of arguments.");
		if(args.front().type()==typeid(array))
			return number(args.front().const_val<array>().size());
		else if(args.front().type()==typeid(string))
			return number(args.front().const_val<string>().size());
		else
			throw syntax_error("Get size of non-array or string object.");
	}
	cov::any to_integer(array& args)
	{
		if(args.size()!=1)
			throw syntax_error("Wrong size of arguments.");
		if(args.front().type()==typeid(number))
			return number(long(args.at(0).const_val<number>()));
		else if(args.front().type()==typeid(string))
			return number(std::stold(args.at(0).const_val<string>()));
		else
			throw syntax_error("Wrong type of arguments.(Request Number or String)");
	}
	cov::any to_string(array& args)
	{
		if(args.size()!=1)
			throw syntax_error("Wrong size of arguments.");
		return args.front().to_string();
	}
	cov::any is_number(array& args)
	{
		if(args.size()!=1)
			throw syntax_error("Wrong size of arguments.");
		return args.front().type()==typeid(number);
	}
	cov::any is_boolean(array& args)
	{
		if(args.size()!=1)
			throw syntax_error("Wrong size of arguments.");
		return args.front().type()==typeid(boolean);
	}
	cov::any is_string(array& args)
	{
		if(args.size()!=1)
			throw syntax_error("Wrong size of arguments.");
		return args.front().type()==typeid(string);
	}
	cov::any is_array(array& args)
	{
		if(args.size()!=1)
			throw syntax_error("Wrong size of arguments.");
		return args.front().type()==typeid(array);
	}
	cov::any clone(array& args)
	{
		if(args.size()!=1)
			throw syntax_error("Wrong size of arguments.");
		return _clone(args.front());
	}
	cov::any link(array& args)
	{
		if(args.size()!=1)
			throw syntax_error("Wrong size of arguments.");
		return linker{args.front()};
	}
// Array
	cov::any push_front_array(array& args)
	{
		if(args.size()!=2)
			throw syntax_error("Wrong size of arguments.");
		if(args.at(0).type()!=typeid(array))
			throw syntax_error("Wrong type of arguments.(Request Array)");
		args.at(0).val<array>(true).push_front(_clone(args.at(1)));
		return number(0);
	}
	cov::any pop_front_array(array& args)
	{
		if(args.size()!=1)
			throw syntax_error("Wrong size of arguments.");
		if(args.at(0).type()!=typeid(array))
			throw syntax_error("Wrong type of arguments.(Request Array)");
		args.at(0).val<array>(true).pop_front();
		return number(0);
	}
	cov::any push_back_array(array& args)
	{
		if(args.size()!=2)
			throw syntax_error("Wrong size of arguments.");
		if(args.at(0).type()!=typeid(array))
			throw syntax_error("Wrong type of arguments.(Request Array)");
		args.at(0).val<array>(true).push_back(_clone(args.at(1)));
		return number(0);
	}
	cov::any pop_back_array(array& args)
	{
		if(args.size()!=1)
			throw syntax_error("Wrong size of arguments.");
		if(args.at(0).type()!=typeid(array))
			throw syntax_error("Wrong type of arguments.(Request Array)");
		args.at(0).val<array>(true).pop_back();
		return number(0);
	}
	cov::any clear_array(array& args)
	{
		if(args.size()!=1)
			throw syntax_error("Wrong size of arguments.");
		if(args.at(0).type()!=typeid(array))
			throw syntax_error("Wrong type of arguments.(Request Array)");
		args.at(0).val<array>(true).clear();
		return number(0);
	}
// Mathematics
	cov::any abs(array& args)
	{
		return number(std::abs(args.at(0).const_val<number>()));
	}
	cov::any ln(array& args)
	{
		return number(std::log(args.at(0).const_val<number>()));
	}
	cov::any log(array& args)
	{
		if(args.size()==1)
			return number(std::log10(args.at(0).const_val<number>()));
		else
			return number(std::log(args.at(1).const_val<number>()/args.at(0).const_val<number>()));
	}
	cov::any sin(array& args)
	{
		return number(std::sin(args.at(0).const_val<number>()));
	}
	cov::any cos(array& args)
	{
		return number(std::cos(args.at(0).const_val<number>()));
	}
	cov::any tan(array& args)
	{
		return number(std::tan(args.at(0).const_val<number>()));
	}
	cov::any asin(array& args)
	{
		return number(std::sin(args.at(0).const_val<number>()));
	}
	cov::any acos(array& args)
	{
		return number(std::cos(args.at(0).const_val<number>()));
	}
	cov::any atan(array& args)
	{
		return number(std::tan(args.at(0).const_val<number>()));
	}
	cov::any sqrt(array& args)
	{
		return number(std::sqrt(args.at(0).const_val<number>()));
	}
	cov::any root(array& args)
	{
		return number(std::pow(args.at(0).const_val<number>(),number(1)/args.at(1).const_val<number>()));
	}
	cov::any pow(array& args)
	{
		return number(std::pow(args.at(0).const_val<number>(),args.at(1).const_val<number>()));
	}
	void init()
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
				} else
					return new statement_if(dynamic_cast<token_expr*>(raw.front().at(1))->get_tree(),body,nullptr,raw.front().back());
			}
		});
		// Else Grammar
		translator.add_method({new token_action(action_types::else_),new token_endline(0)},method_type {grammar_type::single,[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				return new statement_else;
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
		translator.add_method({new token_action(action_types::until_),new token_expr(cov::tree<token_base*>()),new token_endline(0)},method_type {grammar_type::block,[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				std::deque<statement_base*> body;
				kill_action({raw.begin()+1,raw.end()},body);
				return new statement_until(dynamic_cast<token_expr*>(raw.front().at(1))->get_tree(),body,raw.front().back());
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
				std::string name=dynamic_cast<token_id*>(t.root().left().data())->get_id();
				std::deque<std::string> args;
				for(auto& it:dynamic_cast<token_arglist*>(t.root().right().data())->get_arglist())
					args.push_back(dynamic_cast<token_id*>(it.root().data())->get_id());
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
		// Struct Grammar
		translator.add_method({new token_action(action_types::struct_),new token_expr(cov::tree<token_base*>()),new token_endline(0)},method_type {grammar_type::block,[](const std::deque<std::deque<token_base*>>& raw)->statement_base* {
				cov::tree<token_base*>& t=dynamic_cast<token_expr*>(raw.front().at(1))->get_tree();
				std::string name=dynamic_cast<token_id*>(t.root().data())->get_id();
				std::deque<statement_base*> body;
				kill_action({raw.begin()+1,raw.end()},body);
				return new statement_struct(name,body,raw.front().back());
			}
		});
		runtime->storage.add_type("number",[]()->cov::any {return number(0);});
		runtime->storage.add_type("boolean",[]()->cov::any {return boolean(true);});
		runtime->storage.add_type("string",[]()->cov::any {return string();});
		runtime->storage.add_type("array",[]()->cov::any {return array();});
		add_function(to_integer);
		add_function(to_string);
		add_function(is_number);
		add_function(is_boolean);
		add_function(is_string);
		add_function(is_array);
		add_function_name("sizeof",_sizeof);
		add_function(clone);
		add_function(link);
		add_function(push_front_array);
		add_function(pop_front_array);
		add_function(push_back_array);
		add_function(pop_back_array);
		add_function(clear_array);
		add_function(abs);
		add_function(ln);
		add_function(log);
		add_function(sin);
		add_function(cos);
		add_function(tan);
		add_function(asin);
		add_function(acos);
		add_function(atan);
		add_function(sqrt);
		add_function(root);
		add_function(pow);
		system_cbs_ext::init();
		runtime->storage.add_var("system",std::make_shared<extension_holder>(&system_ext));
		runtime_cbs_ext::init();
		runtime->storage.add_var("runtime",std::make_shared<extension_holder>(&runtime_ext));
#ifdef CBS_STRING_EXT
		string_cbs_ext::init();
		runtime->storage.add_var("string",std::make_shared<extension_holder>(&string_ext));
#endif
#ifdef CBS_FILE_EXT
		file_cbs_ext::init();
		runtime->storage.add_var("file",std::make_shared<extension_holder>(&file_ext));
#endif
#ifdef CBS_DARWIN_EXT
		darwin_cbs_ext::init();
		runtime->storage.add_var("darwin",std::make_shared<extension_holder>(&darwin_ext));
#endif
	}
	void reset()
	{
		runtime=std::unique_ptr<runtime_type>(new runtime_type);
	}
	void cov_basic(const std::string& path)
	{
		std::ios::sync_with_stdio(false);
		std::deque<char> buff;
		std::deque<token_base*> tokens;
		std::deque<statement_base*> statements;
		std::ifstream in(path);
		if(!in.is_open())
			throw fatal_error(path+": No such file or directory");
		std::string line;
		std::size_t line_num=0;
		while(std::getline(in,line)) {
			++line_num;
			if(line.empty())
				continue;
			if(line[0]=='#')
				continue;
			for(auto& c:line)
				buff.push_back(c);
			try {
				translate_into_tokens(buff,tokens);
			} catch(const syntax_error& se) {
				throw syntax_error(line_num,se.what());
			} catch(const std::exception& e) {
				throw internal_error(line_num,e.what());
			}
			tokens.push_back(new token_endline(line_num));
			buff.clear();
		}
		init();
		translate_into_statements(tokens,statements);
		for(auto& ptr:statements) {
			try {
				ptr->run();
			} catch(const syntax_error& se) {
				throw syntax_error(ptr->get_line_num(),se.what());
			} catch(const lang_error& le) {
				throw lang_error(ptr->get_line_num(),le.what());
			} catch(const std::exception& e) {
				throw internal_error(ptr->get_line_num(),e.what());
			}
		}
		std::ios::sync_with_stdio(true);
	}
}
