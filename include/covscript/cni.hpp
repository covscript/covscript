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

	template<typename _Target, typename _Source>
	class cni_helper;

	template<>
	class cni_helper<void (*)(), void (*)()> {
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

	template<typename _Target_RetT, typename _Source_RetT>
	class cni_helper<_Target_RetT(*)(),_Source_RetT(*)()> {
		std::function<_Target_RetT()> mFunc;
	public:
		cni_helper() = delete;

		cni_helper(const cni_helper &) = default;

		cni_helper(const std::function<_Target_RetT()> &func) : mFunc(func) {}

		var call(vector &args) const
		{
			if (!args.empty())
				throw syntax_error("Wrong size of the arguments.Expected 0");
			return var::make<_Source_RetT>(std::move(cs_impl::type_convertor<_Target_RetT,_Source_RetT>::convert(mFunc())));
		}
	};

	template<typename..._Target_ArgsT, typename..._Source_ArgsT>
	class cni_helper<void (*)(_Target_ArgsT...), void (*)(_Source_ArgsT...)> {
		std::function<void(_Target_ArgsT...)> mFunc;

		template<int...S>
		void _call(vector &args, const cov::sequence<S...> &) const
		{
			mFunc(cs_impl::type_convertor<_Source_ArgsT,_Target_ArgsT>::convert(convert<_Source_ArgsT>::get_val(args[S]))...);
		}

	public:
		cni_helper() = delete;

		cni_helper(const cni_helper &) = default;

		cni_helper(const std::function<void(_Target_ArgsT...)> &func) : mFunc(func) {}

		var call(vector &args) const
		{
			arglist::check<_Source_ArgsT...>(args);
			_call(args, cov::make_sequence<sizeof...(_Source_ArgsT)>::result);
			return null_pointer;
		}
	};

	template<typename _Target_RetT, typename _Source_RetT, typename..._Target_ArgsT, typename..._Source_ArgsT>
	class cni_helper<_Target_RetT(*)(_Target_ArgsT...), _Source_RetT(*)(_Source_ArgsT...)> {
		std::function<_Target_RetT(_Target_ArgsT...)> mFunc;

		template<int...S>
		_Source_RetT _call(vector &args, const cov::sequence<S...> &) const
		{
			return std::move(cs_impl::type_convertor<_Target_RetT,_Source_RetT>::convert(mFunc(cs_impl::type_convertor<_Source_ArgsT,_Target_ArgsT>::convert(convert<_Source_ArgsT>::get_val(args[S]))...)));
		}

	public:
		cni_helper() = delete;

		cni_helper(const cni_helper &) = default;

		cni_helper(const std::function<_Target_RetT(_Target_ArgsT...)> &func) : mFunc(func) {}

		var call(vector &args) const
		{
			arglist::check<_Source_ArgsT...>(args);
			return std::move(_call(args, cov::make_sequence<sizeof...(_Source_ArgsT)>::result));
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

	template<typename T>struct cni_type {};

	template<typename From, typename To>
	class cni_castable final {
		template<typename From1, typename To1>
		static constexpr bool helper(...)
		{
			return false;
		}

		template<typename From1, typename To1, typename=decltype(cs_impl::type_convertor<From1,To1>::convert(std::declval<From1>()))>
		static constexpr bool helper(int)
		{
			return true;
		}

	public:
		static constexpr bool value = helper<From, To>(0);
	};

	template<typename T>
	class cni_castable<T,T> final {
	public:
		static constexpr bool value = true;
	};

	class cni final {
		class cni_base {
		public:
			cni_base() = default;

			cni_base(const cni_base &) = default;

			virtual ~cni_base() = default;

			virtual cni_base *clone()=0;

			virtual var call(vector &) const =0;
		};

		template<typename T, typename X>
		class cni_holder final : public cni_base {
			cni_helper<typename cov::function_parser<T>::type::common_type, typename cov::function_parser<X>::type::common_type> mCni;
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

		template<typename RetT,typename...ArgsT>
		static constexpr int count_args_size(RetT(*)(ArgsT...))
		{
			return sizeof...(ArgsT);
		}

		static void result_container(short...) {}

		template<typename _From, typename _To>
		static short _check_conversion()
		{
			static_assert(cni_castable<_From,_To>::value,"Invalid conversion.");
			return 0;
		}

		template<typename _Target_RetT, typename _Source_RetT, typename..._Target_ArgsT, typename..._Source_ArgsT>
		static void check_conversion(_Target_RetT(*)(_Target_ArgsT...), _Source_RetT(*)(_Source_ArgsT...))
		{
			result_container(_check_conversion<_Target_ArgsT,_Source_ArgsT>()...);
		}

		template<typename T>
		struct construct_helper {
			template<typename X,typename RetT,typename...ArgsT>
			static cni_base *_construct(X &&val,RetT(*target_function)(ArgsT...))
			{
				using source_function_type=typename cs_impl::type_conversion_cpp<RetT>::target_type(*)(typename cs_impl::type_conversion_cs<ArgsT>::source_type...);
				// Return type
				static_assert(cni_castable<RetT,typename cs_impl::type_conversion_cpp<RetT>::target_type>::value,"Invalid conversion.");
				// Argument type
				check_conversion(target_function,source_function_type(nullptr));
				return new cni_holder<T,source_function_type>(std::forward<X>(val));
			}
			template<typename X>
			static cni_base *construct(X &&val)
			{
				return _construct(std::forward<X>(val),typename cov::function_parser<T>::type::common_type(nullptr));
			}
		};

		cni_base *mCni = nullptr;
	public:
		cni() = delete;

		cni(const cni &c) : mCni(c.mCni->clone()) {}

		template<typename T>
		cni(T &&val):mCni(construct_helper<typename cni_modify<typename cov::remove_reference<T>::type>::type>::construct(std::forward<T>(val))) {}

		template<typename T, typename X>
		cni(T &&val, cni_type<X>):mCni(new cni_holder<typename cni_modify<typename cov::remove_reference<T>::type>::type, typename cni_modify<X>::type>(std::forward<T>(val)))
		{
			// Analysis the function
			using target_function_type=typename cov::function_parser<typename cni_modify<typename cov::remove_reference<T>::type>::type>::type::common_type;
			using source_function_type=typename cov::function_parser<typename cni_modify<X>::type>::type::common_type;
			// Check consistency
			// Argument size
			static_assert(count_args_size(target_function_type(nullptr))==count_args_size(source_function_type(nullptr)),"Invalid argument size.");
			// Return type
			static_assert(cni_castable<typename cov::resolver<target_function_type>::return_type,typename cov::resolver<source_function_type>::return_type>::value,"Invalid conversion.");
			// Argument type
			check_conversion(target_function_type(nullptr),source_function_type(nullptr));
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
