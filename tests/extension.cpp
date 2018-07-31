#include <covscript/extension.hpp>
#include <iostream>

using namespace cs;
static extension my_ext;

namespace cs_impl {
    template<>struct type_conversion<int>{
        using target_type=cs::number;
    };
}

int test0(int a) {
    std::cout << a << std::endl;
    return a + 1;
}

double test1(double b) {
    std::cout << b << std::endl;
    return b - 0.1;
}

extension *cs_extension() {
    my_ext.add_var("test0", var::make_protect<callable>(cni(test0), true));
    my_ext.add_var("test1", var::make_protect<callable>(cni(test1, cni_type<number(number)>()), true));
    return &my_ext;
}