#pragma once
#include "./char_extension.hpp"
#include "./string_extension.hpp"
#include "./list_extension.hpp"
#include "./array_extension.hpp"
#include "./pair_extension.hpp"
#include "./hash_map_extension.hpp"
static cs::extension types_ext;
namespace types_cs_ext {
	using namespace cs;
	void init()
	{
		types_ext.add_var("char",cov::any::make_protect<std::shared_ptr<extension_holder>>(std::make_shared<extension_holder>(&char_ext)));
		types_ext.add_var("string",cov::any::make_protect<std::shared_ptr<extension_holder>>(std::make_shared<extension_holder>(&string_ext)));
		types_ext.add_var("list",cov::any::make_protect<std::shared_ptr<extension_holder>>(std::make_shared<extension_holder>(&list_ext)));
		types_ext.add_var("array",cov::any::make_protect<std::shared_ptr<extension_holder>>(std::make_shared<extension_holder>(&array_ext)));
		types_ext.add_var("pair",cov::any::make_protect<std::shared_ptr<extension_holder>>(std::make_shared<extension_holder>(&pair_ext)));
		types_ext.add_var("hash_map",cov::any::make_protect<std::shared_ptr<extension_holder>>(std::make_shared<extension_holder>(&hash_map_ext)));
	}
}
