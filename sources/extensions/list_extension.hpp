#pragma once
#include "../cni.hpp"
static cov_basic::extension list_ext;
namespace list_cbs_ext {
	using namespace cov_basic;
	void push_front(list& lst,const cov::any& val)
	{
		lst.push_front(copy(val));
	}
	void pop_front(list& lst)
	{
		lst.pop_front();
	}
	void push_back(list& lst,const cov::any& val)
	{
		lst.push_back(copy(val));
	}
	void pop_back(list& lst)
	{
		lst.pop_back();
	}
	void remove(list& lst,const cov::any& val)
	{
		lst.remove(val);
	}
	cov::any front(const list& lst)
	{
		return lst.front();
	}
	cov::any back(const list& lst)
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
		list_ext.add_var("push_front",cov::any::make_protect<native_interface>(cni(push_front),true));
		list_ext.add_var("pop_front",cov::any::make_protect<native_interface>(cni(pop_front),true));
		list_ext.add_var("push_back",cov::any::make_protect<native_interface>(cni(push_back),true));
		list_ext.add_var("pop_back",cov::any::make_protect<native_interface>(cni(pop_back),true));
		list_ext.add_var("remove",cov::any::make_protect<native_interface>(cni(remove),true));
		list_ext.add_var("front",cov::any::make_protect<native_interface>(cni(front),true));
		list_ext.add_var("back",cov::any::make_protect<native_interface>(cni(back),true));
		list_ext.add_var("empty",cov::any::make_protect<native_interface>(cni(empty),true));
		list_ext.add_var("reverse",cov::any::make_protect<native_interface>(cni(reverse),true));
		list_ext.add_var("clear",cov::any::make_protect<native_interface>(cni(clear),true));
		list_ext.add_var("size",cov::any::make_protect<native_interface>(cni(size),true));
	}
}
