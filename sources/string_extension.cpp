#ifndef CBS_STRING_EXT
#include "./extension.hpp"
#endif
#include "./arglist.hpp"
static cov_basic::extension string_ext;
namespace string_cbs_ext {
	using namespace cov_basic;
	cov::any append(array& args)
	{
		if(args.size()!=2)
			throw syntax_error("Wrong size of arguments.");
		if(args.at(0).type()!=typeid(string))
			throw syntax_error("Wrong type of arguments.(Request String)");
		args.at(0).val<string>(true).append(args.at(1).to_string());
		return number(0);
	}
	cov::any cut(array& args)
	{
		arglist::check<string,number>(args);
		for(std::size_t i=0; i<args.at(1).const_val<number>(); ++i)
			args.at(0).val<string>(true).pop_back();
		return number(0);
	}
	cov::any clear(array& args)
	{
		arglist::check<string>(args);
		args.at(0).val<string>(true).clear();
		return number(0);
	}
	void init()
	{
		string_ext.add_var("append",native_interface(append));
		string_ext.add_var("cut",native_interface(cut));
		string_ext.add_var("clear",native_interface(clear));
	}
}
#ifndef CBS_STRING_EXT
cov_basic::extension* cbs_extension()
{
	string_cbs_ext::init();
	return &string_ext;
}
#endif
