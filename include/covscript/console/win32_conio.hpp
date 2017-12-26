#pragma once
/*
* Covariant Script Console
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

		/*
		    class console final {
		        SHORT w, h;
		        std::size_t offset = 0;
		        CHAR_INFO *buffer = nullptr;
		        WORD attri = 0;
		    public:
		        console() = delete;

		        console(std::size_t width, std::size_t height) : w(width), h(height), buffer(new CHAR_INFO[w * h]) {}

		        ~console()
		        {
		            delete[] buffer;
		        }

		        void set_color(int textcolor, int bgcolor)
		        {
		            attri = textcolor + (bgcolor - 8) * 16;
		        }

		        void reset()
		        {
		            set_color(15, 8);
		        }

		        void put_char(char ch)
		        {
		            buffer[offset].Attributes = attri;
		            buffer[offset].Char.AsciiChar = ch;
		            ++offset;
		        }

		        void flush()
		        {
		            static CONSOLE_SCREEN_BUFFER_INFO csbi;
		            GetConsoleScreenBufferInfo(StdHandle, &csbi);
		            WriteConsoleOutput(StdHandle, buffer, {w, h}, {0, 0}, &csbi.srWindow);
		        }
		    };

		    static void set_title(const char *title)
		    {
		        SetConsoleTitle(title);
		    }

		    static void set_color(int textcolor, int bgcolor)
		    {
		        SetConsoleTextAttribute(StdHandle, textcolor + (bgcolor - 8) * 16);
		    }

		    static void reset()
		    {
		        set_color(15, 8);
		    }
		*/
		static void clrscr()
		{
			system("cls");
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
