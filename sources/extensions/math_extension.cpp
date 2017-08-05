#ifndef CS_MATH_EXT
#include "../extension.hpp"
#endif
#include "../cni.hpp"
#include <algorithm>
static cs::extension math_ext;
namespace math_cs_ext {
	using namespace cs;
	number abs(number n)
	{
		return std::abs(n);
	}
	number ln(number n)
	{
		return std::log(n);
	}
	number log10(number n)
	{
		return std::log10(n);
	}
	number log(number a,number b)
	{
		return std::log(b/a);
	}
	number sin(number n)
	{
		return std::sin(n);
	}
	number cos(number n)
	{
		return std::cos(n);
	}
	number tan(number n)
	{
		return std::tan(n);
	}
	number asin(number n)
	{
		return std::asin(n);
	}
	number acos(number n)
	{
		return std::acos(n);
	}
	number atan(number n)
	{
		return std::atan(n);
	}
	number sqrt(number n)
	{
		return std::sqrt(n);
	}
	number root(number a,number b)
	{
		return std::pow(a,number(1)/b);
	}
	number pow(number a,number b)
	{
		return std::pow(a,b);
	}
	number min(number a,number b)
	{
		return std::min(a,b);
	}
	number max(number a,number b)
	{
		return std::max(a,b);
	}
	void init()
	{
		math_ext.add_var("pi",cs::any::make_constant<number>(3.1415926535));
		math_ext.add_var("e",cs::any::make_constant<number>(2.7182818284));
		math_ext.add_var("abs",cs::any::make_protect<native_interface>(cni(abs),true));
		math_ext.add_var("ln",cs::any::make_protect<native_interface>(cni(ln),true));
		math_ext.add_var("log10",cs::any::make_protect<native_interface>(cni(log10),true));
		math_ext.add_var("log",cs::any::make_protect<native_interface>(cni(log),true));
		math_ext.add_var("sin",cs::any::make_protect<native_interface>(cni(sin),true));
		math_ext.add_var("cos",cs::any::make_protect<native_interface>(cni(cos),true));
		math_ext.add_var("tan",cs::any::make_protect<native_interface>(cni(tan),true));
		math_ext.add_var("asin",cs::any::make_protect<native_interface>(cni(asin),true));
		math_ext.add_var("acos",cs::any::make_protect<native_interface>(cni(acos),true));
		math_ext.add_var("atan",cs::any::make_protect<native_interface>(cni(atan),true));
		math_ext.add_var("sqrt",cs::any::make_protect<native_interface>(cni(sqrt),true));
		math_ext.add_var("root",cs::any::make_protect<native_interface>(cni(root),true));
		math_ext.add_var("pow",cs::any::make_protect<native_interface>(cni(pow),true));
		math_ext.add_var("min",cs::any::make_protect<native_interface>(cni(min),true));
		math_ext.add_var("max",cs::any::make_protect<native_interface>(cni(max),true));
	}
}
#ifndef CS_MATH_EXT
cs::extension* cs_extension()
{
	math_cs_ext::init();
	return &math_ext;
}
#endif
