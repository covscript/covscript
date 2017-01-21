#define DARWIN_FORCE_BUILTIN
#include "./darwin_core.hpp"
#include <iostream>
#include <fstream>
#include <cstdlib>
int main(int args,char** argv)
{
	cov_basic::storage.add_var("Print",cov_basic::native_interface([](const std::deque<cov::any>& args)->cov_basic::number{
		for(auto& it:args)
			std::cout<<it;
		std::cout<<std::endl;
		return 0;
	}));
	cov_basic::storage.add_var("Exit",cov_basic::native_interface([](const std::deque<cov::any>& args)->cov_basic::number{
		exit(0);
		return 0;
	}));
	init();
	std::string str;
	if(args>1){
		std::ifstream in(argv[1]);
		while(std::getline(in,str))
			cov_basic::parse(str);
	}else{
		while(std::getline(std::cin,str))
			cov_basic::parse(str);
	}
	return 0;
}