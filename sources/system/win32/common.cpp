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
 * Copyright (C) 2017-2026 Michael Lee(李登淳)
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
#include <cstdio>
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
	} // namespace file_system
} // namespace cs_impl

namespace cs {
	namespace fiber {
		class win32_fiber : public fiber_type {
			friend void cs::fiber::resume(const fiber_t &, schedule_policy);
			friend void cs::fiber::sleep_for(std::size_t);
			friend void cs::fiber::yield();

			stack_type<domain_type> cs_stack;
			context_t cs_context;

			// Non-native fibers fork their own process_context (own value stack);
			// native fibers keep null and reuse the current execution path.
			std::shared_ptr<process_context> process;
			// Caller process, captured at resume(), restored via cs_swap_out().
			process_context *resumer_process = nullptr;

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
				// current_process is restored by resume() after SwitchToFiber returns.
				SwitchToFiber(fi->prev_ctx);
				// This should never execute
				std::abort();
			}

		public:
			win32_fiber() = delete;
			// Native Function
			win32_fiber(std::function<var()> f)
				: cs_stack(0), cs_context(nullptr), process(nullptr), func(std::move(f)), eptr(nullptr), state(fiber_state::ready), ret_val(null_pointer), stack_size(COVSCRIPT_FIBER_STACK_LIMIT) {}

			// CovScript Function
			win32_fiber(const context_t &cxt, std::function<var()> f)
				: cs_stack(current_process->child_stack_size()),
				  cs_context(cxt),
				  process(process_context::fork(process_context::current_owner())),
				  func(std::move(f)),
				  eptr(nullptr),
				  state(fiber_state::ready),
				  ret_val(null_pointer),
				  stack_size(COVSCRIPT_FIBER_STACK_LIMIT) {}

			virtual ~win32_fiber()
			{
				if (state == fiber_state::running || state == fiber_state::suspended || state == fiber_state::sleeping) {
					std::fprintf(stderr,
					             "[fiber] warning: destroying an unfinished fiber (state=%d); "
					             "its suspended stack frames are not unwound and resources will leak\n",
					             static_cast<int>(state));
					assert(false && "Destroying an unfinished fiber");
				}
				// Always release the fiber stack, even when the assertion above
				// is compiled out: skipping DeleteFiber would leak the whole
				// stack on every abandoned suspended fiber.
				if (ctx != nullptr)
					DeleteFiber(ctx);
			}

			void cs_swap_in()
			{
				if (process)
					current_process = process.get();
				if (cs_context)
					cs_context->instance->swap_context(&cs_stack);
			}

			void cs_swap_out()
			{
				// Restore the caller process after resume()'s SwitchToFiber returns.
				if (resumer_process != nullptr)
					current_process = resumer_process;
				if (cs_context)
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
					throw lang_error("The fiber has not completed yet");
			}

			const std::shared_ptr<process_context> &get_process() const noexcept override
			{
				return process;
			}
		};

		fiber_t create(const context_t &cxt, std::function<var()> f)
		{
			return std::make_shared<win32_fiber>(cxt, std::move(f));
		}

		fiber_t create_native(std::function<var()> f)
		{
			return std::make_shared<win32_fiber>(std::move(f));
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

		void resume(const fiber_t &fi_p, schedule_policy policy)
		{
			static global_ctx_holder global_ctx;
			win32_fiber *fi = static_cast<win32_fiber *>(fi_p.get());
			if (fi == nullptr)
				throw internal_error("Resuming a corrupted fiber.");
			if (fi->state == fiber_state::running || fi->state == fiber_state::finished)
				throw lang_error("A fiber cannot be resumed while it is already running or after it has finished");
			if (fi->state == fiber_state::ready) {
				fi->ctx = CreateFiber(fi->stack_size, win32_fiber::entry, fi);
				if (fi->ctx == nullptr)
					throw lang_error("Failed to create the fiber");
			}
			if (fi->state == fiber_state::sleeping) {
				auto now = std::chrono::steady_clock::now();
				if (now < fi->wake_up_time) {
					if (policy == schedule_policy::no_backpressure)
						return;
					fi->busy_skip_count++;
					auto remain_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
					                     fi->wake_up_time - now)
					                 .count();
					auto wait_time = static_cast<std::size_t>(
					                     fi->busy_skip_count * remain_ms * current_process->fiber_cxt->busy_wait_coef);
					if (wait_time > static_cast<std::size_t>(remain_ms))
						wait_time = static_cast<std::size_t>(remain_ms);
					if (wait_time >= current_process->fiber_cxt->busy_wait_min) {
						if (!current_process->fiber_cxt->stack.empty())
							sleep_for(wait_time);
						else
							std::this_thread::sleep_for(std::chrono::milliseconds(wait_time));
						fi->busy_skip_count = 0;
					}
					else if (wait_time == static_cast<std::size_t>(remain_ms) && remain_ms > 0) {
						if (!current_process->fiber_cxt->stack.empty())
							sleep_for(static_cast<std::size_t>(remain_ms));
						else
							std::this_thread::sleep_for(std::chrono::milliseconds(remain_ms));
						fi->busy_skip_count = 0;
					}
					return;
				}
				fi->busy_skip_count = 0;
			}
			// Re-bind prev_ctx every resume: the caller may differ from the one that
			// started the fiber, so binding once at creation could jump back to a
			// stale (possibly destroyed) context.
			if (!current_process->fiber_cxt->stack.empty())
				fi->prev_ctx = static_cast<win32_fiber *>(current_process->fiber_cxt->stack.top().get())->ctx;
			else
				fi->prev_ctx = global_ctx.ctx;
			fi->resumer_process = current_process;
			fi->state = fiber_state::running;
			current_process->fiber_cxt->stack.push(fi_p);
			fi->cs_swap_in();
			SwitchToFiber(fi->ctx);
			if (!current_process->fiber_cxt->stack.empty())
				current_process->fiber_cxt->stack.pop();
			else
				throw internal_error("Fiber stack corrupted.");
			// Restore the caller process before rebinding the remaining caller fiber.
			fi->cs_swap_out();
			if (!current_process->fiber_cxt->stack.empty())
				static_cast<win32_fiber *>(current_process->fiber_cxt->stack.top().get())->cs_swap_in();
			if (fi->eptr != nullptr) {
				std::exception_ptr e = nullptr;
				std::swap(fi->eptr, e);
				std::rethrow_exception(e);
			}
		}

		void yield()
		{
			if (current_process->fiber_cxt->stack.empty())
				throw lang_error("Cannot yield outside a fiber");
			win32_fiber *fi = static_cast<win32_fiber *>(current_process->fiber_cxt->stack.top().get());
			fi->state = fiber_state::suspended;
			SwitchToFiber(fi->prev_ctx);
		}

		void sleep_for(std::size_t ms)
		{
			if (current_process->fiber_cxt->stack.empty())
				throw lang_error("Cannot yield outside a fiber");
			win32_fiber *fi = static_cast<win32_fiber *>(current_process->fiber_cxt->stack.top().get());
			fi->wake_up_time = std::chrono::steady_clock::now() + std::chrono::milliseconds(ms);
			fi->busy_skip_count = 0;
			fi->state = fiber_state::sleeping;
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
