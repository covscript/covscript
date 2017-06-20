#pragma once
#include "./core.hpp"
cov_basic::extension* cbs_extension();
extern "C"
{
	cov_basic::extension* __CBS_EXTENSION__()
	{
		return cbs_extension();
	}
}
