#pragma once
/*
* Covariant Mozart Utility Library: Base
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
* Library Version: 17.2.1
*/
#ifndef __cplusplus
#error E0001
#endif

#ifndef _MSC_VER
#if __cplusplus < 201103L
#error E0002
#endif
#endif

#define __Mozart 170601L

#include <exception>
#include <stdexcept>
#include <typeindex>
#include <typeinfo>
#include <string>

namespace cov {
	class warning final : public std::exception {
		std::string mWhat = "Mozart Warning";
	public:
		warning() = default;

		explicit warning(const std::string &str) noexcept:
			mWhat("Mozart Warning:" + str) {}

		warning(const warning &) = default;

		warning(warning &&) = default;

		~warning() override = default;

		const char *what() const noexcept override
		{
			return this->mWhat.c_str();
		}
	};

	class error final : public std::exception {
		std::string mWhat = "Mozart Error";
	public:
		error() = default;

		explicit error(const std::string &str) noexcept:
			mWhat("Mozart Error:" + str) {}

		error(const error &) = default;

		error(error &&) = default;

		~error() override = default;

		error &operator=(const error &) = default;

		error &operator=(error &&) = default;

		const char *what() const noexcept override
		{
			return this->mWhat.c_str();
		}
	};

	class object {
	public:
		object() = default;

		object(object &&) noexcept = default;

		object(const object &) = default;

		virtual ~object() = default;

		virtual std::type_index object_type() const noexcept final
		{
			return typeid(*this);
		}

		virtual std::string to_string() const noexcept
		{
			return typeid(*this).name();
		}

		virtual object *clone() noexcept
		{
			return nullptr;
		}

		virtual bool equals(const object *ptr) const noexcept
		{
			return this == ptr;
		}
	};
}
