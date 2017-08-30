#pragma once
/*
* Covariant Mozart Utility Library: Argument List
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
* Copyright (C) 2017 Michael Lee(李登淳)
* Email: mikecovlee@163.com
* Github: https://github.com/mikecovlee
*
* Version: 17.1.0
*/
#include "./base.hpp"
#include "./any.hpp"
#include <deque>

namespace cov {
	class argument_list final {
	private:
		std::deque<std::type_index> mTypes;
		std::deque<cov::any> mArgs;

		template<typename _Tp>
		void unpack_types()
		{
			this->mTypes.push_back(typeid(_Tp));
		}

		template<typename _Tp, typename _fT, typename...ArgTypes>
		void unpack_types()
		{
			this->mTypes.push_back(typeid(_Tp));
			unpack_types<_fT, ArgTypes...>();
		}

		template<typename _Tp>
		int count_types(int index = 1) const
		{
			return index;
		}

		template<typename _Tp, typename _fT, typename...ArgTypes>
		int count_types(int index = 1) const
		{
			return count_types<_fT, ArgTypes...>(++index);
		}

		template<typename _Tp>
		std::string get_type(int expect, int index) const
		{
			if (expect == index)
				return typeid(_Tp).name();
		}

		template<typename _Tp, typename _fT, typename...ArgTypes>
		std::string get_type(int expect, int index) const
		{
			if (expect == index)
				return typeid(_Tp).name();
			else
				return get_type<_fT, ArgTypes...>(expect, ++index);
		}

		template<typename _Tp>
		int check_types(int index, std::deque<std::type_index>::const_iterator it) const
		{
			if (it != this->mTypes.end() && *it == typeid(_Tp))
				return -1;
			else
				return index;
		}

		template<typename _Tp, typename _fT, typename...ArgTypes>
		int check_types(int index, std::deque<std::type_index>::const_iterator it) const
		{
			if (it != this->mTypes.end() && *it == typeid(_Tp))
				return check_types<_fT, ArgTypes...>(++index, ++it);
			else
				return index;
		}

	public:
		typedef std::deque<cov::any>::iterator iterator;
		typedef std::deque<cov::any>::const_iterator const_iterator;

		argument_list() = delete;

		template<typename...ArgTypes>
		argument_list(ArgTypes &&...args):mArgs(
		{
			args...
		})
		{
			unpack_types<ArgTypes...>();
		}

		argument_list(const argument_list &) = default;

		argument_list(argument_list &&) = default;

		~argument_list() = default;

		argument_list &operator=(const argument_list &arglist)
		{
			if (&arglist != this && arglist.mTypes == this->mTypes)
				this->mArgs = arglist.mArgs;
			return *this;
		}

		argument_list &operator=(argument_list &&arglist)
		{
			if (&arglist != this && arglist.mTypes == this->mTypes)
				this->mArgs = arglist.mArgs;
			return *this;
		}

		bool operator==(const argument_list &arglist)
		{
			if (&arglist == this)
				return true;
			return arglist.mTypes == this->mTypes;
		}

		bool operator!=(const argument_list &arglist)
		{
			if (&arglist == this)
				return false;
			return arglist.mTypes != this->mTypes;
		}

		cov::any &operator[](std::size_t posit)
		{
			return this->mArgs.at(posit);
		}

		const cov::any &operator[](std::size_t posit) const
		{
			return this->mArgs.at(posit);
		}

		cov::any &at(std::size_t posit)
		{
			return this->mArgs.at(posit);
		}

		const cov::any &at(std::size_t posit) const
		{
			return this->mArgs.at(posit);
		}

		iterator begin()
		{
			return this->mArgs.begin();
		}

		const_iterator begin() const
		{
			return this->mArgs.begin();
		}

		iterator end()
		{
			return this->mArgs.end();
		}

		const_iterator end() const
		{
			return this->mArgs.end();
		}

		std::size_t size() const
		{
			return this->mArgs.size();
		}

		template<typename...ArgTypes>
		void check() const
		{
			if (count_types<ArgTypes...>() == this->mTypes.size()) {
				int result = check_types<ArgTypes...>(1, this->mTypes.begin());
				if (result != -1)
					throw std::invalid_argument("E0008.At " + std::to_string(result) + ".Expected " + get_type<ArgTypes...>(result, 1));
			}
			else
				throw cov::error("E0009.Expected " + std::to_string(count_types<ArgTypes...>()));
		}
	};
}
