#pragma once
/*
* Covariant Mozart Utility Library: Memory
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
* Version: 17.9.1
*/
#include "./base.hpp"
#include "./function.hpp"
#include <cstdint>
#include <cstdlib>
#include <utility>
#include <memory>
#include <atomic>
#include <array>
#include <list>

namespace cov {
	template<typename _Tp, template<typename> class _alloc>
	struct _alloc_helper {
		static _alloc<_Tp> allocator;
	};
	template<typename _Tp, template<typename> class _alloc>
	_alloc<_Tp> _alloc_helper<_Tp, _alloc>::allocator;

	template<typename _Tp,
	         template<typename> class _alloc=std::allocator,
	         template<typename> class _atomic=std::atomic>
	class shared_ptr final {
	public:
		typedef _atomic<unsigned long> counter;
		typedef _Tp data_type;
		typedef _Tp *raw_type;
		typedef cov::function<void(raw_type)> deleter;
	private:
		class proxy final {
			friend class shared_ptr<_Tp, _alloc, _atomic>;

			mutable counter ref_count;
			deleter resolve;
			raw_type data = nullptr;

			void add_ref() const
			{
				++ref_count;
			}

			void cut_ref() const
			{
				if (--ref_count == 0) {
					if (resolve.callable())
						resolve(data);
					_alloc_helper<data_type, _alloc>::allocator.destroy(data);
					_alloc_helper<data_type, _alloc>::allocator.deallocate(data, 1);
				}
			}
		};

		proxy *mProxy;
	public:
		shared_ptr() : mProxy(_alloc_helper<proxy, _alloc>::allocator.allocate(1))
		{
			mProxy->ref_count = 1;
			mProxy->data = _alloc_helper<data_type, _alloc>::allocator.allocate(1);
			_alloc_helper<data_type, _alloc>::allocator.construct(mProxy->data);
		}

		explicit shared_ptr(const deleter &f) : mProxy(_alloc_helper<proxy, _alloc>::allocator.allocate(1))
		{
			mProxy->ref_count = 1;
			mProxy->deleter = f;
			mProxy->data = _alloc_helper<data_type, _alloc>::allocator.allocate(1);
			_alloc_helper<data_type, _alloc>::allocator.construct(mProxy->data);
		}

		shared_ptr(const shared_ptr &ptr) : mProxy(ptr.mProxy)
		{
			mProxy->add_ref();
		}

		explicit shared_ptr(const data_type &obj) : mProxy(_alloc_helper<proxy, _alloc>::allocator.allocate(1))
		{
			mProxy->ref_count = 1;
			mProxy->data = _alloc_helper<_Tp, _alloc>::allocator.allocate(1);
			_alloc_helper<data_type, _alloc>::allocator.construct(mProxy->data, obj);
		}

		shared_ptr(const data_type &obj, const deleter &f) : mProxy(
			    _alloc_helper<proxy, _alloc>::allocator.allocate(1))
		{
			mProxy->ref_count = 1;
			mProxy->deleter = f;
			mProxy->data = _alloc_helper<_Tp, _alloc>::allocator.allocate(1);
			_alloc_helper<data_type, _alloc>::allocator.construct(mProxy->data, obj);
		}

		~shared_ptr()
		{
			mProxy->cut_ref();
			if (mProxy->ref_count == 0) {
				_alloc_helper<proxy, _alloc>::allocator.destroy(mProxy);
				_alloc_helper<proxy, _alloc>::allocator.deallocate(mProxy, 1);
			}
		}

		shared_ptr &operator=(const shared_ptr &ptr)
		{
			if (&ptr != this) {
				mProxy->cut_ref();
				if (mProxy->ref_count == 0) {
					_alloc_helper<proxy, _alloc>::allocator.destroy(mProxy);
					_alloc_helper<proxy, _alloc>::allocator.deallocate(mProxy, 1);
				}
				mProxy = ptr.mProxy;
				++mProxy->ref_count;
			}
			return *this;
		}

		data_type &operator*()
		{
			return *mProxy->data;
		}

		raw_type operator->()
		{
			return mProxy->data;
		}

		const data_type &operator*() const
		{
			return *mProxy->data;
		}

		const raw_type operator->() const
		{
			return mProxy->data;
		}
	};

	template<typename T, long blck_size, template<typename> class allocator_t=std::allocator>
	class allocator final {
		allocator_t<T> mAlloc;
		std::array<T *, blck_size> mPool;
		long mOffset = -1;
	public:
		void blance()
		{
			if (mOffset != 0.5 * blck_size) {
				if (mOffset < 0.5 * blck_size) {
					for (; mOffset < 0.5 * blck_size; ++mOffset)
						mPool[mOffset + 1] = mAlloc.allocate(1);
				}
				else {
					for (; mOffset > 0.5 * blck_size; --mOffset)
						mAlloc.deallocate(mPool[mOffset], 1);
				}
			}
		}

