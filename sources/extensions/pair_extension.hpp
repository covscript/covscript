#pragma once
#include "../cni.hpp"
static cs::extension pair_ext;
namespace pair_cs_ext {
	using namespace cs;
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
