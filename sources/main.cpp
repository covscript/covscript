#define CBS_DARWIN_EXT
#include "./covbasic.hpp"
#include <iostream>
#include <fstream>
#include <cmath>
#define add_function(name) cov_basic::storage.add_var_global(#name,cov_basic::native_interface(name));
#define add_function_name(name,func) cov_basic::storage.add_var_global(name,cov_basic::native_interface(func));
#ifdef CBS_DARWIN_EXT
#include "./darwin_extension.cpp"
#endif
namespace cov_basic {
	cov::any parse_value(const std::string& str)
	{
		if(str=="true"||str=="True"||str=="TRUE")
			return true;
		if(str=="false"||str=="False"||str=="FALSE")
			return false;
		try {
			return number(std::stold(str));
		} catch(...) {
			return str;
		}
		return str;
	}
	cov::any _clone(cov::any val)
	{
		val.clone();
		if(val.type()==typeid(array))
			for(cov::any& v:val.val<array>(true))
				v.clone();
		return val;
	}
	cov::any input(array& args)
	{
		if(args.empty()) {
			std::string str;
			std::cin>>str;
			return parse_value(str);
		}
		for(auto& it:args) {
			std::string str;
			std::cin>>str;
			it.assign(parse_value(str),true);
		}
		return number(0);
	}
	cov::any getline(array&)
	{
		std::string str;
		std::getline(std::cin,str);
		return str;
	}
	cov::any print(array& args)
	{
		for(auto& it:args)
			std::cout<<it;
		return number(0);
	}
	cov::any println(array& args)
	{
		for(auto& it:args)
			std::cout<<it;
		std::cout<<std::endl;
		return number(0);
	}
	cov::any time(array&)
	{
		return number(cov::timer::time(cov::timer::time_unit::milli_sec));
	}
	cov::any delay(array& args)
	{
		if(args.size()!=1)
			throw syntax_error("Wrong size of arguments.");
		if(args.front().type()!=typeid(number))
			throw syntax_error("Wrong type of arguments.(Request Number)");
		cov::timer::delay(cov::timer::time_unit::milli_sec,cov::timer::timer_t(args.front().const_val<number>()));
		return number(0);
	}
	cov::any rand(array& args)
	{
		if(args.size()!=2)
			throw syntax_error("Wrong size of arguments.");
		if(args.at(0).type()!=typeid(number)||args.at(1).type()!=typeid(number))
			throw syntax_error("Wrong type of arguments.(Request Number)");
		return number(cov::rand<number>(args.at(0).const_val<number>(),args.at(1).const_val<number>()));
	}
	cov::any randint(array& args)
	{
		if(args.size()!=2)
			throw syntax_error("Wrong size of arguments.");
		if(args.at(0).type()!=typeid(number)||args.at(1).type()!=typeid(number))
			throw syntax_error("Wrong type of arguments.(Request Number)");
		return number(cov::rand<long>(args.at(0).const_val<number>(),args.at(1).const_val<number>()));
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
	cov::any to_number(array& args)
	{
		if(args.size()!=1)
			throw syntax_error("Wrong size of arguments.");
		if(args.front().type()==typeid(string))
			return number(std::stold(args.at(0).const_val<string>()));
		else
			throw syntax_error("Wrong type of arguments.(Request String)");
	}
	cov::any to_string(array& args)
	{
		if(args.size()!=1)
			throw syntax_error("Wrong size of arguments.");
		return args.front().to_string();
	}
	cov::any clone(array& args)
	{
		if(args.size()!=1)
			throw syntax_error("Wrong size of arguments.");
		return _clone(args.front());
	}
	cov::any error(array& args)
	{
		throw lang_error(args.front().to_string().c_str());
		return number(0);
	}
// String
	cov::any append_string(array& args)
	{
		if(args.size()!=2)
			throw syntax_error("Wrong size of arguments.");
		if(args.at(0).type()!=typeid(string))
			throw syntax_error("Wrong type of arguments.(Request String)");
		args.at(0).val<string>(true).append(args.at(1).to_string());
		return number(0);
	}
	cov::any cut_string(array& args)
	{
		if(args.size()!=2)
			throw syntax_error("Wrong size of arguments.");
		if(args.at(0).type()!=typeid(string)||args.at(1).type()!=typeid(number))
			throw syntax_error("Wrong type of arguments.(Request String,Number)");
		for(std::size_t i=0; i<args.at(1).const_val<number>(); ++i)
			args.at(0).val<string>(true).pop_back();
		return number(0);
	}
	cov::any clear_string(array& args)
	{
		if(args.size()!=1)
			throw syntax_error("Wrong size of arguments.");
		if(args.at(0).type()!=typeid(string))
			throw syntax_error("Wrong type of arguments.(Request String)");
		args.at(0).val<string>(true).clear();
		return number(0);
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
	cov::any load_extension(array& args)
	{
		if(args.size()!=1)
			throw syntax_error("Wrong size of arguments.");
		if(args.at(0).type()!=typeid(string))
			throw syntax_error("Wrong type of arguments.(Request String)");
		extensions.add_extension(args.front().const_val<string>());
		return number(0);
	}
	cov::any get_var_extension(array& args)
	{
		if(args.size()!=2)
			throw syntax_error("Wrong size of arguments.");
		if(args.at(0).type()!=typeid(string)||args.at(1).type()!=typeid(string))
			throw syntax_error("Wrong type of arguments.(Request String,String)");
		return extensions.get_var(args.at(0).const_val<string>(),args.at(1).const_val<string>());
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
		storage.add_type("number",[]()->cov::any {return number(0);});
		storage.add_type("boolean",[]()->cov::any {return boolean(true);});
		storage.add_type("string",[]()->cov::any {return string();});
		storage.add_type("array",[]()->cov::any {return array();});
		add_function(input);
		add_function(getline);
		add_function(print);
		add_function(println);
		add_function(time);
		add_function(delay);
		add_function(rand);
		add_function(randint);
		add_function(to_integer);
		add_function(to_number);
		add_function(to_string);
		add_function(clone);
		add_function(error);
		add_function(load_extension);
		add_function(get_var_extension);
		add_function(append_string);
		add_function(cut_string);
		add_function(clear_string);
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
		add_function_name("sizeof",_sizeof);
#ifdef CBS_DARWIN_EXT
		darwin_cbs_ext::init();
		extensions.add_extension("darwin_ext",&darwin_ext);
#endif
	}
}
int main(int args_size,const char* args[])
{
	using namespace cov_basic;
	if(args_size>1) {
		std::ios::sync_with_stdio(false);
		std::deque<char> buff;
		std::deque<token_base*> tokens;
		std::deque<statement_base*> statements;
		std::ifstream in(args[1]);
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
			}
		}
	} else {
		std::cout<<"Covariant Basic 2.0 Parser\nFatal Error: no input file.\nUsage: "<<args[0]<<" <file>\nCompilation terminated."<<std::endl;
	}
	return 0;
}