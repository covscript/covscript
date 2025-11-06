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
		static long to_integer(const T &val)
		{
			return static_cast<long>(val);
		}
	};

	template <typename T>
	struct to_integer_if<T, false> {
		static long to_integer(const T &)
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
	static std::string to_string(const T &val)
	{
		return to_string_if<T, to_string_helper<T>::value>::to_string(val);
	}

	template <typename T>
	static long to_integer(const T &val)
	{
		return to_integer_if<T, cov::castable<T, long>::value>::to_integer(val);
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

	constexpr std::size_t aligned_element_size = (std::max) (alignof(std::max_align_t), sizeof(void *));

	template <std::size_t align_size,
	          template <typename> class allocator_t>
	class basic_var final {
		friend class any;

		template <typename T>
		static allocator_t<T> &get_allocator()
		{
			static allocator_t<T> allocator;
			return allocator;
		}

		static_assert(align_size % 8 == 0, "align_size must be a multiple of 8.");
		static_assert(align_size >= aligned_element_size,
		              "align_size must greater than alignof(std::max_align_t).");

		using aligned_storage_t = std::aligned_storage_t<align_size - aligned_element_size, alignof(std::max_align_t)>;

		enum class var_op
		{
			get,
			copy,
			move,
			destroy,
			compare,
			rtti_type,
			type_name,
			to_integer,
			to_string,
			hash,
			detach,
			ext_ns
		};

		union var_op_result
		{
			const std::type_info *_typeid;
			std::size_t _hash;
			void *_ptr;
			long _int;
			var_op_result() : _ptr(nullptr) {}
		};

		template <typename T>
		struct var_op_svo_dispatcher
		{
			template <typename... ArgsT>
			static void construct(basic_var *val, ArgsT &&...args)
			{
				::new (&val->m_store.buffer) T(std::forward<ArgsT>(args)...);
			}
			static var_op_result dispatcher(var_op, const basic_var *, void *);
		};

		template <typename T>
		struct var_op_heap_dispatcher
		{
			template <typename... ArgsT>
			static void construct(basic_var *val, ArgsT &&...args)
			{
				T *ptr = get_allocator<T>().allocate(1);
				::new (ptr) T(std::forward<ArgsT>(args)...);
				val->m_store.ptr = ptr;
			}
			static var_op_result dispatcher(var_op, const basic_var *, void *);
		};

		using dispatcher_t = var_op_result (*)(var_op, const basic_var *, void *);

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
			return *static_cast<T *>(m_dispatcher(var_op::get, this, nullptr)._ptr);
		}

		template <typename T>
		inline const T &unchecked_get() const noexcept
		{
			return *static_cast<const T *>(m_dispatcher(var_op::get, this, nullptr)._ptr);
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
				m_dispatcher(var_op::destroy, this, nullptr);
				m_dispatcher = nullptr;
			}
		}

		inline void copy_store(const basic_var &other)
		{
			destroy_store();
			if (other.m_dispatcher != nullptr) {
				other.m_dispatcher(var_op::copy, &other, this);
				m_dispatcher = other.m_dispatcher;
			}
		}

		inline void move_store(basic_var &&other)
		{
			destroy_store();
			if (other.m_dispatcher != nullptr) {
				other.m_dispatcher(var_op::move, &other, this);
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
			std::swap(m_dispatcher, other.m_dispatcher);
			std::swap(m_store, other.m_store);
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
			return *m_dispatcher(var_op::rtti_type, this, nullptr)._typeid;
		}

		template <typename T, typename store_t = cs_impl::var_storage_t<T>>
		inline bool is_type_of() const noexcept
		{
			return m_dispatcher == &dispatcher_class<store_t>::dispatcher || type() == typeid(T);
		}

		inline bool compare(const basic_var &obj) const
		{
			return (m_dispatcher == obj.m_dispatcher || type() == obj.type()) && m_dispatcher(var_op::compare, this, (void *) &obj)._int;
		}

		inline long to_integer() const
		{
			return m_dispatcher(var_op::to_integer, this, nullptr)._int;
		}

		inline std::string to_string() const
		{
			std::string str;
			m_dispatcher(var_op::to_string, this, &str);
			return str;
		}

		inline std::size_t hash() const
		{
			return m_dispatcher(var_op::hash, this, nullptr)._hash;
		}

		inline void detach()
		{
			m_dispatcher(var_op::detach, this, nullptr);
		}

		inline cs::namespace_t &get_ext() const
		{
			return *static_cast<cs::namespace_t *>(m_dispatcher(var_op::ext_ns, this, nullptr)._ptr);
		}

		inline const char *get_type_name() const
		{
			return static_cast<const char *>(m_dispatcher(var_op::type_name, this, nullptr)._ptr);
		}
	};

// Be careful when you adjust the buffer size.
	constexpr std::size_t default_allocate_buffer_size = 64;
	constexpr std::size_t default_allocate_buffer_multiplier = 8;
	template <typename T>
	using default_allocator_provider = std::allocator<T>;
	template <typename T>
	using default_allocator = cs::allocator_type<T, default_allocate_buffer_size, default_allocator_provider>;

	class any final {
		struct align_helper
		{
			bool is_rvalue = false;
			std::uint8_t protect_level = 0;
			std::uint32_t refcount = 1;
		};
		struct alignas(64) proxy
		{
			bool is_rvalue = false;
			std::uint8_t protect_level = 0;
			std::uint32_t refcount = 1;
			basic_var<64 - sizeof(align_helper), default_allocator> data;

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

		static cs::allocator_type<proxy, default_allocate_buffer_size * default_allocate_buffer_multiplier, default_allocator_provider> allocator;

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
					allocator.free(mDat);
					mDat = nullptr;
				}
			}
		}

		explicit any(proxy *dat) : mDat(dat) {}

	public:
		void swap(any &obj, bool raw = false)
		{
			if (this->mDat != nullptr && obj.mDat != nullptr && raw) {
				if (mDat->is_rvalue || this->mDat->protect_level > 0 || obj.mDat->protect_level > 0)
					throw cov::error("E000J");
				this->mDat->data.swap(obj.mDat->data);
			}
			else
				std::swap(this->mDat, obj.mDat);
		}

		void swap(any &&obj, bool raw = false)
		{
			if (this->mDat != nullptr && obj.mDat != nullptr && raw) {
				if (mDat->is_rvalue || this->mDat->protect_level > 0 || obj.mDat->protect_level > 0)
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
				proxy *dat = allocator.alloc(1, mDat->data);
				recycle();
				mDat = dat;
			}
		}

		void try_move() const
		{
			if (mDat != nullptr && mDat->refcount == 1)
			{
				mDat->protect_level = 0;
				mDat->is_rvalue = true;
			}
		}

		bool usable() const noexcept
		{
			return mDat != nullptr;
		}

		template <typename T, typename... ArgsT>
		static any make(ArgsT &&...args)
		{
			proxy *dat = allocator.alloc();
			dat->protect_level = 0;
			dat->data.construct_store<cs_impl::var_storage_t<T>>(std::forward<ArgsT>(args)...);
			return any(dat);
		}

		template <typename T, typename... ArgsT>
		static any make_protect(ArgsT &&...args)
		{
			proxy *dat = allocator.alloc();
			dat->protect_level = 1;
			dat->data.construct_store<cs_impl::var_storage_t<T>>(std::forward<ArgsT>(args)...);
			return any(dat);
		}

		template <typename T, typename... ArgsT>
		static any make_constant(ArgsT &&...args)
		{
			proxy *dat = allocator.alloc();
			dat->protect_level = 2;
			dat->data.construct_store<cs_impl::var_storage_t<T>>(std::forward<ArgsT>(args)...);
			return any(dat);
		}

		template <typename T, typename... ArgsT>
		static any make_single(ArgsT &&...args)
		{
			proxy *dat = allocator.alloc();
			dat->protect_level = 3;
			dat->data.construct_store<cs_impl::var_storage_t<T>>(std::forward<ArgsT>(args)...);
			return any(dat);
		}

		constexpr any() = default;

		template <typename T>
		any(const T &dat)
		{
			mDat = allocator.alloc();
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

		long to_integer() const
		{
			if (this->mDat == nullptr)
				return 0;
			return this->mDat->data.to_integer();
		}

		std::string to_string() const
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
			return this->mDat != nullptr && this->mDat->is_rvalue;
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

		void mark_as_rvalue(bool value) const
		{
			if (this->mDat != nullptr)
				this->mDat->is_rvalue = value;
		}

		void protect()
		{
			if (this->mDat != nullptr) {
				if (this->mDat->protect_level > 1)
					throw cov::error("E000G");
				this->mDat->protect_level = 1;
			}
		}

		void constant()
		{
			if (this->mDat != nullptr) {
				if (this->mDat->protect_level > 2)
					throw cov::error("E000G");
				this->mDat->protect_level = 2;
			}
		}

		void single()
		{
			if (this->mDat != nullptr) {
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
					if (mDat->is_rvalue || this->mDat->protect_level > 0 || obj.mDat->protect_level > 0)
						throw cov::error("E000J");
					mDat->data.copy_store(obj.mDat->data);
				}
				else {
					recycle();
					if (obj.mDat != nullptr)
						mDat = allocator.alloc(1, obj.mDat->data);
					else
						mDat = nullptr;
				}
			}
		}

		template <typename T>
		void assign(const T &dat, bool raw = false)
		{
			if (mDat != nullptr && raw) {
				if (mDat->is_rvalue || this->mDat->protect_level > 0)
					throw cov::error("E000J");
				mDat->data.construct_store<var_storage_t<T>>(dat);
			}
			else {
				recycle();
				mDat = allocator.alloc();
				mDat->data.construct_store<var_storage_t<T>>(dat);
			}
		}

		template <typename T>
		any &operator=(const T &dat)
		{
			assign(dat);
			return *this;
		}
	};

	template <>
	std::string to_string<std::string>(const std::string &str)
	{
		return str;
	}

	template <>
	std::string to_string<bool>(const bool &v)
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

