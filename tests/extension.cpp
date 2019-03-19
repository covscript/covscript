#include <covscript/dll.hpp>
#include <iostream>

using namespace cs;

namespace cs_impl {
    template<>
    struct type_conversion_cs<int> {
        using source_type=cs::string;
    };

    template<>
    struct type_conversion_cpp<int> {
        using target_type=cs::number;
    };

    template<>
    struct type_convertor<cs::string, int> {
        template<typename T>
        static int convert(T &&val) {
            return std::stoi(std::forward<T>(val));
        }
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

void cs_extension_main(cs::name_space *ns) {
    (*ns)
            .add_var("test0", make_cni(test0, true))
            .add_var("test1", make_cni(test1, cni_type<number(number)>(), true));
}