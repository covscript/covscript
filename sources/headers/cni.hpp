#pragma once
/*
* Covariant Script C/C++ Native Interface
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
#include "../../include/mozart/bind.hpp"
#include "../../include/mozart/traits.hpp"
#include "core.hpp"

namespace cs {
	class arglist final {
		template<typename T, int index>
		struct check_arg {
			static inline short check(const var &val)
			{
				if (typeid(T) != val.type())
					throw syntax_error("Invalid Argument.At " + std::to_string(index + 1) + ".Expected " + cs_impl::get_name_of_type<T>());
				else
					return 0;
			}
		};

		static inline void result_container(short...) {}

		template<typename...ArgsT, int...Seq>
		static inline void check_helper(const std::deque<var> &args, const cov::sequence<Seq...> &)
		{
			result_container(check_arg<typename cov::remove_constant<typename cov::remove_reference<ArgsT>::type>::type, Seq>::check(args[Seq])...);
		}

	public:
		template<typename...ArgTypes>
		static inline void check(const std::deque<var> &args)
		{
			if (sizeof...(ArgTypes) == args.size())
				check_helper<ArgTypes...>(args, cov::make_sequence<sizeof...(ArgTypes)>::result);
			else
				throw syntax_error("Wrong size of the arguments.Expected " + std::to_string(sizeof...(ArgTypes)));
		}
	};

	template<int index>
	struct arglist::check_arg<var, index> {
		static inline short check(const var &)
		{
			return 0;
		}
	};

	template<typename T>
	struct convert {
		static inline const T &get_val(var &val)
		{
			return val.const_val<T>();
		}
	};

	template<typename T>
	struct convert<const T &> {
		static inline const T &get_val(var &val)
		{
			return val.const_val<T>();
		}
	};

	template<typename T>
	struct convert<T &> {
		static inline T &get_val(var &val)
		{
			return val.val<T>(true);
		}
	};

	template<>
	struct convert<const var &> {
		static inline const var &get_val(const var &val)
		{
			return val;
		}
	};

	template<>
	struct convert<var &> {
		static inline var &get_val(var &val)
		{
			return val;
		}
	};

	template<typename T>
	class cni_helper;

	template<>
	class cni_helper<void (*)()> {
		std::function<void()> mFunc;
	public:
		cni_helper() = delete;

		cni_helper(const cni_helper &) = default;

		cni_helper(const std::function<void()> &func) : mFunc(func) {}

		var call(array &args) const
		{
			if (!args.empty())
				throw syntax_error("Wrong size of the arguments.Expected 0");
			mFunc();
			return var::make<number>(0);
		}
	};

	template<typename RetT>
	class cni_helper<RetT(*)()> {
		std::function<RetT()> mFunc;
	public:
		cni_helper() = delete;

		cni_helper(const cni_helper &) = default;

		cni_helper(const std::function<RetT()> &func) : mFunc(func) {}

		var call(array &args) const
		{
			if (!args.empty())
				throw syntax_error("Wrong size of the arguments.Expected 0");
			return std::move(mFunc());
		}
	};

	template<typename...ArgsT>
	class cni_helper<void (*)(ArgsT...)> {
		using args_t=typename cov::type_list::make<ArgsT...>::result;
		std::function<void(ArgsT...)> mFunc;

		template<int...S>
		void _call(array &args, const cov::sequence<S...> &) const
		{
			mFunc(convert<ArgsT>::get_val(args[S])...);
		}

	public:
		cni_helper() = delete;

		cni_helper(const cni_helper &) = default;

		cni_helper(const std::function<void(ArgsT...)> &func) : mFunc(func) {}

		var call(array &args) const
		{
			arglist::check<ArgsT...>(args);
			_call(args, cov::make_sequence<cov::type_list::get_size<args_t>::result>::result);
			return var::make<number>(0);
		}
	};

	template<typename RetT, typename...ArgsT>
	class cni_helper<RetT(*)(ArgsT...)> {
		using args_t=typename cov::type_list::make<ArgsT...>::result;
		std::function<RetT(ArgsT...)> mFunc;

		template<int...S>
		RetT _call(array &args, const cov::sequence<S...> &) const
		{
			return std::move(mFunc(convert<ArgsT>::get_val(args[S])...));
		}

	public:
		cni_helper() = delete;

		cni_helper(const cni_helper &) = default;

		cni_helper(const std::function<RetT(ArgsT...)> &func) : mFunc(func) {}

		var call(array &args) const
		{
			arglist::check<ArgsT...>(args);
			return std::move(_call(args, cov::make_sequence<cov::type_list::get_size<args_t>::result>::result));
		}
	};

	template<typename T>
	struct cni_modify {
		using type=T;
	};
	template<typename RetT, typename...ArgsT>
	struct cni_modify<RetT(ArgsT...)> {
		using type=RetT(*)(ArgsT...);
	};

	class cni final {
		class cni_base {
		public:
			cni_base() = default;

			cni_base(const cni_base &) = default;

			virtual ~cni_base() = default;

			virtual cni_base *clone()=0;

			virtual var call(array &) const =0;
		};

		template<typename T>
		class cni_holder final : public cni_base {
			cni_helper<typename cov::function_parser<T>::type::common_type> mCni;
		public:
			cni_holder() = delete;

			cni_holder(const cni_holder &) = default;

			cni_holder(const T &func) : mCni(func) {}

			virtual ~cni_holder() = default;

			virtual cni_base *clone() override
			{
				return new cni_holder(*this);
			}

			virtual var call(array &args) const override
			{
				return mCni.call(args);
			}
		};

		template<typename T>
		struct construct_helper {
			template<typename X>
			static cni_base *construct(X &&val)
			{
				return new cni_holder<T>(std::forward<X>(val));
			}
		};

		cni_base *mCni = nullptr;
	public:
		cni() = delete;

		cni(const cni &c) : mCni(c.mCni->clone()) {}

		template<typename T>
		cni(T &&val):mCni(construct_helper<typename cni_modify<typename cov::remove_reference<T>::type>::type>::construct(std::forward<T>(val))) {}

		~cni()
		{
			delete mCni;
		}

		var operator()(array &args) const
		{
			return mCni->call(args);
		}
	};

	template<>
	struct cni::construct_helper<const cni> {
		static cni_base *construct(const cni &c)
		{
			return c.mCni->clone();
		}
	};

	template<>
	struct cni::construct_helper<cni> {
		static cni_base *construct(const cni &c)
		{
			return c.mCni->clone();
		}
	};
}
