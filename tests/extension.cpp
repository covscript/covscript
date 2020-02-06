#include <covscript/cni.hpp>
#include <covscript/dll.hpp>
#include <iostream>

CNI_ROOT_NAMESPACE {
    int test(int a) {
        std::cout << a << std::endl;
        return a + 1;
    }

    CNI(test)

    CNI_V(test_v, [](int a) { return a + 1; })

    CNI_VALUE(val, 30)

    CNI_VALUE_V(val_v, cs::number, 30)

    class foo_t {
    public:
        float test(float c) {
            std::cout << c << std::endl;
            return c - 0.5;
        }
    };

    CNI_TYPE_EXT(foo, foo_t, foo_t()) {
        CNI_V(test, &foo_t::test)
    }

    CNI_NAMESPACE(child) {

        double test(double b) {
            std::cout << b << std::endl;
            return b - 0.1;
        }

        CNI_CONST(test)

        CNI_CONST_V(test_v, [](double b) { return b - 0.1; })

        CNI_VALUE_CONST(val, 30)

        CNI_VALUE_CONST_V(val_v, double, 30)

        class foo {
        public:
            int val = 10;
            const char *test(const char *str) {
                std::cout << str;
                return ",World";
            }
        };

        CNI_TYPE_EXT_V(foo_ext, foo, foo, foo()) {
            CNI_CLASS_MEMBER(foo, val);
            CNI_V(test, &foo::test)
        }
    }
}

CNI_ENABLE_TYPE_EXT(foo, cni_root_namespace::foo_t)
CNI_ENABLE_TYPE_EXT_V(child::foo_ext, cni_root_namespace::child::foo, child::foo)