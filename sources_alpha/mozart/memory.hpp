#pragma once
/*
* Covariant Mozart Utility Library: Memory
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
* Copyright (C) 2016 Michael Lee(李登淳)
* Email: China-LDC@outlook.com
* Github: https://github.com/mikecovlee
* Website: http://ldc.atd3.cn
*
* Version: 17.1.0
*/
#include "./base.hpp"
#include "./function.hpp"
#include <memory>
#include <atomic>

namespace cov {
	template<typename _Tp,template<typename>class _alloc>
	struct _alloc_helper {
		static _alloc<_Tp> allocator;
	};
	template<typename _Tp,template<typename>class _alloc>
	_alloc<_Tp> _alloc_helper<_Tp,_alloc>::allocator;
	template<typename _Tp,
	         template<typename>class _alloc=std::allocator,
	         template<typename>class _atomic=std::atomic>
	class shared_ptr final {
	public:
		typedef _atomic<unsigned long> counter;
		typedef _Tp data_type;
		typedef _Tp* raw_type;
		typedef cov::function<void(raw_type)> deleter;
	private:
		class proxy final {
			friend class shared_ptr<_Tp,_alloc,_atomic>;
			mutable counter ref_count;
			deleter resolve;
			raw_type data=nullptr;
			void add_ref() const {
				++ref_count;
			}
			void cut_ref() const {
				if(--ref_count==0) {
					if(resolve.callable())
						resolve(data);
					_alloc_helper<data_type,_alloc>::allocator.destroy(data);
					_alloc_helper<data_type,_alloc>::allocator.deallocate(data,1);
				}
			}
		};
		proxy* mProxy;
	public:
		shared_ptr():mProxy(_alloc_helper<proxy,_alloc>::allocator.allocate(1)) {
			mProxy->ref_count=1;
			mProxy->data=_alloc_helper<data_type,_alloc>::allocator.allocate(1);
			_alloc_helper<data_type,_alloc>::allocator.construct(mProxy->data);
		}
		shared_ptr(const deleter& f):mProxy(_alloc_helper<proxy,_alloc>::allocator.allocate(1)) {
			mProxy->ref_count=1;
			mProxy->deleter=f;
			mProxy->data=_alloc_helper<data_type,_alloc>::allocator.allocate(1);
			_alloc_helper<data_type,_alloc>::allocator.construct(mProxy->data);
		}
		shared_ptr(const shared_ptr& ptr):mProxy(ptr.mProxy) {
			mProxy->add_ref();
		}
		shared_ptr(const data_type& obj):mProxy(_alloc_helper<proxy,_alloc>::allocator.allocate(1)) {
			mProxy->ref_count=1;
			mProxy->data=_alloc_helper<_Tp,_alloc>::allocator.allocate(1);
			_alloc_helper<data_type,_alloc>::allocator.construct(mProxy->data,obj);
		}
		shared_ptr(const data_type& obj,const deleter& f):mProxy(_alloc_helper<proxy,_alloc>::allocator.allocate(1)) {
			mProxy->ref_count=1;
			mProxy->deleter=f;
			mProxy->data=_alloc_helper<_Tp,_alloc>::allocator.allocate(1);
			_alloc_helper<data_type,_alloc>::allocator.construct(mProxy->data,obj);
		}
		~shared_ptr() {
			mProxy->cut_ref();
			if(mProxy->ref_count==0) {
				_alloc_helper<proxy,_alloc>::allocator.destroy(mProxy);
				_alloc_helper<proxy,_alloc>::allocator.deallocate(mProxy,1);
			}
		}
		shared_ptr& operator=(const shared_ptr& ptr) {
			if(&ptr!=this) {
				mProxy->cut_ref();
				if(mProxy->ref_count==0) {
					_alloc_helper<proxy,_alloc>::allocator.destroy(mProxy);
					_alloc_helper<proxy,_alloc>::allocator.deallocate(mProxy,1);
				}
				mProxy=ptr.mProxy;
				++mProxy->ref_count;
			}
			return *this;
		}
		data_type& operator*() {
			return *mProxy->data;
		}
		raw_type operator->() {
			return mProxy->data;
		}
		const data_type& operator*() const {
			return *mProxy->data;
		}
		const raw_type operator->() const {
			return mProxy->data;
		}
	};
}