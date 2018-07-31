#pragma once
/*
* Covariant Script C/C++ Native Interface
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
#include <covscript/mozart/traits.hpp>
#include <covscript/mozart/bind.hpp>
#include <covscript/core.hpp>

namespace cs {
	class arglist final {
		template<typename T, int index>
		struct check_arg {
			static inline short check(const var &val)
			{
				if (typeid(T) != val.type())
					throw syntax_error("Invalid Argument.At " + std::to_string(index + 1) + ".Expected " +
					                   cs_impl::get_name_of_type<T>() + ",provided " + val.get_type_name());
				else
					return 0;
			}
		};

		static inline void result_container(short...) {}

		template<typename...ArgsT, int...Seq>
		static inline void check_helper(const vector &args, const cov::sequence<Seq...> &)
		{
			result_container(
			    check_arg<typename cov::remove_constant<typename cov::remove_reference<ArgsT>::type>::type, Seq>::check(
			        args[Seq])...);
		}

	public:
		template<typename...ArgTypes>
		static inline void check(const vector &args)
		{
			if (sizeof...(ArgTypes) == args.size())
				check_helper<ArgTypes...>(args, cov::make_sequence<sizeof...(ArgTypes)>::result);
			else
				throw syntax_error(
				    "Wrong size of the arguments.Expected " + std::to_string(sizeof...(ArgTypes)) + ",provided " +
				    std::to_string(args.size()));
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

	template<typename T, typename...RealArgsT>
	class cni_helper;

	template<>
	class cni_helper<void (*)()> {
		std::function<void()> mFunc;
	public:
		cni_helper() = delete;

		cni_helper(const cni_helper &) = default;

		cni_helper(const std::function<void()> &func) : mFunc(func) {}

		var call(vector &args) const
		{
			if (!args.empty())
				throw syntax_error("Wrong size of the arguments.Expected 0");
			mFunc();
			return null_pointer;
		}
	};

	template<typename RetT>
	class cni_helper<RetT(*)()> {
		std::function<RetT()> mFunc;
	public:
		cni_helper() = delete;

		cni_helper(const cni_helper &) = default;

		cni_helper(const std::function<RetT()> &func) : mFunc(func) {}

		var call(vector &args) const
		{
			if (!args.empty())
				throw syntax_error("Wrong size of the arguments.Expected 0");
			return std::move(mFunc());
		}
	};

	template<typename...ArgsT, typename...RealArgsT>
	class cni_helper<void (*)(ArgsT...), RealArgsT...> {
		using args_t=typename cov::type_list::make<ArgsT...>::result;
		std::function<void(ArgsT...)> mFunc;

		template<int...S>
		void _call(vector &args, const cov::sequence<S...> &) const
		{
			mFunc(convert<RealArgsT>::get_val(args[S])...);
		}

	public:
		cni_helper() = delete;

		cni_helper(const cni_helper &) = default;

		cni_helper(const std::function<void(ArgsT...)> &func) : mFunc(func) {}

		var call(vector &args) const
		{
			arglist::check<RealArgsT...>(args);
			_call(args, cov::make_sequence<cov::type_list::get_size<args_t>::result>::result);
			return null_pointer;
		}
	};

	template<typename RetT, typename...ArgsT, typename...RealArgsT>
	class cni_helper<RetT(*)(ArgsT...), RealArgsT...> {
		using args_t=typename cov::type_list::make<ArgsT...>::result;
		std::function<RetT(ArgsT...)> mFunc;

		template<int...S>
		RetT _call(vector &args, const cov::sequence<S...> &) const
		{
			return std::move(mFunc(convert<RealArgsT>::get_val(args[S])...));
		}

	public:
		cni_helper() = delete;

		cni_helper(const cni_helper &) = default;

		cni_helper(const std::function<RetT(ArgsT...)> &func) : mFunc(func) {}

		var call(vector &args) const
		{
			arglist::check<RealArgsT...>(args);
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

	template<typename...ArgsT>struct cni_args{};

	class cni final {
		class cni_base {
		public:
			cni_base() = default;

			cni_base(const cni_base &) = default;

			virtual ~cni_base() = default;

			virtual cni_base *clone()=0;

			virtual var call(vector &) const =0;
		};

		template<typename T, typename...ArgsT>
		class cni_holder final : public cni_base {
			cni_helper<typename cov::function_parser<T>::type::common_type, ArgsT...> mCni;
		public:
			cni_holder() = delete;

			cni_holder(const cni_holder &) = default;

			cni_holder(const T &func) : mCni(func) {}

			virtual ~cni_holder() = default;

			virtual cni_base *clone() override
			{
				return new cni_holder(*this);
			}

			virtual var call(vector &args) const override
			{
				return mCni.call(args);
			}
		};

		template<typename T>
		struct construct_helper {
			template<typename X,typename RetT,typename...ArgsT>
			static cni_base *_construct(X &&val, RetT(*)(ArgsT...))
			{
				return new cni_holder<T,ArgsT...>(std::forward<X>(val));
			}
			template<typename X>
			static cni_base *construct(X &&val)
			{
				return _construct(std::forward<X>(val),typename cov::function_parser<T>::type::common_type(nullptr));
			}
		};

		template<typename RetT,typename...ArgsT>
		static constexpr int count_args_size(RetT(*)(ArgsT...))
		{
			return sizeof...(ArgsT);
		}

		static void result_container(short...) {}

		template<typename _From, typename _To>
		static short _check_conversion()
		{
			static_assert(cov::castable<_From,_To>::value);
		}

		template<typename RetT,typename...ArgsT,typename...RealArgsT>
		static void check_conversion(RetT(*)(ArgsT...),cni_args<RealArgsT...>)
		{
			result_container(_check_conversion<ArgsT,RealArgsT>()...);
		}

		cni_base *mCni = nullptr;
	public:
		cni() = delete;

		cni(const cni &c) : mCni(c.mCni->clone()) {}

		template<typename T>
		cni(T &&val):mCni(construct_helper<typename cni_modify<typename cov::remove_reference<T>::type>::type>::construct(std::forward<T>(val))) {}

		template<typename T, typename...ArgsT>
		cni(T &&val, cni_args<ArgsT...> args):mCni(new cni_holder<typename cni_modify<typename cov::remove_reference<T>::type>::type, ArgsT...>(std::forward<T>(val))) {
			using function_type=typename cov::function_parser<typename cni_modify<typename cov::remove_reference<T>::type>::type>::type::common_type;
			static_assert(sizeof...(ArgsT)==count_args_size(function_type(nullptr)));
			check_conversion(function_type(nullptr),args);
		}

		~cni()
		{
			delete mCni;
		}

		var operator()(vector &args) const
		{
			try {
				return try_move(mCni->call(args));
			}
			catch (const lang_error &e) {
				exception_handler::cs_eh_callback(e);
			}
			catch (const std::exception &e) {
				exception_handler::std_eh_callback(e);
			}
			catch (...) {
				exception_handler::std_eh_callback(fatal_error("CNI:Unrecognized exception."));
			}
			return null_pointer;
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
