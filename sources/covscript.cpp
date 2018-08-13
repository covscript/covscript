/*
* Covariant Script Programming Language
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
* Copyright (C) 2018 Michael Lee(李登淳)
* Email: mikecovlee@163.com
* Github: https://github.com/mikecovlee
* Website: http://covscript.org
*/
#include <covscript/covscript.hpp>
#include <covscript/console/conio.hpp>
#include <covscript/extensions/extensions.hpp>

#ifdef COVSCRIPT_PLATFORM_WIN32

#include <shlobj.h>

#pragma comment(lib, "shell32.lib")
#endif

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

	void init(const array &args)
	{
		// Init extensions
		init_extensions();
		system_ext.add_var("args", cs::var::make_constant<cs::array>(args));
	}

	void init(int argv, const char *args[])
	{
		// Init args
		cs::array
		arg;
		for (std::size_t i = 0; i < argv; ++i)
			arg.emplace_back(cs::var::make_constant<cs::string>(args[i]));
		init(arg);
	}

#ifdef COVSCRIPT_PLATFORM_WIN32

	std::string get_sdk_path()
	{
#ifdef COVSCRIPT_HOME
		return COVSCRIPT_HOME;
#else
		CHAR path[MAX_PATH];
		SHGetFolderPathA(nullptr, CSIDL_PERSONAL, nullptr, SHGFP_TYPE_CURRENT, path);
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

	void instance_type::init_runtime()
	{
		storage
		// Internal Types
		.add_buildin_type("char", []() -> var { return var::make<char>('\0'); },cs_impl::type_id::get_id<char>(), char_ext_shared)
		.add_buildin_type("number", []() -> var { return var::make<number>(0); },cs_impl::type_id::get_id<number>())
		.add_buildin_type("boolean", []() -> var { return var::make<boolean>(true); },cs_impl::type_id::get_id<boolean>())
		.add_buildin_type("pointer", []() -> var { return var::make<pointer>(null_pointer); },cs_impl::type_id::get_id<pointer>())
		.add_buildin_type("string", []() -> var { return var::make<string>(); },cs_impl::type_id::get_id<string>(), string_ext_shared)
		.add_buildin_type("list", []() -> var { return var::make<list>(); },cs_impl::type_id::get_id<list>(), list_ext_shared)
		.add_buildin_type("array", []() -> var { return var::make<array>(); },cs_impl::type_id::get_id<array>(), array_ext_shared)
		.add_buildin_type("pair", []() -> var { return var::make<pair>(number(0), number(0)); },cs_impl::type_id::get_id<pair>(), pair_ext_shared)
		.add_buildin_type("hash_map", []() -> var { return var::make<hash_map>(); },cs_impl::type_id::get_id<hash_map>(), hash_map_ext_shared)
		// Context
		.add_buildin_var("context", var::make_constant<context_t>(context))
		// Add Internal Functions to storage
		.add_buildin_var("to_integer", make_cni(to_integer, true))
		.add_buildin_var("to_string", make_cni(to_string, true))
		.add_buildin_var("type", make_cni(type, true))
		.add_buildin_var("clone", make_cni(clone))
		.add_buildin_var("move", make_cni(move))
		.add_buildin_var("swap", make_cni(swap, true))
		// Add extensions to storage
		.add_buildin_var("exception", make_namespace(except_ext_shared))
		.add_buildin_var("iostream", make_namespace(make_shared_namespace(iostream_ext)))
		.add_buildin_var("system", make_namespace(make_shared_namespace(system_ext)))
		.add_buildin_var("runtime", make_namespace(make_shared_namespace(runtime_ext)))
		.add_buildin_var("math", make_namespace(make_shared_namespace(math_ext)));
	}
}