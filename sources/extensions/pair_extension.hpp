#pragma once
#include "../cni.hpp"
static cs::extension pair_ext;
namespace pair_cs_ext {
	using namespace cs;
	cs::any first(const pair& p)
	{
		return p.first;
	}
	cs::any second(const pair& p)
	{
		return p.second;
	}
	void init()
	{
		pair_ext.add_var("first",cs::any::make_protect<native_interface>(cni(first),true));
		pair_ext.add_var("second",cs::any::make_protect<native_interface>(cni(second),true));
	}
}
