#pragma once
#include "../cni.hpp"
static cs::extension list_ext;
namespace list_cs_ext {
	using namespace cs;
	void push_front(list& lst,const cs::any& val)
	{
		lst.push_front(copy(val));
	}
	void pop_front(list& lst)
	{
		lst.pop_front();
	}
	void push_back(list& lst,const cs::any& val)
	{
		lst.push_back(copy(val));
	}
	void pop_back(list& lst)
	{
		lst.pop_back();
	}
	void remove(list& lst,const cs::any& val)
	{
		lst.remove(val);
	}
	cs::any front(const list& lst)
	{
		return lst.front();
	}
	cs::any back(const list& lst)
	{
		return lst.back();
	}
	bool empty(const list& lst)
	{
		return lst.empty();
	}
	void reverse(list& lst)
	{
		lst.reverse();
	}
	void clear(list& lst)
	{
		lst.clear();
	}
	number size(const list& lst)
	{
		return lst.size();
	}
	void init()
	{
		list_ext.add_var("push_front",cs::any::make_protect<native_interface>(cni(push_front),true));
		list_ext.add_var("pop_front",cs::any::make_protect<native_interface>(cni(pop_front),true));
		list_ext.add_var("push_back",cs::any::make_protect<native_interface>(cni(push_back),true));
		list_ext.add_var("pop_back",cs::any::make_protect<native_interface>(cni(pop_back),true));
		list_ext.add_var("remove",cs::any::make_protect<native_interface>(cni(remove),true));
		list_ext.add_var("front",cs::any::make_protect<native_interface>(cni(front),true));
		list_ext.add_var("back",cs::any::make_protect<native_interface>(cni(back),true));
		list_ext.add_var("empty",cs::any::make_protect<native_interface>(cni(empty),true));
		list_ext.add_var("reverse",cs::any::make_protect<native_interface>(cni(reverse),true));
		list_ext.add_var("clear",cs::any::make_protect<native_interface>(cni(clear),true));
		list_ext.add_var("size",cs::any::make_protect<native_interface>(cni(size),true));
	}
}
