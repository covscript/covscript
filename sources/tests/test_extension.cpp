#include "../extension.hpp"
#include "../cni.hpp"
#include <iostream>
using namespace cs;
extension my_ext;
number test(number a)
{
	std::cout<<a<<std::endl;
	return a+1;
}
extension* cs_extension()
{
	my_ext.add_var("test",cov::any::make_protect<native_interface>(cni(test),true));
	return &my_ext;
}
