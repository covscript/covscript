#ifndef CBS_ARRAY_EXT
#include "../extension.hpp"
#endif
#include "../arglist.hpp"
static cov_basic::extension array_ext;
namespace array_cbs_ext {
	using namespace cov_basic;
	cov::any push_front(array& args)
	{
		if(args.size()!=2)
			throw syntax_error("Wrong size of arguments.");
		if(args.at(0).type()!=typeid(array))
			throw syntax_error("Wrong type of arguments.(Request Array)");
		args.at(0).val<array>(true).push_front(copy(args.at(1)));
		return number(0);
	}
	cov::any pop_front(array& args)
	{
		if(args.size()!=1)
			throw syntax_error("Wrong size of arguments.");
		if(args.at(0).type()!=typeid(array))
			throw syntax_error("Wrong type of arguments.(Request Array)");
		args.at(0).val<array>(true).pop_front();
		return number(0);
	}
	cov::any push_back(array& args)
	{
		if(args.size()!=2)
			throw syntax_error("Wrong size of arguments.");
		if(args.at(0).type()!=typeid(array))
			throw syntax_error("Wrong type of arguments.(Request Array)");
		args.at(0).val<array>(true).push_back(copy(args.at(1)));
		return number(0);
	}
	cov::any pop_back(array& args)
	{
		if(args.size()!=1)
			throw syntax_error("Wrong size of arguments.");
		if(args.at(0).type()!=typeid(array))
			throw syntax_error("Wrong type of arguments.(Request Array)");
		args.at(0).val<array>(true).pop_back();
		return number(0);
	}
	cov::any clear(array& args)
	{
		if(args.size()!=1)
			throw syntax_error("Wrong size of arguments.");
		if(args.at(0).type()!=typeid(array))
			throw syntax_error("Wrong type of arguments.(Request Array)");
		args.at(0).val<array>(true).clear();
		return number(0);
	}
	cov::any at(array& args)
	{
		arglist::check<array,number>(args);
		return args.at(0).val<array>(true).at(args.at(1).const_val<number>());
	}
	void init()
	{
		array_ext.add_var("push_front",native_interface(push_front));
		array_ext.add_var("pop_front",native_interface(pop_front));
		array_ext.add_var("push_back",native_interface(push_back));
		array_ext.add_var("pop_back",native_interface(pop_back));
		array_ext.add_var("clear",native_interface(clear));
		array_ext.add_var("at",native_interface(at));
	}
}
#ifndef CBS_ARRAY_EXT
cov_basic::extension* cbs_extension()
{
	array_cbs_ext::init();
	return &array_ext;
}
#endif
