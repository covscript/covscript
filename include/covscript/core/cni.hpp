#pragma once
/*
* Covariant Script C/C++ Native Interface
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
#include <covscript/import/mozart/bind.hpp>

namespace cs_impl {
// Utilities
	template<typename...ArgsT>
	inline void result_container(ArgsT &&...) {}

	template<typename T>
	struct cni_modify {
		using type=T;
	};

	template<typename RetT, typename...ArgsT>
	struct cni_modify<RetT(ArgsT...)> {
		using type=RetT(*)(ArgsT...);
	};

// Type conversion
	template<typename T>
	struct convert_helper {
		static inline const T &get_val(any &val)
		{
			return val.const_val<T>();
		}
	};

	template<typename T>
	struct convert_helper<const T &> {
		static inline const T &get_val(any &val)
		{
			return val.const_val<T>();
		}
	};

	template<typename T>
	struct convert_helper<T &> {
		static inline T &get_val(any &val)
		{
			return val.val<T>(true);
		}
	};

	template<>
	struct convert_helper<const any &> {
		static inline const any &get_val(const any &val)
		{
			return val;
		}
	};

	template<>
	struct convert_helper<any &> {
		static inline any &get_val(any &val)
		{
			return val;
		}
	};

	template<typename From, typename To>
	class cni_convertible final {
		template<typename From1, typename To1>
		static constexpr bool helper(...)
		{
			return false;
		}

		template<typename From1, typename To1, typename=decltype(type_convertor<From1, To1>::convert(
		             std::declval<From1>()))>
		static constexpr bool helper(int)
		{
			return true;
		}

	public:
		static constexpr bool value = helper<From, To>(0);
	};

	template<typename T>
	class cni_convertible<T, T> final {
	public:
		static constexpr bool value = true;
	};

// Dynamic argument check
	template<typename T, int index>
	struct check_args_helper {
		static inline char check(const any &val)
		{
			if (typeid(T) != val.type())
				throw cs::runtime_error("Invalid Argument. At " + std::to_string(index + 1) + ". Expected " +
				                        cxx_demangle(get_name_of_type<T>()) + ", provided " + val.get_type_name());
			else
				return 0;
		}
	};

	template<int index>
	struct check_args_helper<any, index> {
		static inline char check(const any &)
		{
			return 0;
		}
	};

	template<typename...ArgsT, int...Seq>
	void check_args_base(const cs::vector &args, const cov::sequence<Seq...> &)
	{
		result_container(
		    check_args_helper<typename cov::remove_constant<typename cov::remove_reference<ArgsT>::type>::type, Seq>::check(
		        args[Seq])...);
	}

	template<typename...ArgTypes>
	void check_args(const cs::vector &args)
	{
		if (sizeof...(ArgTypes) == args.size())
			check_args_base<ArgTypes...>(args, cov::make_sequence<sizeof...(ArgTypes)>::result);
		else
			throw cs::runtime_error(
			    "Wrong size of the arguments. Expected " + std::to_string(sizeof...(ArgTypes)) + ", provided " +
			    std::to_string(args.size()));
	}

// Static argument check
	template<typename RetT, typename...ArgsT>
	constexpr int count_args_size(RetT(*)(ArgsT...))
	{
		return sizeof...(ArgsT);
	}

	template<typename _From, typename _To>
	char check_conversion_base()
	{
		static_assert(cni_convertible<_From, _To>::value, "Invalid conversion.");
		return 0;
	}

	template<typename _Target_RetT, typename _Source_RetT, typename..._Target_ArgsT, typename..._Source_ArgsT>
	void check_conversion(_Target_RetT(*)(_Target_ArgsT...), _Source_RetT(*)(_Source_ArgsT...))
	{
		result_container(check_conversion_base<_Target_ArgsT, _Source_ArgsT>()...);
	}

// CNI Helper
	template<typename _Target, typename _Source>
	class cni_helper;

	template<typename..._Target_ArgsT, typename..._Source_ArgsT>
	class cni_helper<void (*)(_Target_ArgsT...), void (*)(_Source_ArgsT...)> {
		std::function<void(_Target_ArgsT...)> mFunc;

		template<int...S>
		void _call(cs::vector &args, const cov::sequence<S...> &) const
		{
			mFunc(type_convertor<_Source_ArgsT, _Target_ArgsT>::convert(
			          convert_helper<_Source_ArgsT>::get_val(args[S]))...);
		}

	public:
		cni_helper() = delete;

		cni_helper(const cni_helper &) = default;

		explicit cni_helper(const std::function<void(_Target_ArgsT...)> &func) : mFunc(func) {}

		any call(cs::vector &args) const
		{
			check_args<_Source_ArgsT...>(args);
			_call(args, cov::make_sequence<sizeof...(_Source_ArgsT)>::result);
			return cs::null_pointer;
		}
	};

	template<typename _Target_RetT, typename _Source_RetT, typename..._Target_ArgsT, typename..._Source_ArgsT>
	class cni_helper<_Target_RetT(*)(_Target_ArgsT...), _Source_RetT(*)(_Source_ArgsT...)> {
		std::function<_Target_RetT(_Target_ArgsT...)> mFunc;

		template<int...S>
		_Source_RetT _call(cs::vector &args, const cov::sequence<S...> &) const
		{
			return std::move(type_convertor<_Target_RetT, _Source_RetT>::convert(
			                     mFunc(type_convertor<_Source_ArgsT, _Target_ArgsT>::convert(
			                               convert_helper<_Source_ArgsT>::get_val(args[S]))...)));
		}

	public:
		cni_helper() = delete;

		cni_helper(const cni_helper &) = default;

		explicit cni_helper(const std::function<_Target_RetT(_Target_ArgsT...)> &func) : mFunc(func) {}

		any call(cs::vector &args) const
		{
			check_args<_Source_ArgsT...>(args);
			return std::move(_call(args, cov::make_sequence<sizeof...(_Source_ArgsT)>::result));
		}
	};

// CNI Holder
	class cni_holder_base {
	public:
		cni_holder_base() = default;

		cni_holder_base(const cni_holder_base &) = default;

		virtual ~cni_holder_base() = default;

		virtual cni_holder_base *clone() = 0;

		virtual any call(cs::vector &) const = 0;
	};

	template<typename T, typename X>
	class cni_holder final : public cni_holder_base {
		cni_helper<typename cov::function_parser<T>::type::common_type, typename cov::function_parser<X>::type::common_type> mCni;
	public:
		cni_holder() = delete;

		cni_holder(const cni_holder &) = default;

		explicit cni_holder(const T &func) : mCni(func) {}

		~cni_holder() override = default;

		cni_holder_base *clone() override
		{
			return new cni_holder(*this);
		}

		any call(cs::vector &args) const override
		{
			return mCni.call(args);
		}
	};

// CNI Implementation
	template<typename T>
	struct cni_type {
	};

	class cni final {
		template<typename T>
		struct construct_helper {
			template<typename X, typename RetT, typename...ArgsT>
			static cni_holder_base *_construct(X &&val, RetT(*target_function)(ArgsT...))
			{
				using source_function_type=typename type_conversion_cpp<RetT>::target_type(*)(
				                               typename type_conversion_cs<ArgsT>::source_type...);
				// Return type
				static_assert(cni_convertible<RetT, typename type_conversion_cpp<RetT>::target_type>::value,
				              "Invalid conversion.");
				// Argument type
				check_conversion(target_function, source_function_type(nullptr));
				return new cni_holder<T, source_function_type>(std::forward<X>(val));
			}

			template<typename X>
			static cni_holder_base *construct(X &&val)
			{
				return _construct(std::forward<X>(val), typename cov::function_parser<T>::type::common_type(nullptr));
			}
		};

		cni_holder_base *mCni = nullptr;
	public:
		cni() = delete;

		cni(const cni &c) : mCni(c.mCni->clone()) {}

		template<typename T>
		explicit cni(T &&val):mCni(
			    construct_helper<typename cni_modify<typename cov::remove_reference<T>::type>::type>::construct(
			        std::forward<T>(val))) {}

		template<typename T, typename X>
		cni(T &&val, cni_type<X>):mCni(
			    new cni_holder<typename cni_modify<typename cov::remove_reference<T>::type>::type, typename cni_modify<X>::type>(
			        std::forward<T>(val)))
		{
			// Analysis the function
			using target_function_type=typename cov::function_parser<typename cni_modify<typename cov::remove_reference<T>::type>::type>::type::common_type;
			using source_function_type=typename cov::function_parser<typename cni_modify<X>::type>::type::common_type;
			// Check consistency
			// Argument size
			static_assert(
			    count_args_size(target_function_type(nullptr)) == count_args_size(source_function_type(nullptr)),
			    "Invalid argument size.");
			// Return type
			static_assert(
			    cni_convertible<typename cov::resolver<target_function_type>::return_type, typename cov::resolver<source_function_type>::return_type>::value,
			    "Invalid conversion.");
			// Argument type
			check_conversion(target_function_type(nullptr), source_function_type(nullptr));
		}

		~cni()
		{
			delete mCni;
		}

		any operator()(cs::vector &args) const
		{
			try {
				return cs::try_move(mCni->call(args));
			}
			catch (const cs::lang_error &e) {
				cs::current_process->cs_eh_callback(e);
			}
			catch (const std::exception &e) {
				cs::current_process->std_eh_callback(e);
			}
			catch (...) {
				cs::current_process->std_eh_callback(cs::fatal_error("CNI:Unrecognized exception."));
			}
			return cs::null_pointer;
		}
	};

	template<>
	struct cni::construct_helper<const cni> {
		static cni_holder_base *construct(const cni &c)
		{
			return c.mCni->clone();
		}
	};

	template<>
	struct cni::construct_helper<cni> {
		static cni_holder_base *construct(const cni &c)
		{
			return c.mCni->clone();
		}
	};
}
namespace cs {
	using cs_impl::cni_type;
	using cs_impl::cni;

	template<typename T>
	var make_cni(T &&func, bool constant = false)
	{
		return var::make_protect<callable>(cni(func), constant ? callable::types::constant : callable::types::normal);
	}

	template<typename T, typename X>
	var make_cni(T &&func, const cni_type<X> &type, bool constant = false)
	{
		return var::make_protect<callable>(cni(func, type),
		                                   constant ? callable::types::constant : callable::types::normal);
	}
}