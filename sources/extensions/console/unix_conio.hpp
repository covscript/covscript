#pragma once

#include <cstdio>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/select.h>

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

		/*
		    static void reset()
		    {
		        printf("\e[37;40;0m");
		    }
		*/
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
}
