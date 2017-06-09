#pragma once
#include "../arglist.hpp"
#include <cstdlib>
static cov_basic::extension runtime_ext;
namespace runtime_cbs_ext {
	using namespace cov_basic;
	cov::any info(array&)
	{
		std::cout<<"Covariant Basic Parser\nVersion:"<<cov_basic::version<<"\nCopyright (C) 2017 Michael Lee"<<std::endl;
		return number(0);
	}
	cov::any time(array&)
	{
		return number(cov::timer::time(cov::timer::time_unit::milli_sec));
	}
	cov::any delay(array& args)
	{
		arglist::check<number>(args);
		cov::timer::delay(cov::timer::time_unit::milli_sec,cov::timer::timer_t(args.front().const_val<number>()));
		return number(0);
	}
	cov::any rand(array& args)
	{
		arglist::check<number,number>(args);
		return number(cov::rand<number>(args.at(0).const_val<number>(),args.at(1).const_val<number>()));
	}
	cov::any randint(array& args)
	{
		arglist::check<number,number>(args);
		return number(cov::rand<long>(args.at(0).const_val<number>(),args.at(1).const_val<number>()));
	}
	cov::any error(array& args)
	{
		arglist::check<string>(args);
		throw lang_error(args.front().const_val<string>());
		return number(0);
	}
	cov::any load_extension(array& args)
	{
		arglist::check<string>(args);
		return std::make_shared<extension_holder>(args.front().const_val<string>());
	}
	cov::any type_hash(array& args)
	{
		if(args.size()!=1)
			throw syntax_error("Wrong size of arguments.");
		if(args.front().type()==typeid(structure))
			return cov::hash<std::string>(args.front().const_val<structure>().get_name());
		else
			return cov::hash<std::string>(args.front().type().name());
	}
	cov::any hash(array& args)
	{
		if(args.size()!=1)
			throw syntax_error("Wrong size of arguments.");
		return args.front().hash();
	}
	using expression_t=cov::tree<token_base*>;
	cov::any build(array& args)
	{
		arglist::check<string>(args);
		std::deque<char> buff;
		std::deque<token_base*> tokens;
		expression_t tree;
		for(auto& ch:args.at(0).const_val<string>())
			buff.push_back(ch);
		translate_into_tokens(buff,tokens);
		process_brackets(tokens);
		kill_brackets(tokens);
		gen_tree(tree,tokens);
		return cov::any::make<expression_t>(tree);
	}
	cov::any solve(array& args)
	{
		arglist::check<expression_t>(args);
		return parse_expr(args.at(0).val<expression_t>(true).root());
	}
	cov::any exit(array& args)
	{
		arglist::check<number>(args);
		std::exit(args.at(0).const_val<number>());
		return number(0);
	}
	void init()
	{
		runtime_ext.add_var("info",cov::any::make_constant<native_interface>(info));
		runtime_ext.add_var("time",cov::any::make_constant<native_interface>(time));
		runtime_ext.add_var("delay",cov::any::make_constant<native_interface>(delay));
		runtime_ext.add_var("rand",cov::any::make_constant<native_interface>(rand));
		runtime_ext.add_var("randint",cov::any::make_constant<native_interface>(randint));
		runtime_ext.add_var("error",cov::any::make_constant<native_interface>(error));
		runtime_ext.add_var("load_extension",cov::any::make_constant<native_interface>(load_extension));
		runtime_ext.add_var("type_hash",cov::any::make_constant<native_interface>(type_hash,true));
		runtime_ext.add_var("hash",cov::any::make_constant<native_interface>(hash,true));
		runtime_ext.add_var("build",cov::any::make_constant<native_interface>(build));
		runtime_ext.add_var("solve",cov::any::make_constant<native_interface>(solve));
		runtime_ext.add_var("exit",cov::any::make_constant<native_interface>(exit));
	}
}
