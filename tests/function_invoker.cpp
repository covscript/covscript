#include <covscript/covscript.hpp>

int main(int argc, char *argv[]) {
    if (argc <= 1)
        return -1;
    cs::bootstrap env(argc - 1, argv + 1);
    env.run(argv[1]);
    cs::function_invoker<cs::number(cs::number, cs::number)> test(cs::eval(env.context, "test"));
    int result = test(1, 2);
    cs::function_invoker<void(cs::number)> println(cs::eval(env.context, "system.out.println"));
    println(result);
    return 0;
}