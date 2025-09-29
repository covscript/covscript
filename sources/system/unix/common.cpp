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
#include <covscript_impl/system.hpp>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <termios.h>
#include <unistd.h>
#include <sstream>
#include <climits>
#include <cassert>
#include <cstdio>
#include <string>
#include <cerrno>

#ifdef COVSCRIPT_PLATFORM_DARWIN

#include <mach-o/loader.h>

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
}

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

namespace cs {
	namespace fiber {
		class unix_fiber_stack {
			inline static uintptr_t align_up(uintptr_t ptr, size_t align)
			{
				return (ptr + align - 1) & ~(align - 1);
			}

			void *base = nullptr;
			size_t size = 0;
			void *sp = nullptr;
			size_t usable_size = 0;

		public:
			unix_fiber_stack() = delete;
			unix_fiber_stack(const unix_fiber_stack &) = delete;
			unix_fiber_stack(unix_fiber_stack &&) noexcept = delete;

			explicit unix_fiber_stack(size_t stack_size, size_t align = 16)
			{
				long pagesize = sysconf(_SC_PAGESIZE);
				if (pagesize <= 0)
					throw std::runtime_error("Failed to get page size.");
				if ((align & (align - 1)) != 0 || align == 0)
					throw std::invalid_argument("align must be a power of two.");
				if ((size_t)pagesize % align != 0)
					throw std::invalid_argument("align must divide system page size.");

				size_t rounded = (stack_size + pagesize - 1) & ~(pagesize - 1);
				size_t total = rounded + 2 * pagesize;

				void *addr = mmap(nullptr, total, PROT_READ | PROT_WRITE,
				                  MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
				if (addr == MAP_FAILED)
					throw std::runtime_error(std::string("mmap failed: ") + std::strerror(errno));

				if (mprotect(addr, pagesize, PROT_NONE) != 0 ||
				        mprotect((char *)addr + pagesize + rounded, pagesize, PROT_NONE) != 0) {
					munmap(addr, total);
					throw std::runtime_error(std::string("mprotect failed: ") + std::strerror(errno));
				}

				base = addr;
				size = total;
				uintptr_t aligned_sp = align_up((uintptr_t)addr + pagesize, align);
				sp = (void *)aligned_sp;
				usable_size = rounded - (aligned_sp - ((uintptr_t)addr + pagesize));

				if (usable_size == 0)
					throw std::runtime_error("Stack usable size is zero after alignment");
			}

			~unix_fiber_stack()
			{
				if (base != nullptr)
					munmap(base, size);
			}

			inline bool initialized() const
			{
				return base != nullptr;
			}

			inline void *get_sp() const
			{
				return sp;
			}

			inline size_t get_ss() const
			{
				return usable_size;
			}
		};

		class unix_fiber : public fiber_type {
			friend void cs::fiber::resume(const fiber_t &);
			friend void cs::fiber::yield();

			stack_type<domain_type> cs_stack;
			context_t cs_context;

			std::function<var()> func;
			std::exception_ptr eptr;
			fiber_state state;
			var ret_val;

			unix_fiber_stack stack;
			cs_fiber_ucontext_t ctx;
			cs_fiber_ucontext_t *prev_ctx = nullptr;

			static void entry() noexcept
			{
				unix_fiber *fi = dynamic_cast<unix_fiber *>(current_process->fiber_stack.top().get());
				try {
					fi->state = fiber_state::running;
					var ret = fi->func();
					fi->ret_val.swap(ret);
				}
				catch (...) {
					fi->eptr = std::current_exception();
				}
				fi->state = fiber_state::finished;
				cs_fiber_swapcontext(&fi->ctx, fi->prev_ctx);
				// This should never execute
				std::abort();
			}

		public:
			unix_fiber() = delete;
			unix_fiber(const context_t &cxt, std::function<var()> f)
				: cs_stack(current_process->child_stack_size()),
				  cs_context(cxt),
				  func(std::move(f)),
				  eptr(nullptr),
				  state(fiber_state::ready),
				  ret_val(null_pointer),
				  stack(COVSCRIPT_FIBER_STACK_LIMIT) {}

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
			return std::make_shared<unix_fiber>(cxt, std::move(f));
		}

		void resume(const fiber_t &fi_p)
		{
			static cs_fiber_ucontext_t global_ctx;
			unix_fiber *fi = dynamic_cast<unix_fiber *>(fi_p.get());
			if (fi == nullptr)
				throw internal_error("Resuming a corrupted fiber.");
			if (fi->state == fiber_state::running || fi->state == fiber_state::finished)
				throw lang_error("Fiber is not reentrant.");
			if (fi->state == fiber_state::ready) {
				memset(&fi->ctx, 0, sizeof(fi->ctx));
				cs_fiber_getcontext(&fi->ctx);
				fi->ctx.uc_stack.ss_sp = fi->stack.get_sp();
				fi->ctx.uc_stack.ss_size = fi->stack.get_ss();
				fi->ctx.uc_link = nullptr;
				cs_fiber_makecontext(&fi->ctx, reinterpret_cast<void (*)()>(unix_fiber::entry), 0);
				if (!current_process->fiber_stack.empty())
					fi->prev_ctx = &dynamic_cast<unix_fiber *>(current_process->fiber_stack.top().get())->ctx;
				else
					fi->prev_ctx = &global_ctx;
			}
			else if (fi->state == fiber_state::suspended) {
				if (fi->prev_ctx == nullptr)
					throw internal_error("Fiber context corrupted.");
				fi->state = fiber_state::running;
			}
			fi->state = fiber_state::running;
			current_process->fiber_stack.push(fi_p);
			fi->cs_swap_in();
			cs_fiber_swapcontext(fi->prev_ctx, &fi->ctx);
			if (!current_process->fiber_stack.empty())
				current_process->fiber_stack.pop();
			else
				throw internal_error("Fiber stack corrupted.");
			if (!current_process->fiber_stack.empty())
				dynamic_cast<unix_fiber *>(current_process->fiber_stack.top().get())->cs_swap_in();
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
			unix_fiber *fi = dynamic_cast<unix_fiber *>(current_process->fiber_stack.top().get());
			fi->state = fiber_state::suspended;
			cs_fiber_swapcontext(&fi->ctx, fi->prev_ctx);
		}
	}
}
