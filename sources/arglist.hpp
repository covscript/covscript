#pragma once
#include "../include/mozart/any.hpp"
#include "./core.hpp"
namespace cov_basic {
	class arglist final {
		template<typename _Tp>static int count_types(int index=1)
		{
			return index;
		}
		template<typename _Tp,typename _fT,typename...ArgTypes>
		static int count_types(int index=1)
		{
			return count_types<_fT,ArgTypes...>(++index);
		}
		template<typename _Tp>static std::string get_type(int expect,int index)
		{
			if(expect==index)
				return typeid(_Tp).name();
			else
				throw internal_error("Wrong index of arglist.");
		}
		template<typename _Tp,typename _fT,typename...ArgTypes>
		static std::string get_type(int expect,int index)
		{
			if(expect==index)
				return typeid(_Tp).name();
			else
				return get_type<_fT,ArgTypes...>(expect,++index);
		}
		template<typename _Tp>static int check_types(int index,const std::deque<cov::any>& args)
		{
			if(index<=args.size()&&args.at(index-1).type()==typeid(_Tp))
				return -1;
			else
				return index;
		}
		template<typename _Tp,typename _fT,typename...ArgTypes>
		static int check_types(int index,const std::deque<cov::any>& args)
		{
			if(index<=args.size()&&args.at(index-1).type()==typeid(_Tp))
				return check_types<_fT,ArgTypes...>(++index,args);
			else
				return index;
		}
	public:
		template<typename...ArgTypes>
		static void check(const std::deque<cov::any>& args)
		{
			if(count_types<ArgTypes...>()==args.size()) {
				int result=check_types<ArgTypes...>(1,args);
				if(result!=-1)
					throw syntax_error("Invalid Argument.At "+std::to_string(result)+".Expected "+get_type<ArgTypes...>(result,1));
			}
			else
				throw syntax_error("Wrong size of the arguments.Expected "+std::to_string(count_types<ArgTypes...>()));
		}
	};
}
