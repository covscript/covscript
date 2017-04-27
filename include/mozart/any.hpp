#pragma once
/*
* Covariant Mozart Utility Library: Any
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
* Version: 17.4.2
*/
#include "./base.hpp"
#include <functional>
#include <iostream>

namespace std {
	template<typename T> std::string to_string(const T&)
	{
		throw cov::error("E000D");
	}
	template<> std::string to_string<std::string>(const std::string& str)
	{
		return str;
	}
	template<> std::string to_string<bool>(const bool& v)
	{
		if(v)
			return "true";
		else
			return "false";
	}
}

namespace cov {
	template < typename _Tp > class compare_helper {
		template < typename T,typename X=bool >struct matcher;
		template < typename T > static constexpr bool match(T*)
		{
			return false;
		}
		template < typename T > static constexpr bool match(matcher < T, decltype(std::declval<T>()==std::declval<T>()) > *)
		{
			return true;
		}
	public:
		static constexpr bool value = match < _Tp > (nullptr);
	};
	template<typename,bool> struct compare_if;
	template<typename T>struct compare_if<T,true> {
		static bool compare(const T& a,const T& b)
		{
			return a==b;
		}
	};
	template<typename T>struct compare_if<T,false> {
		static bool compare(const T& a,const T& b)
		{
			return &a==&b;
		}
	};
	template<typename T>bool compare(const T& a,const T& b)
	{
		return compare_if<T,compare_helper<T>::value>::compare(a,b);
	}
	template < typename _Tp > class hash_helper {
		template < typename T,decltype(&std::hash<T>::operator()) X >struct matcher;
		template < typename T > static constexpr bool match(T*)
		{
			return false;
		}
		template < typename T > static constexpr bool match(matcher<T,&std::hash<T>::operator()>*)
		{
			return true;
		}
	public:
		static constexpr bool value = match < _Tp > (nullptr);
	};
	template<typename,bool> struct hash_if;
	template<typename T>struct hash_if<T,true> {
		static std::size_t hash(const T& val)
		{
			static std::hash<T> gen;
			return gen(val);
		}
	};
	template<typename T>struct hash_if<T,false> {
		static std::size_t hash(const T& val)
		{
			throw cov::error("E000F");
		}
	};
	template<typename T>std::size_t hash(const T& val)
	{
		return hash_if<T,hash_helper<T>::value>::hash(val);
	}
	class any final {
		class baseHolder {
		public:
			baseHolder() = default;
			virtual ~ baseHolder() = default;
			virtual const std::type_info & type() const = 0;
			virtual baseHolder *duplicate() = 0;
			virtual bool compare(const baseHolder *) const = 0;
			virtual std::string to_string() const = 0;
			virtual std::size_t hash() const = 0;
		};
		template < typename T > class holder:public baseHolder {
		protected:
			T mDat;
		public:
			holder() = default;
			holder(const T& dat):mDat(dat) {}
			virtual ~ holder() = default;
			virtual const std::type_info & type() const override
			{
				return typeid(T);
			}
			virtual baseHolder *duplicate() override
			{
				return new holder(mDat);
			}
			virtual bool compare(const baseHolder * obj)const override
			{
				if (obj->type() == this->type()) {
					const holder < T > *ptr = dynamic_cast < const holder < T > *>(obj);
					return ptr!=nullptr?cov::compare(mDat,ptr->data()):false;
				}
				return false;
			}
			virtual std::string to_string() const override
			{
				return std::move(std::to_string(mDat));
			}
			virtual std::size_t hash() const override
			{
				return cov::hash<T>(mDat);
			}
			T & data()
			{
				return mDat;
			}
			const T & data() const
			{
				return mDat;
			}
			void data(const T & dat)
			{
				mDat = dat;
			}
		};
		using size_t=unsigned long;
		struct proxy {
			mutable size_t refcount=1;
			baseHolder* data=nullptr;
			proxy()=default;
			proxy(size_t rc,baseHolder* d):refcount(rc),data(d) {}
			~proxy()
			{
				delete data;
			}
		};
		proxy* mDat=nullptr;
		proxy* duplicate() const noexcept
		{
			if(mDat!=nullptr) {
				++mDat->refcount;
			}
			return mDat;
		}
		void recycle() noexcept
		{
			if(mDat!=nullptr) {
				--mDat->refcount;
				if(mDat->refcount==0) {
					delete mDat;
					mDat=nullptr;
				}
			}
		}
	public:
		static any infer_value(const std::string&);
		void swap(any& obj) noexcept
		{
			proxy* tmp=this->mDat;
			this->mDat=obj.mDat;
			obj.mDat=tmp;
		}
		void swap(any&& obj) noexcept
		{
			proxy* tmp=this->mDat;
			this->mDat=obj.mDat;
			obj.mDat=tmp;
		}
		void clone() noexcept
		{
			if(mDat!=nullptr) {
				proxy* dat=new proxy(1,mDat->data->duplicate());
				recycle();
				mDat=dat;
			}
		}
		bool usable() const noexcept
		{
			return mDat != nullptr;
		}
		any()=default;
		template < typename T > any(const T & dat):mDat(new proxy(1,new holder < T > (dat))) {}
		any(const any & v):mDat(v.duplicate()) {}
		any(any&& v) noexcept
		{
			swap(std::forward<any>(v));
		}
		~any()
		{
			recycle();
		}
		const std::type_info & type() const
		{
			return this->mDat != nullptr?this->mDat->data->type():typeid(void);
		}
		std::string to_string() const
		{
			if(this->mDat == nullptr)
				return "Null";
			return std::move(this->mDat->data->to_string());
		}
		std::size_t hash() const
		{
			if(this->mDat == nullptr)
				return cov::hash<void*>(nullptr);
			return this->mDat->data->hash();
		}
		any & operator=(const any & var)
		{
			if(&var!=this) {
				recycle();
				mDat = var.duplicate();
			}
			return *this;
		}
		any & operator=(any&& var) noexcept
		{
			if(&var!=this)
				swap(std::forward<any>(var));
			return *this;
		}
		bool operator==(const any & var) const
		{
			return usable()?this->mDat->data->compare(var.mDat->data):!var.usable();
		}
		bool operator!=(const any & var)const
		{
			return usable()?!this->mDat->data->compare(var.mDat->data):var.usable();
		}
		template < typename T > T & val(bool raw=false)
		{
			if(typeid(T) != this->type())
				throw cov::error("E0006");
			if(this->mDat == nullptr)
				throw cov::error("E0005");
			if(!raw)
				clone();
			return dynamic_cast < holder < T > *>(this->mDat->data)->data();
		}
		template < typename T > const T & val(bool raw=false) const
		{
			if(typeid(T) != this->type())
				throw cov::error("E0006");
			if(this->mDat == nullptr)
				throw cov::error("E0005");
			return dynamic_cast < const holder < T > *>(this->mDat->data)->data();
		}
		template < typename T > const T& const_val() const
		{
			if(typeid(T) != this->type())
				throw cov::error("E0006");
			if(this->mDat == nullptr)
				throw cov::error("E0005");
			return dynamic_cast < const holder < T > *>(this->mDat->data)->data();
		}
		template < typename T > operator T&()
		{
			return this->val<T>();
		}
		template < typename T > operator const T&() const
		{
			return this->const_val<T>();
		}
		void assign(const any& obj,bool raw=false)
		{
			if(&obj!=this) {
				if(mDat!=obj.mDat&&mDat!=nullptr&&obj.mDat!=nullptr&&raw) {
					delete mDat->data;
					mDat->data=obj.mDat->data->duplicate();
				} else {
					recycle();
					if(obj.mDat!=nullptr)
						mDat=new proxy(1,obj.mDat->data->duplicate());
					else
						mDat=nullptr;
				}
			}
		}
		template < typename T > void assign(const T & dat,bool raw=false)
		{
			if(raw) {
				delete mDat->data;
				mDat->data=new holder < T > (dat);
			} else {
				recycle();
				mDat = new proxy(1,new holder < T > (dat));
			}
		}
		template < typename T > any & operator=(const T & dat)
		{
			assign(dat);
			return *this;
		}
		bool is_same(const any& obj) const
		{
			return this->mDat==obj.mDat;
		}
	};
	template<int N> class any::holder<char[N]>:public any::holder<std::string> {
	public:
		using holder<std::string>::holder;
	};
	template<> class any::holder<std::type_info>:public any::holder<std::type_index> {
	public:
		using holder<std::type_index>::holder;
	};
}

