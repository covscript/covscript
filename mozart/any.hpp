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
* Version: 17.1.0
*/
#include "./base.hpp"
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
}

namespace cov {
	class any final {
		class baseHolder {
		public:
			baseHolder() = default;
			virtual ~ baseHolder() = default;
			virtual const std::type_info & type() const = 0;
			virtual baseHolder *duplicate() = 0;
			virtual bool compare(const baseHolder *) const = 0;
			virtual std::string to_string() const = 0;
		};
		template < typename T > class holder:public baseHolder {
		protected:
			T mDat;
		public:
			holder() = default;
			holder(const T& dat):mDat(dat) {}
			virtual ~ holder() = default;
			virtual const std::type_info & type() const override {
				return typeid(T);
			}
			virtual baseHolder *duplicate() override {
				return new holder(mDat);
			}
			virtual bool compare(const baseHolder * obj)const override {
				if (obj->type() == this->type()) {
					const holder < T > *ptr = dynamic_cast < const holder < T > *>(obj);
					return ptr!=nullptr?mDat == ptr->data():false;
				}
				return false;
			}
			virtual std::string to_string() const override {
				return std::move(std::to_string(mDat));
			}
			T & data() {
				return mDat;
			}
			const T & data() const {
				return mDat;
			}
			void data(const T & dat) {
				mDat = dat;
			}
		};
		baseHolder * mDat=nullptr;
	public:
		static any infer_value(const std::string&);
		void swap(any& obj) noexcept {
			baseHolder* tmp=this->mDat;
			this->mDat=obj.mDat;
			obj.mDat=tmp;
		}
		void swap(any&& obj) {
			baseHolder* tmp=this->mDat;
			this->mDat=obj.mDat;
			obj.mDat=tmp;
		}
		bool usable() const noexcept {
			return mDat != nullptr;
		}
		any()=default;
		template < typename T > any(const T & dat):mDat(new holder < T > (dat)) {}
		any(const any & v):mDat(v.usable()?v.mDat->duplicate():nullptr) {}
		any(any&& v) noexcept {
			swap(std::forward<any>(v));
		}
		~any() {
			delete mDat;
		}
		const std::type_info & type() const {
			return this->mDat != nullptr?this->mDat->type():typeid(void);
		}
		std::string to_string() const {
			if(this->mDat == nullptr)
				throw cov::error("E0005");
			return std::move(this->mDat->to_string());
		}
		any & operator=(const any & var) {
			if(&var!=this) {
				delete mDat;
				mDat = var.usable()?var.mDat->duplicate():nullptr;
			}
			return *this;
		}
		any & operator=(any&& var) noexcept {
			if(&var!=this)
				swap(std::forward<any>(var));
			return *this;
		}
		bool operator==(const any & var) const {
			return usable()?this->mDat->compare(var.mDat):!var.usable();
		}
		bool operator!=(const any & var)const {
			return usable()?!this->mDat->compare(var.mDat):var.usable();
		}
		template < typename T > T & val() {
			if(typeid(T) != this->type())
				throw cov::error("E0006");
			if(this->mDat == nullptr)
				throw cov::error("E0005");
			return dynamic_cast < holder < T > *>(this->mDat)->data();
		}
		template < typename T > const T & val() const {
			if(typeid(T) != this->type())
				throw cov::error("E0006");
			if(this->mDat == nullptr)
				throw cov::error("E0005");
			return dynamic_cast < const holder < T > *>(this->mDat)->data();
		}
		template < typename T > operator T&() {
			return this->val<T>();
		}
		template < typename T > operator const T&() const {
			return this->val<T>();
		}
		template < typename T > void assign(const T & dat) {
			delete mDat;
			mDat = new holder < T > (dat);
		}
		template < typename T > any & operator=(const T & dat) {
			assign(dat);
			return *this;
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