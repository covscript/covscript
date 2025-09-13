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
 * Copyright (C) 2017-2025 Michael Lee(李登淳)
 *
 * This software is registered with the National Copyright Administration
 * of the People's Republic of China(Registration Number: 2020SR0408026)
 * and is protected by the Copyright Law of the People's Republic of China.
 *
 * Email:   lee@covariant.cn, mikecovlee@163.com
 * Github:  https://github.com/mikecovlee
 * Website: http://covscript.org.cn
 */

#include <covscript/impl/impl.hpp>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <unistd.h>
#include <sstream>
#include <climits>
#include <cassert>
#include <cstdio>
#include <string>
#include <cerrno>

#ifndef STACK_LIMIT

#define STACK_LIMIT (1024 * 1024)

#endif

#ifdef COVSCRIPT_PLATFORM_DARWIN

#include <mach-o/loader.h>

#endif

#ifdef CS_FIBER_LIBUCONTEXT_IMPL

extern "C"
{
#include <libucontext/libucontext.h>
}

#define cs_fiber_ucontext_t libucontext_ucontext_t
#define cs_fiber_getcontext libucontext_getcontext
#define cs_fiber_makecontext libucontext_makecontext
#define cs_fiber_setcontext libucontext_setcontext
#define cs_fiber_swapcontext libucontext_swapcontext

#else

#ifdef COVSCRIPT_PLATFORM_DARWIN

#define _XOPEN_SOURCE

#endif

#include <ucontext.h>

#define cs_fiber_ucontext_t ucontext_t
#define cs_fiber_getcontext getcontext
#define cs_fiber_makecontext makecontext
#define cs_fiber_setcontext setcontext
#define cs_fiber_swapcontext swapcontext

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
	if (!yn)
		newt.c_lflag &= ~ICANON;
	else
		newt.c_lflag |= ICANON;
	tcsetattr(0, TCSANOW, &newt);
}

