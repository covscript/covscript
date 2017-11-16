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
#include <string>
#include <exception>
#include "./timer.hpp"

namespace darwin {
	class warning final : public std::exception {
		std::string mWhat = "Darwin Warning";
	public:
		warning() = default;

		warning(const std::string &str) noexcept:
			mWhat("Darwin Warning:" + str) {}

		warning(const warning &) = default;

		warning(warning &&) = default;

		virtual ~warning() = default;

		virtual const char *what() const noexcept override
		{
			return this->mWhat.c_str();
		}
	};

	class error final : public std::exception {
		std::string mWhat = "Darwin Error";
	public:
		error() = default;

		error(const std::string &str) noexcept:
			mWhat("Darwin Error:" + str) {}

		error(const error &) = default;

		error(error &&) = default;

		virtual ~error() = default;

		error &operator=(const error &)= default;

		error &operator=(error &&)= default;

		virtual const char *what() const noexcept override
		{
			return this->mWhat.c_str();
		}
	};

	class outfs final {
		FILE *mfp = nullptr;
	public:
		outfs() = default;

		outfs(const char *path) : mfp(fopen(path, "w")) {}

		outfs(const std::string &path) : mfp(fopen(path.c_str(), "w")) {}

		~outfs()
		{
			if (this->mfp != nullptr) fclose(this->mfp);
		}

		outfs &operator=(const outfs &)= delete;

		operator bool() const noexcept
		{
			return this->mfp != nullptr;
		}

		bool usable() const noexcept
		{
			return this->mfp != nullptr;
		}

		void open(const char *path)
		{
			if (this->mfp == nullptr) {
				this->mfp = fopen(path, "w");
			}
		}

		void open(const std::string &path)
		{
			if (this->mfp == nullptr) {
				this->mfp = fopen(path.c_str(), "w");
			}
		}

		void close()
		{
			if (this->mfp != nullptr)
				fclose(this->mfp);
			this->mfp = nullptr;
		}

		template<typename...ArgsT>
		void printf(const char *format, ArgsT...args)
		{
			fprintf(this->mfp, format, args...);
		}

		void flush()
		{
			if (this->mfp != nullptr) {
				fflush(this->mfp);
			}
		}
	};

#ifndef DARWIN_DISABLE_LOG
	class debugger final {
		static outfs out;
	public:
#ifdef DARWIN_STRICT_CHECK
		static constexpr bool strict=true;
#else
		static constexpr bool strict=false;
#endif
#ifdef DARWIN_IGNORE_WARNING
		static constexpr bool ignore=true;
#else
		static constexpr bool ignore=false;
#endif
		static void log(const char* file,int line,const char* func,const char* msg)
		{
			if(!out.usable())
				out.open("./darwin_runtime.log");
			out.printf("[Darwin Log(%ums)]:In function \"%s\"(%s:%d):%s\n",timer::time(),func,file,line,msg);
		}
		static void warning(const char* file,int line,const char* func,const char* msg)
		{
			if(ignore)
				return;
			if(!out.usable())
				out.open("./darwin_runtime.log");
			out.printf("[Darwin Warning(%ums)]:In function \"%s\"(%s:%d):%s\n",timer::time(),func,file,line,msg);
			if(strict) {
				out.flush();
				std::terminate();
			}
		}
		static void error(const char* file,int line,const char* func,const char* msg)
		{
			out.printf("[Darwin Error(%ums)]:In function \"%s\"(%s:%d):%s\n",timer::time(),func,file,line,msg);
			out.flush();
			std::terminate();
		}
		static void log_path(const char* path)
		{
			out.close();
			out.open(path);
		}
	};
	outfs debugger::out;
#endif
}

#ifndef DARWIN_DISABLE_LOG

#define Darwin_Log(msg) darwin::debugger::log(__FILE__,__LINE__,__func__,msg);
#define Darwin_Warning(msg) darwin::debugger::warning(__FILE__,__LINE__,__func__,msg);
#define Darwin_Error(msg) darwin::debugger::error(__FILE__,__LINE__,__func__,msg);
#define Darwin_Set_Log_Path(path) darwin::debugger::log_path(path);

#else

#define Darwin_Log(msg)

#ifndef DARWIN_STRICT_CHECK

#define Darwin_Warning(msg)

#else

#define Darwin_Warning(msg) throw darwin::warning(msg);

#endif

#define Darwin_Error(msg) throw darwin::error(msg);
#define Darwin_Set_Log_Path(path)

#endif