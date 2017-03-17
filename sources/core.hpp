#pragma once
#include "../include/mozart/random.hpp"
#include "../include/mozart/timer.hpp"
#include "../include/mozart/tree.hpp"
#include "../include/mozart/any.hpp"
#include <forward_list>
#include <functional>
#include <exception>
#include <stdexcept>
#include <string>
#include <deque>
namespace cov_basic {
	using number=long double;
	using boolean=bool;
	using string=std::string;
	using array=std::deque<cov::any>;
	class token_base;
	class statement_base;
	class native_interface final {
	public:
		using function_type=std::function<cov::any(array&)>;
	private:
		function_type mFunc;
	public:
		native_interface()=delete;
		native_interface(const native_interface&)=default;
		native_interface(const function_type& func):mFunc(func) {}
		cov::any call(array& args) const
		{
			return mFunc(args);
		}
	};
	class function final {
		friend class statement_return;
		mutable cov::any* mRetVal=nullptr;
		std::deque<std::string> mArgs;
		std::deque<statement_base*> mBody;
	public:
		function()=delete;
		function(const std::deque<std::string>& args,const std::deque<statement_base*>& body):mArgs(args),mBody(body) {}
		~function()=default;
		cov::any call(const array&) const;
	};
	class syntax_error final:public std::exception {
		std::string mWhat="Covariant Basic Syntax Error";
	public:
		syntax_error()=default;
		syntax_error(const std::string& str) noexcept:
			mWhat("Covariant Basic Syntax Error:"+str) {}
		syntax_error(const syntax_error&)=default;
		syntax_error(syntax_error&&)=default;
		virtual ~syntax_error()=default;
		syntax_error& operator=(const syntax_error&)=default;
		syntax_error& operator=(syntax_error&&)=default;
		virtual const char* what() const noexcept override
		{
			return this->mWhat.c_str();
		}
	};
	class lang_error final:public std::exception {
		std::string mWhat="Covariant Basic Language Error";
	public:
		lang_error()=default;
		lang_error(const std::string& str) noexcept:
			mWhat("Covariant Basic Language Error:"+str) {}
		lang_error(const lang_error&)=default;
		lang_error(lang_error&&)=default;
		virtual ~lang_error()=default;
		lang_error& operator=(const lang_error&)=default;
		lang_error& operator=(lang_error&&)=default;
		virtual const char* what() const noexcept override
		{
			return this->mWhat.c_str();
		}
	};
	template<typename T>class garbage_collector final {
		std::forward_list<T*> table_new;
		std::forward_list<T*> table_delete;
	public:
		garbage_collector()=default;
		garbage_collector(const garbage_collector&)=delete;
		~garbage_collector()
		{
			for(auto& ptr:table_delete)
				table_new.remove(ptr);
			for(auto& ptr:table_new)
				delete ptr;
		}
		void add(void* ptr)
		{
			table_new.push_front(static_cast<T*>(ptr));
		}
		void remove(void* ptr)
		{
			table_delete.push_front(static_cast<T*>(ptr));
		}
	};
}