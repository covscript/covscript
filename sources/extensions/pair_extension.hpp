#pragma once
#include "../cni.hpp"
static cs::extension pair_ext;
static cs::extension_t pair_ext_shared=std::make_shared<cs::extension_holder>(&pair_ext);
namespace cs_impl {
	template<>cs::extension_t& get_ext<cs::pair>()
	{
		return pair_ext_shared;
	}
}
namespace pair_cs_ext {
	using namespace cs;
	var first(const pair& p)
	{
		return p.first;
	}
	var second(const pair& p)
	{
		return p.second;
	}
	void init()
	{
		pair_ext.add_var("first",var::make_protect<callable>(cni(first),true));
		pair_ext.add_var("second",var::make_protect<callable>(cni(second),true));
	}
}
