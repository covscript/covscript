#pragma once
/*
 * Covariant Script C/C++ Native Interface Extension
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
 * Copyright (C) 2017-2025 Michael Lee(李登淳)
 *
 * This software is registered with the National Copyright Administration
 * of the People's Republic of China(Registration Number: 2020SR0408026)
 * and is protected by the Copyright Law of the People's Republic of China.
 *
 * Email:   mikecovlee@163.com
 * Github:  https://github.com/mikecovlee
 * Website: http://covscript.org.cn
 */
#include <covscript/core/core.hpp>

// Define this macro to disable all standard type conversion
#ifndef CNI_DISABLE_STD_CONVERSION
namespace cs_impl {
// Compatible Type Conversions
// bool->cs::boolean
// signed short int->cs::numeric
	template <>
	struct type_conversion_cs<signed short int> {
		using source_type = cs::numeric;
	};

	template <>
	struct type_conversion_cpp<signed short int> {
		using target_type = cs::numeric;
	};

	template <>
	struct type_convertor<cs::numeric, signed short int> {
		static signed short int convert(const cs::numeric &num)
		{
			return num.as_integer();
		}
	};

// unsigned short int->cs::numeric
	template <>
	struct type_conversion_cs<unsigned short int> {
		using source_type = cs::numeric;
	};

	template <>
	struct type_conversion_cpp<unsigned short int> {
		using target_type = cs::numeric;
	};

	template <>
	struct type_convertor<cs::numeric, unsigned short int> {
		static unsigned short int convert(const cs::numeric &num)
		{
			return num.as_integer();
		}
	};

// signed int->cs::numeric
	template <>
	struct type_conversion_cs<signed int> {
		using source_type = cs::numeric;
	};

	template <>
	struct type_conversion_cpp<signed int> {
		using target_type = cs::numeric;
	};

	template <>
	struct type_convertor<cs::numeric, signed int> {
		static signed int convert(const cs::numeric &num)
		{
			return num.as_integer();
		}
	};

// unsigned int->cs::numeric
	template <>
	struct type_conversion_cs<unsigned int> {
		using source_type = cs::numeric;
	};

	template <>
	struct type_conversion_cpp<unsigned int> {
		using target_type = cs::numeric;
	};

	template <>
	struct type_convertor<cs::numeric, unsigned int> {
		static unsigned int convert(const cs::numeric &num)
		{
			return num.as_integer();
		}
	};

// signed long int->cs::numeric
	template <>
	struct type_conversion_cs<signed long int> {
		using source_type = cs::numeric;
	};

	template <>
	struct type_conversion_cpp<signed long int> {
		using target_type = cs::numeric;
	};

	template <>
	struct type_convertor<cs::numeric, signed long int> {
		static signed long convert(const cs::numeric &num)
		{
			return num.as_integer();
		}
	};

// unsigned long int->cs::numeric
	template <>
	struct type_conversion_cs<unsigned long int> {
		using source_type = cs::numeric;
	};

	template <>
	struct type_conversion_cpp<unsigned long int> {
		using target_type = cs::numeric;
	};

	template <>
	struct type_convertor<cs::numeric, unsigned long int> {
		static unsigned long int convert(const cs::numeric &num)
		{
			return num.as_integer();
		}
	};

// signed long long int->cs::numeric
	template <>
	struct type_conversion_cs<signed long long int> {
		using source_type = cs::numeric;
	};

	template <>
	struct type_conversion_cpp<signed long long int> {
		using target_type = cs::numeric;
	};

	template <>
	struct type_convertor<cs::numeric, signed long long int> {
		static signed long long int convert(const cs::numeric &num)
		{
			return num.as_integer();
		}
	};

// unsigned long long int->cs::numeric
	template <>
	struct type_conversion_cs<unsigned long long int> {
		using source_type = cs::numeric;
	};

	template <>
	struct type_conversion_cpp<unsigned long long int> {
		using target_type = cs::numeric;
	};

	template <>
	struct type_convertor<cs::numeric, unsigned long long int> {
		static unsigned long long int convert(const cs::numeric &num)
		{
			return num.as_integer();
		}
	};

// signed char->cs::char
	template <>
	struct type_conversion_cs<signed char> {
		using source_type = char;
	};

	template <>
	struct type_conversion_cpp<signed char> {
		using target_type = char;
	};
// unsigned char->cs::char
	template <>
	struct type_conversion_cs<unsigned char> {
		using source_type = char;
	};

