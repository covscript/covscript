#pragma once
#include "../cni.hpp"
static cs::extension list_ext;
static cs::extension_t list_ext_shared=std::make_shared<cs::extension_holder>(&list_ext);
namespace cs_impl {
	template<>cs::extension_t& get_ext<cs::list>()
	{
		return list_ext_shared;
	}
}
static cs::extension list_iterator_ext;
static cs::extension_t list_iterator_ext_shared=std::make_shared<cs::extension_holder>(&list_iterator_ext);
namespace cs_impl {
	template<>cs::extension_t& get_ext<cs::list::iterator>()
	{
		return list_iterator_ext_shared;
	}
}
namespace list_cs_ext {
	using namespace cs;
// Element access
	var front(const list& lst)
	{
		return lst.front();
	}
	var back(const list& lst)
	{
		return lst.back();
	}
// Iterators
	list::iterator begin(list& lst)
	{
		return lst.begin();
	}
	list::iterator term(list& lst)
	{
		return lst.end();
	}
	list::iterator forward(list::iterator& it)
	{
		return ++it;
	}
	list::iterator backward(list::iterator& it)
	{
		return --it;
	}
	var data(list::iterator& it)
	{
		return *it;
	}
// Capacity
	bool empty(const list& lst)
	{
		return lst.empty();
	}
	number size(const list& lst)
	{
		return lst.size();
	}
// Modifiers
	void clear(list& lst)
	{
		lst.clear();
	}
	list::iterator insert(list& lst,list::iterator& pos,const var& val)
	{
		return lst.insert(pos,copy(val));
	}
	list::iterator erase(list& lst,list::iterator& pos)
	{
		return lst.erase(pos);
	}
	void push_front(list& lst,const var& val)
	{
		lst.push_front(copy(val));
	}
	void pop_front(list& lst)
	{
		lst.pop_front();
	}
	void push_back(list& lst,const var& val)
	{
		lst.push_back(copy(val));
	}
	void pop_back(list& lst)
	{
		lst.pop_back();
	}
// Operations
	void remove(list& lst,const var& val)
	{
		lst.remove(val);
	}
	void reverse(list& lst)
	{
		lst.reverse();
	}
	void unique(list& lst)
	{
		lst.unique();
	}
	void init()
	{
		list_ext.add_var("front",var::make_protect<native_interface>(cni(front),true));
		list_ext.add_var("back",var::make_protect<native_interface>(cni(back),true));
		list_ext.add_var("begin",var::make_protect<native_interface>(cni(begin),true));
		list_ext.add_var("term",var::make_protect<native_interface>(cni(term),true));
		list_iterator_ext.add_var("forward",var::make_protect<native_interface>(cni(forward),true));
		list_iterator_ext.add_var("backward",var::make_protect<native_interface>(cni(backward),true));
		list_iterator_ext.add_var("data",var::make_protect<native_interface>(cni(data),true));
		list_ext.add_var("empty",var::make_protect<native_interface>(cni(empty),true));
		list_ext.add_var("size",var::make_protect<native_interface>(cni(size),true));
		list_ext.add_var("clear",var::make_protect<native_interface>(cni(clear),true));
		list_ext.add_var("insert",var::make_protect<native_interface>(cni(insert),true));
		list_ext.add_var("erase",var::make_protect<native_interface>(cni(erase),true));
		list_ext.add_var("push_front",var::make_protect<native_interface>(cni(push_front),true));
		list_ext.add_var("pop_front",var::make_protect<native_interface>(cni(pop_front),true));
		list_ext.add_var("push_back",var::make_protect<native_interface>(cni(push_back),true));
		list_ext.add_var("pop_back",var::make_protect<native_interface>(cni(pop_back),true));
		list_ext.add_var("remove",var::make_protect<native_interface>(cni(remove),true));
		list_ext.add_var("reverse",var::make_protect<native_interface>(cni(reverse),true));
		list_ext.add_var("unique",var::make_protect<native_interface>(cni(unique),true));
	}
}
