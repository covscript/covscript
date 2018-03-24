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
#include <covscript/covscript.hpp>
#include <covscript/console/conio.hpp>
#include <covscript/extensions/iostream.hpp>
#include <covscript/extensions/system.hpp>
#include <covscript/extensions/runtime.hpp>
#include <covscript/extensions/exception.hpp>
#include <covscript/extensions/char.hpp>
#include <covscript/extensions/string.hpp>
#include <covscript/extensions/list.hpp>
#include <covscript/extensions/array.hpp>
#include <covscript/extensions/pair.hpp>
#include <covscript/extensions/hash_map.hpp>
#include <covscript/extensions/math.hpp>
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

	var move(const var &val)
	{
		return rvalue(val);
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
	}

	void init(int argv, char** args)
	{
		cs::array arg;
		for (std::size_t i; i < argv; ++i)
			arg.emplace_back(cs::var::make_constant<cs::string>(args[i]));
		system_ext.add_var("args", cs::var::make_constant<cs::array>(arg));
	    cs::init_ext();
    }

	void instance_type::init_runtime()
	{
		// Context
		storage.add_buildin_var("context", var::make_constant<context_t>(context));
		// Internal Types
		storage.add_buildin_type("char", []() -> var { return var::make<char>('\0'); },
		                         cs_impl::hash<std::string>(typeid(char).name()), char_ext_shared);
		storage.add_buildin_type("number", []() -> var { return var::make<number>(0); },
		                         cs_impl::hash<std::string>(typeid(number).name()));
		storage.add_buildin_type("boolean", []() -> var { return var::make<boolean>(true); },
		                         cs_impl::hash<std::string>(typeid(boolean).name()));
		storage.add_buildin_type("pointer", []() -> var { return var::make<pointer>(null_pointer); },
		                         cs_impl::hash<std::string>(typeid(pointer).name()));
		storage.add_buildin_type("string", []() -> var { return var::make<string>(); },
		                         cs_impl::hash<std::string>(typeid(string).name()), string_ext_shared);
		storage.add_buildin_type("list", []() -> var { return var::make<list>(); },
		                         cs_impl::hash<std::string>(typeid(list).name()), list_ext_shared);
		storage.add_buildin_type("array", []() -> var { return var::make<array>(); },
		                         cs_impl::hash<std::string>(typeid(array).name()), array_ext_shared);
		storage.add_buildin_type("pair", []() -> var { return var::make<pair>(number(0), number(0)); },
		                         cs_impl::hash<std::string>(typeid(pair).name()), pair_ext_shared);
		storage.add_buildin_type("hash_map", []() -> var { return var::make<hash_map>(); },
		                         cs_impl::hash<std::string>(typeid(hash_map).name()), hash_map_ext_shared);
		// Add Internal Functions to storage
		storage.add_buildin_var("to_integer", cs::var::make_protect<cs::callable>(cs::cni(to_integer), true));
		storage.add_buildin_var("to_string", cs::var::make_protect<cs::callable>(cs::cni(to_string), true));
		storage.add_buildin_var("type", cs::var::make_protect<cs::callable>(cs::cni(type), true));
		storage.add_buildin_var("clone", cs::var::make_protect<cs::callable>(cs::cni(clone)));
		storage.add_buildin_var("move", cs::var::make_protect<cs::callable>(cs::cni(move)));
		storage.add_buildin_var("swap", cs::var::make_protect<cs::callable>(cs::cni(swap), true));
		// Add extensions to storage
		storage.add_buildin_var("exception", var::make_protect<extension_t>(except_ext_shared));
		storage.add_buildin_var("iostream", var::make_protect<extension_t>(make_shared_extension(iostream_ext)));
		storage.add_buildin_var("system", var::make_protect<extension_t>(make_shared_extension(system_ext)));
		storage.add_buildin_var("runtime", var::make_protect<extension_t>(make_shared_extension(runtime_ext)));
		storage.add_buildin_var("math", var::make_protect<extension_t>(make_shared_extension(math_ext)));
	}
}

#ifdef COVSCRIPT_PLATFORM_WIN32

#include <shlobj.h>

#pragma comment(lib, "shell32.lib")

std::string get_sdk_path()
{
#ifdef COVSCRIPT_HOME
	return COVSCRIPT_HOME;
#else
	CHAR path[MAX_PATH];
	HRESULT result = SHGetFolderPathA(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, path);
	return std::strcat(path, "\\CovScript");
#endif
}

#else
std::string get_sdk_path()
{
#ifdef COVSCRIPT_HOME
	return COVSCRIPT_HOME;
#else
	return "/usr/share/covscript";
#endif
}
#endif

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