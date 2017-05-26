#pragma once
#include "../include/mozart/random.hpp"
#include "../include/mozart/timer.hpp"
#include "../include/mozart/tree.hpp"
#include "../include/mozart/any.hpp"
#include "../include/libdll/dll.hpp"
#include "./exceptions.hpp"
#include <unordered_map>
#include <forward_list>
#include <functional>
#include <string>
#include <memory>
#include <cmath>
#include <deque>
namespace cov_basic {
	const std::string version="2.1.4.6";
	using number=long double;
	using boolean=bool;
	using string=std::string;
	using array=std::deque<cov::any>;
	using hash_map=std::unordered_map<cov::any,cov::any>;
	class token_base;
	class statement_base;
	struct linker final {
		cov::any data;
		bool operator==(const linker& l) const
		{
			return data.is_same(l.data);
		}
	};
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
		mutable cov::any mRetVal;
		std::deque<std::string> mArgs;
		std::deque<statement_base*> mBody;
		std::shared_ptr<std::unordered_map<string,cov::any>> mData;
	public:
		function()=delete;
		function(const std::deque<std::string>& args,const std::deque<statement_base*>& body):mArgs(args),mBody(body) {}
		~function()=default;
		cov::any call(const array&) const;
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
		cov::any call(array& args) const
		{
			args.push_front(mObj);
			cov::any retval;
			if(mCallable.type()==typeid(function)) {
				retval=mCallable.val<function>(true).call(args);
			}
			else if(mCallable.type()==typeid(native_interface)) {
				retval=mCallable.val<native_interface>(true).call(args);
			}
			else
				throw syntax_error("Call non-function object.");
			args.pop_front();
			return std::move(retval);
		}
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
	class extension final {
		std::unordered_map<string,cov::any> m_data;
	public:
		extension()=default;
		extension(const extension&)=delete;
		extension(const std::unordered_map<string,cov::any>& dat):m_data(dat) {}
		~extension()=default;
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
	class extension_holder final {
		extension* m_ext=nullptr;
		cov::dll m_dll;
	public:
		extension_holder()=delete;
		extension_holder(extension* ptr):m_ext(ptr) {}
		extension_holder(const std::string& path):m_dll(path)
		{
			m_ext=reinterpret_cast<extension*(*)()>(m_dll.get_address("__CBS_EXTENSION__"))();
		}
		~extension_holder()=default;
		cov::any& get_var(const std::string& name)
		{
			if(m_ext==nullptr)
				throw internal_error("Use of nullptr of extension.");
			return m_ext->get_var(name);
		}
	};
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
	inline void copy_no_return(cov::any& val)
	{
		val.clone();
		val.detach();
	}
	inline cov::any copy(cov::any val)
	{
		val.clone();
		val.detach();
		return val;
	}
}
namespace cov {
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
}
namespace std {
	template<>std::string to_string<char>(const char& c)
	{
		return std::move(std::string(1,c));
	}
}
