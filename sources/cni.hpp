#include "../include/mozart/bind.hpp"
#include "./core.hpp"
#include "./arglist.hpp"
#include <functional>
namespace cov_basic {
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
	template<typename T>class cni_helper;
	template<>class cni_helper<void(*)()> {
		std::function<void()> mFunc;
	public:
		cni_helper()=delete;
		cni_helper(const cni_helper&)=default;
		cni_helper(const std::function<void()>& func):mFunc(func) {}
		cov::any call(array& args) const
		{
			if(!args.empty())
				throw syntax_error("Wrong size of the arguments.Expected 0");
			mFunc();
			return number(0);
		}
	};
	template<typename RetT>class cni_helper<RetT(*)()> {
		std::function<RetT()> mFunc;
	public:
		cni_helper()=delete;
		cni_helper(const cni_helper&)=default;
		cni_helper(const std::function<RetT()>& func):mFunc(func) {}
		cov::any call(array& args) const
		{
			if(!args.empty())
				throw syntax_error("Wrong size of the arguments.Expected 0");
			return mFunc();
		}
	};
	template<typename...ArgsT>
	class cni_helper<void(*)(ArgsT...)> {
		using args_t=typename cov::type_list::make<ArgsT...>::result;
		std::function<void(ArgsT...)> mFunc;
		template<int...S>void _call(array& args,cov::sequence<S...>) const
		{
			mFunc(convert<ArgsT>::get_val(args.at(S))...);
		}
	public:
		cni_helper()=delete;
		cni_helper(const cni_helper&)=default;
		cni_helper(const std::function<void(ArgsT...)>& func):mFunc(func) {}
		cov::any call(array& args) const
		{
			arglist::check<ArgsT...>(args);
			_call(args,cov::make_sequence<cov::type_list::get_size<args_t>::result>::result);
			return number(0);
		}
	};
	template<typename RetT,typename...ArgsT>
	class cni_helper<RetT(*)(ArgsT...)> {
		using args_t=typename cov::type_list::make<ArgsT...>::result;
		std::function<RetT(ArgsT...)> mFunc;
		template<int...S>RetT _call(array& args,cov::sequence<S...>) const
		{
			return mFunc(convert<ArgsT>::get_val(args.at(S))...);
		}
	public:
		cni_helper()=delete;
		cni_helper(const cni_helper&)=default;
		cni_helper(const std::function<RetT(ArgsT...)>& func):mFunc(func) {}
		cov::any call(array& args) const
		{
			arglist::check<ArgsT...>(args);
			return _call(args,cov::make_sequence<cov::type_list::get_size<args_t>::result>::result);
		}
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
			cni_helper<typename cov::function_parser<T>::type::common_type> mCni;
		public:
			cni_holder()=delete;
			cni_holder(const cni_holder&)=default;
			cni_holder(T func):mCni(func) {}
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
		cni_base* mCni=nullptr;
	public:
		cni()=delete;
		cni(const cni& c):mCni(c.mCni->clone()) {}
		template<typename T>cni(T func):mCni(new cni_holder<T>(func)) {}
		~cni()
		{
			delete mCni;
		}
		cov::any operator()(array& args) const
		{
			return mCni->call(args);
		}
	};
}
