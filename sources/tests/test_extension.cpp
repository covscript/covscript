#include "../extension.hpp"
#include "../cni.hpp"
#include <iostream>
using namespace cov_basic;
extension my_ext;
number test(number a)
{
	std::cout<<a<<std::endl;
	return a+1;
}
extension* cbs_extension()
{
	my_ext.add_var("test",cov::any::make_protect<native_interface>(cni(test),true));
	return &my_ext;
}
