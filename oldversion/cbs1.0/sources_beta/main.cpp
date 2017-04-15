#define DARWIN_FORCE_BUILTIN
#include "./core.hpp"
#include <iostream>
#include <fstream>
#include <random>
#include <cstdlib>
#include <cmath>
int main(int args,const char** argv);
namespace cov_basic {
	darwin::pixel pencil;
	darwin::sync_clock clock(30);
	array files;
	cov::any to_any(const std::string& str)
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
		// Basic I/O
		storage.add_var_global("Exec", native_interface([](std::deque<cov::any>& args)-> number {
			string path=args.at(0).const_val<string>();
			const char* arg[]={"CovBasic",path.c_str()};
			return ::main(2,arg);
		}));
		storage.add_var_global("Print", native_interface([](std::deque<cov::any>& args)-> number {
			for(auto& it:args)
				std::cout<<it;
			std::cout<<std::endl;
			return 0;
		}));
		storage.add_var_global("Input", native_interface([](std::deque<cov::any>& args)-> number {
			if(args.empty())
			{
				number in;
				std::cin>>in;
				return in;
			}
			for(auto& it:args)
			{
				string tmp;
				std::cin>>tmp;
				it.assign(to_any(tmp),true);
			}
			return 0;
		}));
		storage.add_var_global("PrintType", native_interface([](std::deque<cov::any>& args)-> number {
			for(auto& it:args)
			{
				CovSwitch(it.type()) {
					CovCase(typeid(number)) {
						std::cout<<"Types::Number"<<std::endl;
					}
					EndCovCase;
					CovCase(typeid(string)) {
						std::cout<<"Types::String"<<std::endl;
					}
					EndCovCase;
					CovCase(typeid(array)) {
						std::cout<<"Types::Array"<<std::endl;
					}
					EndCovCase;
					CovCase(typeid(void)) {
						std::cout<<"Types::Null"<<std::endl;
					}
					EndCovCase;
				}
				EndCovSwitch;
			}
			return 0;
		}));
		storage.add_var_global("Time", native_interface([](std::deque<cov::any>& args)-> number {
			return cov::timer::time(cov::timer::time_unit::milli_sec);
		}));
		storage.add_var_global("Delay", native_interface([](std::deque<cov::any>& args)-> number {
			cov::timer::delay(cov::timer::time_unit::milli_sec,args.at(0).val<number>());
			return 0;
		}));
		storage.add_var_global("Rand", native_interface([](std::deque<cov::any>& args)-> number {
			std::uniform_real_distribution<number> u(args.at(0).const_val<number>(),args.at(1).const_val<number>());
			static std::default_random_engine e;
			return u(e);
		}));
		storage.add_var_global("RandInt", native_interface([](std::deque<cov::any>& args)-> number {
			std::uniform_int_distribution<long> u(long(args.at(0).const_val<number>()),long(args.at(1).const_val<number>()));
			static std::default_random_engine e;
			return u(e);
		}));
		storage.add_var_global("Exit", native_interface([](std::deque<cov::any>& args)-> number {
			if(args.empty())
				darwin::runtime.exit(0);
			else
				darwin::runtime.exit(args.at(0).val<number>());
			return 0;
		}));
		// File I/O
		storage.add_var_global("OpenFile", native_interface([](std::deque<cov::any>& args)-> number {
			number serial=-1;
			CovSwitch(args.at(1).const_val<string>())
			{
				CovDefault {
					Darwin_Error("Uknow file method.");
				} EndCovCase;
				CovCase("Read") {
					std::ifstream* ifs=new std::ifstream(args.at(0).const_val<string>());
					if(*ifs) {
						files.push_back(ifs);
						serial=files.size()-1;
					} else
						delete ifs;
				}
				EndCovCase;
				CovCase("Write") {
					std::ofstream* ofs=new std::ofstream(args.at(0).const_val<string>());
					if(*ofs) {
						files.push_back(ofs);
						serial=files.size()-1;
					} else
						delete ofs;
				}
				EndCovCase;
			}
			EndCovSwitch;
			return serial;
		}));
		storage.add_var_global("CloseFile", native_interface([](std::deque<cov::any>& args)-> number {
			cov::any& fs=files.at(args.at(0).const_val<number>());
			CovSwitch(fs.type())
			{
				CovCase(typeid(std::ifstream*)) {
					delete fs.val<std::ifstream*>(true);
				}
				EndCovCase;
				CovCase(typeid(std::ofstream*)) {
					delete fs.val<std::ofstream*>(true);
				}
				EndCovCase;
			}
			EndCovSwitch;
			return 0;
		}));
		storage.add_var_global("WriteFile", native_interface([](std::deque<cov::any>& args)-> number {
			cov::any& fs=files.at(args.at(0).const_val<number>());
			if(fs.type()!=typeid(std::ofstream*))
				Darwin_Error("Write read-only file.");
			std::ofstream* ofs=fs.val<std::ofstream*>(true);
			for(std::size_t i=1; i<args.size(); ++i)
				*ofs<<args.at(i);
			*ofs<<std::endl;
			return 0;
		}));
		storage.add_var_global("ReadFile", native_interface([](std::deque<cov::any>& args)-> number {
			cov::any& fs=files.at(args.at(0).const_val<number>());
			if(fs.type()!=typeid(std::ifstream*))
				Darwin_Error("Write write-only file.");
			std::ifstream* ifs=fs.val<std::ifstream*>(true);
			std::cerr<<__LINE__<<std::endl;
			for(std::size_t i=1; i<args.size(); ++i)
			{
				string tmp;
				if(!(*ifs>>tmp))
					return -1;
				args.at(i).assign(to_any(tmp),true);
			}
			return 0;
		}));
		// String
		storage.add_var_global("SizeOfString", native_interface([](std::deque<cov::any>& args)-> number {
			return args.at(0).const_val<string>().size();
		}));
		storage.add_var_global("AppendString", native_interface([](std::deque<cov::any>& args)-> number {
			args.at(0).val<string>(true).append(args.at(1).const_val<string>());
			return 0;
		}));
		storage.add_var_global("CutString", native_interface([](std::deque<cov::any>& args)-> number {
			if(args.empty())
				args.at(0).val<string>(true).pop_back();
			else{
				for(std::size_t i=0; i<args.at(1).const_val<number>(); ++i)
					args.at(0).val<string>(true).pop_back();
			}
			return 0;
		}));
		storage.add_var_global("ClearString", native_interface([](std::deque<cov::any>& args)-> number {
			args.at(0).val<string>(true).clear();
			return 0;
		}));
		// Array
		storage.add_var_global("SizeOfArray", native_interface([](std::deque<cov::any>& args)-> number {
			return args.at(0).const_val<array>().size();
		}));
		storage.add_var_global("PushFrontArray", native_interface([](std::deque<cov::any>& args)-> number {
			args.at(0).val<array>(true).push_front(args.at(1));
			return 0;
		}));
		storage.add_var_global("PopFrontArray", native_interface([](std::deque<cov::any>& args)-> number {
			args.at(0).val<array>(true).pop_front();
			return 0;
		}));
		storage.add_var_global("PushBackArray", native_interface([](std::deque<cov::any>& args)-> number {
			args.at(0).val<array>(true).push_back(args.at(1));
			return 0;
		}));
		storage.add_var_global("PopBackArray", native_interface([](std::deque<cov::any>& args)-> number {
			args.at(0).val<array>(true).pop_back();
			return 0;
		}));
		storage.add_var_global("ClearArray", native_interface([](std::deque<cov::any>& args)-> number {
			args.at(0).val<array>(true).clear();
			return 0;
		}));
		storage.add_var_global("ResizeArray", native_interface([](std::deque<cov::any>& args)-> number {
			args.at(0).val<array>(true).resize(args.at(1).val<number>());
			return 0;
		}));
		// Mathematics
		storage.add_var_global("e", number(2.7182818284));
		storage.add_var_global("pi", number(3.1415926535));
		storage.add_var_global("abs", native_interface([](std::deque<cov::any>& args)-> number {
			return std::abs(args.at(0).val<number>());
		}));
		storage.add_var_global("ln", native_interface([](std::deque<cov::any>& args)-> number {
			return std::log(args.at(0).val<number>());
		}));
		storage.add_var_global("log", native_interface([](std::deque<cov::any>& args)-> number {
			if(args.size()==1)
				return std::log10(args.at(0).val<number>());
			else
				return std::log(args.at(1).val<number>()/args.at(0).val<number>());
		}));
		storage.add_var_global("sin", native_interface([](std::deque<cov::any>& args)-> number {
			return std::sin(args.at(0).val<number>());
		}));
		storage.add_var_global("cos", native_interface([](std::deque<cov::any>& args)-> number {
			return std::cos(args.at(0).val<number>());
		}));
		storage.add_var_global("tan", native_interface([](std::deque<cov::any>& args)-> number {
			return std::tan(args.at(0).val<number>());
		}));
		storage.add_var_global("asin", native_interface([](std::deque<cov::any>& args)-> number {
			return std::sin(args.at(0).val<number>());
		}));
		storage.add_var_global("acos", native_interface([](std::deque<cov::any>& args)-> number {
			return std::cos(args.at(0).val<number>());
		}));
		storage.add_var_global("atan", native_interface([](std::deque<cov::any>& args)-> number {
			return std::tan(args.at(0).val<number>());
		}));
		storage.add_var_global("sqrt", native_interface([](std::deque<cov::any>& args)-> number {
			return std::sqrt(args.at(0).val<number>());
		}));
		storage.add_var_global("root", native_interface([](std::deque<cov::any>& args)-> number {
			return std::pow(args.at(0).val<number>(),number(1)/args.at(1).val<number>());
		}));
		storage.add_var_global("pow", native_interface([](std::deque<cov::any>& args)-> number {
			return std::pow(args.at(0).val<number>(),args.at(1).val<number>());
		}));
		// Graphics
		storage.add_var_global("InitGraphics", native_interface([](std::deque<cov::any>& args)-> number {
			darwin::runtime.load(args.at(0).const_val<string>());
			return 0;
		}));
		storage.add_var_global("FitDrawable", native_interface([](std::deque<cov::any>& args)-> number {
			darwin::runtime.fit_drawable();
			return 0;
		}));
		storage.add_var_global("SetFrameLimit", native_interface([](std::deque<cov::any>& args)-> number {
			clock.set_freq(args.at(0).const_val<number>());
			return 0;
		}));
		storage.add_var_global("UpdateDrawable", native_interface([](std::deque<cov::any>& args)-> number {
			darwin::runtime.update_drawable();
			clock.sync();
			clock.reset();
			return 0;
		}));
		storage.add_var_global("ClearDrawable", native_interface([](std::deque<cov::any>& args)-> number {
			darwin::runtime.get_drawable()->clear();
			return 0;
		}));
		storage.add_var_global("FillDrawable", native_interface([](std::deque<cov::any>& args)-> number {
			darwin::runtime.get_drawable()->fill(pencil);
			return 0;
		}));
		storage.add_var_global("GetDrawableHeight", native_interface([](std::deque<cov::any>& args)-> number {
			return darwin::runtime.get_drawable()->get_height();
		}));
		storage.add_var_global("GetDrawableWidth", native_interface([](std::deque<cov::any>& args)-> number {
			return darwin::runtime.get_drawable()->get_width();
		}));
		storage.add_var_global("DrawPoint", native_interface([](std::deque<cov::any>& args)-> number {
			darwin::runtime.get_drawable()->draw_pixel(args.at(0).const_val<number>(),args.at(1).const_val<number>(),pencil);
			return 0;
		}));
		storage.add_var_global("DrawLine", native_interface([](std::deque<cov::any>& args)-> number {
			darwin::runtime.get_drawable()->draw_line(args.at(0).const_val<number>(),args.at(1).const_val<number>(),args.at(2).const_val<number>(),args.at(3).const_val<number>(),pencil);
			return 0;
		}));
		storage.add_var_global("GetColor", native_interface([](std::deque<cov::any>& args)-> number {
			number color;
			CovSwitch(args.at(0).val<string>())
			{
				CovCase("Black") {
					color=0;
				}
				EndCovCase;
				CovCase("White") {
					color=1;
				}
				EndCovCase;
				CovCase("Red") {
					color=2;
				}
				EndCovCase;
				CovCase("Green") {
					color=3;
				}
				EndCovCase;
				CovCase("Blue") {
					color=4;
				}
				EndCovCase;
				CovCase("Pink") {
					color=5;
				}
				EndCovCase;
				CovCase("Yellow") {
					color=6;
				}
				EndCovCase;
				CovCase("Cyan") {
					color=7;
				}
				EndCovCase;
			}
			EndCovSwitch;
			return color;
		}));
		storage.add_var_global("SetPencil", native_interface([](std::deque<cov::any>& args)-> number {
			char ch=args.at(0).const_val<string>().at(0);
			bool bright=args.at(1).const_val<number>();
			bool underline=args.at(2).const_val<number>();
			darwin::colors fc,bc;
			switch(int(args.at(3).const_val<number>()))
			{
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
			switch(int(args.at(4).const_val<number>()))
			{
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
		std::cout<<"Covariant Basic Parser 17.20 Beta\nCopyright (C) 2017 Covariant Studio\nPlease visit https://github.com/mikecovlee/covbasic for help."<<std::endl;
		while(std::getline(std::cin,str))
			parse(str);
	}
	return 0;
}