#pragma once
/*
* Covariant Mozart Utility Library: Tuple
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
* Version: 17.1.1
*/
#include "./base.hpp"
#include "./traits.hpp"

namespace cov {
	template<typename, typename...>
	struct tuple_type_iterator;
	template<int, typename...>
	struct tuple_random_iterator;

	template<typename...>
	class tuple;

	template<typename _Arg, typename _Tp>
	struct tuple_type_iterator<_Arg, _Tp> {
		static typename cov::add_reference<_Arg>::type get(tuple<_Tp> &t)
		{
			static_assert(cov::is_same_type<_Arg, _Tp>::value, "E000B");
		}

		static typename cov::add_constant_reference<_Arg>::type get(const tuple<_Tp> &t)
		{
			static_assert(cov::is_same_type<_Arg, _Tp>::value, "E000B");
		}
	};

	template<typename _Tp>
	struct tuple_type_iterator<_Tp, _Tp> {
		static typename cov::add_reference<_Tp>::type get(tuple<_Tp> &t)
		{
			return t.get_current();
		}

		static typename cov::add_constant_reference<_Tp>::type get(const tuple<_Tp> &t)
		{
			return t.get_current();
		}
	};

	template<typename _Arg, typename _Tp, typename..._ArgsT>
	struct tuple_type_iterator<_Arg, _Tp, _ArgsT...> {
		static typename cov::add_reference<_Arg>::type get(tuple<_Tp, _ArgsT...> &t)
		{
			return tuple_type_iterator<_Arg, _ArgsT...>::get(t.get_forward());
		}

		static typename cov::add_constant_reference<_Arg>::type get(const tuple<_Tp, _ArgsT...> &t)
		{
			return tuple_type_iterator<_Arg, _ArgsT...>::get(t.get_forward());
		}
	};

	template<typename _Tp, typename..._ArgsT>
	struct tuple_type_iterator<_Tp, _Tp, _ArgsT...> {
		static typename cov::add_reference<_Tp>::type get(tuple<_Tp, _ArgsT...> &t)
		{
			return t.get_current();
		}

		static typename cov::add_constant_reference<_Tp>::type get(const tuple<_Tp, _ArgsT...> &t)
		{
			return t.get_current();
		}
	};

	template<typename _Tp, typename..._ArgsT>
	struct tuple_random_iterator<0, _Tp, _ArgsT...> {
		typedef _Tp type;

		static typename cov::add_reference<type>::type get(tuple<_Tp, _ArgsT...> &t)
		{
			return t.get_current();
		}

		static typename cov::add_constant_reference<type>::type get(const tuple<_Tp, _ArgsT...> &t)
		{
			return t.get_current();
		}
	};

	template<int N, typename _Tp, typename..._ArgsT>
	struct tuple_random_iterator<N, _Tp, _ArgsT...> {
		typedef typename tuple_random_iterator<N - 1, _ArgsT...>::type type;

		static typename cov::add_reference<type>::type get(tuple<_Tp, _ArgsT...> &t)
		{
			return tuple_random_iterator<N - 1, _ArgsT...>::get(t.get_forward());
		}

		static typename cov::add_constant_reference<type>::type get(const tuple<_Tp, _ArgsT...> &t)
		{
			return tuple_random_iterator<N - 1, _ArgsT...>::get(t.get_forward());
		}
	};

	template<typename _Tp>
	class tuple<_Tp> {
	protected:
		_Tp mCurrent;
	public:
		tuple() = default;

		explicit tuple(const _Tp &val) : mCurrent(val) {}

		~tuple() = default;

		_Tp &get_current()
		{
			return mCurrent;
		}

		const _Tp &get_current() const
		{
			return mCurrent;
		}

		template<int N>
		typename cov::add_reference<typename tuple_random_iterator<N, _Tp>::type>::type get()
		{
			return tuple_random_iterator<N, _Tp>::get(*this);
		}

		template<int N>
		typename cov::add_constant_reference<typename tuple_random_iterator<N, _Tp>::type>::type get() const
		{
			return tuple_random_iterator<N, _Tp>::get(*this);
		}

		template<typename _Arg>
		typename cov::add_reference<_Arg>::type get()
		{
			return tuple_type_iterator<_Arg, _Tp>::get(*this);
		}

		template<typename _Arg>
		typename cov::add_constant_reference<_Arg>::type get() const
		{
			return tuple_type_iterator<_Arg, _Tp>::get(*this);
		}
	};

	template<typename _Tp, typename..._ArgsT>
	class tuple<_Tp, _ArgsT...> {
	protected:
		_Tp mCurrent;
		tuple<_ArgsT...> mForward;
	public:
		tuple() = default;

		template<typename T, typename...Args>
		explicit tuple(const T &val, Args &&...args):mCurrent(val), mForward(std::forward<Args>(args)...) {}

		~tuple() = default;

		_Tp &get_current()
		{
			return mCurrent;
		}

		const _Tp &get_current() const
		{
			return mCurrent;
		}

		tuple<_ArgsT...> &get_forward()
		{
			return mForward;
		}

		const tuple<_ArgsT...> &get_forward() const
		{
			return mForward;
		}

		template<int N>
		typename cov::add_reference<typename tuple_random_iterator<N, _Tp, _ArgsT...>::type>::type get()
		{
			return tuple_random_iterator<N, _Tp, _ArgsT...>::get(*this);
		}

		template<int N>
		typename cov::add_constant_reference<typename tuple_random_iterator<N, _Tp, _ArgsT...>::type>::type
		get() const
		{
			return tuple_random_iterator<N, _Tp, _ArgsT...>::get(*this);
		}

		template<typename _Arg>
		typename cov::add_reference<_Arg>::type get()
		{
			return tuple_type_iterator<_Arg, _Tp, _ArgsT...>::get(*this);
		}

		template<typename _Arg>
		typename cov::add_constant_reference<_Arg>::type get() const
		{
			return tuple_type_iterator<_Arg, _Tp, _ArgsT...>::get(*this);
		}
	};

	template<typename..._ArgsT>
	tuple<_ArgsT...> make_tuple(typename cov::add_constant_reference<_ArgsT>::type...args)
	{
		return tuple<_ArgsT...>(std::forward<_ArgsT>(args)...);
	}
}
