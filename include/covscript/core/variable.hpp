#pragma once
/*
* Covariant Script Variable
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
#include <covscript/import/mozart/memory.hpp>
#include <type_traits>

namespace cs_impl {
// Name Demangle
	std::string cxx_demangle(const char *);

// Type support auto-detection(SFINAE)
// Compare
	template<typename _Tp>
	class compare_helper {
		template<typename T, typename X=bool>
		struct matcher;

		template<typename T>
		static constexpr bool match(T *)
		{
			return false;
		}

		template<typename T>
		static constexpr bool match(matcher<T, decltype(std::declval<T>() == std::declval<T>())> *)
		{
			return true;
		}

	public:
		static constexpr bool value = match<_Tp>(nullptr);
	};

	template<typename, bool>
	struct compare_if;

	template<typename T>
	struct compare_if<T, true> {
		static bool compare(const T &a, const T &b)
		{
			return a == b;
		}
	};

	template<typename T>
	struct compare_if<T, false> {
		static bool compare(const T &a, const T &b)
		{
			return &a == &b;
		}
	};

// To String
	template<typename _Tp>
	class to_string_helper {
		template<typename T, typename X>
		struct matcher;

		template<typename T>
		static constexpr bool match(T *)
		{
			return false;
		}

		template<typename T>
		static constexpr bool match(matcher<T, decltype(std::to_string(std::declval<T>()))> *)
		{
			return true;
		}

	public:
		static constexpr bool value = match<_Tp>(nullptr);
	};

	template<typename, bool>
	struct to_string_if;

	template<typename T>
	struct to_string_if<T, true> {
		static std::string to_string(const T &val)
		{
			return std::to_string(val);
		}
	};

	template<typename T>
	struct to_string_if<T, false> {
		static std::string to_string(const T &)
		{
			throw cov::error("E000D");
		}
	};

// To Integer
	template<typename, bool>
	struct to_integer_if;

	template<typename T>
	struct to_integer_if<T, true> {
		static long to_integer(const T &val)
		{
			return static_cast<long>(val);
		}
	};

	template<typename T>
	struct to_integer_if<T, false> {
		static long to_integer(const T &)
		{
			throw cov::error("E000M");
		}
	};

// Hash
	template<typename _Tp>
	class hash_helper {
		template<typename T, decltype(&std::hash<T>::operator()) X>
		struct matcher;

		template<typename T>
		static constexpr bool match(T *)
		{
			return false;
		}

		template<typename T>
		static constexpr bool match(matcher<T, &std::hash<T>::operator()> *)
		{
			return true;
		}

	public:
		static constexpr bool value = match<_Tp>(nullptr);
	};

	template<typename T>
	struct hash_gen_base {
		static std::size_t base_code;
	};

	template<typename T> std::size_t hash_gen_base<T>::base_code = typeid(T).hash_code();

	template<typename, typename, bool>
	struct hash_if;

	template<typename T, typename X>
	struct hash_if<T, X, true> {
		static std::size_t hash(const X &val)
		{
			static std::hash<T> gen;
			return gen(static_cast<const T>(val)) + hash_gen_base<X>::base_code;
		}
	};

	template<typename T>
	struct hash_if<T, T, true> {
		static std::size_t hash(const T &val)
		{
			static std::hash<T> gen;
			return gen(val);
		}
	};

	template<typename T, typename X>
	struct hash_if<T, X, false> {
		static std::size_t hash(const X &val)
		{
			throw cov::error("E000F");
		}
	};

	template<typename, bool>
	struct hash_enum_resolver;

	template<typename T>
	struct hash_enum_resolver<T, true> {
		using type=hash_if<std::size_t, T, true>;
	};

	template<typename T>
	struct hash_enum_resolver<T, false> {
		using type=hash_if<T, T, hash_helper<T>::value>;
	};

	/*
	* Type support
	* You can specialize template functions to support type-related functions.
	* But if you do not have specialization, CovScript can also automatically detect related functions.
	*/
	template<typename T>
	static bool compare(const T &a, const T &b)
	{
		return compare_if<T, compare_helper<T>::value>::compare(a, b);
	}

	template<typename T>
	static std::string to_string(const T &val)
	{
		return to_string_if<T, to_string_helper<T>::value>::to_string(val);
	}

	template<typename T>
	static long to_integer(const T &val)
	{
		return to_integer_if<T, cov::castable<T, long>::value>::to_integer(val);
	}

	template<typename T>
	static std::size_t hash(const T &val)
	{
		using type=typename hash_enum_resolver<T, std::is_enum<T>::value>::type;
		return type::hash(val);
	}

	template<typename T>
	static void detach(T &val)
	{
		// Do something if you want when data is copying.
	}

	template<typename T>
	constexpr const char *get_name_of_type()
	{
		return typeid(T).name();
	}

	template<typename T>
	static cs::namespace_t &get_ext()
	{
		throw cs::runtime_error("Target type does not support extensions(Default Extension Function Detected).");
	}

	template<typename _Target>
	struct type_conversion_cs {
		using source_type=_Target;
	};

	template<typename _Source>
	struct type_conversion_cpp {
		using target_type=_Source;
	};

	template<typename _From, typename _To>
	struct type_convertor {
		template<typename T>
		static inline _To convert(T &&val) noexcept
		{
			return std::move(static_cast<_To>(std::forward<T>(val)));
		}
	};

	template<typename T>
	struct type_convertor<T, T> {
		template<typename X>
		static inline X &&convert(X &&val) noexcept
		{
			return std::forward<X>(val);
		}
	};

	template<typename T>
	struct type_convertor<T, void> {
		template<typename X>
		static inline void convert(X &&) noexcept {}
	};

	/*
	* Implementation of Any Container
	* A customized version of Mozart Any(cov::any)
	* Github: https://github.com/mikecovlee/mozart
	*/

