/*
* Covariant Script OS Support: Unix Common Functions
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
* Copyright (C) 2017-2020 Michael Lee(李登淳)
*
* This software is registered with the National Copyright Administration
* of the People's Republic of China(Registration Number: 2020SR0408026)
* and is protected by the Copyright Law of the People's Republic of China.
* 
* Email:   lee@covariant.cn, mikecovlee@163.com
* Github:  https://github.com/mikecovlee
* Website: http://covscript.org.cn
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
			struct winsize size;
			ioctl(STDIN_FILENO, TIOCGWINSZ, &size);
			return size.ws_col;
		}

		int terminal_height()
		{
			struct winsize size;
			ioctl(STDIN_FILENO, TIOCGWINSZ, &size);
			return size.ws_row - 1;
		}

		void gotoxy(int x, int y)
		{
			printf("\x1B[%d;%df", y + 1, x + 1);
		}

		void echo(bool in)
		{
			if (in)
				printf("\33[?25h");
			else
				printf("\33[?25l");
		}

		void clrscr()
		{
			printf("\x1B[2J\x1B[0;0f");
		}

		int getch()
		{
			terminal_lnbuf(0);
			terminal_echo(0);
			int ch = getchar();
			terminal_lnbuf(1);
			terminal_echo(1);
			return ch;
		}

		int kbhit()
		{
			fd_set fds;
			terminal_lnbuf(0);
			terminal_echo(0);
			struct timeval tv;
			tv.tv_sec = 0;
			tv.tv_usec = 0;
			FD_ZERO(&fds);
			FD_SET(0, &fds);
			select(1, &fds, 0, 0, &tv);
			int ret = FD_ISSET(0, &fds);
			terminal_lnbuf(1);
			terminal_echo(1);
			return ret;
		}
	}

	namespace file_system {
		bool is_exe(const std::string &path)
		{
			int fd = open(path.c_str(), O_RDONLY);
			if (fd < 0)
				return false;
#ifdef COVSCRIPT_PLATFORM_DARWIN
			uint32_t header;
#else
#ifndef COVSCRIPT_PLATFORM_LINUX
#warning Compatible Mode for Linux System
#endif
			char header[4] = {0};
#endif
			int nread = read(fd, reinterpret_cast<void *>(&header), sizeof(header));
			close(fd);
			if (nread < 0)
				return false;
#ifdef COVSCRIPT_PLATFORM_DARWIN
			if (header == MH_MAGIC || header == MH_CIGAM || header == MH_MAGIC_64 || header == MH_CIGAM_64)
				return true;
#else
#ifndef COVSCRIPT_PLATFORM_LINUX
#warning Compatible Mode for Linux System
#endif
			if (header[0] == 0x7f && header[1] == 'E' && header[2] == 'L' && header[3] == 'F')
				return true;
#endif
			return false;
		}

		bool can_read(const std::string &path)
		{
			return access(path.c_str(), R_OK) == 0;
		}

		bool can_write(const std::string &path)
		{
			return access(path.c_str(), W_OK) == 0;
		}

		bool can_execute(const std::string &path)
		{
			return access(path.c_str(), X_OK) == 0;
		}

		bool is_absolute_path(const std::string &path)
		{
			return !path.empty() && path[0] == '/';
		}

		std::string get_current_dir()
		{
			char temp[PATH_MAX] = "";

			if (::getcwd(temp, PATH_MAX) != nullptr) {
				return std::string(temp);
			}

			int error = errno;
			switch (error) {
			case EACCES:
				throw cs::runtime_error("Permission denied");

			case ENOMEM:
				throw cs::runtime_error("Out of memory");

			default: {
				std::stringstream str;
				str << "Unrecognised errno: " << error;
				throw cs::runtime_error(str.str());
			}
			}
		}
	}
}