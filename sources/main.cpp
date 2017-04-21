#define CBS_STRING_EXT
#define CBS_ARRAY_EXT
#define CBS_MATH_EXT
#define CBS_FILE_EXT
#define CBS_DARWIN_EXT
#include "./covbasic.hpp"
int main(int args_size,const char* args[])
{
	if(args_size>1) {
		cov_basic::reset();
		cov_basic::cov_basic(args[1]);
	} else
		throw cov_basic::fatal_error("no input file.\nUsage: cbs2 <file>");
	return 0;
}
