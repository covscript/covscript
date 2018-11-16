#pragma once
/*
* Covariant Script Typedef
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
* Copyright (C) 2018 Michael Lee(李登淳)
* Email: mikecovlee@163.com
* Github: https://github.com/mikecovlee
*/

namespace cs_impl {
	class any;
}
namespace cs {
	class translator_type;

	class compiler_type;

	class instance_type;

	class runtime_type;

	class context_type;

	class token_base;

	class statement_base;

	class callable;

	class name_space;

	class name_space_holder;

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
	using context_t=std::shared_ptr<context_type>;
	using extension=name_space;
	using extension_holder=name_space_holder;
	using domain_t=std::shared_ptr<map_t<string, var>>;
	using name_space_t=std::shared_ptr<name_space_holder>;
	using extension_t=std::shared_ptr<extension_holder>;
	using istream=std::shared_ptr<std::istream>;
	using ostream=std::shared_ptr<std::ostream>;

	typedef void(*cs_exception_handler)(const lang_error &);

	typedef void(*std_exception_handler)(const std::exception &);

	typedef name_space *(*extension_entrance_t)(int *, cs_exception_handler, std_exception_handler);
}
