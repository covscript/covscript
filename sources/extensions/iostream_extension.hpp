#pragma once
#include "../cni.hpp"
static cs::extension iostream_ext;
static cs::extension seekdir_ext;
static cs::extension openmode_ext;
static cs::extension istream_ext;
static cs::extension_t istream_ext_shared=std::make_shared<cs::extension_holder>(&istream_ext);
namespace cs_impl {
	template<>cs::extension_t& get_ext<cs::istream>()
	{
		return istream_ext_shared;
	}
}
static cs::extension ostream_ext;
static cs::extension_t ostream_ext_shared=std::make_shared<cs::extension_holder>(&ostream_ext);
namespace cs_impl {
	template<>cs::extension_t& get_ext<cs::ostream>()
	{
		return ostream_ext_shared;
	}
}
namespace iostream_cs_ext {
	using namespace cs;
	var fstream(const string& path,std::ios_base::openmode openmode)
	{
		switch(openmode) {
		case std::ios_base::in:
			return var::make<istream>(new std::ifstream(path,std::ios_base::in));
			break;
		case std::ios_base::out:
			return var::make<ostream>(new std::ofstream(path,std::ios_base::out));
			break;
		case std::ios_base::app:
			return var::make<ostream>(new std::ofstream(path,std::ios_base::app));
			break;
		default:
			throw lang_error("Unsupported openmode.");
		}
	}
	void setprecision(number pre)
	{
		output_precision=pre;
	}
	void init()
	{
		iostream_ext.add_var("istream",var::make_protect<extension_t>(istream_ext_shared));
		iostream_ext.add_var("ostream",var::make_protect<extension_t>(ostream_ext_shared));
		iostream_ext.add_var("seekdir",var::make_protect<extension_t>(std::make_shared<cs::extension_holder>(&seekdir_ext)));
		iostream_ext.add_var("openmode",var::make_protect<extension_t>(std::make_shared<cs::extension_holder>(&openmode_ext)));
		seekdir_ext.add_var("start",var::make_constant<std::ios_base::seekdir>(std::ios_base::beg));
		seekdir_ext.add_var("finish",var::make_constant<std::ios_base::seekdir>(std::ios_base::end));
		seekdir_ext.add_var("present",var::make_constant<std::ios_base::seekdir>(std::ios_base::cur));
		openmode_ext.add_var("in",var::make_constant<std::ios_base::openmode>(std::ios_base::in));
		openmode_ext.add_var("out",var::make_constant<std::ios_base::openmode>(std::ios_base::out));
		openmode_ext.add_var("app",var::make_constant<std::ios_base::openmode>(std::ios_base::app));
		iostream_ext.add_var("fstream",var::make_protect<native_interface>(cni(fstream)));
		iostream_ext.add_var("setprecision",var::make_protect<callable>(cni(setprecision)));
	}
}
namespace istream_cs_ext {
	using namespace cs;
// Input Stream
	char get(istream& in)
	{
		return in->get();
	}
	char peek(istream& in)
	{
		return in->peek();
	}
	void unget(istream& in)
	{
		in->unget();
	}
	string getline(istream& in)
	{
		std::string str;
		std::getline(*in,str);
		return std::move(str);
	}
	number tell(istream& in)
	{
		return in->tellg();
	}
	void seek(istream& in,number pos)
	{
		in->seekg(pos);
	}
	void seek_from(istream& in,std::ios_base::seekdir dir,number offset)
	{
		in->seekg(offset,dir);
	}
	bool good(const istream& in)
	{
		return static_cast<bool>(*in);
	}
	bool eof(const istream& in)
	{
		return in->eof();
	}
	var input(istream& in)
	{
		std::string str;
		*in>>str;
		return parse_value(str);
	}
	void init()
	{
		istream_ext.add_var("get",var::make_protect<callable>(cni(get)));
		istream_ext.add_var("peek",var::make_protect<callable>(cni(peek)));
		istream_ext.add_var("unget",var::make_protect<callable>(cni(unget)));
		istream_ext.add_var("getline",var::make_protect<callable>(cni(getline)));
		istream_ext.add_var("tell",var::make_protect<callable>(cni(tell)));
		istream_ext.add_var("seek",var::make_protect<callable>(cni(seek)));
		istream_ext.add_var("seek_from",var::make_protect<callable>(cni(seek_from)));
		istream_ext.add_var("good",var::make_protect<callable>(cni(good)));
		istream_ext.add_var("eof",var::make_protect<callable>(cni(eof)));
		istream_ext.add_var("input",var::make_protect<callable>(cni(input)));
	}
}
namespace ostream_cs_ext {
	using namespace cs;
// Output Stream
	void put(ostream& out,char c)
	{
		out->put(c);
	}
	number tell(ostream& out)
	{
		return out->tellp();
	}
	void seek(ostream& out,number pos)
	{
		out->seekp(pos);
	}
	void seek_from(ostream& out,std::ios_base::seekdir dir,number offset)
	{
		out->seekp(offset,dir);
	}
	void flush(ostream& out)
	{
		out->flush();
	}
	bool good(const ostream& out)
	{
		return static_cast<bool>(*out);
	}
	void print(ostream& out,const var& val)
	{
		*out<<val;
	}
	void println(ostream& out,const var& val)
	{
		*out<<val<<std::endl;
	}
	void init()
	{
		ostream_ext.add_var("put",var::make_protect<callable>(cni(put)));
		ostream_ext.add_var("tell",var::make_protect<callable>(cni(tell)));
		ostream_ext.add_var("seek",var::make_protect<callable>(cni(seek)));
		ostream_ext.add_var("seek_from",var::make_protect<callable>(cni(seek_from)));
		ostream_ext.add_var("flush",var::make_protect<callable>(cni(flush)));
		ostream_ext.add_var("good",var::make_protect<callable>(cni(good)));
		ostream_ext.add_var("print",var::make_protect<callable>(cni(print)));
		ostream_ext.add_var("println",var::make_protect<callable>(cni(println)));
	}
}