	template <>
	struct type_conversion_cpp<unsigned char> {
		using target_type = char;
	};
// float->cs::numeric
	template <>
	struct type_conversion_cs<float> {
		using source_type = cs::numeric;
	};

	template <>
	struct type_conversion_cpp<float> {
		using target_type = cs::numeric;
	};

	template <>
	struct type_convertor<cs::numeric, float> {
		static float convert(const cs::numeric &num)
		{
			return num.as_float();
		}
	};

// double->cs::numeric
	template <>
	struct type_conversion_cs<double> {
		using source_type = cs::numeric;
	};

	template <>
	struct type_conversion_cpp<double> {
		using target_type = cs::numeric;
	};

	template <>
	struct type_convertor<cs::numeric, double> {
		static double convert(const cs::numeric &num)
		{
			return num.as_float();
		}
	};

// long double->cs::numeric
	template <>
	struct type_conversion_cs<long double> {
		using source_type = cs::numeric;
	};

	template <>
	struct type_conversion_cpp<long double> {
		using target_type = cs::numeric;
	};

	template <>
	struct type_convertor<cs::numeric, long double> {
		static long double convert(const cs::numeric &num)
		{
			return num.as_float();
		}
	};

// std::string->cs::string
// const char*->cs::string
	template <>
	struct type_conversion_cs<const char *> {
		using source_type = cs::string;
	};

	template <>
	struct type_conversion_cpp<const char *> {
		using target_type = cs::string;
	};

	template <>
	struct type_convertor<cs::string, const char *> {
		static const char *convert(const cs::string &str)
		{
			return str.c_str();
		}
	};
} // namespace cs_impl
#endif

namespace cs {
	/**
	 * CNI Convertor
	 * Available in Standard 191201 or later
	 * Provided a convenient way to convert types from C++ to CovScript
	 * Note: This function will follow CNI Standard Conversion Mechanism,
	 * 		 if you don't want any type conversion, please using constru-
	 * 		 ctor of cs::var directly.
	 */
	namespace cni_convertor {
		template <typename T>
		auto to_covscript(T &&val)
		{
			using decay_t = typename std::decay<T>::type;
			return cs_impl::type_convertor<decay_t, typename cs_impl::type_conversion_cpp<decay_t>::target_type>::convert(
			           std::forward<T>(val));
		}
	} // namespace cni_convertor
} // namespace cs

namespace cni_namespace_impl {
	class cni_register final {
	public:
		template <typename T>
		cni_register(const cs::namespace_t &ns, const char *name, T &&val)
		{
			ns->add_var(name, std::forward<T>(val));
		}
	};

	template <typename T>
	cs::var make_var_normal(const T &val)
	{
		return cs::var::make<typename cs_impl::type_conversion_cpp<T>::target_type>(val);
	}

	template <typename T, typename X>
	cs::var make_var_normal_v(X &&val)
	{
		return cs::var::make<T>(std::forward<X>(val));
	}

	template <typename T>
	cs::var make_var_const(const T &val)
	{
		return cs::var::make_constant<typename cs_impl::type_conversion_cpp<T>::target_type>(val);
	}

	template <typename T, typename X>
	cs::var make_var_const_v(X &&val)
	{
		return cs::var::make_constant<T>(std::forward<X>(val));
	}
} // namespace cni_namespace_impl

#define CNI_NAME_MIXER(PREFIX, NAME) static cni_namespace_impl::cni_register PREFIX##NAME
#define CNI_REGISTER(NAME, ARGS) CNI_NAME_MIXER(_cni_register_, NAME)(__cni_namespace, #NAME, ARGS);
#define CNI_ROOT_NAMESPACE                                                                    \
	namespace cni_root_namespace                                                              \
	{                                                                                         \
		static cs::namespace_t __cni_namespace = cs::make_shared_namespace<cs::name_space>(); \
	}                                                                                         \
	void cs_extension_main(cs::name_space *ns)                                                \
	{                                                                                         \
		ns->copy_namespace(*cni_root_namespace::__cni_namespace);                             \
	}                                                                                         \
	namespace cni_root_namespace
