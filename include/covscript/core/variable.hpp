#pragma once
/*
 * Covariant Script Variable
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
#include <covscript/import/mozart/traits.hpp>

namespace cs_impl {
// Name Demangle
	std::string cxx_demangle(const char *);

// Type support auto-detection(SFINAE)
	template <typename...>
	using void_t = void;

// Compare
	template <typename _Tp>
	class compare_helper {
		template <typename T, typename X = bool>
		struct matcher;

		template <typename T>
		static constexpr bool match(T *)
		{
			return false;
		}

		template <typename T>
		static constexpr bool match(matcher<T, decltype(std::declval<T>() == std::declval<T>())> *)
		{
			return true;
		}

	public:
		static constexpr bool value = match<_Tp>(nullptr);
	};

	template <typename, bool>
	struct compare_if;

	template <typename T>
	struct compare_if<T, true> {
		static bool compare(const T &a, const T &b)
		{
			return a == b;
		}
	};

	template <typename T>
	struct compare_if<T, false> {
		static bool compare(const T &a, const T &b)
		{
			return &a == &b;
		}
	};

// To String
	template <typename _Tp>
	class to_string_helper {
		template <typename T, typename X>
		struct matcher;

		template <typename T>
		static constexpr bool match(T *)
		{
			return false;
		}

		template <typename T>
		static constexpr bool match(matcher<T, decltype(std::to_string(std::declval<T>()))> *)
		{
			return true;
		}

	public:
		static constexpr bool value = match<_Tp>(nullptr);
	};

	template <typename, bool>
	struct to_string_if;

	template <typename T>
	struct to_string_if<T, true> {
		static std::string to_string(const T &val)
		{
			return std::to_string(val);
		}
	};

// To Integer
	template <typename, bool>
	struct to_integer_if;

	template <typename T>
	struct to_integer_if<T, true> {
		static std::intptr_t to_integer(const T &val)
		{
			return static_cast<std::intptr_t>(val);
		}
	};

	template <typename T>
	struct to_integer_if<T, false> {
		static std::intptr_t to_integer(const T &)
		{
			throw cov::error("E000M");
		}
	};

// Hash
	template <typename T, typename = void>
	struct hash_helper {
		static constexpr bool value = false;
	};

	template <typename T>
	struct hash_helper<T, void_t<decltype(std::hash<T> {}(std::declval<T>()))>> {
		static constexpr bool value = true;
	};

	template <typename T>
	struct hash_gen_base {
		static std::size_t base_code;
	};

	template <typename T>
	std::size_t hash_gen_base<T>::base_code = typeid(T).hash_code();

	template <typename, typename, bool>
	struct hash_if;

	template <typename T, typename X>
	struct hash_if<T, X, true> {
		static std::size_t hash(const X &val)
		{
			static std::hash<T> gen;
			return gen(static_cast<const T>(val)) + hash_gen_base<X>::base_code;
		}
	};

	template <typename T>
	struct hash_if<T, T, true> {
		static std::size_t hash(const T &val)
		{
			static std::hash<T> gen;
			return gen(val);
		}
	};

	template <typename T, typename X>
	struct hash_if<T, X, false> {
		static std::size_t hash(const X &val)
		{
			throw cov::error("E000F");
		}
	};

	template <typename, bool>
	struct hash_enum_resolver;

	template <typename T>
	struct hash_enum_resolver<T, true> {
		using type = hash_if<std::size_t, T, true>;
	};

	template <typename T>
	struct hash_enum_resolver<T, false> {
		using type = hash_if<T, T, hash_helper<T>::value>;
	};

	/*
	 * Type support
	 * You can specialize template functions to support type-related functions.
	 * But if you do not have specialization, CovScript can also automatically detect related functions.
	 */
	template <typename T>
	static bool compare(const T &a, const T &b)
	{
		return compare_if<T, compare_helper<T>::value>::compare(a, b);
	}

	template <typename T>
	static cs::string_borrower to_string(const T &val)
	{
		return to_string_if<T, to_string_helper<T>::value>::to_string(val);
	}

	template <typename T>
	static std::intptr_t to_integer(const T &val)
	{
		return to_integer_if<T, cov::castable<T, std::intptr_t>::value>::to_integer(val);
	}

	template <typename T>
	static std::size_t hash(const T &val)
	{
		using type = typename hash_enum_resolver<T, std::is_enum<T>::value>::type;
		return type::hash(val);
	}

	template <typename T>
	static void detach(T &val)
	{
		// Do something if you want when data is copying.
	}

	template <typename T>
	constexpr const char *get_name_of_type()
	{
		return typeid(T).name();
	}

	template <typename T>
	struct to_string_if<T, false> {
		static std::string to_string(const T &)
		{
			return "[" + cxx_demangle(get_name_of_type<T>()) + "]";
		}
	};

	template <typename T>
	static cs::namespace_t &get_ext()
	{
		throw cs::runtime_error(std::string("Target type \"") + cs_impl::cxx_demangle(cs_impl::get_name_of_type<T>()) +
		                        "\" doesn't have extension field.");
	}

	template <typename _Target>
	struct type_conversion_cs {
		using source_type = _Target;
		using _not_specialized = void;
	};

	template <typename _Source>
	struct type_conversion_cpp {
		using target_type = _Source;
		using _not_specialized = void;
	};

	template <typename _From, typename _To>
	struct type_convertor {
		using _not_specialized = void;
		template <typename T>
		static inline _To convert(T &&val) noexcept
		{
			return std::move(static_cast<_To>(std::forward<T>(val)));
		}
	};

	template <typename T>
	struct type_convertor<T, T> {
		template <typename X>
		static inline X &&convert(X &&val) noexcept
		{
			return std::forward<X>(val);
		}
	};

	template <typename T>
	struct type_convertor<T, void> {
		template <typename X>
		static inline void convert(X &&) noexcept
		{
		}
	};

	template <typename T>
	struct var_storage {
		using type = T;
	};

	template <typename T>
	using var_storage_t = typename var_storage<std::decay_t<T>>::type;

	namespace operators {
		enum class type
		{
			get,
			copy,
			move,
			swap,
			destroy,
			type_id,    // typeid(val)
			type_name,  // type(val)
			to_integer, // to_integer(val)
			to_string,  // to_string(val)
			hash,       // hash(val)
			detach,     // clone(val)
			ext_ns,     // helper for access
			add,        // lhs + rhs
			sub,        // lhs - rhs
			mul,        // lhs * rhs
			div,        // lhs / rhs
			mod,        // lhs % rhs
			pow,        // lhs ^ rhs
			minus,      // -val
			escape,     // *val
			selfinc,    // ++val
			selfdec,    // --val
			compare,    // lhs == rhs
			abocmp,     // lhs > rhs
			undcmp,     // lhs < rhs
			aeqcmp,     // lhs >= rhs
			ueqcmp,     // lhs <= rhs
			index,      // data[index]
			access,     // data.member
			fcall,      // func(args)
		};
	}

	constexpr std::size_t aligned_element_size = (std::max) (alignof(std::max_align_t), sizeof(void *));

	template <std::size_t align_size,
	          template <typename> class allocator_t = default_allocator>
	class basic_var final {
		friend class any;

		static_assert(align_size % 8 == 0, "align_size must be a multiple of 8.");
		static_assert(align_size >= aligned_element_size,
		              "align_size must greater than alignof(std::max_align_t).");

		using aligned_storage_t = std::aligned_storage_t<align_size - aligned_element_size, alignof(std::max_align_t)>;

		union var_op_result
		{
			std::uintptr_t _uint;
			std::intptr_t _int;
			void *_ptr;

			constexpr var_op_result() : _ptr(nullptr) {}

			static constexpr var_op_result from_ptr(void *p) noexcept
			{
				var_op_result r;
				r._ptr = p;
				return r;
			}

			static constexpr var_op_result from_int(std::intptr_t i) noexcept
			{
				var_op_result r;
				r._int = i;
				return r;
			}

			static constexpr var_op_result from_uint(std::uintptr_t ui) noexcept
			{
				var_op_result r;
				r._uint = ui;
				return r;
			}
		};

		template <typename T>
		static var_op_result call_operator(operators::type, const basic_var *, void *);

		template <typename T>
		struct var_op_svo_dispatcher
		{
			template <typename... ArgsT>
			static inline void construct(basic_var *val, ArgsT &&...args)
			{
				::new (&val->m_store.buffer) T(std::forward<ArgsT>(args)...);
			}
			static inline var_op_result dispatcher(operators::type, const basic_var *, void *);
		};

		template <typename T>
		struct var_op_heap_dispatcher
		{
			inline static allocator_t<T> &get_allocator()
			{
				static allocator_t<T> allocator;
				return allocator;
			}
			template <typename... ArgsT>
			static inline void construct(basic_var *val, ArgsT &&...args)
			{
				T *ptr = get_allocator().allocate(1);
				::new (ptr) T(std::forward<ArgsT>(args)...);
				val->m_store.ptr = ptr;
			}
			static inline var_op_result dispatcher(operators::type, const basic_var *, void *);
		};

		using dispatcher_t = var_op_result (*)(operators::type, const basic_var *, void *);

		template <typename T>
		using dispatcher_class = std::conditional_t<(sizeof(T) > sizeof(aligned_storage_t)),
		      var_op_heap_dispatcher<T>, var_op_svo_dispatcher<T>>;

		dispatcher_t m_dispatcher = nullptr;
		union store_impl
		{
			aligned_storage_t buffer;
			void *ptr;

			store_impl() : ptr(nullptr) {}
		} m_store;

		template <typename T>
		inline T &unchecked_get() noexcept
		{
			return *static_cast<T *>(m_dispatcher(operators::type::get, this, nullptr)._ptr);
		}

		template <typename T>
		inline const T &unchecked_get() const noexcept
		{
			return *static_cast<const T *>(m_dispatcher(operators::type::get, this, nullptr)._ptr);
		}

		template <typename T, typename... ArgsT>
		inline void construct_store(ArgsT &&...args)
		{
			destroy_store();
			m_dispatcher = &dispatcher_class<T>::dispatcher;
			dispatcher_class<T>::construct(this, std::forward<ArgsT>(args)...);
		}

		inline void destroy_store()
		{
			if (m_dispatcher != nullptr) {
				m_dispatcher(operators::type::destroy, this, nullptr);
				m_dispatcher = nullptr;
			}
		}

		inline void copy_store(const basic_var &other)
		{
			destroy_store();
			if (other.m_dispatcher != nullptr) {
				other.m_dispatcher(operators::type::copy, &other, this);
				m_dispatcher = other.m_dispatcher;
			}
		}

		inline void move_store(basic_var &other)
		{
			destroy_store();
			if (other.m_dispatcher != nullptr) {
				other.m_dispatcher(operators::type::move, &other, this);
				m_dispatcher = other.m_dispatcher;
				other.m_dispatcher = nullptr;
			}
		}

		basic_var() noexcept = default;

	public:
		template <typename T, typename store_t = cs_impl::var_storage_t<T>, typename... ArgsT>
		inline static basic_var make(ArgsT &&...args)
		{
			basic_var data;
			data.construct_store<store_t>(std::forward<ArgsT>(args)...);
			return data;
		}

		inline void swap(basic_var &other) noexcept
		{
			if (m_dispatcher != other.m_dispatcher && type() != other.type())
			{
				basic_var tmp;
				tmp.move_store(*this);
				move_store(other);
				other.move_store(tmp);
			}
			else
				m_dispatcher(operators::type::swap, this, (void *) &other);
		}

		template <typename T, typename store_t = cs_impl::var_storage_t<T>,
		          typename = std::enable_if_t<!std::is_same<store_t, basic_var>::value>>
		basic_var(T &&val)
		{
			construct_store<store_t>(std::forward<T>(val));
		}

		basic_var(const basic_var &v)
		{
			copy_store(v);
		}

		basic_var(basic_var &&v) noexcept
		{
			move_store(std::move(v));
		}

		~basic_var()
		{
			destroy_store();
		}

		basic_var &operator=(const basic_var &obj)
		{
			if (&obj != this)
				copy_store(obj);
			return *this;
		}

		basic_var &operator=(basic_var &&obj) noexcept
		{
			if (&obj != this)
			{
				destroy_store();
				m_dispatcher = obj.m_dispatcher;
				m_store = obj.m_store;
				obj.m_dispatcher = nullptr;
			}
			return *this;
		}

		inline const std::type_info &type() const noexcept
		{
			return *static_cast<const std::type_info *>(m_dispatcher(operators::type::type_id, this, nullptr)._ptr);
		}

		template <typename T, typename store_t = cs_impl::var_storage_t<T>>
		inline bool is_type_of() const noexcept
		{
			return m_dispatcher == &dispatcher_class<store_t>::dispatcher || type() == typeid(T);
		}

		inline bool compare(const basic_var &obj) const
		{
			return (m_dispatcher == obj.m_dispatcher || type() == obj.type()) && m_dispatcher(operators::type::compare, this, (void *) &obj)._int;
		}

		inline std::intptr_t to_integer() const
		{
			return m_dispatcher(operators::type::to_integer, this, nullptr)._int;
		}

		inline cs::string_borrower to_string() const
		{
			cs::string_borrower borrower;
			m_dispatcher(operators::type::to_string, this, &borrower);
			return borrower;
		}

		inline std::size_t hash() const
		{
			return m_dispatcher(operators::type::hash, this, nullptr)._uint;
		}

		inline void detach()
		{
			m_dispatcher(operators::type::detach, this, nullptr);
		}

		inline cs::namespace_t &get_ext() const
		{
			return *static_cast<cs::namespace_t *>(m_dispatcher(operators::type::ext_ns, this, nullptr)._ptr);
		}

		inline const char *get_type_name() const
		{
			return static_cast<const char *>(m_dispatcher(operators::type::type_name, this, nullptr)._ptr);
		}
	};

