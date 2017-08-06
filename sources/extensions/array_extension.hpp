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
static cs::extension array_iterator_ext;
static cs::extension_t array_iterator_ext_shared=std::make_shared<cs::extension_holder>(&array_iterator_ext);
namespace cs_any {
	template<>cs::extension_t& get_ext<cs::array::iterator>()
	{
		return array_iterator_ext_shared;
	}
}
namespace array_cs_ext {
	using namespace cs;
// Element access
	cs::any at(const array& arr,number posit)
	{
		return arr.at(posit);
	}
	cs::any front(const array& arr)
	{
		return arr.front();
	}
	cs::any back(const array& arr)
	{
		return arr.back();
	}
// Iterators
	array::iterator begin(array& arr)
	{
		return arr.begin();
	}
	array::iterator term(array& arr)
	{
		return arr.end();
	}
	array::iterator forward(array::iterator& it)
	{
		return ++it;
	}
	array::iterator backward(array::iterator& it)
	{
		return --it;
	}
	cs::any data(array::iterator& it)
	{
		return *it;
	}
// Capacity
	bool empty(const array& arr)
	{
		return arr.empty();
	}
	number size(const array& arr)
	{
		return arr.size();
	}
// Modifiers
	void clear(array& arr)
	{
		arr.clear();
	}
	array::iterator insert(array& arr,array::iterator& pos,const cs::any& val)
	{
		return arr.insert(pos,copy(val));
	}
	array::iterator erase(array& arr,array::iterator& pos)
	{
		return arr.erase(pos);
	}
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
// Operations
	cs::any to_list(const array& arr)
	{
		return cs::any::make<list>(arr.begin(),arr.end());
	}
	void init()
	{
		array_ext.add_var("at",cs::any::make_protect<native_interface>(cni(at),true));
		array_ext.add_var("front",cs::any::make_protect<native_interface>(cni(front),true));
		array_ext.add_var("back",cs::any::make_protect<native_interface>(cni(back),true));
		array_ext.add_var("begin",cs::any::make_protect<native_interface>(cni(begin),true));
		array_ext.add_var("term",cs::any::make_protect<native_interface>(cni(term),true));
		array_iterator_ext.add_var("forward",cs::any::make_protect<native_interface>(cni(forward),true));
		array_iterator_ext.add_var("backward",cs::any::make_protect<native_interface>(cni(backward),true));
		array_iterator_ext.add_var("data",cs::any::make_protect<native_interface>(cni(data),true));
		array_ext.add_var("empty",cs::any::make_protect<native_interface>(cni(empty),true));
		array_ext.add_var("size",cs::any::make_protect<native_interface>(cni(size),true));
		array_ext.add_var("clear",cs::any::make_protect<native_interface>(cni(clear),true));
		array_ext.add_var("insert",cs::any::make_protect<native_interface>(cni(insert),true));
		array_ext.add_var("erase",cs::any::make_protect<native_interface>(cni(erase),true));
		array_ext.add_var("push_front",cs::any::make_protect<native_interface>(cni(push_front),true));
		array_ext.add_var("pop_front",cs::any::make_protect<native_interface>(cni(pop_front),true));
		array_ext.add_var("push_back",cs::any::make_protect<native_interface>(cni(push_back),true));
		array_ext.add_var("pop_back",cs::any::make_protect<native_interface>(cni(pop_back),true));
		array_ext.add_var("to_list",cs::any::make_protect<native_interface>(cni(to_list),true));
	}
}
