#pragma once
/*
* Covariant Mozart Utility Library: Random
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
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
