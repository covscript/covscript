#pragma once
/*
* Covariant Script Programming Language
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
* Copyright (C) 2017 Michael Lee(李登淳)
* Email: mikecovlee@163.com
* Github: https://github.com/mikecovlee
* Website: http://covariant.cn/cs
*
* Namespaces:
* cs: Main Namespace
* cs_impl: Implement Namespace
*/
// Mozart
#include "../include/mozart/static_stack.hpp"
#include "../include/mozart/random.hpp"
#include "../include/mozart/timer.hpp"
#include "../include/mozart/tree.hpp"
// LibDLL
#include "../include/libdll/dll.hpp"
// STL
#include <unordered_map>
#include <forward_list>
#include <functional>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <utility>
#include <string>
#include <memory>
#include <cmath>
#include <deque>
#include <list>
#include <map>
// CovScript Headers
#include "./exceptions.hpp"
#include "./any.hpp"
#include "./typedef.hpp"

namespace cs {
// Version
	const std::string version="1.0.3";
// Output Precision
	static int output_precision=8;
// Callable and Function
	class callable final {
	public:
		using function_type=std::function<var(array&)>;
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
		var call(array& args) const
		{
			return mFunc(args);
		}
	};
	class function final {
		friend class statement_return;
		mutable var mRetVal;
		std::deque<std::string> mArgs;
		std::deque<statement_base*> mBody;
		std::shared_ptr<std::unordered_map<string,var>> mData;
	public:
		function()=delete;
		function(const std::deque<std::string>& args,const std::deque<statement_base*>& body):mArgs(args),mBody(body) {}
		~function()=default;
		var call(array&) const;
		void set_data(const std::shared_ptr<std::unordered_map<string,var>>& data)
		{
			mData=data;
		}
	};
	class object_method final {
		var mObj;
		var mCallable;
	public:
		object_method()=delete;
		object_method(const var& obj,const var& callable):mObj(obj),mCallable(callable) {}
		~object_method()=default;
		const var& get_callable() const
		{
			return mCallable;
		}
		var operator()(array& args) const
		{
			args.push_front(mObj);
			var retval=mCallable.const_val<callable>().call(args);
			args.pop_front();
			return retval;
		}
	};
// Type and struct
	struct pointer final {
		var data;
		pointer()=default;
		pointer(const var& v):data(v) {}
		bool operator==(const pointer& ptr) const
		{
			return data.is_same(ptr.data);
		}
	};
	static const pointer null_pointer= {};
	struct type final {
		std::function<var()> constructor;
		std::size_t id;
		extension_t extensions;
		type()=delete;
		type(const std::function<var()>& c,std::size_t i):constructor(c),id(i) {}
		type(const std::function<var()>& c,std::size_t i,extension_t ext):constructor(c),id(i),extensions(ext) {}
		var& get_var(const std::string&) const;
	};
	class structure final {
		std::size_t m_hash;
		std::string m_name;
		std::shared_ptr<std::unordered_map<string,var>> m_data;
	public:
		structure()=delete;
		structure(std::size_t hash,const std::string& name,const std::shared_ptr<std::unordered_map<string,var>>& data):m_hash(hash),m_name(typeid(structure).name()+name),m_data(data) {}
		structure(const structure& s):m_hash(s.m_hash),m_name(s.m_name),m_data(std::make_shared<std::unordered_map<string,var>>(*s.m_data))
		{
			for(auto& it:*m_data) {
				it.second.clone();
				if(it.second.type()==typeid(function))
					it.second.val<function>(true).set_data(m_data);
			}
		}
		~structure()=default;
		std::shared_ptr<std::unordered_map<string,var>>& get_domain()
		{
			return m_data;
		}
		std::size_t get_hash() const
		{
			return m_hash;
		}
		var& get_var(const std::string& name) const
		{
			if(m_data->count(name)>0)
				return (*m_data)[name];
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
		var operator()();
	};
	std::size_t struct_builder::mCount=0;
// Internal Garbage Collection
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
// Namespace and extensions
	class name_space final {
		std::unordered_map<string,var> m_data;
	public:
		name_space()=default;
		name_space(const name_space&)=delete;
		name_space(const std::unordered_map<string,var>& dat):m_data(dat) {}
		~name_space()=default;
		void add_var(const std::string& name,const var& var)
		{
			if(m_data.count(name)>0)
				m_data[name]=var;
			else
				m_data.emplace(name,var);
		}
		var& get_var(const std::string& name)
		{
			if(m_data.count(name)>0)
				return m_data[name];
			else
				throw syntax_error("Use of undefined variable \""+name+"\" in extension.");
		}
	};
	class name_space_holder final {
		name_space* m_ns=nullptr;
		cov::dll m_dll;
	public:
		name_space_holder()=delete;
		name_space_holder(name_space* ptr):m_ns(ptr) {}
		name_space_holder(const std::string& path):m_dll(path)
		{
			m_ns=reinterpret_cast<name_space*(*)()>(m_dll.get_address("__CS_EXTENSION__"))();
		}
		~name_space_holder()=default;
		var& get_var(const std::string& name)
		{
			if(m_ns==nullptr)
				throw internal_error("Use of nullptr of extension.");
			return m_ns->get_var(name);
		}
	};
// Var definition
	struct define_var_profile {
		std::string id;
		cov::tree<token_base*> expr;
	};
	void parse_define_var(cov::tree<token_base*>&,define_var_profile&);
// Implement
	var& type::get_var(const std::string& name) const
	{
		if(extensions.get()!=nullptr)
			return extensions->get_var(name);
		else
			throw syntax_error("Type does not support the extension");
	}
// literal format
	var parse_value(const std::string& str)
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
// Copy
	void copy_no_return(var& val)
	{
		val.clone();
		val.detach();
	}
	var copy(var val)
	{
		val.clone();
		val.detach();
		return val;
	}
}
namespace cs_impl {
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
