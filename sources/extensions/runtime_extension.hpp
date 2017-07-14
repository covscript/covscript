#pragma once
#include "../cni.hpp"
#include <cstdlib>
static cs::extension runtime_ext;
namespace runtime_cs_ext {
	using namespace cs;
	void info()
	{
		std::cout<<"Covariant Script Programming Language Interpreter Version "<<cs::version<<"\n"
		         "Copyright (C) 2017 Michael Lee.All rights reserved.\nThis program comes with ABSOLUTELY NO WARRANTY; for details see <http://www.gnu.org/licenses/>.\n"
		         "This is free software, and you are welcome to redistribute it under certain conditions.\nPlease visit <http://covbasic.org/> for more information."<<std::endl;
	}
	number time()
	{
		return cov::timer::time(cov::timer::time_unit::milli_sec);
	}
	void delay(number time)
	{
		cov::timer::delay(cov::timer::time_unit::milli_sec,time);
	}
	number rand(number b,number e)
	{
		return cov::rand<number>(b,e);
	}
	number randint(number b,number e)
	{
		return cov::rand<long>(b,e);
	}
	void error(const string& str)
	{
		throw lang_error(str);
	}
	extension_t load_extension(const string& path)
	{
		return std::make_shared<extension_holder>(path);
	}
	std::size_t hash(const cov::any& val)
	{
		return val.hash();
	}
	using expression_t=cov::tree<token_base*>;
	cov::any build(const string& expr)
	{
		std::deque<char> buff;
		std::deque<token_base*> tokens;
		expression_t tree;
		for(auto& ch:expr)
			buff.push_back(ch);
		translate_into_tokens(buff,tokens);
		process_brackets(tokens);
		kill_brackets(tokens);
		gen_tree(tree,tokens);
		return cov::any::make<expression_t>(tree);
	}
	cov::any solve(expression_t& tree)
	{
		return parse_expr(tree.root());
	}
	void init()
	{
		runtime_ext.add_var("info",cov::any::make_protect<native_interface>(cni(info)));
		runtime_ext.add_var("time",cov::any::make_protect<native_interface>(cni(time)));
		runtime_ext.add_var("delay",cov::any::make_protect<native_interface>(cni(delay)));
		runtime_ext.add_var("rand",cov::any::make_protect<native_interface>(cni(rand)));
		runtime_ext.add_var("randint",cov::any::make_protect<native_interface>(cni(randint)));
		runtime_ext.add_var("error",cov::any::make_protect<native_interface>(cni(error)));
		runtime_ext.add_var("load_extension",cov::any::make_protect<native_interface>(cni(load_extension),true));
		runtime_ext.add_var("hash",cov::any::make_protect<native_interface>(cni(hash),true));
		runtime_ext.add_var("build",cov::any::make_protect<native_interface>(cni(build)));
		runtime_ext.add_var("solve",cov::any::make_protect<native_interface>(cni(solve)));
	}
}
