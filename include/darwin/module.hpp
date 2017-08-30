#pragma once

#include "./adapter.hpp"

namespace darwin {
	platform_adapter *module_resource();
}
extern "C"
{
	darwin::platform_adapter *COV_DARWIN_MODULE_MAIN()
	{
		return darwin::module_resource();
	}
}