#pragma once
/*
* Covariant Script Variant
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
#include <typeinfo>
#include <typeindex>
#include <stdexcept>

namespace variant_impl {
	using byte_t = unsigned char;

	template<typename _From, typename _To>
	class convertible final {
		template<typename T>
		static void test(T);

		template<typename _F, typename _T>
		static constexpr bool convert_helper(float)
		{
			return false;
		}

		template<typename _F, typename _T, typename = decltype(test<_T>(std::declval<_F>()))>
		static constexpr bool convert_helper(int)
		{
			return true;
		}

	public:
		convertible() = delete;

		static constexpr bool value = convert_helper<_From, _To>(0);
	};

	template<typename T>
	class convertible<T, T> final {
	public:
		convertible() = delete;

		static constexpr bool value = true;
	};

	template<bool value, typename T>
	struct convert_if;

	template<typename T>
	struct convert_if<true, T> {
		template<typename X>
		static void emplace(byte_t *src, X &&dat)
		{
			::new(src) T(std::forward<X>(dat));
		}
	};

	template<typename T>
	struct convert_if<false, T> {
		template<typename X>
		static void emplace(byte_t *, X &&) {}
	};

	template<typename... ArgsT>
	struct template_args_iterator;

	template<typename T>
	struct template_args_iterator<T> {
		static constexpr std::size_t get_max_size(std::size_t size = 0)
		{
			if (sizeof(T) > size)
				return sizeof(T);
			else
				return size;
		}

		template<typename X>
		static std::type_index emplace(byte_t *src, X &&dat)
		{
			if (convertible<X, T>::value) {
				convert_if<convertible<X, T>::value, T>::emplace(src, std::forward<X>(dat));
				return typeid(T);
			}
			else
				throw std::logic_error("Internal Error: No matching types(emplace).");
		}

		static void copy_data(const std::type_index &ti, const byte_t *src, byte_t *dest)
		{
			if (ti == typeid(T))
				::new(dest) T(*reinterpret_cast<const T *>(src));
			else
				throw std::logic_error("Internal Error: No matching types(copy).");
		}

		static void destroy_data(const std::type_index &ti, byte_t *src)
		{
			if (ti == typeid(T))
				reinterpret_cast<T *>(src)->~T();
			else
				throw std::logic_error("Internal Error: No matching types(destroy).");
		}
	};

	template<typename T, typename... ArgsT>
	struct template_args_iterator<T, ArgsT...> {
		static constexpr std::size_t get_max_size(std::size_t size = 0)
		{
			if (sizeof(T) > size)
				return template_args_iterator<ArgsT...>::get_max_size(sizeof(T));
			else
				return template_args_iterator<ArgsT...>::get_max_size(size);
		}

		template<typename X>
		static std::type_index emplace(byte_t *src, X &&dat)
		{
			if (convertible<X, T>::value) {
				convert_if<convertible<X, T>::value, T>::emplace(src, std::forward<X>(dat));
				return typeid(T);
			}
			else
				return template_args_iterator<ArgsT...>::emplace(src, std::forward<X>(dat));
		}

		static void copy_data(const std::type_index &ti, const byte_t *src, byte_t *dest)
		{
			if (ti == typeid(T))
				::new(dest) T(*reinterpret_cast<const T *>(src));
			else
				template_args_iterator<ArgsT...>::copy_data(ti, src, dest);
		}

		static void destroy_data(const std::type_index &ti, byte_t *src)
		{
			if (ti == typeid(T))
				reinterpret_cast<T *>(src)->~T();
			else
				template_args_iterator<ArgsT...>::destroy_data(ti, src);
		}
	};

	struct monostate final {
	};

	template<typename... ArgsT>
	class variant final {
		using template_iterator_t = template_args_iterator<monostate, ArgsT...>;
		std::type_index current_type = typeid(monostate);
		byte_t data_container[template_iterator_t::get_max_size()];

	public:
		variant()
		{
			::new(data_container) monostate;
		}

		variant(const variant &var)
		{
			template_iterator_t::copy_data(var.current_type, var.data_container, data_container);
			current_type = var.current_type;
		}

		template<typename T>
		variant(const T &t)
		{
			::new(data_container) T(t);
			current_type = typeid(T);
		}

		~variant()
		{
			template_iterator_t::destroy_data(current_type, data_container);
		}

		variant &operator=(const variant &var)
		{
			if (&var != this) {
				template_iterator_t::destroy_data(current_type, data_container);
				template_iterator_t::copy_data(var.current_type, var.data_container, data_container);
				current_type = var.current_type;
			}
			return *this;
		}

		template<typename T>
		variant &operator=(const T &t)
		{
			template_iterator_t::destroy_data(current_type, data_container);
			::new(data_container) T(t);
			current_type = typeid(T);
			return *this;
		}

		template<typename T, typename... ElementT>
		void emplace(ElementT &&... args)
		{
			template_iterator_t::destroy_data(current_type, data_container);
			::new(data_container) T(std::forward<ElementT>(args)...);
			current_type = typeid(T);
		}

		template<typename T>
		void force_emplace(T &&dat)
		{
			template_iterator_t::destroy_data(current_type, data_container);
			current_type = template_iterator_t::emplace(data_container, std::forward<T>(dat));
		}

		const std::type_index &type() const
		{
			return current_type;
		}

		template<typename T>
		T &get()
		{
			if (current_type == typeid(T))
				return *reinterpret_cast<T *>(data_container);
			else
				throw std::logic_error("Type does not match.");
		}

		template<typename T>
		const T &get() const
		{
			if (current_type == typeid(T))
				return *reinterpret_cast<T const *>(data_container);
			else
				throw std::logic_error("Type does not match.");
		}

		template<typename T>
		operator T &()
		{
			return get<T>();
		}

		template<typename T>
		operator const T &() const
		{
			return get<T>();
		}
	};
} // namespace variant_impl