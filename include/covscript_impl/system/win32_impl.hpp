#pragma once
/*
* Covariant Script OS Support: Microsoft Windows
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
#if defined(_WIN32) || defined(WIN32)
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <windows.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <string>
#include <conio.h>

namespace cs_impl {
	namespace conio {
		static HANDLE StdHandle = GetStdHandle(STD_OUTPUT_HANDLE);

		static int terminal_width()
		{
			static CONSOLE_SCREEN_BUFFER_INFO csbi;
			GetConsoleScreenBufferInfo(StdHandle, &csbi);
			return csbi.srWindow.Right - csbi.srWindow.Left;
		}

		static int terminal_height()
		{
			static CONSOLE_SCREEN_BUFFER_INFO csbi;
			GetConsoleScreenBufferInfo(StdHandle, &csbi);
			return csbi.srWindow.Bottom - csbi.srWindow.Top;
		}

		static void gotoxy(SHORT x, SHORT y)
		{
			SetConsoleCursorPosition(StdHandle, {x, y});
		}

		static void echo(bool mode)
		{
			static CONSOLE_CURSOR_INFO cci;
			GetConsoleCursorInfo(StdHandle, &cci);
			cci.bVisible = mode;
			SetConsoleCursorInfo(StdHandle, &cci);
		}

		static void clrscr()
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

		static int getch()
		{
			return ::getch();
		}

		static int kbhit()
		{
			return ::kbhit();
		}
	}

	namespace filesystem {
        static bool can_read(const std::string &path)
        {
            return access(path.c_str(), R_OK) == 0;
        }

        static bool can_write(const std::string &path)
        {
            return access(path.c_str(), W_OK) == 0;
        }

        static bool can_execute(const std::string &path)
        {
            return access(path.c_str(), X_OK) == 0;
        }

        static bool chmod_impl(const std::string &path, mode_t mode)
        {
            return ::chmod(path.c_str(), mode) == 0;
        }

        static bool mkdir_impl(const std::string &path, mode_t mode)
        {
            return ::mkdir(path.c_str()) == 0;
        }

        static std::string get_current_dir()
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
#endif
