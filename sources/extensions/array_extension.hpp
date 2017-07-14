#pragma once
#include "../cni.hpp"
static cs::extension array_ext;
namespace array_cs_ext {
	using namespace cs;
	void push_front(array& arr,const cov::any& val)
	{
		arr.push_front(copy(val));
	}
	void pop_front(array& arr)
	{
		arr.pop_front();
	}
	void push_back(array& arr,const cov::any& val)
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
	cov::any at(const array& arr,number posit)
	{
		return arr.at(posit);
	}
	number size(const array& arr)
	{
		return arr.size();
	}
	void init()
	{
		array_ext.add_var("push_front",cov::any::make_protect<native_interface>(cni(push_front),true));
		array_ext.add_var("pop_front",cov::any::make_protect<native_interface>(cni(pop_front),true));
		array_ext.add_var("push_back",cov::any::make_protect<native_interface>(cni(push_back),true));
		array_ext.add_var("pop_back",cov::any::make_protect<native_interface>(cni(pop_back),true));
		array_ext.add_var("clear",cov::any::make_protect<native_interface>(cni(clear),true));
		array_ext.add_var("at",cov::any::make_protect<native_interface>(cni(at),true));
		array_ext.add_var("size",cov::any::make_protect<native_interface>(cni(size),true));
	}
}
