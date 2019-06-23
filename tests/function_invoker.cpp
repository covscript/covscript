#include <covscript/covscript.hpp>

int main(int argc, const char *argv[]) {
    if (argc <= 1)
        return -1;
    cs::context_t context = cs::create_context(cs::parse_cmd_args(argc - 1, argv + 1));
    context->instance->compile(argv[1]);
    context->instance->interpret();
    cs::function_invoker<cs::number(cs::number, cs::number)> test(cs::eval(context, "test"));
    int result = test(1, 2);
    cs::function_invoker<void(cs::number)> println(cs::eval(context, "system.out.println"));
    println(result);
    cs::collect_context(context);
    return 0;
}