#pragma once
/*
* Covariant Script Any
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Affero General Public License as published
* by the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Affero General Public License for more details.
*
* You should have received a copy of the GNU Affero General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
* Copyright (C) 2017 Michael Lee(李登淳)
* Email: mikecovlee@163.com
* Github: https://github.com/mikecovlee
*/
#include "../../include/mozart/memory.hpp"
#include "./typedef.hpp"
#include <functional>
#include <ostream>

namespace cs_impl {

// Be careful when you adjust the buffer and the heap size.
	constexpr std::size_t default_allocate_buffer_size = 64;
	/*	constexpr cov::heap::size_t default_heap_size = 1048576;
	// If you want to improve memory occupy,you can set the best fit policy and allow the memory truncate.
	    constexpr cov::heap::allocate_policy default_heap_policy = cov::heap::allocate_policy::first_fit;
	    constexpr bool default_heap_no_truncate = true;

	    static cov::heap default_heap(default_heap_size, default_heap_policy, default_heap_no_truncate);

	    template<typename T>
	    class allocator final {
	    public:
	        allocator() = default;

	        allocator(const allocator &) = delete;

	        ~allocator() = default;

	        inline T *allocate(std::size_t size)
	        {
	            return static_cast<T *>(default_heap.malloc(size * sizeof(T)));
	        }

	        inline void deallocate(T *ptr, std::size_t)
	        {
	            default_heap.free(ptr);
	        }

	        template<typename...ArgsT>
	        inline void construct(T *ptr, ArgsT &&...args)
	        {
	            if (ptr != nullptr)
	                ::new(ptr) T(std::forward<ArgsT>(args)...);
	        }

	        inline void destroy(T *ptr)
	        {
	            if (ptr != nullptr)
	                ptr->~T();
	        }
	    };
	*/
	template<typename T> using default_allocator_provider=std::allocator<T>;

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

	template<typename T>
	bool compare(const T &a, const T &b)
	{
		return compare_if<T, compare_helper<T>::value>::compare(a, b);
	}

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

	template<typename T>
	long to_integer(const T &val)
	{
		return to_integer_if<T, cov::castable<T, long>::value>::to_integer(val);
	}

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

	template<typename T>
	std::string to_string(const T &val)
	{
		return to_string_if<T, to_string_helper<T>::value>::to_string(val);
	}

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

	template<typename, bool>
	struct hash_if;

	template<typename T>
	struct hash_if<T, true> {
		static std::size_t hash(const T &val)
		{
			static std::hash <T> gen;
			return gen(val);
		}
	};

	template<typename T>
	struct hash_if<T, false> {
		static std::size_t hash(const T &val)
		{
			throw cov::error("E000F");
		}
	};

	template<typename T>
	std::size_t hash(const T &val)
	{
		return hash_if<T, hash_helper<T>::value>::hash(val);
	}

	template<typename T>
	void detach(T &val)
	{
		// Do something if you want when data is copying.
	}

	template<typename T>
	cs::extension_t &get_ext()
	{
		throw cs::syntax_error("Target type does not support extensions.");
	}

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

			virtual cs::extension_t &get_ext() const =0;
		};

		template<typename T>
		class holder : public baseHolder {
		protected:
			T mDat;
		public:
			static cov::allocator<holder<T>, default_allocate_buffer_size, default_allocator_provider> allocator;

			holder() = default;

			template<typename...ArgsT>
			holder(ArgsT &&...args):mDat(std::forward<ArgsT>(args)...) {}

			virtual ~ holder() = default;

			virtual const std::type_info &type() const override
			{
				return typeid(T);
			}

			virtual baseHolder *duplicate() override
			{
				return allocator.alloc(mDat);
			}

			virtual bool compare(const baseHolder *obj) const override
			{
				if (obj->type() == this->type()) {
					const holder<T> *ptr = static_cast<const holder<T> *>(obj);
					return cs_impl::compare(mDat, ptr->data());
				}
				else
					return false;
			}

			virtual long to_integer() const override
			{
				return cs_impl::to_integer(mDat);
			}

			virtual std::string to_string() const override
			{
				return cs_impl::to_string(mDat);
			}

			virtual std::size_t hash() const override
			{
				return cs_impl::hash<T>(mDat);
			}

			virtual void detach() override
			{
				cs_impl::detach(mDat);
			}

			virtual void kill() override
			{
				allocator.free(this);
			}

			virtual cs::extension_t &get_ext() const override
			{
				return cs_impl::get_ext<T>();
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

		any(proxy *dat) : mDat(dat) {}

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

		any() = default;

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

		void detach()
		{
			if (this->mDat != nullptr) {
				if (this->mDat->protect_level > 2)
					throw cov::error("E000L");
				this->mDat->data->detach();
			}
		}

		cs::extension_t &get_ext() const
		{
			if (this->mDat == nullptr)
				throw cs::syntax_error("Target type does not support extensions.");
			return this->mDat->data->get_ext();
		}

		bool is_same(const any &obj) const
		{
			return this->mDat == obj.mDat;
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
		operator const T &() const
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

	template<typename T> cov::allocator<any::holder<T>, default_allocate_buffer_size, default_allocator_provider> any::holder<T>::allocator;
	cov::allocator<any::proxy, default_allocate_buffer_size, default_allocator_provider> any::allocator;

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
}

std::ostream &operator<<(std::ostream &out, const cs_impl::any &val)
{
	out << val.to_string();
	return out;
}

namespace std {
	template<>
	struct hash<cs_impl::any> {
		std::size_t operator()(const cs_impl::any &val) const
		{
			return val.hash();
		}
	};
}
