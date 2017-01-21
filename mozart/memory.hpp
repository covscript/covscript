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

namespace cov {
	template<typename _Tp,template<typename>typename _Alloc>
	struct _alloc_helper {
		static _Alloc<_Tp> allocator;
	};
	template<typename _Tp,template<typename>typename _Alloc>
	_Alloc<_Tp> _alloc_helper<_Tp,_Alloc>::allocator;
	template<typename _Tp,
	         template<typename>typename _Alloc=std::allocator,
	         template<typename>typename _Atomic=std::atomic>
	class shared_ptr final {
	public:
		typedef _Atomic<unsigned long> counter;
		typedef _Tp data_type;
		typedef _Tp* raw_type;
		typedef cov::function<void(raw_type)> deleter;
	private:
		class proxy final {
			friend class shared_ptr<_Tp,_Alloc,_Atomic>;
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
					_alloc_helper<data_type,_Alloc>::allocator.destroy(data);
					_alloc_helper<data_type,_Alloc>::allocator.deallocate(data,1);
				}
			}
		};
		proxy* mProxy;
	public:
		shared_ptr():mProxy(_alloc_helper<proxy,_Alloc>::allocator.allocate(1)) {
			mProxy->ref_count=1;
			mProxy->data=_alloc_helper<data_type,_Alloc>::allocator.allocate(1);
			_alloc_helper<data_type,_Alloc>::allocator.construct(mProxy->data);
		}
		shared_ptr(const deleter& f):mProxy(_alloc_helper<proxy,_Alloc>::allocator.allocate(1)) {
			mProxy->ref_count=1;
			mProxy->deleter=f;
			mProxy->data=_alloc_helper<data_type,_Alloc>::allocator.allocate(1);
			_alloc_helper<data_type,_Alloc>::allocator.construct(mProxy->data);
		}
		shared_ptr(const shared_ptr& ptr):mProxy(ptr.mProxy) {
			mProxy->add_ref();
		}
		shared_ptr(const data_type& obj):mProxy(_alloc_helper<proxy,_Alloc>::allocator.allocate(1)) {
			mProxy->ref_count=1;
			mProxy->data=_alloc_helper<_Tp,_Alloc>::allocator.allocate(1);
			_alloc_helper<data_type,_Alloc>::allocator.construct(mProxy->data,obj);
		}
		shared_ptr(const data_type& obj,const deleter& f):mProxy(_alloc_helper<proxy,_Alloc>::allocator.allocate(1)) {
			mProxy->ref_count=1;
			mProxy->deleter=f;
			mProxy->data=_alloc_helper<_Tp,_Alloc>::allocator.allocate(1);
			_alloc_helper<data_type,_Alloc>::allocator.construct(mProxy->data,obj);
		}
		~shared_ptr() {
			mProxy->cut_ref();
			if(mProxy->ref_count==0) {
				_alloc_helper<proxy,_Alloc>::allocator.destroy(mProxy);
				_alloc_helper<proxy,_Alloc>::allocator.deallocate(mProxy,1);
			}
		}
		shared_ptr& operator=(const shared_ptr& ptr) {
			if(&ptr!=this) {
				mProxy->cut_ref();
				if(mProxy->ref_count==0) {
					_alloc_helper<proxy,_Alloc>::allocator.destroy(mProxy);
					_alloc_helper<proxy,_Alloc>::allocator.deallocate(mProxy,1);
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