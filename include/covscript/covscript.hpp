#pragma once
/*
* Covariant Script Programming Language
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
* Copyright (C) 2018 Michael Lee(李登淳)
* Email: mikecovlee@163.com
* Github: https://github.com/mikecovlee
* Website: http://covariant.cn/cs
*/
#include <covscript/instance.hpp>

namespace cs {
	void init(const array &);

	void init(int, const char *[]);

	std::string get_sdk_path();

	std::string process_path(const std::string &raw)
	{
		auto pos0 = raw.find('\"');
		auto pos1 = raw.rfind('\"');
		if (pos0 != std::string::npos) {
			if (pos0 == pos1)
				throw cs::fatal_error("argument syntax error.");
			else
				return raw.substr(pos0 + 1, pos1 - pos0 - 1);
		}
		else
			return raw;
	}

	std::string get_import_path()
	{
		const char *import_path = std::getenv("CS_IMPORT_PATH");
		if (import_path != nullptr)
			return process_path(import_path);
		else
			return process_path(get_sdk_path() + cs::path_separator + "imports");
	}
}