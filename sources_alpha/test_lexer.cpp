#include "./lexer.hpp"
#include <iostream>
#include <fstream>
int main()
{
	using namespace cov_basic;
	std::deque<char> buff;
	std::deque<token_base*> token;
	std::ifstream in("./test.cbs");
	std::string line;
	while(std::getline(in,line)) {
		for(auto& c:line)
			buff.push_back(c);
		buff.push_back('\n');
	}
	lexer(buff,token);
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
			case action_types::if_:
				std::cout<<"<action:if>";
				break;
			case action_types::then_:
				std::cout<<"<action:then>";
				break;
			case action_types::else_:
				std::cout<<"<action:else>";
				break;
			case action_types::while_:
				std::cout<<"<action:while>";
				break;
			case action_types::do_:
				std::cout<<"<action:do>";
				break;
			case action_types::for_:
				std::cout<<"<action:for>";
				break;
			case action_types::break_:
				std::cout<<"<action:break>";
				break;
			case action_types::continue_:
				std::cout<<"<action:continue>";
				break;
			case action_types::function_:
				std::cout<<"<action:function>";
				break;
			case action_types::return_:
				std::cout<<"<action:return>";
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
			case signal_types::sub_:
				std::cout<<"<signal:->";
				break;
			case signal_types::mul_:
				std::cout<<"<signal:*>";
				break;
			case signal_types::div_:
				std::cout<<"<signal:/>";
				break;
			case signal_types::mod_:
				std::cout<<"<signal:%>";
				break;
			case signal_types::pow_:
				std::cout<<"<signal:^>";
				break;
			case signal_types::com_:
				std::cout<<"<signal:,>";
				break;
			case signal_types::dot_:
				std::cout<<"<signal:.>";
				break;
			case signal_types::und_:
				std::cout<<"<signal:<>";
				break;
			case signal_types::abo_:
				std::cout<<"<signal:>>";
				break;
			case signal_types::asi_:
				std::cout<<"<signal:=>";
				break;
			case signal_types::equ_:
				std::cout<<"<signal:==>";
				break;
			case signal_types::ueq_:
				std::cout<<"<signal:<=>";
				break;
			case signal_types::aeq_:
				std::cout<<"<signal:>=>";
				break;
			case signal_types::neq_:
				std::cout<<"<signal:!=>";
				break;
			case signal_types::and_:
				std::cout<<"<signal:&&>";
				break;
			case signal_types::or_:
				std::cout<<"<signal:||>";
				break;
			case signal_types::not_:
				std::cout<<"<signal:!>";
				break;
			case signal_types::slb_:
				std::cout<<"<signal:(>";
				break;
			case signal_types::srb_:
				std::cout<<"<signal:)>";
				break;
			case signal_types::esb_:
				std::cout<<"<signal:()>";
				break;
			case signal_types::mlb_:
				std::cout<<"<signal:[>";
				break;
			case signal_types::mrb_:
				std::cout<<"<signal:]>";
				break;
			case signal_types::emb_:
				std::cout<<"<signal:[]>";
				break;
			case signal_types::llb_:
				std::cout<<"<signal:{>";
				break;
			case signal_types::lrb_:
				std::cout<<"<signal:}>";
				break;
			case signal_types::elb_:
				std::cout<<"<signal:{}>";
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