std::ostream &operator<<(std::ostream &, const cs_impl::any &);

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
    cs_impl::basic_var<align_size, allocator_t>::var_op op, const cs_impl::basic_var<align_size, allocator_t> *lhs, void *rhs)
{
	const T *ptr = reinterpret_cast<const T *>(&lhs->m_store.buffer);
	var_op_result result;
	switch (op) {
	case var_op::get:
		result._ptr = const_cast<T *>(ptr);
		break;
	case var_op::copy:
		::new (&static_cast<basic_var *>(rhs)->m_store.buffer) T(*ptr);
		break;
	case var_op::move:
		::new (&static_cast<basic_var *>(rhs)->m_store.buffer) T(std::move(*const_cast<T *>(ptr)));
		break;
	case var_op::destroy:
		ptr->~T();
		break;
	case var_op::compare:
		result._int = cs_impl::compare(lhs->template unchecked_get<T>(), static_cast<const basic_var *>(rhs)->template unchecked_get<T>());
		break;
	case var_op::rtti_type:
		result._typeid = &typeid(T);
		break;
	case var_op::type_name:
		result._ptr = (void *) cs_impl::get_name_of_type<T>();
		break;
	case var_op::to_integer:
		result._int = cs_impl::to_integer(*ptr);
		break;
	case var_op::to_string:
		*static_cast<std::string *>(rhs) = cs_impl::to_string(*ptr);
		break;
	case var_op::hash:
		result._hash = cs_impl::hash<T>(*ptr);
		break;
	case var_op::detach:
		cs_impl::detach<T>(*const_cast<T *>(ptr));
		break;
	case var_op::ext_ns:
		result._ptr = &cs_impl::get_ext<T>();
	}
	return result;
}

