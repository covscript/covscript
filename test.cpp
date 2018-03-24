#include <covscript/covscript.hpp>
#include <iostream>
#include <string>
int main(int argv,char** args)
{
    cs::init(argv,args);
	cs::instance_type instance;
	instance.context->file_path = "<TEST_ENV>";
	cs::repl repl(instance.context);
    std::string line;
    while(std::getline(std::cin,line))
        repl.exec(line);
    return 0;
}