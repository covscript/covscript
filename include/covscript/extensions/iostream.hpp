#pragma once
/*
* Covariant Script Iostream Extension
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
* Copyright (C) 2018 Michael Lee(李登淳)
* Email: mikecovlee@163.com
* Github: https://github.com/mikecovlee
*/
#include <covscript/cni.hpp>

static cs::name_space iostream_ext;
static cs::name_space seekdir_ext;
static cs::name_space openmode_ext;
static cs::name_space istream_ext;
static cs::name_space ostream_ext;
static cs::namespace_t istream_ext_shared = cs::make_shared_namespace(istream_ext);
static cs::namespace_t ostream_ext_shared = cs::make_shared_namespace(ostream_ext);
namespace cs_impl {
	template<>
	cs::namespace_t &get_ext<cs::istream>()
	{
		return istream_ext_shared;
	}

	template<>
	cs::namespace_t &get_ext<cs::ostream>()
	{
		return ostream_ext_shared;
	}

#ifndef _MSC_VER

	template<>
	constexpr const char *get_name_of_type<std::ios_base::seekdir>()
	{
		return "cs::iostream::seekdir";
	}

	template<>
	constexpr const char *get_name_of_type<std::ios_base::openmode>()
	{
		return "cs::iostream::openmode";
	}

#endif
}
namespace iostream_cs_ext {
	using namespace cs;

	var parse_value(const std::string &str)
	{
		if (str == "true")
			return true;
		if (str == "false")
			return false;
		try {
			return parse_number(str);
		}
		catch (...) {
			return str;
		}
		return str;
	}

	var fstream(const string &path, std::ios_base::openmode openmode)
	{
		switch (openmode) {
		case std::ios_base::in:
			return var::make<istream>(new std::ifstream(path, std::ios_base::in));
			break;
		case std::ios_base::out:
			return var::make<ostream>(new std::ofstream(path, std::ios_base::out));
			break;
		case std::ios_base::app:
			return var::make<ostream>(new std::ofstream(path, std::ios_base::app));
			break;
		default:
			throw lang_error("Unsupported openmode.");
		}
	}

	void setprecision(number pre)
	{
		output_precision = pre;
	}

	void init()
	{
		iostream_ext
		.add_var("istream", make_namespace(istream_ext_shared))
		.add_var("ostream", make_namespace(ostream_ext_shared))
		.add_var("seekdir", make_namespace(cs::make_shared_namespace(seekdir_ext)))
		.add_var("openmode", make_namespace(cs::make_shared_namespace(openmode_ext)));
		seekdir_ext
		.add_var("start", var::make_constant<std::ios_base::seekdir>(std::ios_base::beg))
		.add_var("finish", var::make_constant<std::ios_base::seekdir>(std::ios_base::end))
		.add_var("present", var::make_constant<std::ios_base::seekdir>(std::ios_base::cur));
		openmode_ext
		.add_var("in", var::make_constant<std::ios_base::openmode>(std::ios_base::in))
		.add_var("out", var::make_constant<std::ios_base::openmode>(std::ios_base::out))
		.add_var("app", var::make_constant<std::ios_base::openmode>(std::ios_base::app));
		iostream_ext
		.add_var("fstream", make_cni(fstream))
		.add_var("setprecision", make_cni(setprecision));
	}
}
namespace istream_cs_ext {
	using namespace cs;

// Input Stream
	char get(istream &in)
	{
		return in->get();
	}

	char peek(istream &in)
	{
		return in->peek();
	}

	void unget(istream &in)
	{
		in->unget();
	}

	string getline(istream &in)
	{
		std::string str;
		std::getline(*in, str);
		return std::move(str);
	}

	number tell(istream &in)
	{
		return in->tellg();
	}

	void seek(istream &in, number pos)
	{
		in->seekg(pos);
	}

	void seek_from(istream &in, std::ios_base::seekdir dir, number offset)
	{
		in->seekg(offset, dir);
	}

	bool good(const istream &in)
	{
		return static_cast<bool>(*in);
	}

	bool eof(const istream &in)
	{
		return in->eof();
	}

	var input(istream &in)
	{
		std::string str;
		*in >> str;
		return parse_value(str);
	}

	void init()
	{
		istream_ext
		.add_var("get", make_cni(get))
		.add_var("peek", make_cni(peek))
		.add_var("unget", make_cni(unget))
		.add_var("getline", make_cni(getline))
		.add_var("tell", make_cni(tell))
		.add_var("seek", make_cni(seek))
		.add_var("seek_from", make_cni(seek_from))
		.add_var("good", make_cni(good))
		.add_var("eof", make_cni(eof))
		.add_var("input", make_cni(input));
	}
}
namespace ostream_cs_ext {
	using namespace cs;

// Output Stream
	void put(ostream &out, char c)
	{
		out->put(c);
	}

	number tell(ostream &out)
	{
		return out->tellp();
	}

	void seek(ostream &out, number pos)
	{
		out->seekp(pos);
	}

	void seek_from(ostream &out, std::ios_base::seekdir dir, number offset)
	{
		out->seekp(offset, dir);
	}

	void flush(ostream &out)
	{
		out->flush();
	}

	bool good(const ostream &out)
	{
		return static_cast<bool>(*out);
	}

	void print(ostream &out, const var &val)
	{
		*out << val << std::flush;
	}

	void println(ostream &out, const var &val)
	{
		*out << val << std::endl;
	}

	void init()
	{
		ostream_ext
		.add_var("put", make_cni(put))
		.add_var("tell", make_cni(tell))
		.add_var("seek", make_cni(seek))
		.add_var("seek_from", make_cni(seek_from))
		.add_var("flush", make_cni(flush))
		.add_var("good", make_cni(good))
		.add_var("print", make_cni(print))
		.add_var("println", make_cni(println));
	}
}
