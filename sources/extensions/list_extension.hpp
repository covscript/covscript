#pragma once
#include "../arglist.hpp"
static cov_basic::extension list_ext;
namespace list_cbs_ext {
	using namespace cov_basic;
	cov::any push_front(array& args)
	{
		if(args.size()!=2)
			throw syntax_error("Wrong size of arguments.");
		if(args.at(0).type()!=typeid(list))
			throw syntax_error("Wrong type of arguments.(Request list)");
		args.front().val<list>(true).push_front(copy(args.at(1)));
		return number(0);
	}
	cov::any pop_front(array& args)
	{
		arglist::check<list>(args);
		args.front().val<list>(true).pop_front();
		return number(0);
	}
	cov::any push_back(array& args)
	{
		if(args.size()!=2)
			throw syntax_error("Wrong size of arguments.");
		if(args.at(0).type()!=typeid(list))
			throw syntax_error("Wrong type of arguments.(Request list)");
		args.front().val<list>(true).push_back(copy(args.at(1)));
		return number(0);
	}
	cov::any pop_back(array& args)
	{
		arglist::check<list>(args);
		args.front().val<list>(true).pop_back();
		return number(0);
	}
	cov::any remove(array& args)
	{
		if(args.size()!=2)
			throw syntax_error("Wrong size of arguments.");
		if(args.at(0).type()!=typeid(list))
			throw syntax_error("Wrong type of arguments.(Request list)");
		args.front().val<list>(true).remove(args.at(1));
		return number(0);
	}
	cov::any front(array& args)
	{
		arglist::check<list>(args);
		return args.front().const_val<list>().front();
	}
	cov::any back(array& args)
	{
		arglist::check<list>(args);
		return args.front().const_val<list>().back();
	}
	cov::any empty(array& args)
	{
		arglist::check<list>(args);
		return args.front().const_val<list>().empty();
	}
	cov::any reverse(array& args)
	{
		arglist::check<list>(args);
		args.front().val<list>(true).reverse();
		return number(0);
	}
	cov::any clear(array& args)
	{
		arglist::check<list>(args);
		args.front().val<list>(true).clear();
		return number(0);
	}
	void init()
	{
		list_ext.add_var("push_front",cov::any::make_protect<native_interface>(push_front,true));
		list_ext.add_var("pop_front",cov::any::make_protect<native_interface>(pop_front,true));
		list_ext.add_var("push_back",cov::any::make_protect<native_interface>(push_back,true));
		list_ext.add_var("pop_back",cov::any::make_protect<native_interface>(pop_back,true));
		list_ext.add_var("remove",cov::any::make_protect<native_interface>(remove,true));
		list_ext.add_var("front",cov::any::make_protect<native_interface>(front,true));
		list_ext.add_var("back",cov::any::make_protect<native_interface>(back,true));
		list_ext.add_var("empty",cov::any::make_protect<native_interface>(empty,true));
		list_ext.add_var("reverse",cov::any::make_protect<native_interface>(reverse,true));
		list_ext.add_var("clear",cov::any::make_protect<native_interface>(clear,true));
	}
}
