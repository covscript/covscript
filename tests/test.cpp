#include <covscript/covscript.hpp>
#include <iostream>

int main(int args, const char *argv[]) {
    cs::context_t context = cs::create_context(cs::parse_cmd_args(args, argv));
    cs::repl repl(context);
    std::string line;
    while (std::getline(std::cin, line))
        repl.exec(line);
    return 0;
}