		void clean()
		{
			for (; mOffset >= 0; --mOffset)
				mAlloc.deallocate(mPool[mOffset], 1);
		}

		allocator()
		{
			blance();
		}

		allocator(const allocator &) = delete;

		~allocator()
		{
			clean();
		}

		template<typename...ArgsT>
		T *alloc(ArgsT &&...args)
		{
			T *ptr = nullptr;
			if (mOffset >= 0)
				ptr = mPool[mOffset--];
			else
				ptr = mAlloc.allocate(1);
			mAlloc.construct(ptr, std::forward<ArgsT>(args)...);
			return ptr;
		}

		void free(T *ptr)
		{
			mAlloc.destroy(ptr);
			if (mOffset < blck_size - 1)
				mPool[++mOffset] = ptr;
			else
				mAlloc.deallocate(ptr, 1);
		}
	};

	template<typename T, std::size_t pool_size = 10240, template<typename> class allocator_t=std::allocator,
	         template<typename, std::size_t> class array_t=std::array>
	class storage final {
		struct mem_unit {
			T *ptr = nullptr;
			bool raw = true;
		};
		allocator_t<T> allocator;
		array_t<mem_unit, pool_size> pool;
	public:
		class pointer final {
			friend class storage;

			std::size_t posit;

			explicit pointer(std::size_t p) : posit(p) {}

		public:
			pointer() = delete;

			pointer(const pointer &) = default;

			~pointer() = default;

			pointer &operator=(const pointer &) = default;
		};

		storage()
		{
			for (auto &unit:pool)
				unit.ptr = allocator.allocate(1);
		}

		storage(const storage &) = delete;

		~storage()
		{
			for (auto &unit:pool) {
				if (!unit.raw)
					allocator.destroy(unit.ptr);
				allocator.deallocate(unit.ptr, 1);
			}
		}

		template<typename...ArgsT>
		pointer alloc(ArgsT...args)
		{
			for (std::size_t i = 0; i < pool_size; ++i) {
				if (pool[i].raw) {
					allocator.construct(pool[i].ptr, std::forward<ArgsT>(args)...);
					pool[i].raw = false;
					return i;
				}
			}
			throw cov::error("E000M");
		}

		void free(const pointer &p)
		{
			if (p.posit >= pool_size)
				throw cov::error("E000N");
			if (!pool[p.posit].raw) {
				allocator.destroy(pool[p.posit].ptr);
				pool[p.posit].raw = true;
			}
		}

		bool usable(const pointer &p)
		{
			if (p.posit >= pool_size)
				throw cov::error("E000N");
			return !pool[p.posit].raw;
		}

		T &get(const pointer &p)
		{
			if (p.posit >= pool_size)
				throw cov::error("E000N");
			if (pool[p.posit].raw)
				throw cov::error("E000P");
			return *pool[p.posit].ptr;
		}
	};

	class stack final {
	public:
		using byte=uint8_t;
		using size_t=uint64_t;
	private:
		// Stack Start
		void *ss = nullptr;
		// Stack Pointer
		byte *sp = nullptr;
		// Stack Limit
		byte *sl = nullptr;
	public:
		stack() = delete;

		stack(const stack &) = delete;

		explicit stack(size_t size) : ss(::malloc(size))
		{
			sp = reinterpret_cast<byte *>(ss) + size;
			sl = sp;
		}

		~stack()
		{
			::free(ss);
		}

		inline bool empty() const
		{
			return sp == sl;
		}

		void *top()
		{
			if (sp == sl)
				throw std::runtime_error("Stack is empty.");
			return reinterpret_cast<void *>(sp + sizeof(size_t));
		}

		void *push(size_t size)
		{
			if (sp - reinterpret_cast<byte *>(ss) < size + sizeof(size_t))
				throw std::runtime_error("Stack overflow.");
			sp -= size + sizeof(size_t);
			*reinterpret_cast<size_t *>(sp) = size;
			return reinterpret_cast<void *>(sp + sizeof(size_t));
		}

		void pop()
		{
			if (sp == sl)
				throw std::runtime_error("Stack is empty.");
			sp += *reinterpret_cast<size_t *>(sp) + sizeof(size_t);
		}

		size_t size_of(void *ptr)
		{
			return *reinterpret_cast<size_t *>(reinterpret_cast<byte *>(ptr) - sizeof(size_t));
		}
	};

