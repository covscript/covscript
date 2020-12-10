/*
* Covariant Script OS Support: Win32 Common Functions
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

#include <Windows.h>
#include <direct.h>
#include <conio.h>
#include <cstring>
#include <cstdlib>
#include <string>
#include <cstdio>
#include <io.h>

namespace cs_system_impl {
	bool chmod_impl(const std::string &path, unsigned int mode)
	{
		static constexpr unsigned int MS_MODE_MASK = 0x0000ffff;
		return ::_chmod(path.c_str(), (mode & MS_MODE_MASK)) == 0;
	}

	bool mkdir_impl(const std::string &path, unsigned int mode)
	{
		return ::_mkdir(path.c_str()) == 0;
	}
}

HANDLE StdHandle = GetStdHandle(STD_OUTPUT_HANDLE);

namespace cs_impl {
	namespace conio {
		int terminal_width()
		{
			static CONSOLE_SCREEN_BUFFER_INFO csbi;
			GetConsoleScreenBufferInfo(StdHandle, &csbi);
			return csbi.srWindow.Right - csbi.srWindow.Left;
		}

		int terminal_height()
		{
			static CONSOLE_SCREEN_BUFFER_INFO csbi;
			GetConsoleScreenBufferInfo(StdHandle, &csbi);
			return csbi.srWindow.Bottom - csbi.srWindow.Top;
		}

		void gotoxy(int x, int y)
		{
			COORD coord{static_cast<SHORT>(x), static_cast<SHORT>(y)};
			SetConsoleCursorPosition(StdHandle, coord);
		}

		void echo(bool mode)
		{
			static CONSOLE_CURSOR_INFO cci;
			GetConsoleCursorInfo(StdHandle, &cci);
			cci.bVisible = mode;
			SetConsoleCursorInfo(StdHandle, &cci);
		}

		void clrscr()
		{
			static CONSOLE_SCREEN_BUFFER_INFO csbi;
			static DWORD dwWritten;
			GetConsoleScreenBufferInfo(
			    StdHandle,
			    &csbi);
			FillConsoleOutputAttribute(
			    StdHandle,
			    csbi.wAttributes,
			    csbi.dwSize.X * csbi.dwSize.Y,
			{0, 0},
			&dwWritten);
			FillConsoleOutputCharacterW(
			    StdHandle,
			    L' ',
			    csbi.dwSize.X * csbi.dwSize.Y,
			{0, 0},
			&dwWritten);
			SetConsoleCursorPosition(
			    StdHandle,
			{0, 0});
		}

		int getch()
		{
			return ::getch();
		}

		int kbhit()
		{
			return ::kbhit();
		}
	}

	namespace file_system {
		bool is_exe(const std::string &path)
		{
			return can_execute(path);
		}

		bool can_read(const std::string &path)
		{
			return _access_s(path.c_str(), 4) == 0;
		}

		bool can_write(const std::string &path)
		{
			return _access_s(path.c_str(), 2) == 0;
		}

		bool can_execute(const std::string &path)
		{
			int fd = open(path.c_str(), O_RDONLY);
			if (fd < 0)
				return false;
			char header[2] = {0};
			int nread = read(fd, reinterpret_cast<void *>(&header), sizeof(header));
			close(fd);
			if (nread < 0)
				return false;
			return header[0] == 'M' && header[1] == 'Z';
		}

		bool is_absolute_path(const std::string &path)
		{
			return (!path.empty() && path[0] == '/')    // for mingw, cygwin
			       || (path.size() >= 3 && path[1] == ':' && path[2] == '\\');
		}

		std::string get_current_dir()
		{
			char temp[PATH_MAX] = "";

			if (::_getcwd(temp, PATH_MAX) != nullptr) {
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