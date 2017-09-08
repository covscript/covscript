#include "../sources/extension.hpp"
#include "../sources/cni.hpp"
#include <iostream>

using namespace cs;
static extension my_ext;

number test(number a)
{
	std::cout << a << std::endl;
	return a + 1;
}

extension *cs_extension()
{
	my_ext.add_var("test", var::make_protect<callable>(cni(test), true));
	return &my_ext;
}
