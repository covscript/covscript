#pragma once
#ifndef CBS_STATIC
#include "../include/libdll/dll.hpp"
#endif
#include "../include/mozart/static_stack.hpp"
#include "../include/mozart/random.hpp"
#include "../include/mozart/timer.hpp"
#include "../include/mozart/tree.hpp"
#include "../include/mozart/any.hpp"
#include "./exceptions.hpp"
#include <unordered_map>
#include <forward_list>
#include <functional>
#include <sstream>
#include <iomanip>
#include <string>
#include <memory>
#include <cmath>
#include <deque>
#include <list>
namespace cov_basic {
#ifndef CBS_STATIC
	const std::string version="2.1.5.5";
#else
#ifndef CBS_MINIMAL
	const std::string version="2.1.5.5 (Static Build)";
#else
	const std::string version="2.1.5.5 (Minimal Build)";
#endif
#endif
	static int output_precision=8;
	using var=cov::any;
	using number=long double;
	using boolean=bool;
	using string=std::string;
	using list=std::list<var>;
	using array=std::deque<var>;
	using pair=std::pair<var,var>;
	using hash_map=std::unordered_map<var,var>;
	class callable final {
	public:
		using function_type=std::function<cov::any(array&)>;
	private:
		function_type mFunc;
		bool mConstant=false;
	public:
		callable()=delete;
		callable(const callable&)=default;
		callable(const function_type& func,bool constant=false):mFunc(func),mConstant(constant) {}
		bool is_constant() const
		{
			return mConstant;
		}
		cov::any call(array& args) const
		{
			return mFunc(args);
		}
	};
	using native_interface=callable;
	class token_base;
	class statement_base;
	class function final {
		friend class statement_return;
		mutable cov::any mRetVal;
		std::deque<std::string> mArgs;
		std::deque<statement_base*> mBody;
		std::shared_ptr<std::unordered_map<string,cov::any>> mData;
	public:
		function()=delete;
		function(const std::deque<std::string>& args,const std::deque<statement_base*>& body):mArgs(args),mBody(body) {}
		~function()=default;
		cov::any call(array&) const;
		void set_data(const std::shared_ptr<std::unordered_map<string,cov::any>>& data)
		{
			mData=data;
		}
	};
	class object_method final {
		cov::any mObj;
		cov::any mCallable;
	public:
		object_method()=delete;
		object_method(const cov::any& obj,const cov::any& callable):mObj(obj),mCallable(callable) {}
		~object_method()=default;
		const cov::any& get_callable() const
		{
			return mCallable;
		}
		cov::any operator()(array& args) const
		{
			args.push_front(mObj);
			cov::any retval=mCallable.const_val<callable>().call(args);
			args.pop_front();
			return retval;
		}
	};
	struct type final {
		std::function<cov::any()> constructor;
		std::size_t id;
		type()=delete;
		type(const std::function<cov::any()>& c,std::size_t i):constructor(c),id(i) {}
	};
	class structure final {
		std::string m_name;
		std::shared_ptr<std::unordered_map<string,cov::any>> m_data;
	public:
		structure()=delete;
		structure(const std::string& name,const std::shared_ptr<std::unordered_map<string,cov::any>>& data):m_name(typeid(structure).name()+name),m_data(data) {}
		structure(const structure& s):m_name(s.m_name),m_data(std::make_shared<std::unordered_map<string,cov::any>>(*s.m_data))
		{
			for(auto& it:*m_data) {
				it.second.clone();
				if(it.second.type()==typeid(function))
					it.second.val<function>(true).set_data(m_data);
			}
		}
		~structure()=default;
		std::shared_ptr<std::unordered_map<string,cov::any>>& get_domain()
		{
			return m_data;
		}
		const std::string& get_name() const
		{
			return m_name;
		}
		cov::any& get_var(const std::string& name) const
		{
			if(m_data->count(name)>0)
				return m_data->at(name);
			else
				throw syntax_error("Struct \""+m_name+"\" have no member called \""+name+"\".");
		}
	};
	class struct_builder final {
		std::string mName;
		std::deque<statement_base*> mMethod;
	public:
		struct_builder()=delete;
		struct_builder(const std::string& name,const std::deque<statement_base*>& method):mName(name),mMethod(method) {}
		~struct_builder()=default;
		cov::any operator()();
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
	class name_space final {
		std::unordered_map<string,cov::any> m_data;
	public:
		name_space()=default;
		name_space(const name_space&)=delete;
		name_space(const std::unordered_map<string,cov::any>& dat):m_data(dat) {}
		~name_space()=default;
		void add_var(const std::string& name,const cov::any& var)
		{
			if(m_data.count(name)>0)
				m_data.at(name)=var;
			else
				m_data.emplace(name,var);
		}
		cov::any& get_var(const std::string& name)
		{
			if(m_data.count(name)>0)
				return m_data.at(name);
			else
				throw syntax_error("Use of undefined variable \""+name+"\" in extension.");
		}
	};
	class name_space_holder final {
		name_space* m_ns=nullptr;
#ifndef CBS_STATIC
		cov::dll m_dll;
#endif
	public:
		name_space_holder()=delete;
		name_space_holder(name_space* ptr):m_ns(ptr) {}
#ifndef CBS_STATIC
		name_space_holder(const std::string& path):m_dll(path)
		{
			m_ns=reinterpret_cast<name_space*(*)()>(m_dll.get_address("__CBS_EXTENSION__"))();
		}
#else
		name_space_holder(const std::string&)
		{
			throw internal_error("Can not load extension because covbasic is static version.");
		}
#endif
		~name_space_holder()=default;
		cov::any& get_var(const std::string& name)
		{
			if(m_ns==nullptr)
				throw internal_error("Use of nullptr of extension.");
			return m_ns->get_var(name);
		}
	};
	using extension=name_space;
	using extension_holder=name_space_holder;
	using extension_t=std::shared_ptr<name_space_holder>;
	cov::any parse_value(const std::string& str)
	{
		if(str=="true")
			return true;
		if(str=="false")
			return false;
		try {
			return number(std::stold(str));
		}
		catch(...) {
			return str;
		}
		return str;
	}
	void copy_no_return(cov::any& val)
	{
		val.clone();
		val.detach();
	}
	cov::any copy(cov::any val)
	{
		val.clone();
		val.detach();
		return val;
	}
	void cov_basic(const std::string&);
}
namespace cov {
	template<>void detach<cov_basic::pair>(cov_basic::pair& val)
	{
		cov_basic::copy_no_return(val.first);
		cov_basic::copy_no_return(val.second);
	}
	template<>void detach<cov_basic::list>(cov_basic::list& val)
	{
		for(auto& it:val)
			cov_basic::copy_no_return(it);
	}
	template<>void detach<cov_basic::array>(cov_basic::array& val)
	{
		for(auto& it:val)
			cov_basic::copy_no_return(it);
	}
	template<>void detach<cov_basic::hash_map>(cov_basic::hash_map& val)
	{
		for(auto& it:val)
			cov_basic::copy_no_return(it.second);
	}
	template<>std::string to_string<cov_basic::number>(const cov_basic::number& val)
	{
		std::stringstream ss;
		std::string str;
		ss<<std::setprecision(cov_basic::output_precision)<<val;
		ss>>str;
		return std::move(str);
	}
	template<>std::string to_string<char>(const char& c)
	{
		return std::move(std::string(1,c));
	}
}
