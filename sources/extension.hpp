#pragma once
#include "./core.hpp"
cs::extension* cs_extension();
extern "C"
{
	cs::extension* __CS_EXTENSION__()
	{
		return cs_extension();
	}
}
