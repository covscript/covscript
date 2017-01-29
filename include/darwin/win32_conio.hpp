#pragma once
#include <windows.h>
#include <conio.h>
#include <cstdlib>
// Thanks:Dr.Lin
namespace conio {
	static HANDLE StdHandle=GetStdHandle(STD_OUTPUT_HANDLE);
	static int terminal_width()
	{
		static CONSOLE_SCREEN_BUFFER_INFO csbi;
		GetConsoleScreenBufferInfo(StdHandle,&csbi);
		return csbi.srWindow.Right-csbi.srWindow.Left;
	}
	static int terminal_height()
	{
		static CONSOLE_SCREEN_BUFFER_INFO csbi;
		GetConsoleScreenBufferInfo(StdHandle,&csbi);
		return csbi.srWindow.Bottom-csbi.srWindow.Top;
	}
	static void gotoxy(SHORT x,SHORT y)
	{
		SetConsoleCursorPosition(StdHandle, {x,y});
	}
	static void echo(bool mode)
	{
		CONSOLE_CURSOR_INFO cci;
		GetConsoleCursorInfo(StdHandle, &cci);
		cci.bVisible = mode;
		SetConsoleCursorInfo(StdHandle, &cci);
	}
	static void set_color(int textcolor, int bgcolor)
	{
		SetConsoleTextAttribute(StdHandle, textcolor + (bgcolor-8)*16);
	}
	static void set_title(const char *title)
	{
		SetConsoleTitleA(title);
	}
	static void reset()
	{
		set_color(15,0);
	}
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