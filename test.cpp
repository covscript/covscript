#include <covscript/extension.hpp>
#include <covscript/cni.hpp>
#include <iostream>
static cs::extension test_ext;
int id_count=0;
class test
{
    int id=++id_count;
    public:
    test()
    {
        std::clog<<__func__<<id<<std::endl;
    }
    test(const test&)
    {
        std::clog<<__func__<<id<<std::endl;
    }
    test(test&&) noexcept
    {
        std::clog<<__func__<<id<<std::endl;
    }
    ~test()
    {
        std::clog<<__func__<<id<<std::endl;
    }
};
test new_test()
{
    return test();
}
cs::extension* cs_extension()
{
    test_ext.add_var("new_test",cs::var::make_protect<cs::callable>(cs::cni(new_test)));
    return &test_ext;
}