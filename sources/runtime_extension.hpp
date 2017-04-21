#pragma once
#include "./arglist.hpp"
static cov_basic::extension runtime_ext;
namespace runtime_cbs_ext {
	using namespace cov_basic;
	cov::any info(array&)
	{
		std::cout<<"Covariant Basic Parser\nVersion:2.1.2.1\nCopyright (C) 2017 Michael Lee"<<std::endl;
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
	void init()
	{
		runtime_ext.add_var("info",native_interface(info));
		runtime_ext.add_var("time",native_interface(time));
		runtime_ext.add_var("delay",native_interface(delay));
		runtime_ext.add_var("rand",native_interface(rand));
		runtime_ext.add_var("randint",native_interface(randint));
		runtime_ext.add_var("error",native_interface(error));
		runtime_ext.add_var("load_extension",native_interface(load_extension));
	}
}
