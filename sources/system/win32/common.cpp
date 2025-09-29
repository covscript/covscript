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
#include <covscript_impl/system.hpp>
#include <direct.h>
#include <conio.h>
#include <cassert>
#include <cstdlib>
#include <string>
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
			CONSOLE_SCREEN_BUFFER_INFO csbi;
			GetConsoleScreenBufferInfo(StdHandle, &csbi);
			return csbi.srWindow.Right - csbi.srWindow.Left;
		}

		int terminal_height()
		{
			CONSOLE_SCREEN_BUFFER_INFO csbi;
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
			HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
			DWORD conmode;
			GetConsoleMode(hStdin, &conmode);
			if (!mode)
				conmode &= ~ENABLE_ECHO_INPUT;
			else
				conmode |= ENABLE_ECHO_INPUT;
			SetConsoleMode(hStdin, conmode);
		}

		void cursor(bool mode)
		{
			CONSOLE_CURSOR_INFO cci;
			GetConsoleCursorInfo(StdHandle, &cci);
			cci.bVisible = mode ? TRUE : FALSE;
			SetConsoleCursorInfo(StdHandle, &cci);
		}

		void clrscr()
		{
			CONSOLE_SCREEN_BUFFER_INFO csbi;
			DWORD dwWritten;
			GetConsoleScreenBufferInfo(StdHandle, &csbi);
			COORD home{0, 0};
			DWORD cellCount = csbi.dwSize.X * csbi.dwSize.Y;
			FillConsoleOutputCharacterA(StdHandle, ' ', cellCount, home, &dwWritten);
			FillConsoleOutputAttribute(StdHandle, csbi.wAttributes, cellCount, home, &dwWritten);
			SetConsoleCursorPosition(StdHandle, home);
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
			return (!path.empty() && path[0] == '/') // for mingw, cygwin
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

			default:
				std::stringstream str;
				str << "Unrecognised errno: " << error;
				throw cs::runtime_error(str.str());
			}
		}
	}

	namespace fiber {
		struct Routine {
			cs::process_context *this_context = cs::current_process;
			std::unique_ptr<cs::process_context> cs_pcontext;
			cs::stack_type<cs::domain_type> cs_stack;
			const cs::context_t &cs_context;
			std::function<cs::var()> func;

			bool finished = false;
			bool running = false;
			bool error = false;
			cs::var ret_val;

			LPVOID ctx = nullptr;
			LPVOID prev_ctx = nullptr;

			Routine(const cs::context_t &cxt, std::function<cs::var()> f)
				: cs_context(cxt),
				  cs_pcontext(cs::current_process->fork()),
				  cs_stack(cs::current_process->child_stack_size()),
				  func(std::move(f)) {}

			~Routine()
			{
				if (ctx != nullptr)
					DeleteFiber(ctx);
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
			std::list<fiber_id> indexes;
			std::vector<fiber_id> call_stack;
			size_t stack_size;
			LPVOID ctx;

			Ordinator(size_t ss = COVSCRIPT_FIBER_STACK_LIMIT)
				: stack_size(ss)
			{
				ctx = ConvertThreadToFiber(nullptr);
				if (ctx == nullptr)
					throw cs::internal_error("Create basic coroutine context failed.");
			}

			~Ordinator()
			{
				for (auto &routine : routines)
					delete routine;
				ConvertFiberToThread();
			}

			fiber_id current_routine() const
			{
				return call_stack.empty() ? 0 : call_stack.back();
			}
		};

		thread_local static Ordinator ordinator;

		fiber_id create(const cs::context_t &cxt, std::function<cs::var()> f)
		{
			Routine *routine = new Routine(cxt, std::move(f));
			if (ordinator.indexes.empty()) {
				ordinator.routines.push_back(routine);
				return static_cast<fiber_id>(ordinator.routines.size());
			}
			else {
				fiber_id id = ordinator.indexes.front();
				ordinator.indexes.pop_front();
				ordinator.routines[id - 1] = routine;
				return id;
			}
		}

		void destroy(fiber_id id)
		{
			if (id == 0 || id > ordinator.routines.size())
				throw cs::lang_error("Invalid fiber ID.");
			Routine *routine = ordinator.routines[id - 1];
			if (routine == nullptr)
				throw cs::lang_error("Destroying a destroyed fiber.");
			if (routine->running && !routine->finished)
				throw cs::lang_error("Destroying a running fiber is not allowed.");
			for (auto r : ordinator.call_stack) {
				if (r == id)
					throw cs::lang_error("Destroying a nested fiber is not allowed.");
			}
			delete routine;
			ordinator.routines[id - 1] = nullptr;
			ordinator.indexes.push_back(id);
		}

		cs::var return_value(fiber_id id)
		{
			if (id == 0 || id > ordinator.routines.size())
				throw cs::lang_error("Invalid fiber ID.");
			Routine *routine = ordinator.routines[id - 1];
			if (routine == nullptr)
				throw cs::lang_error(
				    "Geting return value from a destroyed fiber.");
			if (routine->finished)
				return routine->ret_val;
			else
				throw cs::lang_error("Fiber has not yet started or ended.");
		}

		void __stdcall entry(LPVOID lpParameter) noexcept
		{
			fiber_id id = ordinator.current_routine();
			Routine *routine = ordinator.routines[id - 1];
			if (routine == nullptr)
				return;
			try {
				routine->running = true;
				routine->ret_val = routine->func();
			}
			catch (...) {
				routine->this_context->eptr = std::current_exception();
				routine->ret_val = cs::null_pointer;
				routine->error = true;
			}
			routine->running = false;
			routine->finished = true;
			SwitchToFiber(routine->prev_ctx);
			// This should never execute
			std::abort();
		}

		bool resume(fiber_id id)
		{
			if (id == 0 || id > ordinator.routines.size())
				throw cs::lang_error("Invalid fiber ID.");
			Routine *routine = ordinator.routines[id - 1];
			if (routine == nullptr)
				throw cs::lang_error("Resuming a destroyed fiber.");
			if (routine->finished || routine->running)
				throw cs::lang_error("Fiber is not reentrant.");
			if (routine->ctx == nullptr) {
				routine->ctx = CreateFiber(ordinator.stack_size, entry, nullptr);
				if (routine->ctx == nullptr)
					throw cs::lang_error("Fiber create failed.");
				if (!ordinator.call_stack.empty()) {
					Routine *prev_routine = ordinator.routines[ordinator.call_stack.back() - 1];
					if (prev_routine == nullptr)
						throw cs::internal_error("Broken call stack.");
					routine->prev_ctx = prev_routine->ctx;
				}
				else
					routine->prev_ctx = ordinator.ctx;
			}
			// Push call stack
			ordinator.call_stack.push_back(id);
			// Swap to routine interpreter context
			routine->cs_context_swap_in();
			// Swap to routine context
			SwitchToFiber(routine->ctx);
			// Pop call stack
			if (!ordinator.call_stack.empty() && ordinator.call_stack.back() == id)
				ordinator.call_stack.pop_back();
			else
				throw cs::internal_error("Call stack corrupted.");
			// Swap to original interpreter context
			if (!ordinator.call_stack.empty()) {
				Routine *next_routine = ordinator.routines[ordinator.call_stack.back() - 1];
				if (next_routine == nullptr)
					throw cs::internal_error("Broken call stack.");
				next_routine->cs_context_swap_in();
			}
			else
				routine->cs_context_swap_out();
			// Handling exception
			if (routine->finished && routine->error) {
				std::exception_ptr e = nullptr;
				std::swap(routine->this_context->eptr, e);
				std::rethrow_exception(e);
			}
			return !routine->finished && !routine->error;
		}

		void yield()
		{
			fiber_id id = ordinator.current_routine();
			if (id == 0)
				throw cs::lang_error("Cannot yield outside a fiber.");
			Routine *routine = ordinator.routines[id - 1];
			if (routine == nullptr)
				throw cs::lang_error("Yield a destroyed fiber.");
			routine->running = false;
			SwitchToFiber(routine->prev_ctx);
		}

		fiber_id current()
		{
			return ordinator.current_routine();
		}
	}
}
