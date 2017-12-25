#pragma once
/*
* Covariant Script Extension Header
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
#include <covscript/core.hpp>

cs::extension *cs_extension();

extern "C"
{
	cs::extension *__CS_EXTENSION__(cs::cs_exception_handler ceh,cs::std_exception_handler seh)
	{
		cs::exception_handler::cs_eh_callback=ceh;
		cs::exception_handler::std_eh_callback=seh;
		return cs_extension();
	}
}
