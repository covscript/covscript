#include "./lexer.hpp"
#include <iostream>
#include <fstream>
int main()
{
	using namespace cov_basic;
	std::deque<char> buff;
	std::deque<token_base*> token;
	std::ifstream in("./test.cbs");
	if(!in)
		throw;
	std::string line;
	while(std::getline(in,line)) {
		for(auto& c:line)
			buff.push_back(c);
		buff.push_back('\n');
	}
	for(auto& c:buff)
		std::cout<<c;
	lexer(buff,token);
	std::cout<<token.size()<<std::endl;
	for(auto& ptr:token) {
		switch(ptr->get_type()) {
		case token_types::action:
			switch(dynamic_cast<token_action*>(ptr)->get_action()) {
			case action_types::define_:
				std::cout<<"<action:define>";
				break;
			case action_types::as_:
				std::cout<<"<action:as>";
				break;
			case action_types::endline_:
				std::cout<<"<action:endline>";
				break;
			}
			break;
		case token_types::signal:
			switch(dynamic_cast<token_signal*>(ptr)->get_signal()) {
			case signal_types::add_:
				std::cout<<"<signal:+>";
				break;
			case signal_types::asi_:
				std::cout<<"<signal:=>";
				break;
			case signal_types::and_:
				std::cout<<"<signal:&&>";
				break;
			case signal_types::equ_:
				std::cout<<"<signal:==>";
				break;
			}
			break;
		case token_types::id:
			std::cout<<"<id:"<<dynamic_cast<token_id*>(ptr)->get_id()<<">";
			break;
		case token_types::value:
			std::cout<<"<value:"<<dynamic_cast<token_value*>(ptr)->get_value()<<">";
			break;
		}
		delete ptr;
	}
	return 0;
}
