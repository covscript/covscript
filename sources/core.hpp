#pragma once
#include "../include/mozart/random.hpp"
#include "../include/mozart/timer.hpp"
#include "../include/mozart/tree.hpp"
#include "../include/mozart/any.hpp"
#include "../include/libdll/dll.hpp"
#include <unordered_map>
#include <forward_list>
#include <functional>
#include <exception>
#include <stdexcept>
#include <string>
#include <memory>
#include <deque>
namespace cov_basic {
	class syntax_error final:public std::exception {
		std::string mWhat="Covariant Basic Syntax Error";
	public:
		syntax_error()=default;
	syntax_error(const std::string& str) noexcept:
		mWhat("Covariant Basic Syntax Error:"+str) {}
	syntax_error(std::size_t line,const std::string& str) noexcept:
		mWhat("In line "+std::to_string(line)+":Covariant Basic Syntax Error:\n"+str) {}
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
	lang_error(std::size_t line,const std::string& str) noexcept:
		mWhat("In line "+std::to_string(line)+":Covariant Basic Language Error:\n"+str) {}
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
	class structure final {
		std::shared_ptr<std::unordered_map<string,cov::any>> m_data;
	public:
		structure()=delete;
		structure(const std::shared_ptr<std::unordered_map<string,cov::any>>& data):m_data(data) {}
		~structure()=default;
		std::shared_ptr<std::unordered_map<string,cov::any>>& get_domain()
		{
			return m_data;
		}
		cov::any get_var(const std::string& name) const
		{
			if(m_data->count(name)>0)
				return m_data->at(name);
			else
				throw syntax_error("Struct have no member called \""+name+"\".");
		}
	};
	class struct_builder final {
		std::deque<statement_base*> mMethod;
	public:
		struct_builder()=delete;
		struct_builder(const std::deque<statement_base*>& method):mMethod(method) {}
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
	class extension_manager final {
		std::deque<cov::dll> m_dll;
		std::unordered_map<string,extension*> m_data;
	public:
		extension_manager()=default;
		extension_manager(const extension_manager&)=delete;
		~extension_manager()=default;
		void add_extension(const std::string& path)
		{
			m_dll.emplace_front(path);
			m_data.emplace(path,reinterpret_cast<extension*(*)()>(m_dll.front().get_address("__CBS_EXTENSION__"))());
		}
		void add_extension(const std::string& name,extension* ptr)
		{
			m_data.emplace(name,ptr);
		}
		cov::any& get_var(const std::string& ext,const std::string& name)
		{
			if(m_data.count(ext)>0)
				return m_data.at(ext)->get_var(name);
			else
				throw syntax_error("Uknow extension \""+ext+"\".");
		}
	};
	static extension_manager extensions;
}
