#pragma once
#include "../include/mozart/any.hpp"
#include "../include/mozart/traits.hpp"
#include "./core.hpp"
namespace cs {
	template<typename T>constexpr const char* get_name_of_type()
	{
		return typeid(T).name();
	}
	template<>constexpr const char* get_name_of_type<var>()
	{
		return "cs::var";
	}
	template<>constexpr const char* get_name_of_type<number>()
	{
		return "cs::number";
	}
	template<>constexpr const char* get_name_of_type<boolean>()
	{
		return "cs::boolean";
	}
	template<>constexpr const char* get_name_of_type<string>()
	{
		return "cs::string";
	}
	template<>constexpr const char* get_name_of_type<list>()
	{
		return "cs::list";
	}
	template<>constexpr const char* get_name_of_type<array>()
	{
		return "cs::array";
	}
	template<>constexpr const char* get_name_of_type<pair>()
	{
		return "cs::pair";
	}
	template<>constexpr const char* get_name_of_type<hash_map>()
	{
		return "cs::hash_map";
	}
	class arglist final {
		template<typename T,int index>struct check_arg {
			static inline short check(const cov::any& val)
			{
				if(typeid(T)!=val.type())
					throw syntax_error("Invalid Argument.At "+std::to_string(index+1)+".Expected "+get_name_of_type<T>());
				else
					return 0;
			}
		};
		static inline void result_container(short...) {}
		template<typename...ArgsT,int...Seq>static inline void check_helper(const std::deque<cov::any>& args,const cov::sequence<Seq...>&)
		{
			result_container(check_arg<typename cov::remove_constant<typename cov::remove_reference<ArgsT>::type>::type,Seq>::check(args.at(Seq))...);
		}
	public:
		template<typename...ArgTypes>static inline void check(const std::deque<cov::any>& args)
		{
			if(sizeof...(ArgTypes)==args.size())
				check_helper<ArgTypes...>(args,cov::make_sequence<sizeof...(ArgTypes)>::result);
			else
				throw syntax_error("Wrong size of the arguments.Expected "+std::to_string(sizeof...(ArgTypes)));
		}
	};
	template<int index>struct arglist::check_arg<cov::any,index> {
		static inline short check(const cov::any&)
		{
			return 0;
		}
	};
}
