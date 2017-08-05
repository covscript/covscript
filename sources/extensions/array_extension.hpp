#pragma once
#include "../cni.hpp"
static cs::extension array_ext;
static cs::extension_t array_ext_shared=std::make_shared<cs::extension_holder>(&array_ext);
namespace cs_any {
	template<>cs::extension_t& get_ext<cs::array>()
	{
		return array_ext_shared;
	}
}
namespace array_cs_ext {
	using namespace cs;
	void push_front(array& arr,const cs::any& val)
	{
		arr.push_front(copy(val));
	}
	void pop_front(array& arr)
	{
		arr.pop_front();
	}
	void push_back(array& arr,const cs::any& val)
	{
		arr.push_back(copy(val));
	}
	void pop_back(array& arr)
	{
		arr.pop_back();
	}
	void clear(array& arr)
	{
		arr.clear();
	}
	cs::any at(const array& arr,number posit)
	{
		return arr.at(posit);
	}
	number size(const array& arr)
	{
		return arr.size();
	}
	void init()
	{
		array_ext.add_var("push_front",cs::any::make_protect<native_interface>(cni(push_front),true));
		array_ext.add_var("pop_front",cs::any::make_protect<native_interface>(cni(pop_front),true));
		array_ext.add_var("push_back",cs::any::make_protect<native_interface>(cni(push_back),true));
		array_ext.add_var("pop_back",cs::any::make_protect<native_interface>(cni(pop_back),true));
		array_ext.add_var("clear",cs::any::make_protect<native_interface>(cni(clear),true));
		array_ext.add_var("at",cs::any::make_protect<native_interface>(cni(at),true));
		array_ext.add_var("size",cs::any::make_protect<native_interface>(cni(size),true));
	}
}
