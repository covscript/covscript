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
 * Copyright (C) 2017-2026 Michael Lee(李登淳)
 *
 * This software is registered with the National Copyright Administration
 * of the People's Republic of China(Registration Number: 2020SR0408026)
 * and is protected by the Copyright Law of the People's Republic of China.
 *
 * Email:   mikecovlee@163.com
 * Github:  https://github.com/mikecovlee
 * Website: http://covscript.org.cn
 */

#include <covscript/impl/system.hpp>
#include <filesystem>
#include <fcntl.h>

namespace cs_system_impl {
	bool mkdir_impl(const std::string &, unsigned int);

	bool chmod_impl(const std::string &, unsigned int);
} // namespace cs_system_impl

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
		for (auto ch : str) {
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
		for (const auto &dir : dirs) {
			path += dir + cs::path_separator;
			if (std::filesystem::is_directory(path))
				continue;
			if (!mkdir_impl(path, mode))
				return false;
		}
		return true;
	}
} // namespace cs_system_impl

#ifdef COVSCRIPT_PLATFORM_WIN32
constexpr char path_separator_reversed = '/';
constexpr char path_delimiter_reversed = ':';
#else
constexpr char path_separator_reversed = '\\';
constexpr char path_delimiter_reversed = ';';
#endif

namespace cs_impl {
	namespace file_system {
		bool exist(const std::string &path)
		{
			return std::filesystem::exists(path);
		}

		bool is_file(const std::string &path)
		{
			return std::filesystem::is_regular_file(path);
		}

		bool is_dir(const std::string &path)
		{
			return std::filesystem::is_directory(path);
		}

		bool is_absolute_path(const std::string &path)
		{
			return std::filesystem::path(path).is_absolute();
		}

		bool chmod_r(const std::string &path_input, const std::string &mode)
		{
			auto dirs = cs_system_impl::split(path_input, {'/', '\\'});
			std::string path;
			if (path_input.size() > 0 && (path_input[0] == '/' || path_input[0] == '\\'))
				path = cs::path_separator;
			for (auto &dir : dirs) {
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
			std::error_code ec;
			std::filesystem::rename(source, dest, ec);
			return !ec;
		}

		bool copy(const std::string &source, const std::string &dest)
		{
			std::error_code ec;
			std::filesystem::copy_file(source, dest,
			                           std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing, ec);
			return !ec;
		}

		bool remove(const std::string &path)
		{
			std::error_code ec;
			return std::filesystem::remove_all(path, ec) > 0 && !ec;
		}

		bool mkdir_p(const std::string &path)
		{
			std::error_code ec;
			std::filesystem::create_directories(path, ec);
			return !ec;
		}

		bool mkdir(std::string path)
		{
			std::error_code ec;
			std::filesystem::create_directory(path, ec);
			return !ec;
		}

		std::string get_current_dir()
		{
			return std::filesystem::current_path().string();
		}
	} // namespace file_system
} // namespace cs_impl
