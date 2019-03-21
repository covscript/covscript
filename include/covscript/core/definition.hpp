#pragma once
/*
* Covariant Script Definition
*
* Licensed under the Covariant Innovation General Public License,
* Version 1.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* https://covariant.cn/licenses/LICENSE-1.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
* Copyright (C) 2019 Michael Lee(李登淳)
* Email: mikecovlee@163.com
* Github: https://github.com/mikecovlee
*/

#if defined(_WIN32) || defined(WIN32)
#define COVSCRIPT_PLATFORM_WIN32
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

	class name_space;

	template<typename _kT, typename _vT> using map_t=spp::sparse_hash_map<_kT, _vT>;
	template<typename _Tp> using set_t=spp::sparse_hash_set<_Tp>;
	using var=cs_impl::any;
	using number=long double;
	using boolean=bool;
	using string=std::string;
	using list=std::list<var>;
	using array=std::deque<var>;
	using pair=std::pair<var, var>;
	using hash_map=map_t<var, var>;
	using vector=std::vector<var>;
	using expression_t=cov::tree<token_base *>;
	using compiler_t=std::shared_ptr<compiler_type>;
	using instance_t=std::shared_ptr<instance_type>;
	using context_t=std::shared_ptr<context_type>;
	using domain_t=std::shared_ptr<map_t<string, var>>;
	using namespace_t=std::shared_ptr<name_space>;
	using istream=std::shared_ptr<std::istream>;
	using ostream=std::shared_ptr<std::ostream>;

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