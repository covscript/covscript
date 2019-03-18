#pragma once
/*
* Covariant Script Programming Language
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
* Website: http://covscript.org
*/
#include <covscript/impl/impl.hpp>

namespace cs_function_invoker_impl {
	template<typename T>
	struct convert_helper {
		static inline const T &get_val(const cs::var &val)
		{
			return val.const_val<T>();
		}
	};

	template<typename T>
	struct convert_helper<const T &> {
		static inline const T &get_val(const cs::var &val)
		{
			return val.const_val<T>();
		}
	};

	template<typename T>
	struct convert_helper<T &> {
		static inline T &get_val(const cs::var &val)
		{
			return val.val<T>(true);
		}
	};

	template<>
	struct convert_helper<void> {
		static inline void get_val(const cs::var &) {}
	};

	template<typename>
	class function_invoker;

	template<typename RetT, typename...ArgsT>
	class function_invoker<RetT(ArgsT...)> {
		cs::var m_func;
	public:
		function_invoker() = default;

		function_invoker(const function_invoker &) = default;

		function_invoker &operator=(const function_invoker &) = default;

		explicit function_invoker(const cs::var &func) : m_func(func) {}

		void assign(const cs::var &func)
		{
			m_func = func;
		}

		cs::var target() const
		{
			return m_func;
		}

		template<typename...ElementT>
		RetT operator()(ElementT &&...args) const
		{
			return convert_helper<RetT>::get_val(cs::invoke(m_func, cs_impl::type_convertor<ElementT, ArgsT>::convert(
			        std::forward<ElementT>(args))...));
		}
	};
}

namespace cs {
	std::string get_sdk_path();

	std::string process_path(const std::string &);

	std::string get_import_path();

	array parse_cmd_args(int, const char *[]);

	context_t create_context(const array &);

	context_t create_subcontext(const context_t &);

	cs::var eval(const context_t &, const std::string &);

	using cs_function_invoker_impl::function_invoker;
}