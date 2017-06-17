#pragma once
#include "../arglist.hpp"
static cov_basic::extension pair_ext;
namespace pair_cbs_ext {
	using namespace cov_basic;
	cov::any first(array& args)
	{
		arglist::check<pair>(args);
		return args.front().const_val<pair>().first;
	}
	cov::any second(array& args)
	{
		arglist::check<pair>(args);
		return args.front().const_val<pair>().second;
	}
	void init()
	{
		pair_ext.add_var("first",cov::any::make_protect<native_interface>(first,true));
		pair_ext.add_var("second",cov::any::make_protect<native_interface>(second,true));
	}
}
