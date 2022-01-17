#pragma once
/*
* Covariant Script Variant
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
* Copyright (C) 2017-2022 Michael Lee(李登淳)
*
* This software is registered with the National Copyright Administration
* of the People's Republic of China(Registration Number: 2020SR0408026)
* and is protected by the Copyright Law of the People's Republic of China.
*
* Email:   lee@covariant.cn, mikecovlee@163.com
* Github:  https://github.com/mikecovlee
* Website: http://covscript.org.cn
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
		static void emplace(const byte_t *src, X &&dat)
		{
			::new(src) T(std::forward<X>(dat));
		}
	};

	template<typename T>
	struct convert_if<false, T> {
		template<typename X>
		static void emplace(byte_t *, X &&) {}
	};

	class base_holder {
	public:
		virtual const std::type_info &get_type() const noexcept = 0;

		virtual void copy_data(byte_t *) = 0;

		virtual ~base_holder() = default;
	};

	template<typename T>
	class holder : public base_holder {
	public:
		T data;

		template<typename... _ArgsT>
		holder(_ArgsT &&... args) : data(std::forward<_ArgsT>(args)...) {}

		const std::type_info &get_type() const noexcept override
		{
			return typeid(T);
		}

		void copy_data(byte_t *dest) override
		{
			::new(dest) T(data);
		}
	};

	template<typename... ArgsT>
	struct template_args_iterator;

	template<typename T>
	struct template_args_iterator<holder<T>> {
		static constexpr std::size_t get_max_size(std::size_t size = 0)
		{
			if (sizeof(holder<T>) > size)
				return sizeof(holder<T>);
			else
				return size;
		}

		template<typename X>
		static void emplace(byte_t *src, X &&dat)
		{
			if (convertible<X, T>::value)
				convert_if<convertible<X, T>::value, holder<T>>::emplace(src, std::forward<X>(dat));
			else
				throw std::logic_error("Internal Error: No matching types(emplace).");
		}
	};

	template<typename T, typename... ArgsT>
	struct template_args_iterator<holder<T>, ArgsT...> {
		static constexpr std::size_t get_max_size(std::size_t size = 0)
		{
			if (sizeof(holder<T>) > size)
				return template_args_iterator<ArgsT...>::get_max_size(sizeof(holder<T>));
			else
				return template_args_iterator<ArgsT...>::get_max_size(size);
		}

		template<typename X>
		static void emplace(byte_t *src, X &&dat)
		{
			if (convertible<X, T>::value)
				convert_if<convertible<X, T>::value, holder<T>>::emplace(src, std::forward<X>(dat));
			else
				template_args_iterator<ArgsT...>::emplace(src, std::forward<X>(dat));
		}
	};

	struct monostate final {
	};

	template<typename... ArgsT>
	class variant final {
		using template_iterator_t = template_args_iterator<holder<monostate>, holder<ArgsT>...>;
		byte_t data_container[template_iterator_t::get_max_size()];
		base_holder *data_ptr = reinterpret_cast<base_holder *>(data_container);

	public:
		variant()
		{
			::new(data_container) holder<monostate>;
		}

		variant(const variant &var)
		{
			data_ptr->copy_data(data_container);
		}

		template<typename T>
		variant(const T &t)
		{
			::new(data_container) holder<T>(t);
		}

		~variant()
		{
			data_ptr->~base_holder();
		}

		variant &operator=(const variant &var)
		{
			if (&var != this) {
				data_ptr->~base_holder();
				var.data_ptr->copy_data(data_container);
			}
			return *this;
		}

		template<typename T>
		variant &operator=(const T &t)
		{
			data_ptr->~base_holder();
			::new(data_container) holder<T>(t);
			return *this;
		}

		template<typename T, typename... ElementT>
		void emplace(ElementT &&... args)
		{
			data_ptr->~base_holder();
			::new(data_container) holder<T>(std::forward<ElementT>(args)...);
		}

		template<typename T>
		void force_emplace(T &&dat)
		{
			data_ptr->~base_holder();
			template_iterator_t::emplace(data_container, std::forward<T>(dat));
		}

		const std::type_info &type() const
		{
			return data_ptr->get_type();
		}

		template<typename T>
		T &get() const
		{
			if (data_ptr->get_type() == typeid(T))
				return static_cast<holder<T> *>(data_ptr)->data;
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