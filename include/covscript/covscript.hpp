#pragma once
/*
* Covariant Script Programming Language
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
* Copyright (C) 2017-2023 Michael Lee(李登淳)
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
#include <initializer_list>

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
			return val.val<T>();
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

		explicit function_invoker(cs::var func) : m_func(std::move(func)) {}

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
	std::string process_path(const std::string &);

	std::string get_import_path();

	void prepend_import_path(const std::string &script, cs::process_context *context);

	array parse_cmd_args(int, char *[]);

	void collect_garbage();

	void collect_garbage(context_t &);

	class raii_collector final {
		context_t context;
	public:
		raii_collector() = delete;

		raii_collector(const raii_collector &) = delete;

		raii_collector(raii_collector &&) noexcept = delete;

		explicit raii_collector(context_t cxt) : context(std::move(cxt)) {}

		~raii_collector()
		{
			collect_garbage(context);
		}
	};

	cs::var eval(const context_t &, const std::string &);

	using cs_function_invoker_impl::function_invoker;

	class bootstrap final {
	public:
		context_t context;

		// Bootstrap from string initializer list
		bootstrap(std::initializer_list<std::string> l) : context(create_context({l.begin(), l.end()})) {}

		// Classic bootstrap from command line
		bootstrap(int argc, char *argv[]) : context(create_context(parse_cmd_args(argc, argv))) {}

		// Zero initialization bootstrap
		bootstrap() : context(create_context({"<BOOTSTRAP_ENV>"})) {}

		~bootstrap()
		{
			collect_garbage(context);
		}

		void run(const std::string &path)
		{
			context->instance->compile(path);
			context->instance->interpret();
		}
	};
}