	class heap final {
	public:
		using byte=uint8_t;
		using size_t=uint64_t;
		enum class allocate_policy {
			first_fit, best_fit, worst_fit
		};
	private:
		// Heap Start
		void *hs = nullptr;
		// Heap Pointer
		byte *hp = nullptr;
		// Heap Limit
		byte *hl = nullptr;
		allocate_policy policy = allocate_policy::best_fit;
		bool no_truncate = false;
		std::list<byte *> free_list;

		inline size_t &get_size(byte *ptr)
		{
			return *reinterpret_cast<size_t *>(ptr);
		}

		void compress()
		{
			std::list<byte *> new_list;
			byte *ptr = nullptr;
			// Sort the spaces by address.
			free_list.sort([](const byte *lhs, const byte *rhs) {
				return lhs < rhs;
			});
			// Compress the free list.
			for (auto p:free_list) {
				if (ptr != nullptr) {
					size_t &size = get_size(ptr);
					if (ptr + size + sizeof(size_t) == p) {
						size += get_size(p) + sizeof(size_t);
					}
					else {
						new_list.push_back(ptr);
						ptr = p;
					}
				}
				else
					ptr = p;
			}
			// Connect the final space and remain spaces.
			if (ptr != nullptr) {
				if (ptr + get_size(ptr) + sizeof(size_t) == hp)
					hp = ptr;
				else
					new_list.push_back(ptr);
			}
			// Swap the new list and old list.
			std::swap(new_list, free_list);
		}

		void *allocate(size_t size)
		{
			// Try to find usable spaces in free list
			if (!free_list.empty()) {
				auto it = free_list.begin();
				switch (policy) {
				case allocate_policy::first_fit: {
					// Find the first fit space.
					for (; it != free_list.end(); ++it)
						if (get_size(*it) >= size)
							break;
					break;
				}
				case allocate_policy::best_fit: {
					// Find the best fit space.
					auto best = it;
					for (; it != free_list.end(); ++it)
						if (get_size(*it) >= size && get_size(*it) < get_size(*best))
							best = it;
					it = best;
					break;
				}
				case allocate_policy::worst_fit: {
					// Find the worst fit space.
					auto max = it;
					for (; it != free_list.end(); ++it)
						if (get_size(*it) > get_size(*max))
							max = it;
					it = max;
					break;
				}
				}
				if (it != free_list.end() && get_size(*it) >= size) {
					// Remove from free list.
					byte *raw = *it;
					free_list.erase(it);
					// Truncate remain spaces
					if (!no_truncate && get_size(raw) - size > sizeof(size_t)) {
						byte *ptr = raw + sizeof(size_t) + size;
						get_size(ptr) = get_size(raw) - size - sizeof(size_t);
						get_size(raw) = size;
						free_list.push_back(ptr);
					}
					return reinterpret_cast<void *>(raw + sizeof(size_t));
				}
			}
			// Checkout remain spaces,if enough,return.
			if (hl - hp >= size + sizeof(size_t)) {
				get_size(hp) = size;
				auto *ptr = reinterpret_cast<void *>(hp + sizeof(size_t));
				hp += size + sizeof(size_t);
				return ptr;
			}
			return nullptr;
		}

	public:
		heap() = delete;

		heap(const heap &) = delete;

		explicit heap(size_t size, allocate_policy p = allocate_policy::first_fit, bool nt = false) : hs(
			    ::malloc(size)), policy(p), no_truncate(nt)
		{
			hp = reinterpret_cast<byte *>(hs);
			hl = hp + size;
		}

		~heap()
		{
			::free(hs);
		}

		void *malloc(size_t size)
		{
			// Try to allocate.
			void *ptr = allocate(size);
			// If successed,return
			if (ptr != nullptr)
				return ptr;
			// Compress the memory spaces
			compress();
			// Try to allocate again.
			ptr = allocate(size);
			// If successed,return.
			if (ptr != nullptr)
				return ptr;
			else // There have no usable spaces,throw bad alloc exception.
				throw std::runtime_error("Bad alloc.");
		}

		void free(void *ptr)
		{
			free_list.push_back(reinterpret_cast<byte *>(ptr) - sizeof(size_t));
		}

		size_t size_of(void *ptr)
		{
			return get_size(reinterpret_cast<byte *>(ptr));
		}
	};

	template<typename T, typename...ArgsT>
	void construct(T *ptr, ArgsT &&...args)
	{
		if (ptr != nullptr)
			::new(ptr) T(std::forward<ArgsT>(args)...);
	}

	template<typename T>
	void destroy(T *ptr)
	{
		if (ptr != nullptr)
			ptr->~T();
	}
}
