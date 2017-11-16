#pragma once
/*
* Covariant Darwin Universal Character Graphics Library
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
* Copyright (C) 2017 Michael Lee(李登淳)
* Email: mikecovlee@163.com
* Github: https://github.com/mikecovlee
*/
#include <cstdio>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/select.h>

namespace conio {
	static int terminal_width()
	{
		struct winsize size;
		ioctl(STDIN_FILENO, TIOCGWINSZ, &size);
		return size.ws_col;
	}

	static int terminal_height()
	{
		struct winsize size;
		ioctl(STDIN_FILENO, TIOCGWINSZ, &size);
		return size.ws_row - 1;
	}

	static void gotoxy(int x, int y)
	{
		printf("\x1B[%d;%df", y, x);
	}

	static void echo(bool in)
	{
		struct termios oldt, newt;
		tcgetattr(0, &oldt);
		newt = oldt;
		if (in) {
			printf("\33[?25h");
			newt.c_lflag |= ECHO;
			newt.c_lflag |= ICANON;
		}
		else {
			printf("\33[?25l");
			newt.c_lflag &= ~ECHO;
			newt.c_lflag &= ~ICANON;
		}
		tcsetattr(0, TCSANOW, &newt);
	}

	static void reset()
	{
		printf("\e[37;40;0m");
	}

	static void clrscr()
	{
		printf("\x1B[2J\x1B[0;0f");
	}

	static int getch()
	{
		int ch;
		ch = getchar();
		return ch;
	}

	static int kbhit()
	{
		int ret;
		fd_set fds;
		struct timeval tv;
		tv.tv_sec = 0;
		tv.tv_usec = 0;
		FD_ZERO(&fds);
		FD_SET(0, &fds);
		select(1, &fds, 0, 0, &tv);
		ret = FD_ISSET(0, &fds);
		return ret;
	}
}