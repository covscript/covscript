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
	const std::string version="2.1.5.4";
#else
#ifndef CBS_MINIMAL
	const std::string version="2.1.5.4 (Static Build)";
#else
	const std::string version="2.1.5.4 (Minimal Build)";
#endif
#endif
	static int output_precision=8;
	using number=long double;
	using boolean=bool;
	using string=std::string;
	using list=std::list<cov::any>;
	using array=std::deque<cov::any>;
	using pair=std::pair<cov::any,cov::any>;
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
	static const linker null_linker=linker();
	class native_interface final {
	public:
		using function_type=std::function<cov::any(array&)>;
	private:
		function_type mFunc;
		bool mConstant=false;
	public:
		native_interface()=delete;
		native_interface(const native_interface&)=default;
		native_interface(const function_type& func,bool constant=false):mFunc(func),mConstant(constant) {}
		bool is_constant() const
		{
			return mConstant;
		}
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
		const cov::any& get_callable() const
		{
			return mCallable;
		}
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
#ifndef CBS_STATIC
		cov::dll m_dll;
#endif
	public:
		extension_holder()=delete;
		extension_holder(extension* ptr):m_ext(ptr) {}
#ifndef CBS_STATIC
		extension_holder(const std::string& path):m_dll(path)
		{
			m_ext=reinterpret_cast<extension*(*)()>(m_dll.get_address("__CBS_EXTENSION__"))();
		}
#else
		extension_holder(const std::string&)
		{
			throw internal_error("Can not load extension because covbasic is static version.");
		}
#endif
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
	class domain_manager final {
	public:
		using domain_t=std::shared_ptr<std::unordered_map<string,cov::any>>;
		std::unordered_map<string,std::function<cov::any()>> m_type;
		std::unordered_map<string,std::size_t> m_hash;
		std::deque<domain_t> m_data;
		std::deque<domain_t> m_this;
	public:
		domain_manager()
		{
			m_data.emplace_front(std::make_shared<std::unordered_map<string,cov::any>>());
		}
		domain_manager(const domain_manager&)=delete;
		~domain_manager()=default;
		void add_type(const std::string& name,const std::function<cov::any()>& func)
		{
			if(m_type.count(name)>0)
				throw syntax_error("Redefinition of type \""+name+"\".");
			else {
				m_type.emplace(name,func);
				m_hash.emplace(name,cov::hash<std::string>(typeid(structure).name()+name));
			}
		}
		void add_type(const std::string& name,const std::function<cov::any()>& func,std::size_t hash)
		{
			if(m_type.count(name)>0)
				throw syntax_error("Redefinition of type \""+name+"\".");
			else {
				m_type.emplace(name,func);
				m_hash.emplace(name,hash);
			}
		}
		void add_domain()
		{
			m_data.emplace_front(std::make_shared<std::unordered_map<string,cov::any>>());
		}
		void add_domain(const domain_t& dat)
		{
			m_data.emplace_front(dat);
		}
		void add_this(const domain_t& dat)
		{
			m_this.emplace_front(dat);
		}
		domain_t& get_domain()
		{
			return m_data.front();
		}
		void remove_domain()
		{
			if(m_data.size()>1)
				m_data.pop_front();
		}
		void remove_this()
		{
			if(m_this.size()>1)
				m_this.pop_front();
		}
		bool type_exsist(const string& name)
		{
			if(m_type.count(name)>0)
				return true;
			else
				return false;
		}
		bool var_exsist(const string& name)
		{
			for(auto& domain:m_data)
				if(domain->count(name)>0)
					return true;
			return false;
		}
		bool var_exsist_current(const string& name)
		{
			if(m_data.front()->count(name)>0)
				return true;
			return false;
		}
		bool var_exsist_global(const string& name)
		{
			if(m_data.back()->count(name)>0)
				return true;
			return false;
		}
		bool var_exsist_this(const string& name)
		{
			if(m_this.front()->count(name)>0)
				return true;
			return false;
		}
		std::size_t get_type_hash(const std::string& type)
		{
			if(type_exsist(type))
				return m_hash.at(type);
			else
				throw syntax_error("Get hash of undefined type \""+type+"\".");
		}
		cov::any get_var_type(const string& type)
		{
			if(type_exsist(type))
				return std::move(m_type.at(type)());
			else
				throw syntax_error("Use of undefined type \""+type+"\".");
		}
		cov::any& get_var(const string& name)
		{
			for(auto& domain:m_data)
				if(domain->count(name)>0)
					return domain->at(name);
			throw syntax_error("Use of undefined variable \""+name+"\".");
		}
		cov::any& get_var_current(const string& name)
		{
			if(m_data.front()->count(name)>0)
				return m_data.front()->at(name);
			throw syntax_error("Use of undefined variable \""+name+"\" in current domain.");
		}
		cov::any& get_var_global(const string& name)
		{
			if(m_data.back()->count(name)>0)
				return m_data.back()->at(name);
			throw syntax_error("Use of undefined variable \""+name+"\" in global domain.");
		}
		cov::any& get_var_this(const string& name)
		{
			if(m_this.front()->count(name)>0)
				return m_this.front()->at(name);
			throw syntax_error("Use of undefined variable \""+name+"\" in current object.");
		}
		void add_var(const string& name,const cov::any& var)
		{
			if(var_exsist_current(name))
				get_var(name)=var;
			else
				m_data.front()->emplace(name,var);
		}
		void add_var_global(const string& name,const cov::any& var)
		{
			if(var_exsist_global(name))
				get_var_global(name)=var;
			else
				m_data.back()->emplace(name,var);
		}
	};
	using extension_t=std::shared_ptr<extension_holder>;
	struct runtime_type final {
		domain_manager storage;
		domain_manager constant_storage;
		extension_t char_ext;
		extension_t string_ext;
		extension_t list_ext;
		extension_t array_ext;
		extension_t pair_ext;
		extension_t hash_map_ext;
	};
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
