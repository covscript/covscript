#pragma once
#include "../cni.hpp"
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
	string read(istream& in,number n)
	{
		long _n=n;
		char buff[_n+1];
		in->read(buff,n);
		buff[_n]='\0';
		return buff;
	}
	number tell(istream& in)
	{
		return in->tellg();
	}
	void seek(istream& in,number pos)
	{
		in->seekg(pos);
	}
	boolean good(const istream& in)
	{
		return in->operator bool();
	}
	boolean eof(const istream& in)
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
		istream_ext.add_var("read",var::make_protect<callable>(cni(read)));
		istream_ext.add_var("tell",var::make_protect<callable>(cni(tell)));
		istream_ext.add_var("seek",var::make_protect<callable>(cni(seek)));
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
	void write(ostream& out,const string& str)
	{
		out->write(str.c_str(),str.size());
	}
	number tell(ostream& out)
	{
		return out->tellp();
	}
	void seek(ostream& out,number pos)
	{
		out->seekp(pos);
	}
	void flush(ostream& out)
	{
		out->flush();
	}
	boolean good(const ostream& out)
	{
		return out->operator bool();
	}
	void print(ostream& out,const string& str)
	{
		*out<<str;
	}
	void println(ostream& out,const string& str)
	{
		*out<<str<<std::endl;
	}
	void init()
	{
		ostream_ext.add_var("put",var::make_protect<callable>(cni(put)));
		ostream_ext.add_var("write",var::make_protect<callable>(cni(write)));
		ostream_ext.add_var("tell",var::make_protect<callable>(cni(tell)));
		ostream_ext.add_var("seek",var::make_protect<callable>(cni(seek)));
		ostream_ext.add_var("flush",var::make_protect<callable>(cni(flush)));
		ostream_ext.add_var("good",var::make_protect<callable>(cni(good)));
		ostream_ext.add_var("print",var::make_protect<callable>(cni(print)));
		ostream_ext.add_var("println",var::make_protect<callable>(cni(println)));
	}
}
