#pragma once
#include "../cni.hpp"
static cov_basic::extension array_ext;
namespace array_cbs_ext {
	using namespace cov_basic;
	cov::any push_front(array& args)
	{
		if(args.size()!=2)
			throw syntax_error("Wrong size of arguments.");
		if(args.at(0).type()!=typeid(array))
			throw syntax_error("Wrong type of arguments.(Request Array)");
		args.at(0).val<array>(true).push_front(copy(args.at(1)));
		return number(0);
	}
	cov::any pop_front(array& args)
	{
		if(args.size()!=1)
			throw syntax_error("Wrong size of arguments.");
		if(args.at(0).type()!=typeid(array))
			throw syntax_error("Wrong type of arguments.(Request Array)");
		args.at(0).val<array>(true).pop_front();
		return number(0);
	}
	cov::any push_back(array& args)
	{
		if(args.size()!=2)
			throw syntax_error("Wrong size of arguments.");
		if(args.at(0).type()!=typeid(array))
			throw syntax_error("Wrong type of arguments.(Request Array)");
		args.at(0).val<array>(true).push_back(copy(args.at(1)));
		return number(0);
	}
	cov::any pop_back(array& args)
	{
		if(args.size()!=1)
			throw syntax_error("Wrong size of arguments.");
		if(args.at(0).type()!=typeid(array))
			throw syntax_error("Wrong type of arguments.(Request Array)");
		args.at(0).val<array>(true).pop_back();
		return number(0);
	}
	cov::any clear(array& args)
	{
		if(args.size()!=1)
			throw syntax_error("Wrong size of arguments.");
		if(args.at(0).type()!=typeid(array))
			throw syntax_error("Wrong type of arguments.(Request Array)");
		args.at(0).val<array>(true).clear();
		return number(0);
	}
	cov::any at(const array& arr,number posit)
	{
		return arr.at(posit);
	}
	void init()
	{
		array_ext.add_var("push_front",cov::any::make_protect<native_interface>(push_front,true));
		array_ext.add_var("pop_front",cov::any::make_protect<native_interface>(pop_front,true));
		array_ext.add_var("push_back",cov::any::make_protect<native_interface>(push_back,true));
		array_ext.add_var("pop_back",cov::any::make_protect<native_interface>(pop_back,true));
		array_ext.add_var("clear",cov::any::make_protect<native_interface>(clear,true));
		array_ext.add_var("at",cov::any::make_protect<native_interface>(cni(at),true));
	}
}
