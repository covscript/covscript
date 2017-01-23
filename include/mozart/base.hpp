#pragma once
/*
* Covariant Mozart Utility Library: Base
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
* Library Version: 17.1.1
*/
#ifndef __cplusplus
#error E0001
#endif

#if __cplusplus < 201103L
#error E0002
#endif

#define __Mozart 170100L

#include <exception>
#include <stdexcept>
#include <typeindex>
#include <typeinfo>
#include <string>

namespace cov {
	class warning final:public std::exception {
		std::string mWhat="Mozart Warning";
	public:
		warning()=default;
	warning(const std::string& str) noexcept:
		mWhat("Mozart Warning:"+str) {}
		warning(const warning&)=default;
		warning(warning&&)=default;
		virtual ~warning()=default;
		virtual const char* what() const noexcept override {
			return this->mWhat.c_str();
		}
	};
	class error final:public std::exception {
		std::string mWhat="Mozart Error";
	public:
		error()=default;
	error(const std::string& str) noexcept:
		mWhat("Mozart Error:"+str) {}
		error(const error&)=default;
		error(error&&)=default;
		virtual ~error()=default;
		error& operator=(const error&)=default;
		error& operator=(error&&)=default;
		virtual const char* what() const noexcept override {
			return this->mWhat.c_str();
		}
	};
	class object {
	public:
		static bool show_warning;
		object()=default;
		object(object&&) noexcept=default;
		object(const object&)=default;
		virtual ~object()=default;
		virtual std::type_index object_type() const noexcept final {
			return typeid(*this);
		}
		virtual std::string to_string() const noexcept {
			return typeid(*this).name();
		}
		virtual object* clone() noexcept {
			return nullptr;
		}
		virtual bool equals(const object* ptr) const noexcept {
			return this==ptr;
		}
	};
	bool object::show_warning=true;
}