cov::any cov::any::infer_value(const std::string& str)
{
	if(str=="true"||str=="True"||str=="TRUE")
		return true;
	if(str=="false"||str=="False"||str=="FALSE")
		return false;
	enum types {
		interger,floating,other
	} type=types::interger;
	for(auto& it:str) {
		if(!std::isdigit(it)&&it!='.') {
			type=other;
			break;
		}
		if(!std::isdigit(it)&&it=='.') {
			if(type==interger) {
				type=floating;
				continue;
			}
			if(type==floating) {
				type=other;
				break;
			}
		}
	}
	switch(type) {
	case interger:
		try {
			return std::stoi(str);
		} catch(std::out_of_range) {
			try {
				return std::stol(str);
			} catch(std::out_of_range) {
				try {
					return std::stoll(str);
				} catch(std::out_of_range) {
					return str;
				}
			}
		}
	case floating:
		return std::stod(str);
	case other:
		break;
	}
	return str;
}

std::istream& operator>>(std::istream& in,cov::any& val)
{
	static std::string str;
	in>>str;
	val=cov::any::infer_value(str);
	return in;
}

std::ostream& operator<<(std::ostream& out,const cov::any& val)
{
	out<<val.to_string();
	return out;
}

namespace std {
	template<> struct hash<cov::any> {
		std::size_t operator()(const cov::any& val) const
		{
			return val.hash();
		}
	};
}
