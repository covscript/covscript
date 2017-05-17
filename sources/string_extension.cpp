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
		return args.at(0);
	}
	cov::any insert(array& args)
	{
		if(args.size()!=3)
			throw syntax_error("Wrong size of arguments.");
		if(args.at(0).type()!=typeid(string)||args.at(1).type()!=typeid(number))
			throw syntax_error("Wrong type of arguments.(Request String and Number)");
		args.at(0).val<string>(true).insert(args.at(1).const_val<number>(),args.at(2).to_string());
		return args.at(0);
	}
	cov::any erase(array& args)
	{
		if(args.size()!=3)
			throw syntax_error("Wrong size of arguments.");
		if(args.at(0).type()!=typeid(string)||args.at(1).type()!=typeid(number)||args.at(2).type()!=typeid(number))
			throw syntax_error("Wrong type of arguments.(Request String and Number)");
		args.at(0).val<string>(true).erase(args.at(1).const_val<number>(),args.at(2).const_val<number>());
		return args.at(0);
	}
	cov::any replace(array& args)
	{
		if(args.size()!=4)
			throw syntax_error("Wrong size of arguments.");
		if(args.at(0).type()!=typeid(string)||args.at(1).type()!=typeid(number)||args.at(2).type()!=typeid(number))
			throw syntax_error("Wrong type of arguments.(Request String and Number)");
		args.at(0).val<string>(true).replace(args.at(1).const_val<number>(),args.at(2).const_val<number>(),args.at(3).to_string());
		return args.at(0);
	}
	cov::any substr(array& args)
	{
		if(args.size()!=3)
			throw syntax_error("Wrong size of arguments.");
		if(args.at(0).type()!=typeid(string)||args.at(1).type()!=typeid(number)||args.at(2).type()!=typeid(number))
			throw syntax_error("Wrong type of arguments.(Request String and Number)");
		return args.at(0).val<string>(true).substr(args.at(1).const_val<number>(),args.at(2).const_val<number>());
	}
	cov::any find(array& args)
	{
		if(args.size()!=3)
			throw syntax_error("Wrong size of arguments.");
		if(args.at(0).type()!=typeid(string)||args.at(2).type()!=typeid(number))
			throw syntax_error("Wrong type of arguments.(Request String and Number)");
		auto pos=args.at(0).val<string>(true).find(args.at(1).to_string(),args.at(2).const_val<number>());
		if(pos==std::string::npos)
			return number(-1);
		else
			return number(pos);
	}
	cov::any rfind(array& args)
	{
		if(args.size()!=3)
			throw syntax_error("Wrong size of arguments.");
		if(args.at(0).type()!=typeid(string)||args.at(2).type()!=typeid(number))
			throw syntax_error("Wrong type of arguments.(Request String and Number)");
		auto pos=args.at(0).val<string>(true).rfind(args.at(1).to_string(),args.at(2).const_val<number>());
		if(pos==std::string::npos)
			return number(-1);
		else
			return number(pos);
	}
	cov::any cut(array& args)
	{
		arglist::check<string,number>(args);
		for(std::size_t i=0; i<args.at(1).const_val<number>(); ++i)
			args.at(0).val<string>(true).pop_back();
		return args.at(0);
	}
	cov::any clear(array& args)
	{
		arglist::check<string>(args);
		args.at(0).val<string>(true).clear();
		return number(0);
	}
	void init()
	{
		string_ext.add_var("npos",number(-1));
		string_ext.add_var("append",native_interface(append));
		string_ext.add_var("insert",native_interface(insert));
		string_ext.add_var("erase",native_interface(erase));
		string_ext.add_var("replace",native_interface(replace));
		string_ext.add_var("substr",native_interface(substr));
		string_ext.add_var("find",native_interface(find));
		string_ext.add_var("rfind",native_interface(rfind));
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