void terminal_echo(int yn)
{
	struct termios oldt, newt;
	tcgetattr(0, &oldt);
	newt = oldt;
	if (!yn)
		newt.c_lflag &= ~ECHO;
	else
		newt.c_lflag |= ECHO;
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

		void echo(bool mode)
		{
			struct termios tty;
			tcgetattr(STDIN_FILENO, &tty);
			if (!mode)
				tty.c_lflag &= ~ECHO;
			else
				tty.c_lflag |= ECHO;
			(void)tcsetattr(STDIN_FILENO, TCSANOW, &tty);
		}

		void cursor(bool mode)
		{
			if (mode)
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

	namespace fiber {
		struct Routine {
			cs::process_context *this_context = cs::current_process;
			std::unique_ptr<cs::process_context> cs_pcontext;
			cs::stack_type<cs::domain_type> cs_stack;
			const cs::context_t &cs_context;
			std::function<void()> func;

			bool finished = false;
			bool running = false;
			bool error = false;
			std::exception_ptr eptr;

			void *stack = nullptr;
			cs_fiber_ucontext_t ctx;
			cs_fiber_ucontext_t *prev_ctx = nullptr;

			Routine(const cs::context_t &cxt, std::function<void()> f)
				: cs_context(cxt),
				  cs_pcontext(cs::current_process->fork()),
				  cs_stack(cs::current_process->child_stack_size()),
				  func(std::move(f)) {}

			~Routine()
			{
				if (stack != nullptr)
					free(stack);
			}

			void cs_context_swap_in()
			{
				cs_context->instance->swap_context(&cs_stack);
				cs::current_process = cs_pcontext.get();
			}

			void cs_context_swap_out()
			{
				cs_context->instance->swap_context(nullptr);
				cs::current_process = this_context;
			}
		};

		struct Ordinator {
			std::vector<Routine *> routines;
			std::list<routine_t> indexes;
			std::vector<routine_t> call_stack;
			cs_fiber_ucontext_t ctx;
			size_t stack_size;

			Ordinator(size_t ss = STACK_LIMIT)
				: stack_size(ss) {}

			~Ordinator()
			{
				for (auto &routine : routines)
					delete routine;
			}

			routine_t current_routine() const
			{
				return call_stack.empty() ? 0 : call_stack.back();
			}
		};

		thread_local static Ordinator ordinator;

		routine_t create(const cs::context_t &cxt, std::function<void()> f)
		{
			Routine *routine = new Routine(cxt, std::move(f));
			if (ordinator.indexes.empty()) {
				ordinator.routines.push_back(routine);
				return static_cast<routine_t>(ordinator.routines.size());
			}
			else {
				routine_t id = ordinator.indexes.front();
				ordinator.indexes.pop_front();
				ordinator.routines[id - 1] = routine;
				return id;
			}
		}

		void destroy(routine_t id)
		{
			if (id == 0 || id > ordinator.routines.size())
				throw cs::internal_error("Invalid coroutine ID.");
			Routine *routine = ordinator.routines[id - 1];
			if (routine == nullptr)
				throw cs::internal_error("Destroying a destroyed coroutine.");
			for (auto r : ordinator.call_stack) {
				if (r == id)
					throw cs::internal_error("Destroying a running or nested routine is not allowed.");
			}
			delete routine;
			ordinator.routines[id - 1] = nullptr;
			ordinator.indexes.push_back(id);
		}

		void entry()
		{
			routine_t id = ordinator.current_routine();
			Routine *routine = ordinator.routines[id - 1];
			if (routine == nullptr)
				throw cs::internal_error("Entering a destroyed coroutine.");
			try {
				routine->running = true;
				routine->func();
			}
			catch (...) {
				routine->eptr = std::current_exception();
				routine->error = true;
			}
			routine->running = false;
			routine->finished = true;
			cs_fiber_swapcontext(&routine->ctx, routine->prev_ctx);
		}

		int resume(routine_t id)
		{
			if (id == 0 || id > ordinator.routines.size())
				throw cs::internal_error("Invalid coroutine ID.");
			Routine *routine = ordinator.routines[id - 1];
			if (routine == nullptr)
				return -1;
			if (routine->finished)
				return routine->error ? -3 : -2;
			if (routine->running)
				return -4;
			if (routine->stack == nullptr) {
				routine->stack = malloc(ordinator.stack_size);
				if (routine->stack == nullptr)
					throw cs::internal_error("Coroutine create failed.");
				cs_fiber_getcontext(&routine->ctx);
				routine->ctx.uc_stack.ss_sp = routine->stack;
				routine->ctx.uc_stack.ss_size = ordinator.stack_size;
				routine->ctx.uc_link = nullptr;
				cs_fiber_makecontext(&routine->ctx, reinterpret_cast<void (*)()>(entry), 0);
				if (!ordinator.call_stack.empty()) {
					Routine *prev_routine = ordinator.routines[ordinator.call_stack.back() - 1];
					if (prev_routine == nullptr)
						throw cs::internal_error("Broken call stack.");
					routine->prev_ctx = &prev_routine->ctx;
				}
				else
					routine->prev_ctx = &ordinator.ctx;
			}
			ordinator.call_stack.push_back(id);
			routine->cs_context_swap_in();
			cs_fiber_swapcontext(routine->prev_ctx, &routine->ctx);
			if (!ordinator.call_stack.empty() && ordinator.call_stack.back() == id)
				ordinator.call_stack.pop_back();
			else
				throw cs::internal_error("Call stack corrupted.");
			routine->cs_context_swap_out();
			if (!ordinator.call_stack.empty()) {
				Routine *next_routine = ordinator.routines[ordinator.call_stack.back() - 1];
				if (next_routine == nullptr)
					throw cs::internal_error("Broken call stack.");
				next_routine->cs_context_swap_in();
			}
			if (routine->finished && routine->error)
				std::rethrow_exception(routine->eptr);
			return routine->finished ? 1 : 0;
		}

		void yield()
		{
			routine_t id = ordinator.current_routine();
			if (id == 0)
				throw cs::lang_error("Cannot yield outside a coroutine.");
			Routine *routine = ordinator.routines[id - 1];
			if (routine == nullptr)
				throw cs::internal_error("Yield a destroyed coroutine.");
			routine->running = false;
			cs_fiber_swapcontext(&routine->ctx, routine->prev_ctx);
		}

		routine_t current()
		{
			return ordinator.current_routine();
		}
	}
}
