#pragma once
/*
 * Covariant Script OS Support
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

#include <covscript/core/core.hpp>

namespace cs_impl {
	namespace platform {
		inline constexpr bool is_platform_win32()
		{
#ifdef COVSCRIPT_PLATFORM_WIN32
			return true;
#else
			return false;
#endif
		}

		inline constexpr bool is_platform_linux()
		{
#ifdef COVSCRIPT_PLATFORM_LINUX
			return true;
#else
			return false;
#endif
		}

		inline constexpr bool is_platform_darwin()
		{
#ifdef COVSCRIPT_PLATFORM_DARWIN
			return true;
#else
			return false;
#endif
		}

		inline constexpr bool is_platform_unix()
		{
#ifdef COVSCRIPT_PLATFORM_UNIX
			return true;
#else
			return false;
#endif
		}
	}
	namespace conio {
		int terminal_width();

		int terminal_height();

		void gotoxy(int, int);

		void echo(bool);

		void cursor(bool);

		void clrscr();

		int getch();

		int kbhit();
	}
	namespace file_system {
// Detection
		bool exist(const std::string &);

		bool is_file(const std::string &);

		bool is_dir(const std::string &);

		bool is_exe(const std::string &);

		bool can_read(const std::string &);

		bool can_write(const std::string &);

		bool can_execute(const std::string &);

		bool is_absolute_path(const std::string &);

// Files
		bool chmod_r(const std::string &, const std::string &);

		bool chmod(const std::string &, const std::string &);

		bool move(const std::string &, const std::string &);

		bool copy(const std::string &, const std::string &);

		bool remove(const std::string &);

// Directories
		bool mkdir_p(const std::string &);

		bool mkdir(std::string);

		std::string get_current_dir();
	}
}
