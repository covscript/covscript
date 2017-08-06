#pragma once
#include "../cni.hpp"
static cs::extension string_ext;
static cs::extension_t string_ext_shared=std::make_shared<cs::extension_holder>(&string_ext);
namespace cs_impl {
	template<>cs::extension_t& get_ext<cs::string>()
	{
		return string_ext_shared;
	}
}
namespace string_cs_ext {
	using namespace cs;
	string append(string& str,const var& val)
	{
		str.append(val.to_string());
		return str;
	}
	string insert(string& str,number posit,const var& val)
	{
		str.insert(posit,val.to_string());
		return str;
	}
	string erase(string& str,number b,number e)
	{
		str.erase(b,e);
		return str;
	}
	string replace(string& str,number posit,number count,const var& val)
	{
		str.replace(posit,count,val.to_string());
		return str;
	}
	string substr(string& str,number b,number e)
	{
		return str.substr(b,e);
	}
	number find(string& str,const string& s,number posit)
	{
		auto pos=str.find(s,posit);
		if(pos==std::string::npos)
			return -1;
		else
			return pos;
	}
	number rfind(string& str,const string& s,number posit)
	{
		std::size_t pos=0;
		if(posit==-1)
			pos=str.rfind(s,std::string::npos);
		else
			pos=str.rfind(s,posit);
		if(pos==std::string::npos)
			return -1;
		else
			return pos;
	}
	string cut(string& str,number n)
	{
		for(std::size_t i=0; i<n; ++i)
			str.pop_back();
		return str;
	}
	void clear(string& str)
	{
		str.clear();
	}
	number size(const string& str)
	{
		return str.size();
	}
	void init()
	{
		string_ext.add_var("append",var::make_protect<native_interface>(cni(append),true));
		string_ext.add_var("insert",var::make_protect<native_interface>(cni(insert),true));
		string_ext.add_var("erase",var::make_protect<native_interface>(cni(erase),true));
		string_ext.add_var("replace",var::make_protect<native_interface>(cni(replace),true));
		string_ext.add_var("substr",var::make_protect<native_interface>(cni(substr),true));
		string_ext.add_var("find",var::make_protect<native_interface>(cni(find),true));
		string_ext.add_var("rfind",var::make_protect<native_interface>(cni(rfind),true));
		string_ext.add_var("cut",var::make_protect<native_interface>(cni(cut),true));
		string_ext.add_var("clear",var::make_protect<native_interface>(cni(clear),true));
		string_ext.add_var("size",var::make_protect<native_interface>(cni(size),true));
	}
}
