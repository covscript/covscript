#pragma once
#include "../cni.hpp"
#include <cctype>
static cov_basic::extension char_ext;
namespace char_cbs_ext {
	using namespace cov_basic;
	bool isalnum(char c)
	{
		return std::isalnum(c);
	}
	bool isalpha(char c)
	{
		return std::isalpha(c);
	}
	bool islower(char c)
	{
		return std::islower(c);
	}
	bool isupper(char c)
	{
		return std::isupper(c);
	}
	bool isdigit(char c)
	{
		return std::isdigit(c);
	}
	bool iscntrl(char c)
	{
		return std::iscntrl(c);
	}
	bool isgraph(char c)
	{
		return std::isgraph(c);
	}
	bool isspace(char c)
	{
		return std::isspace(c);
	}
	bool isblank(char c)
	{
		return std::isblank(c);
	}
	bool isprint(char c)
	{
		return std::isprint(c);
	}
	bool ispunct(char c)
	{
		return std::ispunct(c);
	}
	char tolower(char c)
	{
		return std::tolower(c);
	}
	char toupper(char c)
	{
		return std::toupper(c);
	}
	void init()
	{
		char_ext.add_var("isalnum",cov::any::make_protect<native_interface>(cni(isalnum),true));
		char_ext.add_var("isalpha",cov::any::make_protect<native_interface>(cni(isalpha),true));
		char_ext.add_var("islower",cov::any::make_protect<native_interface>(cni(islower),true));
		char_ext.add_var("isupper",cov::any::make_protect<native_interface>(cni(isupper),true));
		char_ext.add_var("isdigit",cov::any::make_protect<native_interface>(cni(isdigit),true));
		char_ext.add_var("iscntrl",cov::any::make_protect<native_interface>(cni(iscntrl),true));
		char_ext.add_var("isgraph",cov::any::make_protect<native_interface>(cni(isgraph),true));
		char_ext.add_var("isspace",cov::any::make_protect<native_interface>(cni(isspace),true));
		char_ext.add_var("isblank",cov::any::make_protect<native_interface>(cni(isblank),true));
		char_ext.add_var("isprint",cov::any::make_protect<native_interface>(cni(isprint),true));
		char_ext.add_var("ispunct",cov::any::make_protect<native_interface>(cni(ispunct),true));
		char_ext.add_var("tolower",cov::any::make_protect<native_interface>(cni(tolower),true));
		char_ext.add_var("toupper",cov::any::make_protect<native_interface>(cni(toupper),true));
	}
}
