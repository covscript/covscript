#include <covscript/covscript.hpp>

int main(int argc, char *argv[]) {
    if (argc <= 1)
        return -1;
    cs::bootstrap env(argc - 1, argv + 1);
    env.run(argv[1]);
    cs::function_invoker<cs::numeric(cs::numeric, cs::numeric)> test(cs::eval(env.context, "test"));
    int result = test(1, 2).as_integer();
    cs::function_invoker<void(cs::numeric)> println(cs::eval(env.context, "system.out.println"));
    println(result);
    return 0;
}