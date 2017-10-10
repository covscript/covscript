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
* Copyright (C) 2017 Michael Lee(李登淳)
* Email: mikecovlee@163.com
* Github: https://github.com/mikecovlee
* Website: http://covariant.cn/cs
*/
#include "console/conio.hpp"
#include "headers/instance.hpp"
#include "extensions/iostream.hpp"
#include "extensions/system.hpp"
#include "extensions/runtime.hpp"
#include "extensions/exception.hpp"
#include "extensions/char.hpp"
#include "extensions/string.hpp"
#include "extensions/list.hpp"
#include "extensions/array.hpp"
#include "extensions/pair.hpp"
#include "extensions/hash_map.hpp"
#include "extensions/math.hpp"
#include "extensions/darwin.hpp"
#include "extensions/sqlite.hpp"
#include <iostream>
#include <cstring>
#include <cstdlib>

#include "instance.cpp"
#include "lexer.cpp"
#include "parser.cpp"
#include "runtime.cpp"
#include "statement.cpp"
#include "codegen.cpp"

namespace cs {
// Internal Functions
	number to_integer(const var &val)
	{
		return val.to_integer();
	}

	string to_string(const var &val)
	{
		return val.to_string();
	}

	string type(const var &a)
	{
		return a.get_type_name();
	}

	var clone(const var &val)
	{
		return copy(val);
	}

	void swap(var &a, var &b)
	{
		a.swap(b, true);
	}

	void init_ext()
	{
		// Init the extensions
		iostream_cs_ext::init();
		istream_cs_ext::init();
		ostream_cs_ext::init();
		system_cs_ext::init();
		runtime_cs_ext::init();
		except_cs_ext::init();
		char_cs_ext::init();
		string_cs_ext::init();
		list_cs_ext::init();
		array_cs_ext::init();
		pair_cs_ext::init();
		hash_map_cs_ext::init();
		math_cs_ext::init();
		darwin_cs_ext::init();
		sqlite_cs_ext::init();
	}

	void instance_type::init_runtime()
	{
		// Context
		storage.add_var("context", var::make_constant<context_t>(context));
		// Internal Types
		storage.add_type("char", []() -> var { return var::make<char>('\0'); }, cs_impl::hash<std::string>(typeid(char).name()), char_ext_shared);
		storage.add_type("number", []() -> var { return var::make<number>(0); }, cs_impl::hash<std::string>(typeid(number).name()));
		storage.add_type("boolean", []() -> var { return var::make<boolean>(true); }, cs_impl::hash<std::string>(typeid(boolean).name()));
		storage.add_type("pointer", []() -> var { return var::make<pointer>(null_pointer); }, cs_impl::hash<std::string>(typeid(pointer).name()));
		storage.add_type("string", []() -> var { return var::make<string>(); }, cs_impl::hash<std::string>(typeid(string).name()), string_ext_shared);
		storage.add_type("list", []() -> var { return var::make<list>(); }, cs_impl::hash<std::string>(typeid(list).name()), list_ext_shared);
		storage.add_type("array", []() -> var { return var::make<array>(); }, cs_impl::hash<std::string>(typeid(array).name()), array_ext_shared);
		storage.add_type("pair", []() -> var { return var::make<pair>(number(0), number(0)); }, cs_impl::hash<std::string>(typeid(pair).name()), pair_ext_shared);
		storage.add_type("hash_map", []() -> var { return var::make<hash_map>(); }, cs_impl::hash<std::string>(typeid(hash_map).name()), hash_map_ext_shared);
		// Add Internal Functions to storage
		storage.add_var_global("to_integer", cs::var::make_protect<cs::callable>(cs::cni(to_integer), true));
		storage.add_var_global("to_string", cs::var::make_protect<cs::callable>(cs::cni(to_string), true));
		storage.add_var_global("type", cs::var::make_protect<cs::callable>(cs::cni(type), true));
		storage.add_var_global("clone", cs::var::make_protect<cs::callable>(cs::cni(clone)));
		storage.add_var_global("swap", cs::var::make_protect<cs::callable>(cs::cni(swap)));
		// Add extensions to storage
		storage.add_var("exception", var::make_protect<extension_t>(except_ext_shared));
		storage.add_var("iostream", var::make_protect<extension_t>(make_shared_extension(iostream_ext)));
		storage.add_var("system", var::make_protect<extension_t>(make_shared_extension(system_ext)));
		storage.add_var("runtime", var::make_protect<extension_t>(make_shared_extension(runtime_ext)));
		storage.add_var("math", var::make_protect<extension_t>(make_shared_extension(math_ext)));
		storage.add_var("darwin", var::make_protect<extension_t>(make_shared_extension(darwin_ext)));
		storage.add_var("sqlite", var::make_protect<extension_t>(sqlite_ext_shared));
	}
}
