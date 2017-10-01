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
		storage.add_var_global("clone", cs::var::make_protect<cs::callable>(cs::cni(clone)));
		storage.add_var_global("swap", cs::var::make_protect<cs::callable>(cs::cni(swap)));
		// Add extensions to storage
		storage.add_var("exception", var::make_protect<std::shared_ptr<extension_holder>>(except_ext_shared));
		storage.add_var("iostream", var::make_protect<std::shared_ptr<extension_holder>>(std::make_shared<extension_holder>(&iostream_ext)));
		storage.add_var("system", var::make_protect<std::shared_ptr<extension_holder>>(std::make_shared<extension_holder>(&system_ext)));
		storage.add_var("runtime", var::make_protect<std::shared_ptr<extension_holder>>(std::make_shared<extension_holder>(&runtime_ext)));
		storage.add_var("math", var::make_protect<std::shared_ptr<extension_holder>>(std::make_shared<extension_holder>(&math_ext)));
		storage.add_var("darwin", var::make_protect<std::shared_ptr<extension_holder>>(std::make_shared<extension_holder>(&darwin_ext)));
		storage.add_var("sqlite", var::make_protect<std::shared_ptr<extension_holder>>(sqlite_ext_shared));
	}
}

const char *env_name = "CS_IMPORT_PATH";
const char *log_path = nullptr;
bool compile_only = false;
bool wait_before_exit = false;

int covscript_args(int args_size, const char *args[])
{
	int expect_log_path = 0;
	int expect_import_path = 0;
	int index = 1;
	for (; index < args_size; ++index) {
		if (expect_log_path == 1) {
			log_path = args[index];
			expect_log_path = 2;
		}
		else if (expect_import_path == 1) {
			cs::import_path = args[index];
			expect_import_path = 2;
		}
		else if (args[index][0] == '-') {
			if (std::strcmp(args[index], "--compile-only") == 0 && !compile_only)
				compile_only = true;
			else if (std::strcmp(args[index], "--wait-before-exit") == 0 && !wait_before_exit)
				wait_before_exit = true;
			else if (std::strcmp(args[index], "--log-path") == 0 && expect_log_path == 0)
				expect_log_path = 1;
			else if (std::strcmp(args[index], "--import-path") == 0 && expect_import_path == 0)
				expect_import_path = 1;
			else
				throw cs::fatal_error("argument grammar error.");
		}
		else
			break;
	}
	if (expect_log_path == 1 || expect_import_path == 1)
		throw cs::fatal_error("argument grammar error.");
	return index;
}

void covscript_main(int args_size, const char *args[])
{
	if (args_size > 1) {
		const char* import_path=nullptr;
		if((import_path=std::getenv(env_name))!=nullptr)
			cs::import_path = import_path;
		int index = covscript_args(args_size, args);
		if (index == args_size)
			throw cs::fatal_error("no input file.");
		const char *path = args[index];
		cs::array arg;
		for (; index < args_size; ++index)
			arg.emplace_back(cs::var::make_constant<cs::string>(args[index]));
		system_ext.add_var("args", cs::var::make_constant<cs::array>(arg));
		cs::init_ext();
		cs::instance_type instance;
		instance.compile(path);
		if(!compile_only)
			instance.interpret();
	}
	else
		throw cs::fatal_error("no input file.");
}

int main(int args_size, const char *args[])
{
	int errorcode = 0;
	try {
		covscript_main(args_size, args);
	}
	catch (const std::exception &e) {
		if (log_path!=nullptr) {
			std::ofstream out(::log_path);
			if (out) {
				out << e.what();
				out.flush();
			}
			else
				std::cerr << "Write log failed." << std::endl;
		}
		std::cerr << e.what() << std::endl;
		errorcode = -1;
	}
	if(wait_before_exit) {
		std::cerr << "Program has been exited with a return value of " << errorcode << std::endl;
		std::cerr << "Press any key to exit..."<<std::endl;
		while(!cs_impl::conio::kbhit());
	}
	return errorcode;
}
