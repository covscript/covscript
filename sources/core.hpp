#pragma once
#ifndef CS_STATIC
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
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <string>
#include <memory>
#include <cmath>
#include <deque>
#include <list>
namespace cs {
#ifndef CS_STATIC
	const std::string version="1.0.0";
#else
#ifndef CS_MINIMAL
	const std::string version="1.0.0 (Static Build)";
#else
	const std::string version="1.0.0 (Minimal Build)";
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
		std::size_t m_hash;
		std::string m_name;
		std::shared_ptr<std::unordered_map<string,cov::any>> m_data;
	public:
		structure()=delete;
		structure(std::size_t hash,const std::string& name,const std::shared_ptr<std::unordered_map<string,cov::any>>& data):m_hash(hash),m_name(typeid(structure).name()+name),m_data(data) {}
		structure(const structure& s):m_hash(s.m_hash),m_name(s.m_name),m_data(std::make_shared<std::unordered_map<string,cov::any>>(*s.m_data))
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
		std::size_t get_hash() const
		{
			return m_hash;
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
		static std::size_t mCount;
		std::size_t mHash;
		std::string mName;
		std::deque<statement_base*> mMethod;
	public:
		struct_builder()=delete;
		struct_builder(const std::string& name,const std::deque<statement_base*>& method):mHash(++mCount),mName(name),mMethod(method) {}
		struct_builder(const struct_builder&)=default;
		~struct_builder()=default;
		std::size_t get_hash() const
		{
			return mHash;
		}
		cov::any operator()();
	};
	std::size_t struct_builder::mCount=0;
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
#ifndef CS_STATIC
		cov::dll m_dll;
#endif
	public:
		name_space_holder()=delete;
		name_space_holder(name_space* ptr):m_ns(ptr) {}
#ifndef CS_STATIC
		name_space_holder(const std::string& path):m_dll(path)
		{
			m_ns=reinterpret_cast<name_space*(*)()>(m_dll.get_address("__CS_EXTENSION__"))();
		}
#else
		name_space_holder(const std::string&)
		{
			throw internal_error("Can not load extension because covscript is static version.");
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
	using name_space_t=std::shared_ptr<name_space_holder>;
	using extension_t=std::shared_ptr<extension_holder>;
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
	void cs(const std::string&);
}
namespace cov {
	template<>void detach<cs::pair>(cs::pair& val)
	{
		cs::copy_no_return(val.first);
		cs::copy_no_return(val.second);
	}
	template<>void detach<cs::list>(cs::list& val)
	{
		for(auto& it:val)
			cs::copy_no_return(it);
	}
	template<>void detach<cs::array>(cs::array& val)
	{
		for(auto& it:val)
			cs::copy_no_return(it);
	}
	template<>void detach<cs::hash_map>(cs::hash_map& val)
	{
		for(auto& it:val)
			cs::copy_no_return(it.second);
	}
	template<>std::string to_string<cs::number>(const cs::number& val)
	{
		std::stringstream ss;
		std::string str;
		ss<<std::setprecision(cs::output_precision)<<val;
		ss>>str;
		return std::move(str);
	}
	template<>std::string to_string<char>(const char& c)
	{
		return std::move(std::string(1,c));
	}
}
