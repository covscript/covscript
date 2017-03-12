#include "./runtime.hpp"
#include <iostream>
#include <fstream>
#define add_function(name) cov_basic::storage.add_var_global(#name,cov_basic::native_interface(name));
namespace cov_basic {
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
	void init()
	{
		add_function(print);
		add_function(println);
	}
}
int main(int args_size,const char* args[])
{
	using namespace cov_basic;
	if(args_size>1) {
		std::deque<char> buff;
		std::deque<token_base*> tokens;
		std::deque<statement_base*> statements;
		std::ifstream in(args[1]);
		std::string line;
		while(std::getline(in,line)) {
			for(auto& c:line)
				buff.push_back(c);
			buff.push_back('\n');
		}
		init();
		translate_into_tokens(buff,tokens);
		translate_into_statements(tokens,statements);
		for(auto& s:statements)
			s->run();
	} else {
		std::cout<<args[0]<<": fatal error: no input files\ncompilation terminated."<<std::endl;
	}
	return 0;
}