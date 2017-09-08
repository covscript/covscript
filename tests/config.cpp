#include "../sources/covscript.hpp"

int main()
{
	using namespace cs;
	init_grammar();
	auto runtime = covscript("./config.csc");
	const hash_map &config = runtime->storage.get_var("test_configure").const_val<hash_map>();
	std::cout << "File Path:" << config.at("path") << std::endl;
	std::cout << "File Info:" << std::endl;;
	std::cout << "Width:" << config.at("width") << std::endl;
	std::cout << "Height:" << config.at("height") << std::endl;
	std::cout << "Size:" << config.at("width").const_val<number>() * config.at("height").const_val<number>() << std::endl;
	std::cout << "Format:" << config.at("format") << std::endl;
	return 0;
}
