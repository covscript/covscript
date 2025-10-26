/*
* Covariant Script OS Support: Common Functions
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*     http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
* Copyright (C) 2017-2025 Michael Lee(李登淳)
*
* This software is registered with the National Copyright Administration
* of the People's Republic of China(Registration Number: 2020SR0408026)
* and is protected by the Copyright Law of the People's Republic of China.
*
* Email:   mikecovlee@163.com
* Github:  https://github.com/mikecovlee
* Website: http://covscript.org.cn
*/

#include <covscript_impl/dirent/dirent.hpp>
#include <covscript_impl/system.hpp>
#include <fcntl.h>

namespace cs_system_impl {
	bool mkdir_impl(const std::string &, unsigned int);

	bool chmod_impl(const std::string &, unsigned int);
}


#ifdef COVSCRIPT_PLATFORM_WIN32

#include "./win32/common.cpp"

#else
#ifndef COVSCRIPT_PLATFORM_UNIX
#warning Compatible Mode for Unix Platform
#endif

#include "./unix/common.cpp"

#endif

namespace cs_system_impl {
	std::vector<std::string> split(const std::string &str, const cs::set_t<char> &set)
	{
		std::vector<std::string> results;
		std::string buff;
		for (auto ch: str) {
			if (set.count(ch) > 0) {
				if (!buff.empty()) {
					results.emplace_back(buff);
					buff.clear();
				}
			}
			else
				buff.push_back(ch);
		}
		if (!buff.empty()) {
			results.emplace_back(buff);
			buff.clear();
		}
		return std::move(results);
	}

	int get_file_type(const std::string &path)
	{
		struct stat s {};
		if (stat(path.c_str(), &s) != 0) {
			return DT_UNKNOWN;
		}
		if (S_ISDIR(s.st_mode)) {
			return DT_DIR;
		}
		else if (S_ISREG(s.st_mode)) {
			return DT_REG;
		}
		else if (S_ISLNK(s.st_mode)) {
			return DT_LNK;
		}
		else if (S_ISFIFO(s.st_mode)) {
			return DT_FIFO;
		}
		else if (S_ISBLK(s.st_mode)) {
			return DT_BLK;
		}
		else if (S_ISCHR(s.st_mode)) {
			return DT_CHR;
		}
		else if (S_ISSOCK(s.st_mode)) {
			return DT_SOCK;
		}
		return DT_UNKNOWN;
	}

	inline bool is_directory(const std::string &path)
	{
		return get_file_type(path) == DT_DIR;
	}

	inline bool is_regular_file(const std::string &path)
	{
		return get_file_type(path) == DT_REG;
	}

	inline bool is_block_file(const std::string &path)
	{
		return get_file_type(path) == DT_BLK;
	}

	inline bool is_char_file(const std::string &path)
	{
		return get_file_type(path) == DT_CHR;
	}

	inline bool is_link_file(const std::string &path)
	{
		return get_file_type(path) == DT_LNK;
	}

	inline bool is_fifo_file(const std::string &path)
	{
		return get_file_type(path) == DT_FIFO;
	}

	unsigned int parse_mode(const std::string &modeString)
	{
		const char *perm = modeString.c_str();
		unsigned int mode = 0;

		if (std::isdigit(perm[0])) {
			const char *p = perm;
			while (*p) {
				mode = mode * 8 + *p++ - '0';
			}
		}
		else {
			if (modeString.size() == 9) {
				mode = (((perm[0] == 'r') * 4 | (perm[1] == 'w') * 2 | (perm[2] == 'x')) << 6) |
				       (((perm[3] == 'r') * 4 | (perm[4] == 'w') * 2 | (perm[5] == 'x')) << 3) |
				       (((perm[6] == 'r') * 4 | (perm[7] == 'w') * 2 | (perm[8] == 'x')));
			}
		}
		return mode;
	}

	static bool mkdir_dirs(const std::vector<std::string> &dirs, unsigned int mode, bool absolute_path = false)
	{
		std::string path;
		if (absolute_path)
			path = cs::path_separator;
		for (const auto &dir: dirs) {
			path += dir + cs::path_separator;
			if (is_directory(path))
				continue;
			if (!mkdir_impl(path, mode))
				return false;
		}
		return true;
	}
}

#ifdef COVSCRIPT_PLATFORM_WIN32
constexpr char path_separator_reversed = '/';
constexpr char path_delimiter_reversed = ':';
#else
constexpr char path_separator_reversed = '\\';
constexpr char path_delimiter_reversed = ';';
#endif

namespace cs_impl {
	namespace file_system {
		bool chmod_r(const std::string &path_input, const std::string &mode)
		{
			auto dirs = cs_system_impl::split(path_input, {'/', '\\'});
			std::string path;
			if (path_input.size() > 0 && (path_input[0] == '/' || path_input[0] == '\\'))
				path = cs::path_separator;
			for (auto &dir: dirs) {
				path += dir + cs::path_separator;
				// DO NOT SKIP when dir is a directory
				// directory has permissions too
				if (!cs_system_impl::chmod_impl(path, cs_system_impl::parse_mode(mode)))
					return false;
			}
			return true;
		}

		bool chmod(const std::string &path, const std::string &mode)
		{
			return cs_system_impl::chmod_impl(path, cs_system_impl::parse_mode(mode));
		}

		bool move(const std::string &source, const std::string &dest)
		{
			return std::rename(source.c_str(), dest.c_str()) == 0;
		}

		bool copy(const std::string &source, const std::string &dest)
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

		bool remove(const std::string &path)
		{
			return std::remove(path.c_str()) == 0;
		}

		bool mkdir_p(const std::string &path)
		{
			if (path.size() > 0 && (path[0] == '/' || path[0] == '\\'))
				return cs_system_impl::mkdir_dirs(cs_system_impl::split(path, {'/', '\\'}), 0755, true);
			else
				return cs_system_impl::mkdir_dirs(cs_system_impl::split(path, {'/', '\\'}), 0755, false);
		}

		bool mkdir(std::string path)
		{
			for (auto &ch: path) {
				if (ch == path_separator_reversed)
					ch = cs::path_separator;
			}
			return cs_system_impl::mkdir_impl(path, 0755);
		}

		bool exist(const std::string &path)
		{
			return std::ifstream(path).is_open();
		}

		bool is_file(const std::string &path)
		{
			return cs_system_impl::is_regular_file(path);
		}

		bool is_dir(const std::string &path)
		{
			return cs_system_impl::is_directory(path);
		}
	}
}
