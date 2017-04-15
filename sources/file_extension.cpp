#ifndef CBS_FILE_EXT
#include "./extension.hpp"
#endif
#include "./arglist.hpp"
static cov_basic::extension file_ext;
namespace file_cbs_ext {
	using namespace cov_basic;
	using infile=std::shared_ptr<std::ifstream>;
	using outfile=std::shared_ptr<std::ofstream>;
	enum class file_method {
		read,write
	};
	cov::any open(array& args)
	{
		arglist::check<string,file_method>(args);
		if(args.at(1).const_val<file_method>()==file_method::read)
			return infile(std::make_shared<std::ifstream>(args.at(0).const_val<string>()));
		else if(args.at(1).const_val<file_method>()==file_method::write)
			return outfile(std::make_shared<std::ofstream>(args.at(0).const_val<string>()));
	}
	cov::any is_open(array& args)
	{
		arglist::check<infile>(args);
		return args.at(0).const_val<infile>()->is_open();
	}
	cov::any eof(array& args)
	{
		arglist::check<infile>(args);
		return args.at(0).const_val<infile>()->eof();
	}
	cov::any read(array& args)
	{
		if(args.empty())
			throw syntax_error("Wrong size of arguments.");
		if(args.at(0).type()!=typeid(infile))
			throw syntax_error("Wrong type of arguments.(Request Input File)");
		std::ifstream& infs=*args.at(0).val<infile>(true);
		if(args.size()==1) {
			std::string str;
			infs>>str;
			return parse_value(str);
		} else {
			std::string str;
			for(std::size_t i=1; i<args.size(); ++i) {
				infs>>str;
				args.at(i).assign(parse_value(str),true);
			}
		}
		return number(0);
	}
	cov::any write(array& args)
	{
		if(args.empty())
			throw syntax_error("Wrong size of arguments.");
		if(args.at(0).type()!=typeid(outfile))
			throw syntax_error("Wrong type of arguments.(Request Output File)");
		std::ofstream& outfs=*args.at(0).val<outfile>(true);
		for(std::size_t i=1; i<args.size(); ++i)
			outfs<<args.at(i);
		return number(0);
	}
	cov::any getline(array& args)
	{
		arglist::check<infile>(args);
		std::string str;
		std::getline(*args.front().val<infile>(true),str);
		return str;
	}
	void init()
	{
		file_ext.add_var("read_method",file_method::read);
		file_ext.add_var("write_method",file_method::write);
		file_ext.add_var("open",native_interface(open));
		file_ext.add_var("is_open",native_interface(is_open));
		file_ext.add_var("eof",native_interface(eof));
		file_ext.add_var("read",native_interface(read));
		file_ext.add_var("write",native_interface(write));
		file_ext.add_var("getline",native_interface(getline));
	}
}
#ifndef CBS_FILE_EXT
cov_basic::extension* cbs_extension()
{
	file_cbs_ext::init();
	return &file_ext;
}
#endif
