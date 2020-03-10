/*
* Covariant Script OS Support: Common Functions
*
* Licensed under the Covariant Innovation General Public License,
* Version 1.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* https://covariant.cn/licenses/LICENSE-1.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
* Copyright (C) 2020 Michael Lee(李登淳)
* Email: mikecovlee@163.com
* Github: https://github.com/mikecovlee
*/

#include <covscript_impl/dirent/dirent.hpp>
#include <covscript_impl/system.hpp>
#include <sys/stat.h>
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
	std::vector<std::string> split(const std::string &string, char delimiter, bool drop_last)
	{
		std::vector<std::string> results;
		size_t start = 0, end;

		while ((end = string.find(delimiter, start)) != std::string::npos) {
			results.push_back(string.substr(start, end - start));
			start = end + 1;
		}

		if (!drop_last)
			results.push_back(string.substr(start));
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

	static bool mkdir_dirs(const std::vector<std::string> &dirs, unsigned int mode)
	{
		std::stringstream ss;
		for (const auto &dir : dirs) {
			ss << dir << cs::path_separator;
			if (is_directory(ss.str())) {
				continue;
			}

			if (!mkdir_impl(ss.str(), mode)) {
				return false;
			}
		}
		return true;
	}
}

namespace cs_impl {
	namespace file_system {
		bool chmod_r(const std::string &path, const std::string &mode)
		{
			auto dirs = cs_system_impl::split(path, cs::path_separator, false);
			std::stringstream ss;

			for (auto &dir : dirs) {
				ss << dir << cs::path_separator;

				// DO NOT SKIP when dir is a directory
				// directory has permissions too

				if (!cs_system_impl::chmod_impl(ss.str(), cs_system_impl::parse_mode(mode))) {
					return false;
				}
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

		bool mkdir_p(std::string path)
		{
			std::string::size_type pos;
			while (true) {
				pos = path.find('\\');
				if (pos != std::string::npos)
					path[pos] = '/';
				else
					break;
			}
			return cs_system_impl::mkdir_dirs(cs_system_impl::split(path, '/', true), 0755);
		}

		bool mkdir(std::string path)
		{
			std::string::size_type pos;
			while (true) {
				pos = path.find('\\');
				if (pos != std::string::npos)
					path[pos] = '/';
				else
					break;
			}
			return cs_system_impl::mkdir_dirs(cs_system_impl::split(path, '/', false), 0755);
		}

		bool exists(const std::string &path)
		{
			return std::ifstream(path).is_open();
		}

		bool is_dir(const std::string &path)
		{
			return cs_system_impl::is_directory(path);
		}
	}
}