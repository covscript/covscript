#pragma once
/*
* Covariant Script Argument List
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Affero General Public License as published
* by the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Affero General Public License for more details.
*
* You should have received a copy of the GNU Affero General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
* Copyright (C) 2017 Michael Lee(李登淳)
* Email: mikecovlee@163.com
* Github: https://github.com/mikecovlee
*/
#include "../include/mozart/traits.hpp"
#include "./core.hpp"

namespace cs {
	template<typename T>
	constexpr const char *get_name_of_type()
	{
		return typeid(T).name();
	}

	template<>
	constexpr const char *get_name_of_type<var>()
	{
		return "cs::var";
	}

	template<>
	constexpr const char *get_name_of_type<number>()
	{
		return "cs::number";
	}

	template<>
	constexpr const char *get_name_of_type<boolean>()
	{
		return "cs::boolean";
	}

	template<>
	constexpr const char *get_name_of_type<string>()
	{
		return "cs::string";
	}

	template<>
	constexpr const char *get_name_of_type<list>()
	{
		return "cs::list";
	}

	template<>
	constexpr const char *get_name_of_type<array>()
	{
		return "cs::array";
	}

	template<>
	constexpr const char *get_name_of_type<pair>()
	{
		return "cs::pair";
	}

	template<>
	constexpr const char *get_name_of_type<hash_map>()
	{
		return "cs::hash_map";
	}

	class arglist final {
		template<typename T, int index>
		struct check_arg {
			static inline short check(const var &val)
			{
				if (typeid(T) != val.type())
					throw syntax_error("Invalid Argument.At " + std::to_string(index + 1) + ".Expected " + get_name_of_type<T>());
				else
					return 0;
			}
		};

		static inline void result_container(short...) {}

		template<typename...ArgsT, int...Seq>
		static inline void check_helper(const std::deque<var> &args, const cov::sequence<Seq...> &)
		{
			result_container(check_arg<typename cov::remove_constant<typename cov::remove_reference<ArgsT>::type>::type, Seq>::check(args[Seq])...);
		}

	public:
		template<typename...ArgTypes>
		static inline void check(const std::deque<var> &args)
		{
			if (sizeof...(ArgTypes) == args.size())
				check_helper<ArgTypes...>(args, cov::make_sequence<sizeof...(ArgTypes)>::result);
			else
				throw syntax_error("Wrong size of the arguments.Expected " + std::to_string(sizeof...(ArgTypes)));
		}
	};

	template<int index>
	struct arglist::check_arg<var, index> {
		static inline short check(const var &)
		{
			return 0;
		}
	};
}
