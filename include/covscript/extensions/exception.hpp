#pragma once
/*
* Covariant Script Exception Extension
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Affero General Public License as published
* by the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Affero General Public License for more details.
*
* You should have received a copy of the GNU Affero General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
* Copyright (C) 2017 Michael Lee(李登淳)
* Email: mikecovlee@163.com
* Github: https://github.com/mikecovlee
*/
#include <covscript/cni.hpp>

static cs::extension except_ext;
static cs::extension_t except_ext_shared = cs::make_shared_extension(except_ext);
namespace cs_impl {
	template<>
	cs::extension_t &get_ext<cs::lang_error>()
	{
		return except_ext_shared;
	}
}
namespace except_cs_ext {
	using namespace cs;

	string what(const lang_error &le)
	{
		return le.what();
	}

	void init()
	{
		except_ext.add_var("what", var::make_protect<callable>(cni(what)));
	}
}
