#pragma once
#include <type_traits>
#include <random>
#include <ctime>
namespace darwin {
	namespace random {
		static std::default_random_engine random_engine(time(nullptr));
		template<typename T,bool is_integral>struct random_traits;
		template<typename T>struct random_traits<T,true> {
			static T rand(T begin,T end)
			{
				return std::uniform_int_distribution<T>(begin,end)(random_engine);
			}
		};
		template<typename T>struct random_traits<T,false> {
			static T rand(T begin,T end)
			{
				return std::uniform_real_distribution<T>(begin,end)(random_engine);
			}
		};
	}
	template<typename T>T rand(T begin,T end)
	{
		return random::random_traits<T,std::is_integral<T>::value>::rand(begin,end);
	}
}