#define CNI_NAMESPACE(NAME)                                                                   \
	namespace NAME                                                                            \
	{                                                                                         \
		static cs::namespace_t __cni_namespace = cs::make_shared_namespace<cs::name_space>(); \
	}                                                                                         \
	CNI_REGISTER(NAME, cs::make_namespace(NAME::__cni_namespace))                             \
	namespace NAME
#define CNI_NAMESPACE_ALIAS(NAME, ALIAS) CNI_REGISTER(ALIAS, cs::make_namespace(NAME::__cni_namespace))
#define CNI_TYPE_EXT(NAME, TYPE, FUNC)                                                                                                         \
	namespace NAME                                                                                                                             \
	{                                                                                                                                          \
		static cs::namespace_t __cni_namespace = cs::make_shared_namespace<cs::name_space>();                                                  \
	}                                                                                                                                          \
	CNI_REGISTER(NAME, cs::var::make_constant<cs::type_t>([]() -> cs::var { return FUNC; }, cs::type_id(typeid(TYPE)), NAME::__cni_namespace)) \
	namespace NAME
#define CNI_TYPE_EXT_V(NAME, TYPE, TYPE_NAME, FUNC)                                                                                                                                            \
	namespace NAME                                                                                                                                                                             \
	{                                                                                                                                                                                          \
		static cs::namespace_t __cni_namespace = cs::make_shared_namespace<cs::name_space>();                                                                                                  \
	}                                                                                                                                                                                          \
	CNI_NAME_MIXER(_cni_register_, NAME)(__cni_namespace, #TYPE_NAME, cs::var::make_constant<cs::type_t>([]() -> cs::var { return FUNC; }, cs::type_id(typeid(TYPE)), NAME::__cni_namespace)); \
	namespace NAME
#define CNI(NAME) CNI_REGISTER(NAME, cs::make_cni(NAME, false))
#define CNI_CONST(NAME) CNI_REGISTER(NAME, cs::make_cni(NAME, true))
#define CNI_VISITOR(NAME) CNI_REGISTER(NAME, cs::make_cni(NAME, cs::callable::types::member_visitor))
#define CNI_V(NAME, ARGS) CNI_REGISTER(NAME, cs::make_cni(ARGS, false))
#define CNI_CONST_V(NAME, ARGS) CNI_REGISTER(NAME, cs::make_cni(ARGS, true))
#define CNI_VISITOR_V(NAME, ARGS) CNI_REGISTER(NAME, cs::make_cni(ARGS, cs::callable::types::member_visitor))
#define CNI_CLASS_MEMBER(CLASS, MEMBER) CNI_REGISTER(MEMBER, cs::make_member_visitor(&CLASS::MEMBER))
#define CNI_CLASS_MEMBER_CONST(CLASS, MEMBER) CNI_REGISTER(MEMBER, cs::make_const_member_visitor(&CLASS::MEMBER))
#define CNI_VALUE(NAME, ARGS) CNI_REGISTER(NAME, cni_namespace_impl::make_var_normal(ARGS))
#define CNI_VALUE_CONST(NAME, ARGS) CNI_REGISTER(NAME, cni_namespace_impl::make_var_const(ARGS))
#define CNI_VALUE_V(NAME, TYPE, ARGS) CNI_REGISTER(NAME, cni_namespace_impl::make_var_normal_v<TYPE>(ARGS))
#define CNI_VALUE_CONST_V(NAME, TYPE, ARGS) CNI_REGISTER(NAME, cni_namespace_impl::make_var_const_v<TYPE>(ARGS))
#define CNI_ENABLE_TYPE_EXT(NS, TYPE)                       \
	namespace cs_impl                                       \
	{                                                       \
		template <>                                         \
		constexpr const char *get_name_of_type<TYPE>()      \
		{                                                   \
			return #TYPE;                                   \
		}                                                   \
		template <>                                         \
		cs::namespace_t &get_ext<TYPE>()                    \
		{                                                   \
			return cni_root_namespace::NS::__cni_namespace; \
		}                                                   \
	}
#define CNI_ENABLE_TYPE_EXT_V(NS, TYPE, NAME)               \
	namespace cs_impl                                       \
	{                                                       \
		template <>                                         \
		constexpr const char *get_name_of_type<TYPE>()      \
		{                                                   \
			return #NAME;                                   \
		}                                                   \
		template <>                                         \
		cs::namespace_t &get_ext<TYPE>()                    \
		{                                                   \
			return cni_root_namespace::NS::__cni_namespace; \
		}                                                   \
	}