// Be careful when you adjust the buffer size.
	constexpr std::size_t default_allocate_buffer_size = 64;
	template<typename T> using default_allocator_provider=std::allocator<T>;

	class any final {
		class baseHolder {
		public:
			baseHolder() = default;

			virtual ~ baseHolder() = default;

			virtual const std::type_info &type() const = 0;

			virtual baseHolder *duplicate() = 0;

			virtual bool compare(const baseHolder *) const = 0;

			virtual long to_integer() const = 0;

			virtual std::string to_string() const = 0;

			virtual std::size_t hash() const = 0;

			virtual void detach() = 0;

			virtual void kill() = 0;

			virtual cs::namespace_t &get_ext() const = 0;

			virtual const char *get_type_name() const = 0;
		};

		template<typename T>
		class holder : public baseHolder {
		protected:
			T mDat;
		public:
			static cov::allocator<holder<T>, default_allocate_buffer_size, default_allocator_provider> allocator;

			holder() = default;

			template<typename...ArgsT>
			explicit holder(ArgsT &&...args):mDat(std::forward<ArgsT>(args)...) {}

			~ holder() override = default;

			const std::type_info &type() const override
			{
				return typeid(T);
			}

			baseHolder *duplicate() override
			{
				return allocator.alloc(mDat);
			}

			bool compare(const baseHolder *obj) const override
			{
				if (obj->type() == this->type())
					return cs_impl::compare(mDat, static_cast<const holder<T> *>(obj)->data());
				else
					return false;
			}

			long to_integer() const override
			{
				return cs_impl::to_integer(mDat);
			}

			std::string to_string() const override
			{
				return cs_impl::to_string(mDat);
			}

			std::size_t hash() const override
			{
				return cs_impl::hash<T>(mDat);
			}

			void detach() override
			{
				cs_impl::detach(mDat);
			}

			void kill() override
			{
				allocator.free(this);
			}

			virtual cs::namespace_t &get_ext() const override
			{
				return cs_impl::get_ext<T>();
			}

			const char *get_type_name() const override
			{
				return cs_impl::get_name_of_type<T>();
			}

			T &data()
			{
				return mDat;
			}

			const T &data() const
			{
				return mDat;
			}

			void data(const T &dat)
			{
				mDat = dat;
			}
		};

		struct proxy {
			bool is_rvalue = false;
			short protect_level = 0;
			std::size_t refcount = 1;
			baseHolder *data = nullptr;

			proxy() = default;

			proxy(std::size_t rc, baseHolder *d) : refcount(rc), data(d) {}

			proxy(short pl, std::size_t rc, baseHolder *d) : protect_level(pl), refcount(rc), data(d) {}

			~proxy()
			{
				if (data != nullptr)
					data->kill();
			}
		};

		static cov::allocator<proxy, default_allocate_buffer_size, default_allocator_provider> allocator;
		proxy *mDat = nullptr;

		proxy *duplicate() const noexcept
		{
			if (mDat != nullptr) {
				++mDat->refcount;
			}
			return mDat;
		}

		void recycle() noexcept
		{
			if (mDat != nullptr) {
				--mDat->refcount;
				if (mDat->refcount == 0) {
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
				if (this->mDat->protect_level > 0 || obj.mDat->protect_level > 0)
					throw cov::error("E000J");
				baseHolder *tmp = this->mDat->data;
				this->mDat->data = obj.mDat->data;
				obj.mDat->data = tmp;
			}
			else {
				proxy *tmp = this->mDat;
				this->mDat = obj.mDat;
				obj.mDat = tmp;
			}
		}

		void swap(any &&obj, bool raw = false)
		{
			if (this->mDat != nullptr && obj.mDat != nullptr && raw) {
				if (this->mDat->protect_level > 0 || obj.mDat->protect_level > 0)
					throw cov::error("E000J");
				baseHolder *tmp = this->mDat->data;
				this->mDat->data = obj.mDat->data;
				obj.mDat->data = tmp;
			}
			else {
				proxy *tmp = this->mDat;
				this->mDat = obj.mDat;
				obj.mDat = tmp;
			}
		}

		void clone()
		{
			if (mDat != nullptr) {
				if (mDat->protect_level > 2)
					throw cov::error("E000L");
				proxy *dat = allocator.alloc(1, mDat->data->duplicate());
				recycle();
				mDat = dat;
			}
		}

		void try_move() const
		{
			if (mDat != nullptr && mDat->refcount == 1) {
				mDat->protect_level = 0;
				mDat->is_rvalue = true;
			}
		}

		bool usable() const noexcept
		{
			return mDat != nullptr;
		}

		template<typename T, typename...ArgsT>
		static any make(ArgsT &&...args)
		{
			return any(allocator.alloc(1, holder<T>::allocator.alloc(std::forward<ArgsT>(args)...)));
		}

		template<typename T, typename...ArgsT>
		static any make_protect(ArgsT &&...args)
		{
			return any(allocator.alloc(1, 1, holder<T>::allocator.alloc(std::forward<ArgsT>(args)...)));
		}

		template<typename T, typename...ArgsT>
		static any make_constant(ArgsT &&...args)
		{
			return any(allocator.alloc(2, 1, holder<T>::allocator.alloc(std::forward<ArgsT>(args)...)));
		}

		template<typename T, typename...ArgsT>
		static any make_single(ArgsT &&...args)
		{
			return any(allocator.alloc(3, 1, holder<T>::allocator.alloc(std::forward<ArgsT>(args)...)));
		}

		constexpr any() = default;

		template<typename T>
		any(const T &dat):mDat(allocator.alloc(1, holder<T>::allocator.alloc(dat))) {}

		any(const any &v) : mDat(v.duplicate()) {}

		any(any &&v) noexcept
		{
			swap(std::forward<any>(v));
		}

		~any()
		{
			recycle();
		}

		const std::type_info &type() const
		{
			return this->mDat != nullptr ? this->mDat->data->type() : typeid(void);
		}

		long to_integer() const
		{
			if (this->mDat == nullptr)
				return 0;
			return this->mDat->data->to_integer();
		}

		std::string to_string() const
		{
			if (this->mDat == nullptr)
				return "Null";
			return this->mDat->data->to_string();
		}

		std::size_t hash() const
		{
			if (this->mDat == nullptr)
				return cs_impl::hash<void *>(nullptr);
			return this->mDat->data->hash();
		}

		void detach() const
		{
			if (this->mDat != nullptr) {
				if (this->mDat->protect_level > 2)
					throw cov::error("E000L");
				this->mDat->data->detach();
			}
		}

		cs::namespace_t &get_ext() const
		{
			if (this->mDat == nullptr)
				throw cs::runtime_error("Target type does not support extensions.");
			return this->mDat->data->get_ext();
		}

		std::string get_type_name() const
		{
			if (this->mDat == nullptr)
				return cxx_demangle(get_name_of_type<void>());
			else
				return cxx_demangle(this->mDat->data->get_type_name());
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
			if (&var != this) {
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
			return usable() && var.usable() ? this->mDat->data->compare(var.mDat->data) : !usable() && !var.usable();
		}

		bool operator==(const any &var) const
		{
			return this->compare(var);
		}

		bool operator!=(const any &var) const
		{
			return !this->compare(var);
		}

		template<typename T>
		T &val(bool raw = false)
		{
			if (typeid(T) != this->type())
				throw cov::error("E0006");
			if (this->mDat == nullptr)
				throw cov::error("E0005");
			if (this->mDat->protect_level > 1)
				throw cov::error("E000K");
			if (!raw)
				clone();
			return static_cast<holder<T> *>(this->mDat->data)->data();
		}

		template<typename T>
		const T &val(bool raw = false) const
		{
			if (typeid(T) != this->type())
				throw cov::error("E0006");
			if (this->mDat == nullptr)
				throw cov::error("E0005");
			return static_cast<const holder<T> *>(this->mDat->data)->data();
		}

		template<typename T>
		const T &const_val() const
		{
			if (typeid(T) != this->type())
				throw cov::error("E0006");
			if (this->mDat == nullptr)
				throw cov::error("E0005");
			return static_cast<const holder<T> *>(this->mDat->data)->data();
		}

		template<typename T>
		explicit operator const T &() const
		{
			return this->const_val<T>();
		}

		void assign(const any &obj, bool raw = false)
		{
			if (&obj != this && obj.mDat != mDat) {
				if (mDat != nullptr && obj.mDat != nullptr && raw) {
					if (this->mDat->protect_level > 0 || obj.mDat->protect_level > 0)
						throw cov::error("E000J");
					mDat->data->kill();
					mDat->data = obj.mDat->data->duplicate();
				}
				else {
					recycle();
					if (obj.mDat != nullptr)
						mDat = allocator.alloc(1, obj.mDat->data->duplicate());
					else
						mDat = nullptr;
				}
			}
		}

		template<typename T>
		void assign(const T &dat, bool raw = false)
		{
			if (mDat != nullptr && raw) {
				if (this->mDat->protect_level > 0)
					throw cov::error("E000J");
				mDat->data->kill();
				mDat->data = holder<T>::allocator.alloc(dat);
			}
			else {
				recycle();
				mDat = allocator.alloc(1, holder<T>::allocator.alloc(dat));
			}
		}

		template<typename T>
		any &operator=(const T &dat)
		{
			assign(dat);
			return *this;
		}
	};

	template<>
	std::string to_string<std::string>(const std::string &str)
	{
		return str;
	}

	template<>
	std::string to_string<bool>(const bool &v)
	{
		if (v)
			return "true";
		else
			return "false";
	}

	template<int N>
	class any::holder<char[N]> : public any::holder<std::string> {
	public:
		using holder<std::string>::holder;
	};

	template<>
	class any::holder<std::type_info> : public any::holder<std::type_index> {
	public:
		using holder<std::type_index>::holder;
	};

	template<typename T> cov::allocator<any::holder<T>, default_allocate_buffer_size, default_allocator_provider> any::holder<T>::allocator;
}

std::ostream &operator<<(std::ostream &, const cs_impl::any &);

namespace std {
	template<>
	struct hash<cs_impl::any> {
		std::size_t operator()(const cs_impl::any &val) const
		{
			return val.hash();
		}
	};
}
