#ifndef CBS_MATH_EXT
#include "../extension.hpp"
#endif
#include "../arglist.hpp"
static cov_basic::extension math_ext;
namespace math_cbs_ext {
	using namespace cov_basic;
	cov::any abs(array& args)
	{
		arglist::check<number>(args);
		return number(std::abs(args.at(0).const_val<number>()));
	}
	cov::any ln(array& args)
	{
		arglist::check<number>(args);
		return number(std::log(args.at(0).const_val<number>()));
	}
	cov::any log10(array& args)
	{
		arglist::check<number>(args);
		return number(std::log10(args.at(0).const_val<number>()));
	}
	cov::any log(array& args)
	{
		arglist::check<number,number>(args);
		return number(std::log(args.at(1).const_val<number>()/args.at(0).const_val<number>()));
	}
	cov::any sin(array& args)
	{
		arglist::check<number>(args);
		return number(std::sin(args.at(0).const_val<number>()));
	}
	cov::any cos(array& args)
	{
		arglist::check<number>(args);
		return number(std::cos(args.at(0).const_val<number>()));
	}
	cov::any tan(array& args)
	{
		arglist::check<number>(args);
		return number(std::tan(args.at(0).const_val<number>()));
	}
	cov::any asin(array& args)
	{
		arglist::check<number>(args);
		return number(std::sin(args.at(0).const_val<number>()));
	}
	cov::any acos(array& args)
	{
		arglist::check<number>(args);
		return number(std::cos(args.at(0).const_val<number>()));
	}
	cov::any atan(array& args)
	{
		arglist::check<number>(args);
		return number(std::tan(args.at(0).const_val<number>()));
	}
	cov::any sqrt(array& args)
	{
		arglist::check<number>(args);
		return number(std::sqrt(args.at(0).const_val<number>()));
	}
	cov::any root(array& args)
	{
		arglist::check<number,number>(args);
		return number(std::pow(args.at(0).const_val<number>(),number(1)/args.at(1).const_val<number>()));
	}
	cov::any pow(array& args)
	{
		arglist::check<number,number>(args);
		return number(std::pow(args.at(0).const_val<number>(),args.at(1).const_val<number>()));
	}
	void init()
	{
		math_ext.add_var("pi",number(3.1415926535));
		math_ext.add_var("e",number(2.7182818284));
		math_ext.add_var("abs",cov::any::make_constant<native_interface>(abs));
		math_ext.add_var("ln",cov::any::make_constant<native_interface>(ln));
		math_ext.add_var("log10",cov::any::make_constant<native_interface>(log10));
		math_ext.add_var("log",cov::any::make_constant<native_interface>(log));
		math_ext.add_var("sin",cov::any::make_constant<native_interface>(sin));
		math_ext.add_var("cos",cov::any::make_constant<native_interface>(cos));
		math_ext.add_var("tan",cov::any::make_constant<native_interface>(tan));
		math_ext.add_var("asin",cov::any::make_constant<native_interface>(asin));
		math_ext.add_var("acos",cov::any::make_constant<native_interface>(acos));
		math_ext.add_var("atan",cov::any::make_constant<native_interface>(atan));
		math_ext.add_var("sqrt",cov::any::make_constant<native_interface>(sqrt));
		math_ext.add_var("root",cov::any::make_constant<native_interface>(root));
		math_ext.add_var("pow",cov::any::make_constant<native_interface>(pow));
	}
}
#ifndef CBS_MATH_EXT
cov_basic::extension* cbs_extension()
{
	math_cbs_ext::init();
	return &math_ext;
}
#endif