#ifndef CS_VAR_ALLOC_MULTIPLIER
#define CS_VAR_ALLOC_MULTIPLIER 8
#endif

#ifndef CS_VAR_SVO_ALIGN
#define CS_VAR_SVO_ALIGN 32
#endif

	class any final {
		template <std::size_t align_size, template <typename> class allocator_t>
		friend class basic_var;

		struct proxy
		{
			std::uint32_t refcount = 1;
			std::int8_t protect_level = 0;
			basic_var<CS_VAR_SVO_ALIGN> data;

			proxy() = default;

			template <typename T>
			proxy(std::uint32_t rc, T &&d) : refcount(rc), data(std::forward<T>(d))
			{
			}

			template <typename T>
			proxy(std::uint8_t pl, std::uint32_t rc, T &&d) : protect_level(pl), refcount(rc), data(std::forward<T>(d))
			{
			}
		};

		using allocator_t = cs::allocator_type<proxy, CS_ALLOCATOR_BUFFER_MAX * CS_VAR_ALLOC_MULTIPLIER, default_allocator_provider>;

		inline static allocator_t &get_allocator()
		{
			static allocator_t allocator;
			return allocator;
		}

		proxy *mDat = nullptr;

		proxy *duplicate() const noexcept
		{
			if (mDat != nullptr)
			{
				++mDat->refcount;
			}
			return mDat;
		}

		void recycle() noexcept
		{
			if (mDat != nullptr)
			{
				if (--mDat->refcount == 0) {
					get_allocator().free(mDat);
					mDat = nullptr;
				}
			}
		}

		explicit any(proxy *dat) : mDat(dat) {}

	public:
		void swap(any &obj, bool raw = false)
		{
			if (this->mDat != nullptr && obj.mDat != nullptr && raw) {
				if (mDat->protect_level != 0 || obj.mDat->protect_level > 0)
					throw cov::error("E000J");
				this->mDat->data.swap(obj.mDat->data);
			}
			else
				std::swap(this->mDat, obj.mDat);
		}

		void swap(any &&obj, bool raw = false)
		{
			if (this->mDat != nullptr && obj.mDat != nullptr && raw) {
				if (mDat->protect_level != 0 || obj.mDat->protect_level > 0)
					throw cov::error("E000J");
				this->mDat->data.swap(obj.mDat->data);
			}
			else
				std::swap(this->mDat, obj.mDat);
		}

		void clone()
		{
			if (mDat != nullptr) {
				if (mDat->protect_level > 2)
					throw cov::error("E000L");
				proxy *dat = get_allocator().alloc(1, mDat->data);
				recycle();
				mDat = dat;
			}
		}

		void try_move() const
		{
			if (mDat != nullptr && mDat->refcount == 1)
				mDat->protect_level = -1;
		}

		bool usable() const noexcept
		{
			return mDat != nullptr;
		}

		template <typename T, typename... ArgsT>
		static any make(ArgsT &&...args)
		{
			proxy *dat = get_allocator().alloc();
			dat->protect_level = 0;
			dat->data.construct_store<cs_impl::var_storage_t<T>>(std::forward<ArgsT>(args)...);
			return any(dat);
		}

		template <typename T, typename... ArgsT>
		static any make_protect(ArgsT &&...args)
		{
			proxy *dat = get_allocator().alloc();
			dat->protect_level = 1;
			dat->data.construct_store<cs_impl::var_storage_t<T>>(std::forward<ArgsT>(args)...);
			return any(dat);
		}

		template <typename T, typename... ArgsT>
		static any make_constant(ArgsT &&...args)
		{
			proxy *dat = get_allocator().alloc();
			dat->protect_level = 2;
			dat->data.construct_store<cs_impl::var_storage_t<T>>(std::forward<ArgsT>(args)...);
			return any(dat);
		}

		template <typename T, typename... ArgsT>
		static any make_single(ArgsT &&...args)
		{
			proxy *dat = get_allocator().alloc();
			dat->protect_level = 3;
			dat->data.construct_store<cs_impl::var_storage_t<T>>(std::forward<ArgsT>(args)...);
			return any(dat);
		}

		constexpr any() = default;

		template <typename T>
		any(const T &dat)
		{
			mDat = get_allocator().alloc();
			mDat->protect_level = 0;
			mDat->data.construct_store<cs_impl::var_storage_t<T>>(dat);
		}

		any(const any &v) : mDat(v.duplicate()) {}

		any(any &&v) noexcept
		{
			swap(std::forward<any>(v));
		}

		~any()
		{
			recycle();
		}

		const std::type_info &type() const noexcept
		{
			return this->mDat != nullptr ? this->mDat->data.type() : typeid(void);
		}

		template <typename T>
		bool is_type_of() const noexcept
		{
			return this->mDat != nullptr ? this->mDat->data.is_type_of<T>() : false;
		}

		cs::numeric_integer to_integer() const
		{
			if (this->mDat == nullptr)
				return 0;
			return this->mDat->data.to_integer();
		}

		cs::string_borrower to_string() const
		{
			if (this->mDat == nullptr)
				return "Null";
			return this->mDat->data.to_string();
		}

		std::size_t hash() const
		{
			if (this->mDat == nullptr)
				return cs_impl::hash<void *>(nullptr);
			return this->mDat->data.hash();
		}

		void detach() const
		{
			if (this->mDat != nullptr)
			{
				if (this->mDat->protect_level > 2)
					throw cov::error("E000L");
				this->mDat->data.detach();
			}
		}

		cs::namespace_t &get_ext() const
		{
			if (this->mDat == nullptr)
				throw cs::runtime_error("Type doesn't have extension field.");
			return this->mDat->data.get_ext();
		}

		std::string get_type_name() const
		{
			if (this->mDat == nullptr)
				return cxx_demangle(get_name_of_type<void>());
			else
				return cxx_demangle(this->mDat->data.get_type_name());
		}

		bool is_same(const any &obj) const
		{
			return this->mDat == obj.mDat;
		}

		bool is_rvalue() const
		{
			return this->mDat != nullptr && this->mDat->protect_level < 0;
		}

		bool is_trivial() const
		{
			return this->mDat != nullptr && this->mDat->protect_level == 0;
		}

		bool is_protect() const
		{
			return this->mDat != nullptr && this->mDat->protect_level > 0;
		}

		bool is_constant() const
		{
			return this->mDat != nullptr && this->mDat->protect_level > 1;
		}

		bool is_single() const
		{
			return this->mDat != nullptr && this->mDat->protect_level > 2;
		}

		void mark_trivial() const
		{
			if (this->mDat != nullptr)
			{
				if (this->mDat->protect_level > 0)
					throw cov::error("E000G");
				this->mDat->protect_level = 0;
			}
		}

		void mark_protect() const
		{
			if (this->mDat != nullptr)
			{
				if (this->mDat->protect_level > 1)
					throw cov::error("E000G");
				this->mDat->protect_level = 1;
			}
		}

		void mark_constant() const
		{
			if (this->mDat != nullptr)
			{
				if (this->mDat->protect_level > 2)
					throw cov::error("E000G");
				this->mDat->protect_level = 2;
			}
		}

		void mark_single() const
		{
			if (this->mDat != nullptr)
			{
				if (this->mDat->protect_level > 3)
					throw cov::error("E000G");
				this->mDat->protect_level = 3;
			}
		}

		any &operator=(const any &var)
		{
			if (!is_same(var)) {
				recycle();
				mDat = var.duplicate();
			}
			return *this;
		}

		any &operator=(any &&var) noexcept
		{
			if (&var != this)
				swap(std::forward<any>(var));
			return *this;
		}

		bool compare(const any &var) const
		{
			return usable() && var.usable() ? this->mDat->data.compare(var.mDat->data) : !usable() && !var.usable();
		}

		bool operator==(const any &var) const
		{
			return this->compare(var);
		}

		bool operator!=(const any &var) const
		{
			return !this->compare(var);
		}

		template <typename T>
		T &val() const
		{
			if (this->mDat == nullptr)
				throw cov::error("E0005");
			if (!this->mDat->data.is_type_of<T>())
				throw cov::error("E0006");
			if (this->mDat->protect_level > 1)
				throw cov::error("E000K");
			return this->mDat->data.unchecked_get<T>();
		}

		template <typename T>
		const T &const_val() const
		{
			if (this->mDat == nullptr)
				throw cov::error("E0005");
			if (!this->mDat->data.is_type_of<T>())
				throw cov::error("E0006");
			return this->mDat->data.unchecked_get<T>();
		}

		template <typename T>
		explicit operator const T &() const
		{
			return this->const_val<T>();
		}

		void assign(const any &obj, bool raw = false)
		{
			if (&obj != this && obj.mDat != mDat) {
				if (mDat != nullptr && obj.mDat != nullptr && raw) {
					if (mDat->protect_level != 0 || obj.mDat->protect_level > 0)
						throw cov::error("E000J");
					mDat->data.copy_store(obj.mDat->data);
				}
				else {
					recycle();
					if (obj.mDat != nullptr)
						mDat = get_allocator().alloc(1, obj.mDat->data);
					else
						mDat = nullptr;
				}
			}
		}

		template <typename T>
		void assign(const T &dat, bool raw = false)
		{
			if (mDat != nullptr && raw) {
				if (mDat->protect_level != 0)
					throw cov::error("E000J");
				mDat->data.construct_store<var_storage_t<T>>(dat);
			}
			else {
				recycle();
				mDat = get_allocator().alloc();
				mDat->data.construct_store<var_storage_t<T>>(dat);
			}
		}

		template <typename T>
		any &operator=(const T &dat)
		{
			assign(dat);
			return *this;
		}

		any operator+(const any &rhs) const
		{
			if (!usable() || !rhs.usable())
				throw cov::error("E0005");
			else
				return static_cast<proxy *>(mDat->data.m_dispatcher(operators::type::add, &mDat->data, (void *)&rhs)._ptr);
		}

		any operator-(const any &rhs) const
		{
			if (!usable() || !rhs.usable())
				throw cov::error("E0005");
			else
				return static_cast<proxy *>(mDat->data.m_dispatcher(operators::type::sub, &mDat->data, (void *)&rhs)._ptr);
		}

		any operator*(const any &rhs) const
		{
			if (!usable() || !rhs.usable())
				throw cov::error("E0005");
			else
				return static_cast<proxy *>(mDat->data.m_dispatcher(operators::type::mul, &mDat->data, (void *)&rhs)._ptr);
		}

		any operator/(const any &rhs) const
		{
			if (!usable() || !rhs.usable())
				throw cov::error("E0005");
			else
				return static_cast<proxy *>(mDat->data.m_dispatcher(operators::type::div, &mDat->data, (void *)&rhs)._ptr);
		}

		any operator%(const any &rhs) const
		{
			if (!usable() || !rhs.usable())
				throw cov::error("E0005");
			else
				return static_cast<proxy *>(mDat->data.m_dispatcher(operators::type::mod, &mDat->data, (void *)&rhs)._ptr);
		}

		any operator^(const any &rhs) const
		{
			if (!usable() || !rhs.usable())
				throw cov::error("E0005");
			else
				return static_cast<proxy *>(mDat->data.m_dispatcher(operators::type::pow, &mDat->data, (void *)&rhs)._ptr);
		}

		any operator-() const
		{
			if (!usable())
				throw cov::error("E0005");
			else
				return static_cast<proxy *>(mDat->data.m_dispatcher(operators::type::minus, &mDat->data, nullptr)._ptr);
		}

		any &operator*() const
		{
			if (!usable())
				throw cov::error("E0005");
			else
				return *static_cast<any *>(mDat->data.m_dispatcher(operators::type::escape, &mDat->data, nullptr)._ptr);
		}

		any &operator++()
		{
			if (!usable())
				throw cov::error("E0005");
			else
				mDat->data.m_dispatcher(operators::type::selfinc, &mDat->data, nullptr);
			return *this;
		}

		any &operator--()
		{
			if (!usable())
				throw cov::error("E0005");
			else
				mDat->data.m_dispatcher(operators::type::selfdec, &mDat->data, nullptr);
			return *this;
		}

		bool operator>(const any &rhs) const
		{
			if (!usable() || !rhs.usable())
				return !usable() && !rhs.usable();
			else
				return mDat->data.m_dispatcher(operators::type::abocmp, &mDat->data, &rhs.mDat->data)._int;
		}

		bool operator<(const any &rhs) const
		{
			if (!usable() || !rhs.usable())
				return !usable() && !rhs.usable();
			else
				return mDat->data.m_dispatcher(operators::type::undcmp, &mDat->data, &rhs.mDat->data)._int;
		}

		bool operator>=(const any &rhs) const
		{
			if (!usable() || !rhs.usable())
				return !usable() && !rhs.usable();
			else
				return mDat->data.m_dispatcher(operators::type::aeqcmp, &mDat->data, &rhs.mDat->data)._int;
		}

		bool operator<=(const any &rhs) const
		{
			if (!usable() || !rhs.usable())
				return !usable() && !rhs.usable();
			else
				return mDat->data.m_dispatcher(operators::type::ueqcmp, &mDat->data, &rhs.mDat->data)._int;
		}

		any &index(const any &idx) const
		{
			if (!usable() || !idx.usable())
				throw cov::error("E0005");
			else
				return *static_cast<any *>(mDat->data.m_dispatcher(operators::type::index, &mDat->data, (void *)&idx)._ptr);
		}

		any &access(cs::string_borrower member) const
		{
			if (!usable())
				throw cov::error("E0005");
			else
				return *static_cast<any *>(mDat->data.m_dispatcher(operators::type::access, &mDat->data, &member)._ptr);
		}

		any fcall(cs::vector &args) const
		{
			if (!usable())
				throw cov::error("E0005");
			else
				return static_cast<proxy *>(mDat->data.m_dispatcher(operators::type::fcall, &mDat->data, &args)._ptr);
		}
	};

	template <>
	cs::string_borrower to_string<std::string>(const std::string &str)
	{
		return str;
	}

	template <>
	cs::string_borrower to_string<bool>(const bool &v)
	{
		if (v)
			return "true";
		else
			return "false";
	}

	template <>
	struct var_storage<char *> {
		using type = std::string;
	};

	template <>
	struct var_storage<std::type_info> {
		using type = std::type_index;
	};
} // namespace cs_impl

