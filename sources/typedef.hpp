#pragma once
/*
* Covariant Script Typedef
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
* Copyright (C) 2017 Michael Lee(李登淳)
* Email: mikecovlee@163.com
* Github: https://github.com/mikecovlee
*/

namespace cs_impl {
	class any;
}
namespace cs {
	class token_base;
	class statement_base;
	class callable;
	class name_space;
	class name_space_holder;
	class runtime_type;
	using var=cs_impl::any;
	using number=long double;
	using boolean=bool;
	using string=std::string;
	using list=std::list<var>;
	using array=std::deque<var>;
	using pair=std::pair<var,var>;
	using hash_map=std::unordered_map<var,var>;
	using native_interface=callable;
	using extension=name_space;
	using extension_holder=name_space_holder;
	using name_space_t=std::shared_ptr<name_space_holder>;
	using extension_t=std::shared_ptr<extension_holder>;
	using runtime_t=std::shared_ptr<runtime_type>;
	using istream=std::shared_ptr<std::istream>;
	using ostream=std::shared_ptr<std::ostream>;
}
