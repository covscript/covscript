#pragma once
/*
* Covariant Mozart Utility Library: Bind
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
*
* Version: 17.1.0
*/
#include "./base.hpp"
#include "./tuple.hpp"
#include "./function.hpp"
#include "./typelist.hpp"

namespace cov {
	template<int...S>
	struct sequence {
	};
	template<int N, int...S>
	struct make_sequence : make_sequence<N - 1, N - 1, S...> {
	};
	template<int...S>
	struct make_sequence<0, S...> {
		static constexpr sequence<S...> result = sequence<S...>();
	};
	template<int...S> constexpr sequence<S...> make_sequence<0, S...>::result;
	template<int N>
	struct placeholder {
	};

	template<typename T, typename...Args>
	T &select(T &dat, cov::tuple<Args...> &t)
	{
		return dat;
	}

	template<int N, typename...Args>
	auto select(placeholder<N> &ph,
	            cov::tuple<Args...> &t) -> typename cov::add_reference<typename cov::tuple_random_iterator<
	N - 1, Args...>::type>::type
	{
		return cov::tuple_random_iterator<N - 1, Args...>::get(t);
	}

	template<typename T>
	struct resolver;
	template<typename rT, typename...ArgsT>
	struct resolver<rT(*)(ArgsT...)> {
		using return_type=rT;
		using args_type=cov::tuple<ArgsT...>;
	};

	template<int N, typename...Args>
	typename cov::add_reference<typename cov::tuple_random_iterator<N, Args...>::type>::type
	get(cov::tuple<Args...> &t)
	{
		return cov::tuple_random_iterator<N, Args...>::get(t);
	}

	template<typename F, typename...Args>
	class bind_t {
		using func_t=cov::function<typename cov::function_parser<F>::type::common_type>;
		using args_t=typename cov::type_list::make<Args...>::result;
		F mFunc;
		cov::tuple<Args...> mArgs;

		template<typename...ArgsT, int...S>
		typename resolver<typename cov::function_parser<F>::type::common_type>::return_type
		_call(cov::tuple<ArgsT...> &t, sequence<S...>)
		{
			return mFunc(select(get<S>(mArgs), t)...);
		}

		template<int...S>
		typename resolver<typename cov::function_parser<F>::type::common_type>::return_type _call(sequence<S...>)
		{
			return mFunc(get<S>(mArgs)...);
		}

	public:
		explicit bind_t(F func, Args &&...args) : mFunc(func), mArgs(std::forward<Args>(args)...) {}

		template<typename...ArgsT>
		typename resolver<typename cov::function_parser<F>::type::common_type>::return_type
		operator()(ArgsT &&...args)
		{
			cov::tuple<ArgsT...> t(std::forward<ArgsT>(args)...);
			return _call(t, make_sequence<cov::type_list::get_size<args_t>::result>::result);
		}

		typename resolver<typename cov::function_parser<F>::type::common_type>::return_type operator()()
		{
			return _call(make_sequence<cov::type_list::get_size<args_t>::result>::result);
		}
	};

	template<typename F, typename...Args>
	bind_t<F, Args...> bind(F func, Args &&...args)
	{
		return bind_t<F, Args...>(func, std::forward<Args>(args)...);
	}

	extern placeholder<1> _1;
	extern placeholder<2> _2;
	extern placeholder<3> _3;
	extern placeholder<4> _4;
	extern placeholder<5> _5;
	extern placeholder<6> _6;
	extern placeholder<7> _7;
	extern placeholder<8> _8;
	extern placeholder<9> _9;
	extern placeholder<10> _10;
}
