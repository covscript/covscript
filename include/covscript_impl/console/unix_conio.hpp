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
#include <cstdio>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/select.h>

static void terminal_lnbuf(int yn)
{
	struct termios oldt, newt;
	tcgetattr(0, &oldt);
	newt = oldt;
	if (!yn) newt.c_lflag &= ~ICANON;
	else newt.c_lflag |= ICANON;
	tcsetattr(0, TCSANOW, &newt);
}

static void terminal_echo(int yn)
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
			printf("\x1B[%d;%df", y + 1, x + 1);
		}

		static void echo(bool in)
		{
			if (in)
				printf("\33[?25h");
			else
				printf("\33[?25l");
		}

		static void clrscr()
		{
			printf("\x1B[2J\x1B[0;0f");
		}

		static int getch()
		{
			terminal_lnbuf(0);
			terminal_echo(0);
			int ch = getchar();
			terminal_lnbuf(1);
			terminal_echo(1);
			return ch;
		}

		static int kbhit()
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
}
