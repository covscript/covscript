#pragma once

#include <cstdio>
#include <string>
#include <exception>
#include "./timer.hpp"

namespace darwin {
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
}
#define Darwin_Log(msg) darwin::debugger::log(__FILE__,__LINE__,__func__,msg);
#define Darwin_Warning(msg) darwin::debugger::warning(__FILE__,__LINE__,__func__,msg);
#define Darwin_Error(msg) darwin::debugger::error(__FILE__,__LINE__,__func__,msg);
#define Darwin_Set_Log_Path(path) darwin::debugger::log_path(path);
#else
}
#define Darwin_Log(msg)
#define Darwin_Warning(msg)
#define Darwin_Error(msg) std::terminate();
#define Darwin_Set_Log_Path(path)
#endif
