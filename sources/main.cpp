#define DARWIN_FORCE_BUILTIN
#include "./core.hpp"
#include <iostream>
#include <fstream>
#include <random>
#include <cstdlib>
int main(int args,const char** argv);
namespace cov_basic {
	darwin::pixel pencil;
	darwin::sync_clock clock(30);
	cov::any parse_value(const std::string& str)
	{
		if(str=="true"||str=="True"||str=="TRUE")
			return true;
		if(str=="false"||str=="False"||str=="FALSE")
			return false;
		enum types {
		    interger,floating,other
		} type=types::interger;
		for(auto& it:str) {
			if(!std::isdigit(it)&&it!='.') {
				type=other;
				break;
			}
			if(!std::isdigit(it)&&it=='.') {
				if(type==interger) {
					type=floating;
					continue;
				}
				if(type==floating) {
					type=other;
					break;
				}
			}
		}
		switch(type) {
		case interger:
			try {
				return number(std::stoi(str));
			} catch(std::out_of_range) {
				try {
					return number(std::stol(str));
				} catch(std::out_of_range) {
					return str;
				}
			}
		case floating:
			return number(std::stold(str));
		case other:
			break;
		}
		return str;
	}
	void init()
	{
		storage.add_var("Exec", native_interface([](std::deque<cov::any>& args)-> number {
			string path=args.at(0).const_val<string>();
			const char* arg[]={"CovBasic",path.c_str()};
			return ::main(2,arg);
		}));
		storage.add_var("Print", native_interface([](std::deque<cov::any>& args)-> number {
			for(auto& it:args)
				std::cout<<it;
			std::cout<<std::endl;
			return 0;
		}));
		storage.add_var("Input", native_interface([](std::deque<cov::any>& args)-> number {
			for(auto& it:args) {
				string tmp;
				std::cin>>tmp;
				it.assign(parse_value(tmp),true);
			}
			return 0;
		}));
		storage.add_var("PrintType", native_interface([](std::deque<cov::any>& args)-> number {
			for(auto& it:args) {
				Switch(it.type()) {
					Case(typeid(number)) {
						std::cout<<"Number"<<std::endl;
					}
					EndCase;
					Case(typeid(boolean)) {
						std::cout<<"Boolean"<<std::endl;
					}
					EndCase;
					Case(typeid(string)) {
						std::cout<<"String"<<std::endl;
					}
					EndCase;
					Case(typeid(array)) {
						std::cout<<"Array"<<std::endl;
					}
					EndCase;
				}
				EndSwitch;
			}
			return 0;
		}));
		storage.add_var("SizeOf", native_interface([](std::deque<cov::any>& args)-> number {
			return args.at(0).const_val<array>().size();
		}));
		storage.add_var("Time", native_interface([](std::deque<cov::any>& args)-> number {
			return cov::timer::time(cov::timer::time_unit::milli_sec);
		}));
		storage.add_var("Rand", native_interface([](std::deque<cov::any>& args)-> number {
			std::uniform_real_distribution<number> u(args.at(0).const_val<number>(),args.at(1).const_val<number>());
			static std::default_random_engine e;
			return u(e);
		}));
		storage.add_var("RandInt", native_interface([](std::deque<cov::any>& args)-> number {
			std::uniform_int_distribution<long> u(long(args.at(0).const_val<number>()),long(args.at(1).const_val<number>()));
			static std::default_random_engine e;
			return u(e);
		}));
		storage.add_var("Exit", native_interface([](std::deque<cov::any>& args)-> number {
			exit(0);
			return 0;
		}));
		storage.add_var("darwin_loadmodule", native_interface([](std::deque<cov::any>& args)-> number {
			darwin::runtime.load(args.at(0).const_val<std::string>());
			return 0;
		}));
		storage.add_var("darwin_fitwindow", native_interface([](std::deque<cov::any>& args)-> number {
			darwin::runtime.fit_drawable();
			return 0;
		}));
		storage.add_var("darwin_framelimit", native_interface([](std::deque<cov::any>& args)-> number {
			clock.set_freq(args.at(0).const_val<number>());
			return 0;
		}));
		storage.add_var("darwin_updatepaint", native_interface([](std::deque<cov::any>& args)-> number {
			darwin::runtime.update_drawable();
			clock.sync();
			clock.reset();
			return 0;
		}));
		storage.add_var("darwin_clearpaint", native_interface([](std::deque<cov::any>& args)-> number {
			darwin::runtime.get_drawable()->clear();
			return 0;
		}));
		storage.add_var("darwin_fillpaint", native_interface([](std::deque<cov::any>& args)-> number {
			darwin::runtime.get_drawable()->fill(pencil);
			return 0;
		}));
		storage.add_var("darwin_windowheight", native_interface([](std::deque<cov::any>& args)-> number {
			return darwin::runtime.get_drawable()->get_height();
		}));
		storage.add_var("darwin_windowwidth", native_interface([](std::deque<cov::any>& args)-> number {
			return darwin::runtime.get_drawable()->get_width();
		}));
		storage.add_var("darwin_drawpoint", native_interface([](std::deque<cov::any>& args)-> number {
			darwin::runtime.get_drawable()->draw_pixel(args.at(0).const_val< number>(),args.at(1).const_val< number>(),pencil);
			return 0;
		}));
		storage.add_var("darwin_drawline", native_interface([](std::deque<cov::any>& args)-> number {
			darwin::runtime.get_drawable()->draw_line(args.at(0).const_val< number>(),args.at(1).const_val< number>(),args.at(2).const_val< number>(),args.at(3).const_val< number>(),pencil);
			return 0;
		}));
		storage.add_var("darwin_setpencil", native_interface([](std::deque<cov::any>& args)-> number {
			char ch=args.at(0).const_val<std::string>().at(0);
			bool bright=args.at(1).const_val<bool>();
			bool underline=args.at(2).const_val<bool>();
			darwin::colors fc,bc;
			switch(int(args.at(3).const_val< number>())) {
			case 0:
				fc=darwin::colors::black;
				break;
			case 1:
				fc=darwin::colors::white;
				break;
			case 2:
				fc=darwin::colors::red;
				break;
			case 3:
				fc=darwin::colors::green;
				break;
			case 4:
				fc=darwin::colors::blue;
				break;
			case 5:
				fc=darwin::colors::pink;
				break;
			case 6:
				fc=darwin::colors::yellow;
				break;
			case 7:
				fc=darwin::colors::cyan;
				break;
			}
			switch(int(args.at(4).const_val< number>())) {
			case 0:
				bc=darwin::colors::black;
				break;
			case 1:
				bc=darwin::colors::white;
				break;
			case 2:
				bc=darwin::colors::red;
				break;
			case 3:
				bc=darwin::colors::green;
				break;
			case 4:
				bc=darwin::colors::blue;
				break;
			case 5:
				bc=darwin::colors::pink;
				break;
			case 6:
				bc=darwin::colors::yellow;
				break;
			case 7:
				bc=darwin::colors::cyan;
				break;
			}
			pencil=darwin::pixel(ch,bright,underline,fc,bc);
			return 0;
		}));
	}
}
int main(int args,const char** argv)
{
	using namespace cov_basic;
	init();
	std::string str;
	if(args>1) {
		std::ifstream in(argv[1]);
		while(std::getline(in,str))
			parse(str);
	} else {
		std::cout<<"Covariant Basic Parser v1.1"<<std::endl;
		while(std::getline(std::cin,str))
			parse(str);
	}
	return 0;
}