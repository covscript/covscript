#pragma once
/*
* Covariant Script Console
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
* Copyright (C) 2019 Michael Lee(李登淳)
* Email: mikecovlee@163.com
* Github: https://github.com/mikecovlee
*/
#include <windows.h>
#include <cstring>
#include <conio.h>
#include <cstdlib>

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
}
