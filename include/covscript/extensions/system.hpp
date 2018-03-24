#pragma once
/*
* Covariant Script System Extension
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
*/
#include <covscript/console/conio.hpp>
#include <covscript/dirent/dirent.h>
#include <covscript/cni.hpp>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include <limits>

static cs::extension system_ext;
static cs::extension console_ext;
static cs::extension file_ext;
static cs::extension path_ext;
static cs::extension path_type_ext;
static cs::extension path_info_ext;
static cs::extension_t console_ext_shared = cs::make_shared_extension(console_ext);
static cs::extension_t file_ext_shared = cs::make_shared_extension(file_ext);
static cs::extension_t path_ext_shared = cs::make_shared_extension(path_ext);
static cs::extension_t path_type_ext_shared = cs::make_shared_extension(path_type_ext);
static cs::extension_t path_info_ext_shared = cs::make_shared_extension(path_info_ext);
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
		console_ext.add_var("terminal_width", var::make_protect<callable>(cni(terminal_width)));
		console_ext.add_var("terminal_height", var::make_protect<callable>(cni(terminal_height)));
		console_ext.add_var("gotoxy", var::make_protect<callable>(cni(gotoxy)));
		console_ext.add_var("echo", var::make_protect<callable>(cni(echo)));
		console_ext.add_var("clrscr", var::make_protect<callable>(cni(clrscr)));
		console_ext.add_var("getch", var::make_protect<callable>(cni(getch)));
		console_ext.add_var("kbhit", var::make_protect<callable>(cni(kbhit)));
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
		file_ext.add_var("copy", var::make_protect<callable>(cni(copy)));
		file_ext.add_var("remove", var::make_protect<callable>(cni(remove)));
		file_ext.add_var("exists", var::make_protect<callable>(cni(exists)));
		file_ext.add_var("rename", var::make_protect<callable>(cni(rename)));
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
		array entries;
		for (dirent *dp = ::readdir(dir); dp != nullptr; dp = ::readdir(dir))
			entries.push_back(var::make<path_info>(dp->d_name, dp->d_type));
		::closedir(dir);
		return std::move(entries);
	}

	void init()
	{
		path_ext.add_var("type", var::make_protect<extension_t>(path_type_ext_shared));
		path_ext.add_var("info", var::make_protect<extension_t>(path_info_ext_shared));
		path_type_ext.add_var("unknown", var::make_constant<int>(DT_UNKNOWN));
		path_type_ext.add_var("fifo", var::make_constant<int>(DT_FIFO));
		path_type_ext.add_var("sock", var::make_constant<int>(DT_SOCK));
		path_type_ext.add_var("chr", var::make_constant<int>(DT_CHR));
		path_type_ext.add_var("dir", var::make_constant<int>(DT_DIR));
		path_type_ext.add_var("blk", var::make_constant<int>(DT_BLK));
		path_type_ext.add_var("reg", var::make_constant<int>(DT_REG));
		path_type_ext.add_var("lnk", var::make_constant<int>(DT_LNK));
		path_info_ext.add_var("name", var::make_protect<callable>(cni(name)));
		path_info_ext.add_var("type", var::make_protect<callable>(cni(type)));
		path_ext.add_var("separator", var::make_constant<char>(path_separator));
		path_ext.add_var("delimiter", var::make_constant<char>(path_delimiter));
		path_ext.add_var("scan", var::make_protect<callable>(cni(scan)));
	}
}
namespace cs_impl {
	template<>
	cs::extension_t &get_ext<path_cs_ext::path_info>()
	{
		return path_info_ext_shared;
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
		system_ext.add_var("console", var::make_protect<extension_t>(console_ext_shared));
		system_ext.add_var("file", var::make_protect<extension_t>(file_ext_shared));
		system_ext.add_var("path", var::make_protect<extension_t>(path_ext_shared));
		system_ext.add_var("in", var::make_protect<istream>(&std::cin, [](std::istream *) {}));
		system_ext.add_var("out", var::make_protect<ostream>(&std::cout, [](std::ostream *) {}));
		system_ext.add_var("run", var::make_protect<callable>(cni(run)));
		system_ext.add_var("getenv", var::make_protect<callable>(cni(getenv)));
		system_ext.add_var("exit", var::make_protect<callable>(cni(exit)));
	}
}