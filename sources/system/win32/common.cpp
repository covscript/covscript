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
 * Email:   mikecovlee@163.com
 * Github:  https://github.com/mikecovlee
 * Website: http://covscript.org.cn
 */

#include <covscript/impl/impl.hpp>
#include <covscript/impl/system.hpp>
#include <windows.h>
#include <direct.h>
#include <conio.h>
#include <cassert>
#include <cstdlib>
#include <string>
#include <utf8.h>
#include <io.h>

std::wstring utf8_to_wstring(std::string_view utf8)
{
	std::wstring out;
#if WCHAR_MAX == 0xffff
	utf8::utf8to16(utf8.begin(), utf8.end(), std::back_inserter(out));
#else
	utf8::utf8to32(utf8.begin(), utf8.end(), std::back_inserter(out));
#endif
	return out;
}

std::string wstring_to_utf8(std::wstring_view wide)
{
	std::string out;
#if WCHAR_MAX == 0xffff
	utf8::utf16to8(wide.begin(), wide.end(), std::back_inserter(out));
#else
	utf8::utf32to8(wide.begin(), wide.end(), std::back_inserter(out));
#endif
	return out;
}

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
} // namespace cs_system_impl

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
	} // namespace conio

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
			char *buf = ::_getcwd(nullptr, 0);
			if (buf == nullptr) {
				int error = errno;
				switch (error) {
				case EACCES:
					throw cs::runtime_error("Permission denied");
				case ENOMEM:
					throw cs::runtime_error("Out of memory");
				case ERANGE:
					throw cs::runtime_error("Path too long");
				default:
					throw cs::runtime_error("Unknown errno: " + std::to_string(error));
				}
			}
			std::string result(buf);
			free(buf);
			return result;
		}
	} // namespace file_system
} // namespace cs_impl

namespace cs {
	namespace fiber {
		class win32_fiber : public fiber_type {
			friend void cs::fiber::resume(const fiber_t &);
			friend void cs::fiber::yield();

			stack_type<domain_type> cs_stack;
			context_t cs_context;

			std::function<var()> func;
			std::exception_ptr eptr;
			fiber_state state;
			var ret_val;

			size_t stack_size;
			LPVOID ctx = nullptr;
			LPVOID prev_ctx = nullptr;

			static void __stdcall entry(LPVOID lpParameter) noexcept
			{
				win32_fiber *fi = static_cast<win32_fiber *>(lpParameter);
				try {
					fi->state = fiber_state::running;
					var ret = fi->func();
					fi->ret_val.swap(ret);
				}
				catch (...) {
					fi->eptr = std::current_exception();
				}
				fi->state = fiber_state::finished;
				SwitchToFiber(fi->prev_ctx);
				// This should never execute
				std::abort();
			}

		public:
			win32_fiber() = delete;
			win32_fiber(const context_t &cxt, std::function<var()> f)
				: cs_stack(current_process->child_stack_size()),
				  cs_context(cxt),
				  func(std::move(f)),
				  eptr(nullptr),
				  state(fiber_state::ready),
				  ret_val(null_pointer),
				  stack_size(current_process->child_stack_size()) {}

			virtual ~win32_fiber()
			{
				if (ctx != nullptr)
					DeleteFiber(ctx);
			}

			void cs_swap_in()
			{
				cs_context->instance->swap_context(&cs_stack);
			}

			void cs_swap_out()
			{
				cs_context->instance->swap_context(nullptr);
			}

			virtual fiber_state get_state() const
			{
				return state;
			}

			virtual var return_value() const
			{
				if (state == fiber_state::finished)
					return ret_val;
				else
					throw lang_error("Fiber has not yet started or ended.");
			}
		};

		fiber_t create(const context_t &cxt, std::function<var()> f)
		{
			return std::make_shared<win32_fiber>(cxt, std::move(f));
		}

		struct global_ctx_holder {
			LPVOID ctx = nullptr;

			global_ctx_holder()
			{
				ctx = ConvertThreadToFiber(nullptr);
				if (ctx == nullptr)
					throw internal_error("Create basic coroutine context failed.");
			}

			~global_ctx_holder()
			{
				if (ctx != nullptr)
					ConvertFiberToThread();
			}
		};

		void resume(const fiber_t &fi_p)
		{
			static global_ctx_holder global_ctx;
			win32_fiber *fi = static_cast<win32_fiber *>(fi_p.get());
			if (fi == nullptr)
				throw internal_error("Resuming a corrupted fiber.");
			if (fi->state == fiber_state::running || fi->state == fiber_state::finished)
				throw lang_error("Fiber is not reentrant.");
			if (fi->state == fiber_state::ready) {
				fi->ctx = CreateFiber(fi->stack_size, win32_fiber::entry, fi);
				if (fi->ctx == nullptr)
					throw lang_error("Fiber create failed.");
				if (!current_process->fiber_stack.empty())
					fi->prev_ctx = static_cast<win32_fiber *>(current_process->fiber_stack.top().get())->ctx;
				else
					fi->prev_ctx = global_ctx.ctx;
			}
			else if (fi->state == fiber_state::suspended) {
				if (fi->prev_ctx == nullptr)
					throw internal_error("Fiber context corrupted.");
				fi->state = fiber_state::running;
			}
			current_process->fiber_stack.push(fi_p);
			fi->cs_swap_in();
			SwitchToFiber(fi->ctx);
			if (!current_process->fiber_stack.empty())
				current_process->fiber_stack.pop();
			else
				throw internal_error("Fiber stack corrupted.");
			if (!current_process->fiber_stack.empty())
				static_cast<win32_fiber *>(current_process->fiber_stack.top().get())->cs_swap_in();
			else
				fi->cs_swap_out();
			if (fi->eptr != nullptr) {
				std::exception_ptr e = nullptr;
				std::swap(fi->eptr, e);
				std::rethrow_exception(e);
			}
		}

		void yield()
		{
			if (current_process->fiber_stack.empty())
				throw lang_error("Cannot yield outside a fiber.");
			win32_fiber *fi = static_cast<win32_fiber *>(current_process->fiber_stack.top().get());
			fi->state = fiber_state::suspended;
			SwitchToFiber(fi->prev_ctx);
		}
	} // namespace fiber

	namespace dll {
		void *open(std::string_view path)
		{
			std::wstring wpath = utf8_to_wstring(path);
			void *sym = ::LoadLibraryW(wpath.c_str());
			if (sym == nullptr) {
				static WCHAR szBuf[256];
				::FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, ::GetLastError(),
				                 MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), szBuf, sizeof(szBuf) / sizeof(WCHAR), nullptr);
				throw cs::runtime_error(wstring_to_utf8(szBuf));
			}
			return sym;
		}

		void *find_symbol(void *handle, std::string_view symbol)
		{
			return reinterpret_cast<void *>(::GetProcAddress((HMODULE) handle, symbol.data()));
		}

		void close(void *handle)
		{
			::FreeLibrary((HMODULE) handle);
		}
	} // namespace dll
} // namespace cs
