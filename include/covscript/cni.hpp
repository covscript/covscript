#pragma once
/*
* Covariant Script C/C++ Native Interface Extension
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
#include <covscript/core/core.hpp>

namespace cs_impl {
// Compatible Type Conversions
// bool->cs::boolean
// signed short int->cs::number
	template<>
	struct type_conversion_cs<signed short int> {
		using source_type=cs::number;
	};

	template<>
	struct type_conversion_cpp<signed short int> {
		using target_type=cs::number;
	};
// unsigned short int->cs::number
	template<>
	struct type_conversion_cs<unsigned short int> {
		using source_type=cs::number;
	};

	template<>
	struct type_conversion_cpp<unsigned short int> {
		using target_type=cs::number;
	};
// signed int->cs::number
	template<>
	struct type_conversion_cs<signed int> {
		using source_type=cs::number;
	};

	template<>
	struct type_conversion_cpp<signed int> {
		using target_type=cs::number;
	};
// unsigned int->cs::number
	template<>
	struct type_conversion_cs<unsigned int> {
		using source_type=cs::number;
	};

	template<>
	struct type_conversion_cpp<unsigned int> {
		using target_type=cs::number;
	};
// signed long int->cs::number
	template<>
	struct type_conversion_cs<signed long int> {
		using source_type=cs::number;
	};

	template<>
	struct type_conversion_cpp<signed long int> {
		using target_type=cs::number;
	};
// unsigned long int->cs::number
	template<>
	struct type_conversion_cs<unsigned long int> {
		using source_type=cs::number;
	};

	template<>
	struct type_conversion_cpp<unsigned long int> {
		using target_type=cs::number;
	};
// signed long long int->cs::number
	template<>
	struct type_conversion_cs<signed long long int> {
		using source_type=cs::number;
	};

	template<>
	struct type_conversion_cpp<signed long long int> {
		using target_type=cs::number;
	};
// unsigned long long int->cs::number
	template<>
	struct type_conversion_cs<unsigned long long int> {
		using source_type=cs::number;
	};

	template<>
	struct type_conversion_cpp<unsigned long long int> {
		using target_type=cs::number;
	};
// signed char->cs::char
	template<>
	struct type_conversion_cs<signed char> {
		using source_type=char;
	};

	template<>
	struct type_conversion_cpp<signed char> {
		using target_type=char;
	};
// unsigned char->cs::char
	template<>
	struct type_conversion_cs<unsigned char> {
		using source_type=char;
	};

	template<>
	struct type_conversion_cpp<unsigned char> {
		using target_type=char;
	};
// float->cs::number
	template<>
	struct type_conversion_cs<float> {
		using source_type=cs::number;
	};

	template<>
	struct type_conversion_cpp<float> {
		using target_type=cs::number;
	};
// double->cs::number
	template<>
	struct type_conversion_cs<double> {
		using source_type=cs::number;
	};

	template<>
	struct type_conversion_cpp<double> {
		using target_type=cs::number;
	};
// long double->cs::number
	template<>
	struct type_conversion_cs<long double> {
		using source_type=cs::number;
	};

	template<>
	struct type_conversion_cpp<long double> {
		using target_type=cs::number;
	};
// std::string->cs::string
// const char*->cs::string
	template<>
	struct type_conversion_cs<const char*> {
		using source_type=cs::string;
	};

	template<>
	struct type_conversion_cpp<const char*> {
		using target_type=cs::string;
	};

	template<>
	struct type_convertor<cs::string, const char*> {
		template<typename T>
		static const char* convert(const cs::string& str)
		{
			return str.c_str();
		}
	};
}

namespace cni_namespace_impl {
	class cni_register final {
	public:
		template<typename T>
		cni_register(const cs::namespace_t& ns, const char* name, T&& val)
		{
			ns->add_var(name, std::forward<T>(val));
		}
	};
}

#define CNI_NAME_MIXER(PREFIX, NAME) static cni_namespace_impl::cni_register PREFIX##NAME
#define CNI_REGISTER(NAME, ARGS) CNI_NAME_MIXER(_cni_register_, NAME)(__cni_namespace, #NAME, ARGS);
#define BEGIN_CNI_ROOT_NAMESPACE namespace __cni_root_namespace{cs::namespace_t __cni_namespace=cs::make_shared_namespace<cs::name_space>();
#define END_CNI_ROOT_NAMESAPCE }void cs_extension_main(cs::name_space *ns) {ns->copy_namespace(*__cni_root_namespace::__cni_namespace);}
#define BEGIN_CNI_NAMESPACE(NAME) namespace NAME{cs::namespace_t __cni_namespace=cs::make_shared_namespace<cs::name_space>();
#define END_CNI_NAMESPACE(NAME) } CNI_REGISTER(NAME, cs::make_namespace(NAME::__cni_namespace))
#define CNI_NORMAL(NAME) CNI_REGISTER(NAME, cs::make_cni(NAME, false))
#define CNI_CONST(NAME) CNI_REGISTER(NAME, cs::make_cni(NAME, true))