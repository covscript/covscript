/*
* Covariant Script OS Support: Unix Common Functions
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
* Copyright (C) 2019 Michael Lee(李登淳)
* Email: mikecovlee@163.com
* Github: https://github.com/mikecovlee
*/

#include <sys/select.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>
#include <sstream>
#include <climits>
#include <cstdio>
#include <string>
#include <cerrno>

#ifdef COVSCRIPT_PLATFORM_DARWIN

#include <mach-o/loader.h>

#endif

namespace cs_system_impl {
	bool chmod_impl(const std::string &path, unsigned int mode)
	{
		return ::chmod(path.c_str(), mode) == 0;
	}

	bool mkdir_impl(const std::string &path, unsigned int mode)
	{
		return ::mkdir(path.c_str(), mode) == 0;
	}
}

void terminal_lnbuf(int yn)
{
	struct termios oldt, newt;
	tcgetattr(0, &oldt);
	newt = oldt;
	if (!yn) newt.c_lflag &= ~ICANON;
	else newt.c_lflag |= ICANON;
	tcsetattr(0, TCSANOW, &newt);
}

void terminal_echo(int yn)
{
	struct termios oldt, newt;
	tcgetattr(0, &oldt);
	newt = oldt;
	if (!yn) newt.c_lflag &= ~ECHO;
	else newt.c_lflag |= ECHO;
	tcsetattr(0, TCSANOW, &newt);
}

namespace cs_impl {
	namespace conio {
		int terminal_width()
		{
			throw cs::lang_error("SANDBOX_MODE");
		}

		int terminal_height()
		{
			throw cs::lang_error("SANDBOX_MODE");
		}

		void gotoxy(int x, int y)
		{
			throw cs::lang_error("SANDBOX_MODE");
		}

		void echo(bool in)
		{
			throw cs::lang_error("SANDBOX_MODE");
		}

		void clrscr()
		{
			throw cs::lang_error("SANDBOX_MODE");
		}

		int getch()
		{
			throw cs::lang_error("SANDBOX_MODE");
		}

		int kbhit()
		{
			throw cs::lang_error("SANDBOX_MODE");
		}
	}

	namespace file_system {
		bool is_exe(const std::string &path)
		{
			throw cs::lang_error("SANDBOX_MODE");
		}

		bool can_read(const std::string &path)
		{
			throw cs::lang_error("SANDBOX_MODE");
		}

		bool can_write(const std::string &path)
		{
			throw cs::lang_error("SANDBOX_MODE");
		}

		bool can_execute(const std::string &path)
		{
			throw cs::lang_error("SANDBOX_MODE");
		}

		bool is_absolute_path(const std::string &path)
		{
			throw cs::lang_error("SANDBOX_MODE");
		}

		std::string get_current_dir()
		{
			throw cs::lang_error("SANDBOX_MODE");
		}
	}
}