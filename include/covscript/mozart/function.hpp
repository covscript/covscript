#pragma once
/*
* Covariant Mozart Utility Library: Function
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
#include "./traits.hpp"

namespace cov {
	template<typename>
	class function;

	template<typename>
	class function_base;

	template<typename>
	class function_container;

	template<typename>
	class function_index;

	template<typename>
	class executor_index;

	template<typename>
	struct function_parser;
	template<bool, typename>
	struct function_resolver;

	template<typename _Tp>
	class is_functional {
		template<typename T, decltype(&T::operator()) X>
		struct matcher;

		template<typename T>
		static constexpr bool match(T *)
		{
			return false;
		}

		template<typename T>
		static constexpr bool match(matcher<T, &T::operator()> *)
		{
			return true;
		}

	public:
		static constexpr bool value = match<_Tp>(nullptr);
	};

	template<typename _rT, typename..._ArgsT>
	class function_base<_rT(*)(_ArgsT...)> {
	public:
		function_base() = default;

		function_base(const function_base &) = default;

		function_base(function_base &&) noexcept = default;

		virtual ~function_base() = default;

		virtual function_base *copy() const = 0;

		virtual _rT call(_ArgsT &&...) const = 0;
	};

	template<typename _Tp>
	class function_container {
		_Tp function;
	public:
		explicit function_container(_Tp func) : function(func) {}

		template<typename..._ArgsT>
		decltype(std::declval<_Tp>()(std::declval<_ArgsT>()...))
		call(_ArgsT &&...args)
		{
			return function(std::forward<_ArgsT>(args)...);
		}
	};

	template<typename _rT, typename...Args>
	class function_index<_rT(*)(Args...)> : public function_base<_rT(*)(Args...)> {
	public:
		typedef _rT(*type)(Args...);

		typedef _rT(*common_type)(Args...);

	private:
		type function;
	public:
		explicit function_index(type func) : function(func) {}

		_rT call(Args &&...args) const override
		{
			return function(std::forward<Args>(args)...);
		}

		function_base<common_type> *copy() const override
		{
			return new function_index(function);
		}
	};

	template<typename _Tp, typename _rT, typename...Args>
	class function_index<_rT(_Tp::*)(Args...)> : public function_base<_rT(*)(_Tp &, Args...)> {
	public:
		typedef _rT(_Tp::*type)(Args...);

		typedef _rT(*common_type)(_Tp &, Args...);

	private:
		type function;
	public:
		explicit function_index(type func) : function(func) {}

		virtual ~function_index() = default;

		_rT call(_Tp &obj, Args &&...args) const override
		{
			return (obj.*function)(std::forward<Args>(args)...);
		}

		function_base<common_type> *copy() const override
		{
			return new function_index(function);
		}
	};

	template<typename _Tp, typename _rT, typename...Args>
	class function_index<_rT(_Tp::*)(Args...) const> : public function_base<_rT(*)(const _Tp &, Args...)> {
	public:
		typedef _rT(_Tp::*type)(Args...) const;

		typedef _rT(*common_type)(const _Tp &, Args...);

	private:
		type function;
	public:
		explicit function_index(type func) : function(func) {}

		virtual ~function_index() = default;

		_rT call(const _Tp &obj, Args &&...args) const override
		{
			return (obj.*function)(std::forward<Args>(args)...);
		}

		function_base<common_type> *copy() const override
		{
			return new function_index(function);
		}
	};

	template<typename _Tp, typename _rT, typename..._ArgsT>
	class executor_index<_rT(_Tp::*)(_ArgsT...)> : public function_base<_rT(*)(_ArgsT...)> {
	public:
		typedef _rT(_Tp::*type)(_ArgsT...);

		typedef _rT(*common_type)(_ArgsT...);

	private:
		mutable _Tp object;
		type function;
	public:
		explicit executor_index(const _Tp &obj) : object(obj), function(&_Tp::operator()) {}

		virtual ~executor_index() = default;

		_rT call(_ArgsT &&...args) const override
		{
			return (object.*function)(std::forward<_ArgsT>(args)...);
		}

		function_base<common_type> *copy() const override
		{
			return new executor_index(object);
		}
	};

	template<typename _Tp, typename _rT, typename..._ArgsT>
	class executor_index<_rT(_Tp::*)(_ArgsT...) const> : public function_base<_rT(*)(_ArgsT...)> {
	public:
		typedef _rT(_Tp::*type)(_ArgsT...) const;

		typedef _rT(*common_type)(_ArgsT...);

	private:
		const _Tp object;
		type function;
	public:
		explicit executor_index(const _Tp &obj) : object(obj), function(&_Tp::operator()) {}

		virtual ~executor_index() = default;

		_rT call(_ArgsT &&...args) const override
		{
			return (object.*function)(std::forward<_ArgsT>(args)...);
		}

		function_base<common_type> *copy() const override
		{
			return new executor_index(object);
		}
	};

	template<typename _Tp>
	struct function_resolver<true, _Tp> {
		typedef executor_index<decltype(&_Tp::operator())> type;

		static type make(const _Tp &f)
		{
			return executor_index<decltype(&_Tp::operator())>(f);
		}

		static type *make_ptr(const _Tp &f)
		{
			return new executor_index<decltype(&_Tp::operator())>(f);
		}
	};

	template<typename _Tp>
	struct function_resolver<false, _Tp> {
		typedef function_index<_Tp> type;

		static type make(const _Tp &f)
		{
			return function_index<_Tp>(f);
		}

		static type *make_ptr(const _Tp &f)
		{
			return new function_index<_Tp>(f);
		}
	};

	template<typename _Tp>
	struct function_parser {
		typedef typename function_resolver<is_functional<_Tp>::value, _Tp>::type type;

		static type make_func(const _Tp &f)
		{
			return function_resolver<is_functional<_Tp>::value, _Tp>::make(f);
		}

		static type *make_func_ptr(const _Tp &f)
		{
			return function_resolver<is_functional<_Tp>::value, _Tp>::make_ptr(f);
		}
	};

	template<typename _rT, typename...ArgsT>
	class function<_rT(ArgsT...)> final {
		function_base<_rT(*)(ArgsT...)> *mFunc = nullptr;
	public:
		bool callable() const
		{
			return mFunc != nullptr;
		}

		void swap(function &func) noexcept
		{
			function_base<_rT(*)(ArgsT...)> *tmp = mFunc;
			mFunc = func.mFunc;
			func.mFunc = tmp;
		}

		void swap(function &&func) noexcept
		{
			function_base<_rT(*)(ArgsT...)> *tmp = mFunc;
			mFunc = func.mFunc;
			func.mFunc = tmp;
		}

		function() = default;

		template<typename _Tp>
		explicit function(const _Tp &func)
		{
			static_assert(is_same_type<_rT(*)(ArgsT...), typename function_parser<_Tp>::type::common_type>::value,
			              "E000B");
			mFunc = function_parser<_Tp>::make_func_ptr(func);
		}

		function(const function &func)
		{
			if (func.mFunc == nullptr)
				mFunc = func.mFunc;
			else
				mFunc = func.mFunc->copy();
		}

		function(function &&func) noexcept
		{
			swap(std::forward<function>(func));
		}

		~function()
		{
			delete mFunc;
		}

		_rT call(ArgsT &&...args) const
		{
			if (!callable())
				throw cov::error("E0005");
			return mFunc->call(std::forward<ArgsT>(args)...);
		}

		_rT operator()(typename add_reference<ArgsT>::type...args) const
		{
			if (!callable())
				throw cov::error("E0005");
			return mFunc->call(std::forward<ArgsT>(args)...);
		}

		template<typename _Tp>
		function &operator=(_Tp func)
		{
			static_assert(is_same_type<_rT(*)(ArgsT...), typename function_parser<_Tp>::type::common_type>::value,
			              "E000B");
			delete mFunc;
			mFunc = function_parser<_Tp>::make_func_ptr(func);
			return *this;
		}

		function &operator=(const function &func)
		{
			if (this != &func) {
				delete mFunc;
				if (func.mFunc == nullptr)
					mFunc = func.mFunc;
				else
					mFunc = func.mFunc->copy();
			}
			return *this;
		}

		function &operator=(function &&func) noexcept
		{
			if (this != &func) {
				swap(std::forward<function>(func));
			}
			return *this;
		}
	};

	template<typename _Tp>
	function_container<_Tp>
	make_function_container(_Tp func)
	{
		return function_container<_Tp>(func);
	}
}
