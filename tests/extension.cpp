#include <covscript/extension.hpp>
#include <iostream>

using namespace cs;
static extension my_ext;

int test(int a) {
    std::cout << a << std::endl;
    return a + 1;
}

extension *cs_extension() {
    my_ext.add_var("test", var::make_protect<callable>(cni(test, cni_type<number(number)>()), true));
    return &my_ext;
}