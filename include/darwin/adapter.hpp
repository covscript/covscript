#pragma once
/*
* Covariant Darwin Universal Character Graphics Library
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
#include "./core.hpp"
#include <functional>

namespace darwin {
	class platform_adapter {
	public:
		platform_adapter() = default;

		platform_adapter(const platform_adapter &) = delete;

		virtual ~platform_adapter() = default;

		virtual status get_state() const =0;

		virtual results init()=0;

		virtual results stop()=0;

		virtual results exec_commands(commands)=0;

		virtual bool is_kb_hit()=0;

		virtual int get_kb_hit()=0;

		virtual results fit_drawable()=0;

		virtual drawable *get_drawable()=0;

		virtual results update_drawable()=0;
	};

	class module_adapter {
	public:
		module_adapter() = default;

		module_adapter(const module_adapter &) = delete;

		virtual ~module_adapter() = default;

		virtual status get_state() const =0;

		virtual results load_module(const std::string &)=0;

		virtual results free_module()=0;

		virtual platform_adapter *get_platform_adapter()=0;
	};

	typedef platform_adapter *(*module_enterance)();

	const char *module_enterance_name = "COV_DARWIN_MODULE_MAIN";
}