static std::ostream &operator<<(std::ostream &out, const cs_impl::any &val)
{
	out << val.to_string();
	return out;
}

namespace std {
	template <>
	struct hash<cs_impl::any> {
		std::size_t operator()(const cs_impl::any &val) const
		{
			return val.hash();
		}
	};
} // namespace std

template <std::size_t align_size, template <typename> class allocator_t>
template <typename T>
typename cs_impl::basic_var<align_size, allocator_t>::var_op_result cs_impl::basic_var<align_size, allocator_t>::var_op_svo_dispatcher<T>::dispatcher(
    cs_impl::operators::type op, const cs_impl::basic_var<align_size, allocator_t> *lhs, void *rhs)
{
	const T *ptr = reinterpret_cast<const T *>(&lhs->m_store.buffer);
	switch (op) {
	case operators::type::get:
		return var_op_result::from_ptr(const_cast<T *>(ptr));
	case operators::type::copy:
		::new (&static_cast<basic_var *>(rhs)->m_store.buffer) T(*ptr);
		return var_op_result();
	case operators::type::move:
		::new (&static_cast<basic_var *>(rhs)->m_store.buffer) T(std::move(*const_cast<T *>(ptr)));
		return var_op_result();
	case operators::type::swap:
		std::swap(*const_cast<T *>(ptr), static_cast<basic_var *>(rhs)->template unchecked_get<T>());
		return var_op_result();
	case operators::type::destroy:
		ptr->~T();
		return var_op_result();
	default:
		return basic_var::call_operator<T>(op, lhs, rhs);
	}
}

