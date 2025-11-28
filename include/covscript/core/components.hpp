#pragma once
/*
 * Covariant Script Basic Components
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
#include <cwctype>
#include <atomic>
#include <cfenv>

namespace cs {
	extern std::atomic_size_t global_thread_counter;

	struct thread_guard final {
		thread_guard()
		{
			global_thread_counter.fetch_add(1, std::memory_order_relaxed);
		}
		thread_guard(const thread_guard &) = delete;
		thread_guard(thread_guard &&) noexcept = delete;
		~thread_guard()
		{
			global_thread_counter.fetch_sub(1, std::memory_order_relaxed);
		}
	};

	struct csym_info;

// Exceptions
	class exception final : public std::exception {
		std::size_t mLine = 0;
		std::string mFile, mCode, mWhat, mStr;

		static std::string compose_what(const std::string &file, std::size_t line, const std::string &code, const std::string &what)
		{
			return "File \"" + file + "\", line " + std::to_string(line) + ": " + what + "\n>\t" + code + "\n";
		}

		static std::string compose_what_internal(const std::string &file, const std::string &what)
		{
			return "File \"" + file + "\", line <INTERNAL>: " + what + "\n";
		}

	public:
		exception() = delete;

		exception(std::size_t line, std::string file, std::string code, std::string what) noexcept : mLine(line), mFile(std::move(file)), mCode(std::move(code)), mWhat(std::move(what))
		{
			mStr = compose_what(mFile, mLine, mCode, mWhat);
		}

		exception(const exception &) = default;

		exception(exception &&) noexcept = default;

		~exception() override = default;

		exception &operator=(const exception &) = default;

		exception &operator=(exception &&) = default;

		const std::string &file() const noexcept
		{
			return mFile;
		}

		void relocate_to_csym(const csym_info &);

		const char *what() const noexcept override
		{
			return this->mStr.c_str();
		}
	};

	class compile_error final : public std::exception {
		std::string mWhat = "Compile Error";

	public:
		compile_error() = default;

		explicit compile_error(const std::string &str) noexcept : mWhat("Compile Error: " + str) {}

		compile_error(const compile_error &) = default;

		compile_error(compile_error &&) noexcept = default;

		~compile_error() override = default;

		compile_error &operator=(const compile_error &) = default;

		compile_error &operator=(compile_error &&) = default;

		const char *what() const noexcept override
		{
			return this->mWhat.c_str();
		}
	};

	class runtime_error final : public std::exception {
		std::string mWhat = "Runtime Error";

	public:
		runtime_error() = default;

		explicit runtime_error(const std::string &str) noexcept : mWhat("Runtime Error: " + str) {}

		runtime_error(const runtime_error &) = default;

		runtime_error(runtime_error &&) noexcept = default;

		~runtime_error() override = default;

		runtime_error &operator=(const runtime_error &) = default;

		runtime_error &operator=(runtime_error &&) = default;

		const char *what() const noexcept override
		{
			return this->mWhat.c_str();
		}
	};

	class internal_error final : public std::exception {
		std::string mWhat = "Internal Error";

	public:
		internal_error() = default;

		explicit internal_error(const std::string &str) noexcept : mWhat("Internal Error: " + str) {}

		internal_error(const internal_error &) = default;

		internal_error(internal_error &&) noexcept = default;

		~internal_error() override = default;

		internal_error &operator=(const internal_error &) = default;

		internal_error &operator=(internal_error &&) = default;

		const char *what() const noexcept override
		{
			return this->mWhat.c_str();
		}
	};

	class lang_error final {
		std::string mWhat;

	public:
		lang_error() = default;

		explicit lang_error(std::string str) noexcept : mWhat(std::move(str)) {}

		lang_error(const lang_error &) = default;

		lang_error(lang_error &&) noexcept = default;

		~lang_error() = default;

		lang_error &operator=(const lang_error &) = default;

		lang_error &operator=(lang_error &&) = default;

		const char *what() const noexcept
		{
			return this->mWhat.c_str();
		}
	};

	class fatal_error final : public std::exception {
		std::string mWhat = "Fatal Error";

	public:
		fatal_error() = default;

		explicit fatal_error(const std::string &str) noexcept : mWhat("Fatal Error: " + str) {}

		fatal_error(const fatal_error &) = default;

		fatal_error(fatal_error &&) noexcept = default;

		~fatal_error() override = default;

		fatal_error &operator=(const fatal_error &) = default;

		fatal_error &operator=(fatal_error &&) = default;

		const char *what() const noexcept override
		{
			return this->mWhat.c_str();
		}
	};

	class forward_exception final : public std::exception {
		std::string mWhat;

	public:
		forward_exception() = delete;

		explicit forward_exception(const char *str) noexcept : mWhat(str) {}

		forward_exception(const forward_exception &) = default;

		forward_exception(forward_exception &&) noexcept = default;

		~forward_exception() override = default;

		forward_exception &operator=(const forward_exception &) = default;

		forward_exception &operator=(forward_exception &&) = default;

		const char *what() const noexcept override
		{
			return this->mWhat.c_str();
		}
	};

// Numeric
	using numeric_float = long double;
	using numeric_integer = long long int;

	class numeric final {
		union {
			numeric_float _num;
			numeric_integer _int;
		} data;
		bool type = 1;

		static COVSCRIPT_ALWAYS_INLINE std::uint8_t get_composite_type(bool lhs, bool rhs) noexcept
		{
			return lhs << 1 | rhs;
		}

		static inline numeric int_pow(numeric_integer base, numeric_integer exp)
		{
			if (exp == 0) // base^0
				return 1;
			if (base == 0 && exp < 0) {
				// 0^negative
				errno = EDOM;
				feraiseexcept(FE_DIVBYZERO);
				return std::numeric_limits<numeric_float>::infinity();
			}
			bool neg = false;
			if (exp < 0) {
				// negative exponent
				neg = true;
				if (exp == (std::numeric_limits<numeric_integer>::min)())
					return numeric_float(1.0) / (int_pow(base, exp + 1).as_float() * base);
				exp = -exp;
			}
			numeric_integer result = 1;
			while (exp > 0) {
				if (exp & 1) result *= base;
				base *= base;
				exp >>= 1;
			}
			if (neg) // return float if negative exponent
				return numeric_float(1.0) / result;
			else
				return result;
		}

	public:
		numeric()
		{
			data._int = 0;
		}

		template <typename T, typename = std::enable_if_t<!std::is_same<std::decay_t<T>, numeric>::value>>
		numeric(T &&dat)
		{
			if (std::is_floating_point<std::decay_t<T>>::value) {
				type = 0;
				data._num = static_cast<numeric_float>(dat);
			}
			else if (std::is_integral<std::decay_t<T>>::value) {
				type = 1;
				data._int = static_cast<numeric_integer>(dat);
			}
			else
				throw runtime_error("Construct numeric with incompatible type.");
		}

		numeric(const numeric &rhs) : data(rhs.data), type(rhs.type) {}

		numeric(numeric &&rhs) noexcept : data(rhs.data), type(rhs.type) {}

		~numeric() = default;

		numeric operator+(const numeric &rhs) const noexcept
		{
			switch (get_composite_type(type, rhs.type)) {
			default:
			case 0b00:
				return data._num + rhs.data._num;
			case 0b01:
				return data._num + rhs.data._int;
			case 0b10:
				return data._int + rhs.data._num;
			case 0b11:
				return data._int + rhs.data._int;
			}
		}

		template <typename T, typename = std::enable_if_t<!std::is_same<std::decay_t<T>, numeric>::value>>
		numeric operator+(T &&rhs) const noexcept
		{
			if (type)
				return data._int + rhs;
			else
				return data._num + rhs;
		}

		numeric operator-(const numeric &rhs) const noexcept
		{
			switch (get_composite_type(type, rhs.type)) {
			default:
			case 0b00:
				return data._num - rhs.data._num;
			case 0b01:
				return data._num - rhs.data._int;
			case 0b10:
				return data._int - rhs.data._num;
			case 0b11:
				return data._int - rhs.data._int;
			}
		}

		template <typename T, typename = std::enable_if_t<!std::is_same<std::decay_t<T>, numeric>::value>>
		numeric operator-(T &&rhs) const noexcept
		{
			if (type)
				return data._int - rhs;
			else
				return data._num - rhs;
		}

		numeric operator*(const numeric &rhs) const noexcept
		{
			switch (get_composite_type(type, rhs.type)) {
			default:
			case 0b00:
				return data._num * rhs.data._num;
			case 0b01:
				return data._num * rhs.data._int;
			case 0b10:
				return data._int * rhs.data._num;
			case 0b11:
				return data._int * rhs.data._int;
			}
		}

		template <typename T, typename = std::enable_if_t<!std::is_same<std::decay_t<T>, numeric>::value>>
		numeric operator*(T &&rhs) const noexcept
		{
			if (type)
				return data._int * rhs;
			else
				return data._num * rhs;
		}

		numeric operator/(const numeric &rhs) const
		{
			switch (get_composite_type(type, rhs.type)) {
			default:
			case 0b00:
				return data._num / rhs.data._num;
			case 0b01:
				return data._num / rhs.data._int;
			case 0b10:
				return data._int / rhs.data._num;
			case 0b11:
				std::lldiv_t divres = std::lldiv(data._int, rhs.data._int);
				if (divres.rem == 0)
					return divres.quot;
				else
					return static_cast<numeric_float>(data._int) / rhs.data._int;
			}
		}

		template <typename T, typename = std::enable_if_t<!std::is_same<std::decay_t<T>, numeric>::value>>
		numeric operator/(T &&rhs) const
		{
			if (type)
				return data._int / rhs;
			else
				return data._num / rhs;
		}

		numeric operator%(const numeric &rhs) const
		{
			switch (get_composite_type(type, rhs.type)) {
			default:
			case 0b00:
				return std::fmod(data._num, rhs.data._num);
			case 0b01:
				return std::fmod(data._num, rhs.data._int);
			case 0b10:
				return std::fmod(data._int, rhs.data._num);
			case 0b11:
				return data._int % rhs.data._int;
			}
		}

		template <typename T, typename = std::enable_if_t<!std::is_same<std::decay_t<T>, numeric>::value>>
		numeric operator%(T &&rhs) const
		{
			if (type)
				return data._int % rhs;
			else
				return std::fmod(data._num, rhs);
		}

		numeric operator^(const numeric &rhs) const noexcept
		{
			switch (get_composite_type(type, rhs.type)) {
			default:
			case 0b00:
				return std::pow(data._num, rhs.data._num);
			case 0b01:
				return std::pow(data._num, rhs.data._int);
			case 0b10:
				return std::pow(data._int, rhs.data._num);
			case 0b11:
				return int_pow(data._int, rhs.data._int);
			}
		}

		template <typename T, typename = std::enable_if_t<!std::is_same<std::decay_t<T>, numeric>::value>>
		numeric operator^(T &&rhs) const noexcept
		{
			if (type)
				return std::pow(data._int, rhs);
			else
				return std::pow(data._num, rhs);
		}

		numeric &operator=(const numeric &num)
		{
			if (this != &num) {
				data = num.data;
				type = num.type;
			}
			return *this;
		}

		template <typename T, typename = std::enable_if_t<!std::is_same<std::decay_t<T>, numeric>::value>>
		numeric &operator=(T &&dat)
		{
			if (std::is_floating_point<std::decay_t<T>>::value) {
				type = 0;
				data._num = static_cast<numeric_float>(dat);
			}
			else if (std::is_integral<std::decay_t<T>>::value) {
				type = 1;
				data._int = static_cast<numeric_integer>(dat);
			}
			else
				throw runtime_error("Assigning numeric with incompatible type.");
			return *this;
		}

		bool operator<(const numeric &rhs) const noexcept
		{
			switch (get_composite_type(type, rhs.type)) {
			default:
			case 0b00:
				return data._num < rhs.data._num;
			case 0b01:
				return data._num < rhs.data._int;
			case 0b10:
				return data._int < rhs.data._num;
			case 0b11:
				return data._int < rhs.data._int;
			}
		}

		template <typename T, typename = std::enable_if_t<!std::is_same<std::decay_t<T>, numeric>::value>>
		bool operator<(T &&rhs) const noexcept
		{
			if (type)
				return data._int < rhs;
			else
				return data._num < rhs;
		}

		bool operator<=(const numeric &rhs) const noexcept
		{
			switch (get_composite_type(type, rhs.type)) {
			default:
			case 0b00:
				return data._num <= rhs.data._num;
			case 0b01:
				return data._num <= rhs.data._int;
			case 0b10:
				return data._int <= rhs.data._num;
			case 0b11:
				return data._int <= rhs.data._int;
			}
		}

		template <typename T, typename = std::enable_if_t<!std::is_same<std::decay_t<T>, numeric>::value>>
		bool operator<=(T &&rhs) const noexcept
		{
			if (type)
				return data._int <= rhs;
			else
				return data._num <= rhs;
		}

		bool operator>(const numeric &rhs) const noexcept
		{
			switch (get_composite_type(type, rhs.type)) {
			default:
			case 0b00:
				return data._num > rhs.data._num;
			case 0b01:
				return data._num > rhs.data._int;
			case 0b10:
				return data._int > rhs.data._num;
			case 0b11:
				return data._int > rhs.data._int;
			}
		}

		template <typename T, typename = std::enable_if_t<!std::is_same<std::decay_t<T>, numeric>::value>>
		bool operator>(T &&rhs) const noexcept
		{
			if (type)
				return data._int > rhs;
			else
				return data._num > rhs;
		}

		bool operator>=(const numeric &rhs) const noexcept
		{
			switch (get_composite_type(type, rhs.type)) {
			default:
			case 0b00:
				return data._num >= rhs.data._num;
			case 0b01:
				return data._num >= rhs.data._int;
			case 0b10:
				return data._int >= rhs.data._num;
			case 0b11:
				return data._int >= rhs.data._int;
			}
		}

		template <typename T, typename = std::enable_if_t<!std::is_same<std::decay_t<T>, numeric>::value>>
		bool operator>=(T &&rhs) const noexcept
		{
			if (type)
				return data._int >= rhs;
			else
				return data._num >= rhs;
		}

		bool operator==(const numeric &rhs) const noexcept
		{
			switch (get_composite_type(type, rhs.type)) {
			default:
			case 0b00:
				return data._num == rhs.data._num;
			case 0b01:
				return data._num == rhs.data._int;
			case 0b10:
				return data._int == rhs.data._num;
			case 0b11:
				return data._int == rhs.data._int;
			}
		}

		template <typename T, typename = std::enable_if_t<!std::is_same<std::decay_t<T>, numeric>::value>>
		bool operator==(T &&rhs) const noexcept
		{
			if (type)
				return data._int == rhs;
			else
				return data._num == rhs;
		}

		bool operator!=(const numeric &rhs) const noexcept
		{
			switch (get_composite_type(type, rhs.type)) {
			default:
			case 0b00:
				return data._num != rhs.data._num;
			case 0b01:
				return data._num != rhs.data._int;
			case 0b10:
				return data._int != rhs.data._num;
			case 0b11:
				return data._int != rhs.data._int;
			}
		}

		template <typename T, typename = std::enable_if_t<!std::is_same<std::decay_t<T>, numeric>::value>>
		bool operator!=(T &&rhs) const noexcept
		{
			if (type)
				return data._int != rhs;
			else
				return data._num != rhs;
		}

		numeric &operator++() noexcept
		{
			if (type)
				++data._int;
			else
				++data._num;
			return *this;
		}

		numeric &operator--() noexcept
		{
			if (type)
				--data._int;
			else
				--data._num;
			return *this;
		}

		numeric operator++(int) noexcept
		{
			if (type)
				return data._int++;
			else
				return data._num++;
		}

		numeric operator--(int) noexcept
		{
			if (type)
				return data._int--;
			else
				return data._num--;
		}

		numeric operator-() const noexcept
		{
			if (type)
				return -data._int;
			else
				return -data._num;
		}

		bool is_integer() const noexcept
		{
			return type;
		}

		bool is_float() const noexcept
		{
			return !type;
		}

		numeric_integer as_integer() const noexcept
		{
			if (type)
				return data._int;
			else
				return data._num;
		}

		numeric_float as_float() const noexcept
		{
			if (type)
				return data._int;
			else
				return data._num;
		}
	};

// Static Stack
	template <typename T>
	class stack_type final {
		mutable std::vector<T> m_impl;

	public:
		using iterator = typename std::vector<T>::reverse_iterator;

		void resize(std::size_t size)
		{
			m_impl.reserve(size);
		}

		stack_type()
		{
			resize(COVSCRIPT_STACK_PRESERVE);
		}

		explicit stack_type(std::size_t s)
		{
			resize(s);
		}

		stack_type(const stack_type &) = delete;

		~stack_type() = default;

		inline bool empty() const
		{
			return m_impl.empty();
		}

		inline std::size_t size() const
		{
			return m_impl.size();
		}

		inline bool full() const
		{
			return false;
		}

		inline T &top() const
		{
			return m_impl.back();
		}

		inline T &bottom() const
		{
			return m_impl.front();
		}

		inline T &at(std::size_t offset) const
		{
			return m_impl.at(m_impl.size() - offset - 1);
		}

		inline T &operator[](std::size_t offset) const
		{
			return m_impl[m_impl.size() - offset - 1];
		}

		template <typename... ArgsT>
		inline void push(ArgsT &&...args)
		{
			m_impl.emplace_back(std::forward<ArgsT>(args)...);
		}

		inline T pop()
		{
			T data(m_impl.back());
			m_impl.pop_back();
			return std::move(data);
		}

		inline void pop_no_return()
		{
			m_impl.pop_back();
		}

		iterator begin() const noexcept
		{
			return m_impl.rbegin();
		}

		iterator end() const noexcept
		{
			return m_impl.rend();
		}
	};

// Buffer Pool
	template <typename T, std::size_t blck_size, template <typename> class allocator_t = std::allocator>
	class allocator_type final {
		T *mPool[blck_size];
		allocator_t<T> mAlloc;
		std::size_t mOffset = 0;

	public:
		allocator_type()
		{
			while (mOffset < blck_size / 2)
				mPool[mOffset++] = mAlloc.allocate(1);
		}

		allocator_type(const allocator_type &) = delete;

		~allocator_type()
		{
			while (mOffset > 0)
				mAlloc.deallocate(mPool[--mOffset], 1);
		}

		template <typename... ArgsT>
		inline T *alloc(ArgsT &&...args)
		{
			T *ptr = nullptr;
			if (mOffset > 0 && global_thread_counter.load(std::memory_order_acquire) == 0)
				ptr = mPool[--mOffset];
			else
				ptr = mAlloc.allocate(1);
			::new (ptr) T(std::forward<ArgsT>(args)...);
			return ptr;
		}

		inline void free(T *ptr)
		{
			ptr->~T();
			if (mOffset < blck_size && global_thread_counter.load(std::memory_order_acquire) == 0)
				mPool[mOffset++] = ptr;
			else
				mAlloc.deallocate(ptr, 1);
		}

		inline T *allocate(std::size_t n)
		{
			if (n == 1 && mOffset > 0)
				return mPool[--mOffset];
			else
				return mAlloc.allocate(n);
		}

		inline void deallocate(T *ptr, std::size_t n)
		{
			if (n == 1 && mOffset < blck_size)
				mPool[mOffset++] = ptr;
			else
				mAlloc.deallocate(ptr, n);
		}
	};
} // namespace cs

#ifndef CS_ALLOCATOR_BUFFER_MAX
#define CS_ALLOCATOR_BUFFER_MAX 64
#endif

namespace cs_impl {
	template <typename T>
	using default_allocator_provider = std::allocator<T>;
	template <typename T>
	using default_allocator = cs::allocator_type<T, CS_ALLOCATOR_BUFFER_MAX, default_allocator_provider>;

// String borrower
	template <typename CharT,
	          template <typename> class allocator_t = default_allocator>
	class basic_string_borrower final {
		using stl_string = std::basic_string<CharT>;
		using allocator_type = allocator_t<stl_string>;

		static inline allocator_type &get_allocator()
		{
			static allocator_type allocator;
			return allocator;
		}

		void *m_data = nullptr;
		bool m_own = false;

		void destroy()
		{
			if (m_own && m_data) {
				stl_string *p = static_cast<stl_string *>(m_data);
				p->~basic_string();
				get_allocator().deallocate(p, 1);
			}
			m_data = nullptr;
			m_own = false;
		}

	public:
		basic_string_borrower() noexcept = default;

		basic_string_borrower(const CharT *str) noexcept : m_data(const_cast<CharT *>(str)), m_own(false) {}

		basic_string_borrower(const stl_string &str) noexcept : m_data(const_cast<CharT *>(str.data())), m_own(false) {}

		basic_string_borrower(stl_string &&str) : m_own(true)
		{
			stl_string *p = get_allocator().allocate(1);
			::new (p) stl_string(std::move(str));
			m_data = p;
		}

		basic_string_borrower(const basic_string_borrower &other) : m_own(other.m_own)
		{
			if (other.m_own) {
				stl_string *p = get_allocator().allocate(1);
				::new (p) stl_string(*static_cast<const stl_string *>(other.m_data));
				m_data = p;
			}
			else
				m_data = other.m_data;
		}

		basic_string_borrower(basic_string_borrower &&other) noexcept
			: m_data(other.m_data), m_own(other.m_own)
		{
			other.m_data = nullptr;
			other.m_own = false;
		}

		basic_string_borrower &operator=(const basic_string_borrower &other)
		{
			if (this != &other) {
				destroy();
				m_own = other.m_own;
				if (other.m_own) {
					stl_string *p = get_allocator().allocate(1);
					::new (p) stl_string(*static_cast<const stl_string *>(other.m_data));
					m_data = p;
				}
				else
					m_data = other.m_data;
			}
			return *this;
		}

		basic_string_borrower &operator=(basic_string_borrower &&other) noexcept
		{
			if (this != &other) {
				destroy();
				m_data = other.m_data;
				m_own = other.m_own;
				other.m_data = nullptr;
				other.m_own = false;
			}
			return *this;
		}

		~basic_string_borrower()
		{
			destroy();
		}

		const CharT *data() const noexcept
		{
			if (m_own)
				return static_cast<const stl_string *>(m_data)->data();
			else if (m_data)
				return static_cast<const CharT *>(m_data);
			else
				return nullptr;
		}

		operator const CharT *() const noexcept
		{
			return data();
		}

		bool usable() const noexcept
		{
			return m_data != nullptr;
		}

		operator bool() const noexcept
		{
			return usable();
		}

		stl_string extract() && {
			if (m_own)
			{
				stl_string str = std::move(*static_cast<stl_string *>(m_data));
				destroy();
				return str;
			}
			else
				return static_cast<const CharT *>(m_data);
		}

		stl_string extract() const &
		{
			if (m_own)
				return *static_cast<const stl_string *>(m_data);
			else
				return static_cast<const CharT *>(m_data);
		}
	};
} // namespace cs_impl

namespace cs {
	using string_borrower = cs_impl::basic_string_borrower<char>;

// Binary Tree
	template <typename T>
	class tree_type final {
		struct tree_node final {
			tree_node *root = nullptr;
			tree_node *left = nullptr;
			tree_node *right = nullptr;
			T data;

			tree_node() = default;

			tree_node(const tree_node &) = default;

			tree_node(tree_node &&) noexcept = default;

			tree_node(tree_node *a, tree_node *b, tree_node *c, const T &dat) : root(a), left(b), right(c), data(dat) {}

			template <typename... Args_T>
			tree_node(tree_node *a, tree_node *b, tree_node *c, Args_T &&...args) : root(a), left(b), right(c), data(std::forward<Args_T>(args)...)
			{
			}

			~tree_node() = default;
		};

		static tree_node *copy(tree_node *raw, tree_node *root = nullptr)
		{
			if (raw == nullptr) return nullptr;
			tree_node *node = new tree_node(root, nullptr, nullptr, raw->data);
			node->left = copy(raw->left, node);
			node->right = copy(raw->right, node);
			return node;
		}

		static void destroy(tree_node *raw)
		{
			if (raw != nullptr) {
				destroy(raw->left);
				destroy(raw->right);
				delete raw;
			}
		}

		tree_node *mRoot = nullptr;

	public:
		class iterator final {
			friend class tree_type;

			tree_node *mData = nullptr;

			iterator(tree_node *ptr) : mData(ptr) {}

		public:
			iterator() = default;

			iterator(const iterator &) = default;

			iterator(iterator &&) noexcept = default;

			~iterator() = default;

			iterator &operator=(const iterator &) = default;

			iterator &operator=(iterator &&) noexcept = default;

			bool usable() const noexcept
			{
				return this->mData != nullptr;
			}

			T &data()
			{
				if (!this->usable())
					throw runtime_error("Tree node is empty");
				return this->mData->data;
			}

			const T &data() const
			{
				if (!this->usable())
					throw runtime_error("Tree node is empty");
				return this->mData->data;
			}

			iterator root() const
			{
				if (!this->usable())
					throw runtime_error("Tree node is empty");
				return this->mData->root;
			}

			iterator left() const
			{
				if (!this->usable())
					throw runtime_error("Tree node is empty");
				return this->mData->left;
			}

			iterator right() const
			{
				if (!this->usable())
					throw runtime_error("Tree node is empty");
				return this->mData->right;
			}
		};

		void swap(tree_type &t)
		{
			tree_node *ptr = this->mRoot;
			this->mRoot = t.mRoot;
			t.mRoot = ptr;
		}

		void swap(tree_type &&t) noexcept
		{
			tree_node *ptr = this->mRoot;
			this->mRoot = t.mRoot;
			t.mRoot = ptr;
		}

		tree_type() = default;

		explicit tree_type(iterator it) : mRoot(copy(it.mData)) {}

		tree_type(const tree_type &t) : mRoot(copy(t.mRoot)) {}

		tree_type(tree_type &&t) noexcept : mRoot(nullptr)
		{
			swap(t);
		}

		~tree_type()
		{
			destroy(this->mRoot);
		}

		tree_type &operator=(const tree_type &t)
		{
			if (&t != this) {
				destroy(this->mRoot);
				this->mRoot = copy(t.mRoot);
			}
			return *this;
		}

		tree_type &operator=(tree_type &&t) noexcept
		{
			swap(t);
			return *this;
		}

		void assign(const tree_type &t)
		{
			if (&t != this) {
				destroy(this->mRoot);
				this->mRoot = copy(t.mRoot);
			}
		}

		bool empty() const noexcept
		{
			return this->mRoot == nullptr;
		}

		void clear()
		{
			destroy(this->mRoot);
			this->mRoot = nullptr;
		}

		iterator root()
		{
			return this->mRoot;
		}

		iterator root() const
		{
			return this->mRoot;
		}

		iterator insert_root_left(iterator it, const T &data)
		{
			if (it.mData == mRoot) {
				mRoot = new tree_node(nullptr, mRoot, nullptr, data);
				return mRoot;
			}
			if (!it.usable())
				throw runtime_error("Tree node is empty");
			tree_node *node = new tree_node(it.mData->root, it.mData, nullptr, data);
			if (it.mData->root->left == it.mData)
				it.mData->root->left = node;
			else
				it.mData->root->right = node;
			it.mData->root = node;
			return node;
		}

		iterator insert_root_right(iterator it, const T &data)
		{
			if (it.mData == mRoot) {
				mRoot = new tree_node(nullptr, nullptr, mRoot, data);
				return mRoot;
			}
			if (!it.usable())
				throw runtime_error("Tree node is empty");
			tree_node *node = new tree_node(it.mData->root, nullptr, it.mData, data);
			if (it.mData->root->left == it.mData)
				it.mData->root->left = node;
			else
				it.mData->root->right = node;
			it.mData->root = node;
			return node;
		}

		iterator insert_left_left(iterator it, const T &data)
		{
			if (!it.usable())
				throw runtime_error("Tree node is empty");
			tree_node *node = new tree_node(it.mData, it.mData->left, nullptr, data);
			if (it.mData->left != nullptr)
				it.mData->left->root = node;
			it.mData->left = node;
			return node;
		}

		iterator insert_left_right(iterator it, const T &data)
		{
			if (!it.usable())
				throw runtime_error("Tree node is empty");
			tree_node *node = new tree_node(it.mData, nullptr, it.mData->left, data);
			if (it.mData->left != nullptr)
				it.mData->left->root = node;
			it.mData->left = node;
			return node;
		}

		iterator insert_right_left(iterator it, const T &data)
		{
			if (!it.usable())
				throw runtime_error("Tree node is empty");
			tree_node *node = new tree_node(it.mData, it.mData->right, nullptr, data);
			if (it.mData->right != nullptr)
				it.mData->right->root = node;
			it.mData->right = node;
			return node;
		}

		iterator insert_right_right(iterator it, const T &data)
		{
			if (!it.usable())
				throw runtime_error("Tree node is empty");
			tree_node *node = new tree_node(it.mData, nullptr, it.mData->right, data);
			if (it.mData->right != nullptr)
				it.mData->right->root = node;
			it.mData->right = node;
			return node;
		}

		template <typename... Args>
		iterator emplace_root_left(iterator it, Args &&...args)
		{
			if (it.mData == mRoot) {
				mRoot = new tree_node(nullptr, mRoot, nullptr, std::forward<Args>(args)...);
				return mRoot;
			}
			if (!it.usable())
				throw runtime_error("Tree node is empty");
			tree_node *node = new tree_node(it.mData->root, it.mData, nullptr, std::forward<Args>(args)...);
			if (it.mData->root->left == it.mData)
				it.mData->root->left = node;
			else
				it.mData->root->right = node;
			it.mData->root = node;
			return node;
		}

		template <typename... Args>
		iterator emplace_root_right(iterator it, Args &&...args)
		{
			if (it.mData == mRoot) {
				mRoot = new tree_node(nullptr, nullptr, mRoot, std::forward<Args>(args)...);
				return mRoot;
			}
			if (!it.usable())
				throw runtime_error("Tree node is empty");
			tree_node *node = new tree_node(it.mData->root, nullptr, it.mData, std::forward<Args>(args)...);
			if (it.mData->root->left == it.mData)
				it.mData->root->left = node;
			else
				it.mData->root->right = node;
			it.mData->root = node;
			return node;
		}

		template <typename... Args>
		iterator emplace_left_left(iterator it, Args &&...args)
		{
			if (!it.usable())
				throw runtime_error("Tree node is empty");
			tree_node *node = new tree_node(it.mData, it.mData->left, nullptr, std::forward<Args>(args)...);
			if (it.mData->left != nullptr)
				it.mData->left->root = node;
			it.mData->left = node;
			return node;
		}

		template <typename... Args>
		iterator emplace_left_right(iterator it, Args &&...args)
		{
			if (!it.usable())
				throw runtime_error("Tree node is empty");
			tree_node *node = new tree_node(it.mData, nullptr, it.mData->left, std::forward<Args>(args)...);
			if (it.mData->left != nullptr)
				it.mData->left->root = node;
			it.mData->left = node;
			return node;
		}

		template <typename... Args>
		iterator emplace_right_left(iterator it, Args &&...args)
		{
			if (!it.usable())
				throw runtime_error("Tree node is empty");
			tree_node *node = new tree_node(it.mData, it.mData->right, nullptr, std::forward<Args>(args)...);
			if (it.mData->right != nullptr)
				it.mData->right->root = node;
			it.mData->right = node;
			return node;
		}

		template <typename... Args>
		iterator emplace_right_right(iterator it, Args &&...args)
		{
			if (!it.usable())
				throw runtime_error("Tree node is empty");
			tree_node *node = new tree_node(it.mData, nullptr, it.mData->right, std::forward<Args>(args)...);
			if (it.mData->right != nullptr)
				it.mData->right->root = node;
			it.mData->right = node;
			return node;
		}

		iterator erase(iterator it)
		{
			if (!it.usable())
				throw runtime_error("Tree node is empty");
			if (it.mData == mRoot) {
				destroy(mRoot);
				mRoot = nullptr;
				return nullptr;
			}
			tree_node *root = it.mData->root;
			if (root != nullptr) {
				if (it.mData == root->left)
					root->left = nullptr;
				else
					root->right = nullptr;
			}
			destroy(it.mData);
			return root;
		}

		iterator reserve_left(iterator it)
		{
			if (!it.usable())
				throw runtime_error("Tree node is empty");
			tree_node *reserve = it.mData->left;
			tree_node *root = it.mData->root;
			it.mData->left = nullptr;
			reserve->root = root;
			if (root != nullptr) {
				if (it.mData == root->left)
					root->left = reserve;
				else
					root->right = reserve;
			}
			destroy(it.mData);
			if (it.mData == mRoot)
				mRoot = reserve;
			return reserve;
		}

		iterator reserve_right(iterator it)
		{
			if (!it.usable())
				throw runtime_error("Tree node is empty");
			tree_node *reserve = it.mData->right;
			tree_node *root = it.mData->root;
			it.mData->right = nullptr;
			reserve->root = root;
			if (root != nullptr) {
				if (it.mData == root->left)
					root->left = reserve;
				else
					root->right = reserve;
			}
			destroy(it.mData);
			if (it.mData == mRoot)
				mRoot = reserve;
			return reserve;
		}

		iterator erase_left(iterator it)
		{
			if (!it.usable())
				throw runtime_error("Tree node is empty");
			destroy(it.mData->left);
			it.mData->left = nullptr;
			return it;
		}

		iterator erase_right(iterator it)
		{
			if (!it.usable())
				throw runtime_error("Tree node is empty");
			destroy(it.mData->right);
			it.mData->right = nullptr;
			return it;
		}

		iterator merge(iterator it, const tree_type<T> &tree)
		{
			if (!it.usable())
				throw runtime_error("Tree node is empty");
			tree_node *root = tree.mRoot;
			it.data() = root->data;
			destroy(it.mData->left);
			destroy(it.mData->right);
			it.mData->left = copy(root->left, it.mData);
			it.mData->right = copy(root->right, it.mData);
			return it;
		}
	};

	class event_type final {
	public:
		using listener_type = std::function<bool(void *)>;

	private:
		std::forward_list<listener_type> m_listener;

	public:
		event_type() = delete;

		event_type(const event_type &) = delete;

		explicit event_type(listener_type default_listener)
		{
			m_listener.emplace_front(std::move(default_listener));
		}

		void add_listener(listener_type listener)
		{
			m_listener.emplace_front(std::move(listener));
		}

		bool touch(void *arg)
		{
			for (auto &listener : m_listener)
				if (listener(arg))
					return true;
			return false;
		}
	};

	namespace codecvt {
		class charset {
		public:
			virtual ~charset() = default;

			virtual std::u32string local2wide(const std::deque<char> &) = 0;

			virtual std::u32string local2wide(std::string_view) = 0;

			virtual std::string wide2local(const std::u32string &) = 0;

			virtual bool is_identifier(char32_t) = 0;
		};

		class ascii final : public charset {
		public:
			std::u32string local2wide(const std::deque<char> &local) override
			{
				return std::u32string(local.begin(), local.end());
			}

			std::u32string local2wide(std::string_view local) override
			{
				return std::u32string(local.begin(), local.end());
			}

			std::string wide2local(const std::u32string &wide) override
			{
				return std::string(wide.begin(), wide.end());
			}

			bool is_identifier(char32_t ch) override
			{
				return ch == '_' || std::iswalnum(ch);
			}
		};

		class utf8 final : public charset {
		public:
			std::u32string local2wide(const std::deque<char> &local) override;

			std::u32string local2wide(std::string_view local) override;

			std::string wide2local(const std::u32string &ustr) override;

			bool is_identifier(char32_t ch) override;
		};

		class gbk final : public charset {
		public:
			std::u32string local2wide(const std::deque<char> &local) override;

			std::u32string local2wide(std::string_view local) override;

			std::string wide2local(const std::u32string &wide) override;

			bool is_identifier(char32_t ch) override;
		};
	} // namespace codecvt
} // namespace cs

static std::string operator+(const std::string &lhs, const cs::string_borrower &rhs)
{
	return lhs + rhs.data();
}

static std::ostream &operator<<(std::ostream &lhs, const cs::string_borrower &rhs)
{
	return lhs << rhs.data();
}