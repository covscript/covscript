#include <covscript/cni.hpp>
#include <covscript/dll.hpp>
#include <iostream>

BEGIN_CNI_ROOT_NAMESPACE

int test(int a)
{
    std::cout << a << std::endl;
    return a + 1;
}

CNI_NORMAL(test)

BEGIN_CNI_NAMESPACE(child)

double test(double b)
{
    std::cout << b << std::endl;
    return b - 0.1;
}

CNI_CONST(test)

END_CNI_NAMESPACE(child)

END_CNI_ROOT_NAMESAPCE