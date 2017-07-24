#pragma once
/*
* Covariant Script C/C++ Native Interface
* CNI's effect is to forward CovScript function call to C/C++ function and need to cooperate with cs::native_interface to use.
* Please do not bind a CNI directly to CovScript. CovScript can not directly identify a CNI.
* CNI can recognize any form of C/C++ functions, but please note that CNI does not support implicit conversion of parameter types.
* To avoid compatibility issues, use CovScript built-in types as much as possible.
* You can use the reference to modify the value of the CovScript passed parameters, but please note that this may cause an error because some of the variables in CovScript are protected.
* To improve the efficiency of a function, you can use constant references instead of values. Please note that CNI does not currently support forwarding right-value references.
* If you want to receive a variable parameter, just use cov::any as the argument type.
* This program was written by Micheal Lee(mikecovlee@163.com)。
*/
#include "../include/mozart/bind.hpp"
#include "../include/mozart/traits.hpp"
#include "./core.hpp"
#include "./arglist.hpp"
namespace cs {
	template<typename T>struct convert {
		static const T& get_val(cov::any& val)
		{
			return val.const_val<T>();
		}
	};
	template<typename T>struct convert<const T&> {
		static const T& get_val(cov::any& val)
		{
			return val.const_val<T>();
		}
	};
	template<typename T>struct convert<T&> {
		static T& get_val(cov::any& val)
		{
			return val.val<T>(true);
		}
	};
	template<>struct convert<const cov::any&> {
		static const cov::any& get_val(const cov::any& val)
		{
			return val;
		}
	};
	template<>struct convert<cov::any&> {
		static cov::any& get_val(cov::any& val)
		{
			return val;
		}
	};
	template<typename T,typename X>class cni_helper;
	template<typename X>class cni_helper<void(*)(),X> {
		X mFunc;
	public:
		cni_helper()=delete;
		cni_helper(const cni_helper&)=default;
		cni_helper(const X& func):mFunc(func) {}
		cov::any call(array& args) const
		{
			if(!args.empty())
				throw syntax_error("Wrong size of the arguments.Expected 0");
			mFunc();
			return cov::any::make<number>(0);
		}
	};
	template<typename RetT,typename X>class cni_helper<RetT(*)(),X> {
		X mFunc;
	public:
		cni_helper()=delete;
		cni_helper(const cni_helper&)=default;
		cni_helper(const X& func):mFunc(func) {}
		cov::any call(array& args) const
		{
			if(!args.empty())
				throw syntax_error("Wrong size of the arguments.Expected 0");
			return std::move(mFunc());
		}
	};
	template<typename...ArgsT,typename X>
	class cni_helper<void(*)(ArgsT...),X> {
		using args_t=typename cov::type_list::make<ArgsT...>::result;
		X mFunc;
		template<int...S>void _call(array& args,const cov::sequence<S...>&) const
		{
			mFunc(convert<ArgsT>::get_val(args.at(S))...);
		}
	public:
		cni_helper()=delete;
		cni_helper(const cni_helper&)=default;
		cni_helper(const X& func):mFunc(func) {}
		cov::any call(array& args) const
		{
			arglist::check<ArgsT...>(args);
			_call(args,cov::make_sequence<cov::type_list::get_size<args_t>::result>::result);
			return cov::any::make<number>(0);
		}
	};
	template<typename RetT,typename...ArgsT,typename X>
	class cni_helper<RetT(*)(ArgsT...),X> {
		using args_t=typename cov::type_list::make<ArgsT...>::result;
		X mFunc;
		template<int...S>RetT _call(array& args,const cov::sequence<S...>&) const
		{
			return std::move(mFunc(convert<ArgsT>::get_val(args.at(S))...));
		}
	public:
		cni_helper()=delete;
		cni_helper(const cni_helper&)=default;
		cni_helper(const X& func):mFunc(func) {}
		cov::any call(array& args) const
		{
			arglist::check<ArgsT...>(args);
			return std::move(_call(args,cov::make_sequence<cov::type_list::get_size<args_t>::result>::result));
		}
	};
	template<typename T>struct cni_modify {
		using type=T;
	};
	template<typename RetT,typename...ArgsT>struct cni_modify<RetT(ArgsT...)> {
		using type=RetT(*)(ArgsT...);
	};
	class cni final {
		class cni_base {
		public:
			cni_base()=default;
			cni_base(const cni_base&)=default;
			virtual ~cni_base()=default;
			virtual cni_base* clone()=0;
			virtual cov::any call(array&) const=0;
		};
		template<typename T>class cni_holder final:public cni_base {
			cni_helper<typename cov::function_parser<T>::type::common_type,T> mCni;
		public:
			cni_holder()=delete;
			cni_holder(const cni_holder&)=default;
			cni_holder(const T& func):mCni(func) {}
			virtual ~cni_holder()=default;
			virtual cni_base* clone() override
			{
				return new cni_holder(*this);
			}
			virtual cov::any call(array& args) const override
			{
				return mCni.call(args);
			}
		};
		template<typename T>struct construct_helper {
			template<typename X>static cni_base* construct(X&& val)
			{
				return new cni_holder<T>(std::forward<X>(val));
			}
		};
		cni_base* mCni=nullptr;
	public:
		cni()=delete;
		cni(const cni& c):mCni(c.mCni->clone()) {}
		template<typename T>cni(T&& val):mCni(construct_helper<typename cni_modify<typename cov::remove_reference<T>::type>::type>::construct(std::forward<T>(val))) {}
		~cni()
		{
			delete mCni;
		}
		cov::any operator()(array& args) const
		{
			return mCni->call(args);
		}
	};
	template<>struct cni::construct_helper<const cni> {
		static cni_base* construct(const cni& c)
		{
			return c.mCni->clone();
		}
	};
	template<>struct cni::construct_helper<cni> {
		static cni_base* construct(const cni& c)
		{
			return c.mCni->clone();
		}
	};
}
