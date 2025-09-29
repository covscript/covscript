#pragma once
/*
* Covariant Script Definition
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

// OS Detection

#if defined(_WIN32) || defined(_WIN64)
#define COVSCRIPT_PLATFORM_WIN32
#ifdef _UCRT
#define COVSCRIPT_PLATFORM_NAME "windows-ucrt"
#else
#define COVSCRIPT_PLATFORM_NAME "windows-msvcrt"
#endif
#define COVSCRIPT_PLATFORM_HOME "C:\\Program Files (x86)\\CovScript"
#elif defined(__ANDROID__)
#define COVSCRIPT_PLATFORM_ANDROID
#define COVSCRIPT_PLATFORM_LINUX
#define COVSCRIPT_PLATFORM_UNIX
#define COVSCRIPT_PLATFORM_NAME "android"
#elif defined(__linux__) || defined(__gnu_linux__)
#define COVSCRIPT_PLATFORM_LINUX
#define COVSCRIPT_PLATFORM_UNIX
#define COVSCRIPT_PLATFORM_NAME "linux"
#ifndef COVSCRIPT_PLATFORM_HOME
#define COVSCRIPT_PLATFORM_HOME "/usr/share/covscript"
#endif
#elif defined(__APPLE__) && defined(__MACH__)
#define COVSCRIPT_PLATFORM_DARWIN
#define COVSCRIPT_PLATFORM_UNIX
#define COVSCRIPT_PLATFORM_NAME "macos"
#ifndef COVSCRIPT_PLATFORM_HOME
#define COVSCRIPT_PLATFORM_HOME "/Applications/CovScript.app/Contents/MacOS/covscript"
#endif
#elif defined(__FreeBSD__)
#define COVSCRIPT_PLATFORM_FREEBSD
#define COVSCRIPT_PLATFORM_UNIX
#define COVSCRIPT_PLATFORM_NAME "freebsd"
#ifndef COVSCRIPT_PLATFORM_HOME
#define COVSCRIPT_PLATFORM_HOME "/usr/local/share/covscript"
#endif
#else
#define COVSCRIPT_PLATFORM_UNKNOWN
#warning Platform detection failed. Using unix mode as fallback.
#define COVSCRIPT_PLATFORM_UNIX
#ifndef COVSCRIPT_PLATFORM_NAME
#error COVSCRIPT_PLATFORM_NAME not defined. Can not detect automatically.
#endif
#ifndef COVSCRIPT_PLATFORM_HOME
#error COVSCRIPT_PLATFORM_HOME not defined. Can not detect automatically.
#endif
#endif

// Compiler Detection

#if defined(__clang__)
#define COVSCRIPT_COMPILER_CLANG
#define COVSCRIPT_COMPILER_SIMPLE_NAME "clang"
#elif defined(__GNUC__)
#define COVSCRIPT_COMPILER_GNUC
#define COVSCRIPT_COMPILER_SIMPLE_NAME "gcc"
#elif defined(_MSC_VER)
#define COVSCRIPT_COMPILER_MSVC
#define COVSCRIPT_COMPILER_SIMPLE_NAME "msvc"
#else
#define COVSCRIPT_COMPILER_UNKNOWN
#define COVSCRIPT_COMPILER_SIMPLE_NAME "unknown"
#endif

// MinGW suffix

#if defined(__MINGW64__)
#undef COVSCRIPT_COMPILER_NAME
#define COVSCRIPT_COMPILER_NAME COVSCRIPT_COMPILER_SIMPLE_NAME "-mingw-w64"
#define COVSCRIPT_COMPILER_MINGW
#define COVSCRIPT_COMPILER_MINGW64
#elif defined(__MINGW32__)
#undef COVSCRIPT_COMPILER_NAME
#define COVSCRIPT_COMPILER_NAME COVSCRIPT_COMPILER_SIMPLE_NAME "-mingw"
#define COVSCRIPT_COMPILER_MINGW
#define COVSCRIPT_COMPILER_MINGW32
#endif

#ifndef COVSCRIPT_COMPILER_NAME
#define COVSCRIPT_COMPILER_NAME COVSCRIPT_COMPILER_SIMPLE_NAME
#endif

// Architecture Detection

#if defined(__x86_64__) || defined(_M_X64)
#define COVSCRIPT_ARCH_AMD64
#define COVSCRIPT_ARCH_NAME "amd64"
#elif defined(__i386) || defined(_M_IX86)
#define COVSCRIPT_ARCH_I386
#define COVSCRIPT_ARCH_NAME "i386"
#elif defined(__aarch64__) || defined(_M_ARM64)
#define COVSCRIPT_ARCH_ARM64
#ifdef COVSCRIPT_PLATFORM_LINUX
#define COVSCRIPT_ARCH_NAME "aarch64"
#else
#define COVSCRIPT_ARCH_NAME "arm64"
#endif
#elif defined(__arm__) || defined(_M_ARM)
#define COVSCRIPT_ARCH_ARM32
#define COVSCRIPT_ARCH_NAME "arm"
#elif defined(__riscv) && (__riscv_xlen == 64)
#define COVSCRIPT_ARCH_RISCV64
#define COVSCRIPT_ARCH_NAME "riscv64"
#elif defined(__riscv) && (__riscv_xlen == 32)
#define COVSCRIPT_ARCH_RISCV32
#define COVSCRIPT_ARCH_NAME "riscv32"
#elif defined(__loongarch__)
#if defined(__loongarch64)
#define COVSCRIPT_ARCH_LOONGARCH64
#define COVSCRIPT_ARCH_NAME "loongarch64"
#else
#define COVSCRIPT_ARCH_LOONGARCH32
#define COVSCRIPT_ARCH_NAME "loongarch32"
#endif
#elif defined(__mips__)
#if defined(__mips64) || defined(_LP64)
#define COVSCRIPT_ARCH_MIPS64
#define COVSCRIPT_ARCH_NAME "mips64"
#else
#define COVSCRIPT_ARCH_MIPS32
#define COVSCRIPT_ARCH_NAME "mips32"
#endif
#else
#define COVSCRIPT_ARCH_UNKNOWN
#ifndef COVSCRIPT_ARCH_NAME
#error COVSCRIPT_ARCH_NAME not defined. Can not detect automatically.
#endif
#endif

// Types

namespace cs_impl {
	class any;

	class cni;
}
namespace cs {
	class compiler_type;

	class instance_type;

	class runtime_type;

	class context_type;

	class token_base;

	class statement_base;

	class callable;

	class fiber_type;

	class domain_type;

	class name_space;

#ifndef CS_COMPATIBILITY_MODE
	template<typename _kT, typename _vT> using map_t = phmap::flat_hash_map<_kT, _vT>;
	template<typename _Tp> using set_t = phmap::flat_hash_set<_Tp>;
#else
	template<typename _kT, typename _vT> using map_t = std::unordered_map<_kT, _vT>;
	template<typename _Tp> using set_t = std::unordered_set<_Tp>;
#endif
	using var = cs_impl::any;
	using boolean = bool;
	using string = std::string;
	using list = std::list<var>;
	using array = std::deque<var>;
	using pair = std::pair<var, var>;
	using hash_set = set_t<var>;
	using hash_map = map_t<var, var>;
	using vector = std::vector<var>;
	using expression_t = tree_type<token_base *>;
	using compiler_t = std::shared_ptr<compiler_type>;
	using instance_t = std::shared_ptr<instance_type>;
	using context_t = std::shared_ptr<context_type>;
	using fiber_t = std::shared_ptr<fiber_type>;
	using domain_t = std::shared_ptr<domain_type>;
	using namespace_t = std::shared_ptr<name_space>;
	using char_buff = std::shared_ptr<std::stringstream>;
	using istream = std::shared_ptr<std::istream>;
	using ostream = std::shared_ptr<std::ostream>;

// Compatible declaration since 210503
	using number = numeric_float;

	typedef void(*cs_exception_handler)(const lang_error &);

	typedef void(*std_exception_handler)(const std::exception &);

// Path seperator and delimiter
#ifdef COVSCRIPT_PLATFORM_WIN32
	constexpr char path_separator = '\\';
	constexpr char path_delimiter = ';';
#else
	constexpr char path_separator = '/';
	constexpr char path_delimiter = ':';
#endif

	constexpr char copyright_info[] =
	    "Licensed under the Apache License, Version 2.0 (the \"License\");\n"
	    "you may not use this file except in compliance with the License.\n"
	    "You may obtain a copy of the License at\n"
	    "\nhttp://www.apache.org/licenses/LICENSE-2.0\n\n"
	    "Unless required by applicable law or agreed to in writing, software\n"
	    "distributed under the License is distributed on an \"AS IS\" BASIS,\n"
	    "WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.\n"
	    "See the License for the specific language governing permissions and\n"
	    "limitations under the License.\n"
	    "\nCopyright (C) 2017-2025 Michael Lee\n\n"
	    "This software is registered with the National Copyright Administration\n"
	    "of the People's Republic of China(Registration Number: 2020SR0408026)\n"
	    "and is protected by the Copyright Law of the People's Republic of China.\n"
	    "\nPlease visit http://covscript.org.cn for more information.";
}

// Debugger Hooks

#ifdef CS_DEBUGGER
void cs_debugger_step_callback(cs::statement_base*);
void cs_debugger_func_breakpoint(const std::string&, const cs::var&);
void cs_debugger_func_callback(const std::string&, cs::statement_base*);
#define CS_DEBUGGER_STEP(STMT) cs_debugger_step_callback(STMT)
#else
#define CS_DEBUGGER_STEP(STMT)
#endif