template <std::size_t align_size, template <typename> class allocator_t>
template <typename T>
typename cs_impl::basic_var<align_size, allocator_t>::var_op_result cs_impl::basic_var<align_size, allocator_t>::var_op_heap_dispatcher<T>::dispatcher(
    cs_impl::basic_var<align_size, allocator_t>::var_op op, const cs_impl::basic_var<align_size, allocator_t> *lhs, void *rhs)
{
	const T *ptr = static_cast<const T *>(lhs->m_store.ptr);
	var_op_result result;
	switch (op) {
	case var_op::get:
		result._ptr = const_cast<T *>(ptr);
		break;
	case var_op::copy: {
		T *nptr = get_allocator<T>().allocate(1);
		::new (nptr) T(*ptr);
		static_cast<basic_var *>(rhs)->m_store.ptr = nptr;
		break;
	}
	case var_op::move: {
		T *nptr = get_allocator<T>().allocate(1);
		::new (nptr) T(std::move(*const_cast<T *>(ptr)));
		static_cast<basic_var *>(rhs)->m_store.ptr = nptr;
		break;
	}
	case var_op::destroy:
		ptr->~T();
		get_allocator<T>().deallocate(const_cast<T *>(ptr), 1);
		break;
	case var_op::compare:
		result._int = cs_impl::compare(lhs->template unchecked_get<T>(), static_cast<const basic_var *>(rhs)->template unchecked_get<T>());
		break;
	case var_op::rtti_type:
		result._typeid = &typeid(T);
		break;
	case var_op::type_name:
		result._ptr = (void *) cs_impl::get_name_of_type<T>();
		break;
	case var_op::to_integer:
		result._int = cs_impl::to_integer(*ptr);
		break;
	case var_op::to_string:
		*static_cast<std::string *>(rhs) = cs_impl::to_string(*ptr);
		break;
	case var_op::hash:
		result._hash = cs_impl::hash<T>(*ptr);
		break;
	case var_op::detach:
		cs_impl::detach<T>(*const_cast<T *>(ptr));
		break;
	case var_op::ext_ns:
		result._ptr = &cs_impl::get_ext<T>();
	}
	return result;
}