template <std::size_t align_size, template <typename> class allocator_t>
template <typename T>
typename cs_impl::basic_var<align_size, allocator_t>::var_op_result cs_impl::basic_var<align_size, allocator_t>::var_op_heap_dispatcher<T>::dispatcher(
    cs_impl::operators::type op, const cs_impl::basic_var<align_size, allocator_t> *lhs, void *rhs)
{
	const T *ptr = static_cast<const T *>(lhs->m_store.ptr);
	switch (op) {
	case operators::type::get:
		return var_op_result::from_ptr(const_cast<T *>(ptr));
	case operators::type::copy: {
		T *nptr = get_allocator().allocate(1);
		::new (nptr) T(*ptr);
		static_cast<basic_var *>(rhs)->m_store.ptr = nptr;
		return var_op_result();
	}
	case operators::type::move: {
		T *nptr = get_allocator().allocate(1);
		::new (nptr) T(std::move(*const_cast<T *>(ptr)));
		static_cast<basic_var *>(rhs)->m_store.ptr = nptr;
		return var_op_result();
	}
	case operators::type::swap:
		std::swap(*const_cast<T *>(ptr), static_cast<basic_var *>(rhs)->template unchecked_get<T>());
		return var_op_result();
	case operators::type::destroy:
		ptr->~T();
		get_allocator().deallocate(const_cast<T *>(ptr), 1);
		return var_op_result();
	default:
		return basic_var::call_operator<T>(op, lhs, rhs);
	}
}