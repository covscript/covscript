#pragma once
#include "../cni.hpp"
static cs::extension except_ext;
static cs::extension_t except_ext_shared=std::make_shared<cs::extension_holder>(&except_ext);
namespace cs_impl {
	template<>cs::extension_t& get_ext<cs::lang_error>()
	{
		return except_ext_shared;
	}
}
namespace except_cs_ext {
	using namespace cs;
	string what(const lang_error& le)
	{
		return le.what();
	}
	void init()
	{
		except_ext.add_var("what",var::make_protect<callable>(cni(what)));
	}
}
