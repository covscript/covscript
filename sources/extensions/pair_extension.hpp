#pragma once
#include "../cni.hpp"
static cov_basic::extension pair_ext;
namespace pair_cbs_ext {
	using namespace cov_basic;
	cov::any first(const pair& p)
	{
		return p.first;
	}
	cov::any second(const pair& p)
	{
		return p.second;
	}
	void init()
	{
		pair_ext.add_var("first",cov::any::make_protect<native_interface>(cni(first),true));
		pair_ext.add_var("second",cov::any::make_protect<native_interface>(cni(second),true));
	}
}
