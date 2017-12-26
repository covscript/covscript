#pragma once
/*
* Covariant Mozart Utility Library: Random
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
* Copyright (C) 2018 Michael Lee(李登淳)
* Email: mikecovlee@163.com
* Github: https://github.com/mikecovlee
*
* Version: 17.2.1
*/
#include "./base.hpp"
#include <type_traits>
#include <random>
#include <ctime>

namespace cov {
	namespace random {
		static std::default_random_engine random_engine(time(nullptr));
		template<typename T, bool is_integral>
		struct random_traits;

		template<typename T>
		struct random_traits<T, true> {
			static T rand(T begin, T end)
			{
				return std::uniform_int_distribution<T>(begin, end)(random_engine);
			}
		};

		template<typename T>
		struct random_traits<T, false> {
			static T rand(T begin, T end)
			{
				return std::uniform_real_distribution<T>(begin, end)(random_engine);
			}
		};
	}

	template<typename T>
	T rand(T begin, T end)
	{
		return random::random_traits<T, std::is_integral<T>::value>::rand(begin, end);
	}
}
