#define DARWIN_DISABLE_LOG
#define CBS_STRING_EXT
#define CBS_ARRAY_EXT
#define CBS_HASH_MAP_EXT
#define CBS_MATH_EXT
#define CBS_FILE_EXT
#define CBS_DARWIN_EXT
#include "./covbasic.hpp"
int main(int args_size,const char* args[])
{
	if(args_size>1) {
		cov_basic::array arg;
		for(int i=1; i<args_size; ++i)
			arg.push_back(std::string(args[i]));
		system_ext.add_var("args",arg);
		cov_basic::reset();
		cov_basic::cov_basic(args[1]);
	} else
		throw cov_basic::fatal_error("no input file.\nUsage: cbs2.1 <file> <args...>");
	return 0;
}
