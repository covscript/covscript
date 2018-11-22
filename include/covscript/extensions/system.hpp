#pragma once
/*
* Covariant Script System Extension
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
*/
#include <covscript/console/conio.hpp>
#include <covscript/dirent/dirent.hpp>
#include <covscript/cni.hpp>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include <limits>

static cs::namespace_t system_ext=cs::make_shared_namespace<cs::name_space>();
static cs::namespace_t console_ext=cs::make_shared_namespace<cs::name_space>();
static cs::namespace_t file_ext=cs::make_shared_namespace<cs::name_space>();
static cs::namespace_t path_ext=cs::make_shared_namespace<cs::name_space>();
static cs::namespace_t path_type_ext=cs::make_shared_namespace<cs::name_space>();
static cs::namespace_t path_info_ext=cs::make_shared_namespace<cs::name_space>();
namespace console_cs_ext {
	using namespace cs;
	using namespace cs_impl;

	number terminal_width()
	{
		return conio::terminal_width();
	}

	number terminal_height()
	{
		return conio::terminal_height();
	}

	void gotoxy(number x, number y)
	{
		conio::gotoxy(x, y);
	}

	void echo(bool v)
	{
		conio::echo(v);
	}

	void clrscr()
	{
		conio::clrscr();
	}

	char getch()
	{
		return conio::getch();
	}

	bool kbhit()
	{
		return conio::kbhit();
	}

	void init()
	{
		(*console_ext)
		.add_var("terminal_width", make_cni(terminal_width))
		.add_var("terminal_height", make_cni(terminal_height))
		.add_var("gotoxy", make_cni(gotoxy))
		.add_var("echo", make_cni(echo))
		.add_var("clrscr", make_cni(clrscr))
		.add_var("getch", make_cni(getch))
		.add_var("kbhit", make_cni(kbhit));
	}
}

namespace file_cs_ext {
	using namespace cs;
	using namespace cs_impl;

	bool copy(const string &source, const string &dest)
	{
		std::ifstream in(source, std::ios_base::in | std::ios_base::binary);
		std::ofstream out(dest, std::ios_base::out | std::ios_base::binary);
		if (!in || !out)
			return false;
		char buffer[256];
		while (!in.eof()) {
			in.read(buffer, 256);
			out.write(buffer, in.gcount());
		}
		return true;
	}

	bool remove(const string &path)
	{
		return std::remove(path.c_str()) == 0;
	}

	bool exists(const string &path)
	{
		return std::ifstream(path).is_open();
	}

	bool rename(const string &source, const string &dest)
	{
		return std::rename(source.c_str(), dest.c_str()) == 0;
	}

	void init()
	{
		(*file_ext)
		.add_var("copy", make_cni(copy))
		.add_var("remove", make_cni(remove))
		.add_var("exists", make_cni(exists))
		.add_var("rename", make_cni(rename));
	}
}

namespace path_cs_ext {
	using namespace cs;
	using namespace cs_impl;

	struct path_info final {
		std::string name;
		int type;

		path_info() = delete;

		path_info(const char *n, int t) : name(n), type(t) {}
	};

	string name(const path_info &info)
	{
		return info.name;
	}

	int type(const path_info &info)
	{
		return info.type;
	}

	array scan(const string &path)
	{
		DIR *dir = ::opendir(path.c_str());
		array
		entries;
		for (dirent *dp = ::readdir(dir); dp != nullptr; dp = ::readdir(dir))
			entries.push_back(var::make<path_info>(dp->d_name, dp->d_type));
		::closedir(dir);
		return std::move(entries);
	}

	void init()
	{
		(*path_type_ext)
		.add_var("unknown", var::make_constant<int>(DT_UNKNOWN))
		.add_var("fifo", var::make_constant<int>(DT_FIFO))
		.add_var("sock", var::make_constant<int>(DT_SOCK))
		.add_var("chr", var::make_constant<int>(DT_CHR))
		.add_var("dir", var::make_constant<int>(DT_DIR))
		.add_var("blk", var::make_constant<int>(DT_BLK))
		.add_var("reg", var::make_constant<int>(DT_REG))
		.add_var("lnk", var::make_constant<int>(DT_LNK));
		(*path_info_ext)
		.add_var("name", make_cni(name))
		.add_var("type", make_cni(type));
		(*path_ext)
		.add_var("type", make_namespace(path_type_ext))
		.add_var("info", make_namespace(path_info_ext))
		.add_var("separator", var::make_constant<char>(path_separator))
		.add_var("delimiter", var::make_constant<char>(path_delimiter))
		.add_var("scan", make_cni(scan));
	}
}
namespace cs_impl {
	template<>
	cs::namespace_t &get_ext<path_cs_ext::path_info>()
	{
		return path_info_ext;
	}

	template<>
	constexpr const char *get_name_of_type<path_cs_ext::path_info>()
	{
		return "cs::system::path_info";
	}
}
namespace system_cs_ext {
	using namespace cs;

	number run(const string &str)
	{
		return std::system(str.c_str());
	}

	string getenv(const string &name)
	{
		const char *str = std::getenv(name.c_str());
		if (str == nullptr)
			throw lang_error("Environment variable \"" + name + "\" is not exist.");
		return str;
	}

	void exit(number code)
	{
		std::exit(code);
	}

	void init()
	{
		console_cs_ext::init();
		file_cs_ext::init();
		path_cs_ext::init();
		(*system_ext)
		.add_var("console", make_namespace(console_ext))
		.add_var("file", make_namespace(file_ext))
		.add_var("path", make_namespace(path_ext))
		.add_var("in", var::make_protect<istream>(&std::cin, [](std::istream *) {}))
		.add_var("out", var::make_protect<ostream>(&std::cout, [](std::ostream *) {}))
		.add_var("run", make_cni(run))
		.add_var("getenv", make_cni(getenv))
		.add_var("exit", make_cni(exit));
	}
}