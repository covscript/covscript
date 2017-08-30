#pragma once

#include "../cni.hpp"
#include <cctype>

static cs::extension char_ext;
static cs::extension_t char_ext_shared = std::make_shared<cs::extension_holder>(&char_ext);
namespace cs_impl {
	template<>
	cs::extension_t &get_ext<char>()
	{
		return char_ext_shared;
	}
}
namespace char_cs_ext {
	using namespace cs;

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
		char_ext.add_var("isalnum", var::make_protect<callable>(cni(isalnum), true));
		char_ext.add_var("isalpha", var::make_protect<callable>(cni(isalpha), true));
		char_ext.add_var("islower", var::make_protect<callable>(cni(islower), true));
		char_ext.add_var("isupper", var::make_protect<callable>(cni(isupper), true));
		char_ext.add_var("isdigit", var::make_protect<callable>(cni(isdigit), true));
		char_ext.add_var("iscntrl", var::make_protect<callable>(cni(iscntrl), true));
		char_ext.add_var("isgraph", var::make_protect<callable>(cni(isgraph), true));
		char_ext.add_var("isspace", var::make_protect<callable>(cni(isspace), true));
		char_ext.add_var("isblank", var::make_protect<callable>(cni(isblank), true));
		char_ext.add_var("isprint", var::make_protect<callable>(cni(isprint), true));
		char_ext.add_var("ispunct", var::make_protect<callable>(cni(ispunct), true));
		char_ext.add_var("tolower", var::make_protect<callable>(cni(tolower), true));
		char_ext.add_var("toupper", var::make_protect<callable>(cni(toupper), true));
	}
}
