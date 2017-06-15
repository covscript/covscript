#pragma once
#include "../arglist.hpp"
#include <cctype>
static cov_basic::extension char_ext;
namespace char_cbs_ext {
	using namespace cov_basic;
	cov::any isalnum(array& args)
	{
		arglist::check<char>(args);
		return bool(std::isalnum(args.at(0).const_val<char>()));
	}
	cov::any isalpha(array& args)
	{
		arglist::check<char>(args);
		return bool(std::isalpha(args.at(0).const_val<char>()));
	}
	cov::any islower(array& args)
	{
		arglist::check<char>(args);
		return bool(std::islower(args.at(0).const_val<char>()));
	}
	cov::any isupper(array& args)
	{
		arglist::check<char>(args);
		return bool(std::isupper(args.at(0).const_val<char>()));
	}
	cov::any isdigit(array& args)
	{
		arglist::check<char>(args);
		return bool(std::isdigit(args.at(0).const_val<char>()));
	}
	cov::any iscntrl(array& args)
	{
		arglist::check<char>(args);
		return bool(std::iscntrl(args.at(0).const_val<char>()));
	}
	cov::any isgraph(array& args)
	{
		arglist::check<char>(args);
		return bool(std::isgraph(args.at(0).const_val<char>()));
	}
	cov::any isspace(array& args)
	{
		arglist::check<char>(args);
		return bool(std::isspace(args.at(0).const_val<char>()));
	}
	cov::any isblank(array& args)
	{
		arglist::check<char>(args);
		return bool(std::isblank(args.at(0).const_val<char>()));
	}
	cov::any isprint(array& args)
	{
		arglist::check<char>(args);
		return bool(std::isprint(args.at(0).const_val<char>()));
	}
	cov::any ispunct(array& args)
	{
		arglist::check<char>(args);
		return bool(std::ispunct(args.at(0).const_val<char>()));
	}
	cov::any tolower(array& args)
	{
		arglist::check<char>(args);
		return char(std::tolower(args.at(0).const_val<char>()));
	}
	cov::any toupper(array& args)
	{
		arglist::check<char>(args);
		return char(std::toupper(args.at(0).const_val<char>()));
	}
	void init()
	{
		char_ext.add_var("isalnum",cov::any::make_protect<native_interface>(isalnum,true));
		char_ext.add_var("isalpha",cov::any::make_protect<native_interface>(isalpha,true));
		char_ext.add_var("islower",cov::any::make_protect<native_interface>(islower,true));
		char_ext.add_var("isupper",cov::any::make_protect<native_interface>(isupper,true));
		char_ext.add_var("isdigit",cov::any::make_protect<native_interface>(isdigit,true));
		char_ext.add_var("iscntrl",cov::any::make_protect<native_interface>(iscntrl,true));
		char_ext.add_var("isgraph",cov::any::make_protect<native_interface>(isgraph,true));
		char_ext.add_var("isspace",cov::any::make_protect<native_interface>(isspace,true));
		char_ext.add_var("isblank",cov::any::make_protect<native_interface>(isblank,true));
		char_ext.add_var("isprint",cov::any::make_protect<native_interface>(isprint,true));
		char_ext.add_var("ispunct",cov::any::make_protect<native_interface>(ispunct,true));
		char_ext.add_var("tolower",cov::any::make_protect<native_interface>(tolower,true));
		char_ext.add_var("toupper",cov::any::make_protect<native_interface>(toupper,true));
	}
}
