#pragma once
/*
* Covariant Script Programming Language
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
* Copyright (C) 2018 Michael Lee(李登淳)
* Email: mikecovlee@163.com
* Github: https://github.com/mikecovlee
* Website: http://covscript.org
*
* Namespaces:
* cs: Main Namespace
* cs_impl: Implement Namespace
*/
// Mozart
#include <covscript/mozart/static_stack.hpp>
#include <covscript/mozart/tree.hpp>
// LibDLL
#include <covscript/libdll/dll.hpp>
// Sparsepp
#include <covscript/sparsepp/spp.h>
// STL
#include <forward_list>
#include <functional>
#include <typeindex>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <istream>
#include <ostream>
#include <utility>
#include <cctype>
#include <string>
#include <vector>
#include <memory>
#include <cmath>
#include <deque>
#include <list>
#include <map>
// CovScript Headers
#include <covscript/exceptions.hpp>
#include <covscript/typedef.hpp>
#include <covscript/version.hpp>
#include <covscript/types.hpp>
#include <covscript/any.hpp>

namespace cs {
// Process Context
	struct process_context final {
		// Version
		const std::string version = COVSCRIPT_VERSION_STR;
		const number std_version = COVSCRIPT_STD_VERSION;
// Output Precision
		int output_precision = 8;
// Import Path
		std::string import_path = ".";
// Exception Handling
		static void cs_defalt_exception_handler(const lang_error &e)
		{
			throw e;
		}

		static void std_defalt_exception_handler(const std::exception &e)
		{
			throw forward_exception(e.what());
		}

		std_exception_handler std_eh_callback=&std_defalt_exception_handler;
		cs_exception_handler cs_eh_callback=&cs_defalt_exception_handler;

		static void init_extensions();

		process_context()
		{
			init_extensions();
		}
	} this_process;
	process_context* current_process=&this_process;
// Path seperator and delimiter
#ifdef COVSCRIPT_PLATFORM_WIN32
	constexpr char path_separator = '\\';
	constexpr char path_delimiter = ';';
#else
	constexpr char path_separator = '/';
	constexpr char path_delimiter = ':';
#endif

// Context
	class context_type final {
	public:
		compiler_t compiler = nullptr;
		instance_t instance = nullptr;
		std::deque<string> file_buff;
		string file_path = "<Unknown>";
		string package_name;
		var cmd_args;

		context_type() = default;

		context_type(const context_type &) = default;

		~context_type() = default;
	};

// Callable and Function
	class callable final {
	public:
		using function_type=std::function<var(vector &)>;
		enum class types {
			normal, constant, member_fn
		};
	private:
		function_type mFunc;
		types mType = types::normal;
	public:
		callable() = delete;

		callable(const callable &) = default;

		explicit callable(function_type func, types type=types::normal) : mFunc(std::move(func)), mType(type) {}

		bool is_constant() const
		{
			return mType == types::constant;
		}

		bool is_member_fn() const
		{
			return mType == types::member_fn;
		}

		var call(vector &args) const
		{
			return mFunc(args);
		}
	};

	class function final {
		context_t mContext;
		std::vector<std::string> mArgs;
		std::deque<statement_base *> mBody;
	public:
		function() = delete;

		function(const function &) = default;

		function(context_t c, std::vector<std::string> args, std::deque<statement_base *> body)
			: mContext(std::move(
			               std::move(c))), mArgs(std::move(args)), mBody(std::move(body)) {}

		~function() = default;

		var call(vector &) const;

		var operator()(vector &args) const
		{
			return call(args);
		}

		void add_this()
		{
			std::vector<std::string> args{"this"};
			args.reserve(mArgs.size());
			for (auto &name:mArgs) {
				if (name != "this")
					args.push_back(name);
				else
					throw runtime_error("Overwrite the default argument \"this\".");
			}
			std::swap(mArgs, args);
		}
	};

	struct object_method final {
		var object;
		var callable;
		bool is_constant = false;

		object_method() = delete;

		object_method(var obj, var func, bool constant = false) : object(std::move(obj)), callable(std::move(func)),
			is_constant(constant) {}

		~object_method() = default;
	};

// Copy
	void copy_no_return(var &val)
	{
		if (!val.is_rvalue()) {
			val.clone();
			val.detach();
		}
		else
			val.mark_as_rvalue(false);
	}

	var copy(var val)
	{
		if (!val.is_rvalue()) {
			val.clone();
			val.detach();
		}
		else
			val.mark_as_rvalue(false);
		return val;
	}

// Move Semantics
	var lvalue(const var &val)
	{
		val.mark_as_rvalue(false);
		return val;
	}

	var rvalue(const var &val)
	{
		val.mark_as_rvalue(true);
		return val;
	}

	var try_move(const var &val)
	{
		val.try_move();
		return val;
	}

// Invoke
	template<typename... ArgsT>
	var invoke(const var &func, ArgsT &&... _args)
	{
		if (func.type() == typeid(callable)) {
			vector args{std::forward<ArgsT>(_args)...};
			return func.const_val<callable>().call(args);
		}
		else if (func.type() == typeid(object_method)) {
			const auto &om = func.const_val<object_method>();
			vector args{om.object, std::forward<ArgsT>(_args)...};
			return om.callable.const_val<callable>().call(args);
		}
		else
			throw runtime_error("Invoke non-callable object.");
	}

// Type and struct
	struct pointer final {
		var data;

		pointer() = default;

		explicit pointer(var v) : data(std::move(v)) {}

		bool operator==(const pointer &ptr) const
		{
			return data.is_same(ptr.data);
		}
	};

	static const pointer null_pointer = {};

	struct type_id final {
		std::type_index type_idx;
		std::size_t type_hash;

		type_id() = delete;

		type_id(const std::type_index &id, std::size_t hash = 0) : type_idx(id), type_hash(hash) {}

		bool compare(const type_id &id) const
		{
			if (&id == this)
				return true;
			if (type_hash != 0)
				return type_hash == id.type_hash;
			else
				return type_idx == id.type_idx;
		}

		bool operator==(const type_id &id) const
		{
			return compare(id);
		}

		bool operator!=(const type_id &id) const
		{
			return !compare(id);
		}
	};

	struct type final {
		std::function<var()> constructor;
		namespace_t extensions;
		type_id id;

		type() = delete;

		type(std::function<var()> c, const type_id &i) : constructor(std::move(c)), id(i) {}

		type(std::function<var()> c, const type_id &i, namespace_t ext) : constructor(std::move(c)), id(i),
			extensions(std::move(std::move(ext))) {}

		var &get_var(const std::string &) const;
	};

	class structure final {
		bool m_shadow = false;
		std::string m_name;
		domain_t m_data;
		type_id m_id;
	public:
		structure() = delete;

		structure(const type_id &id, const std::string &name, domain_t data) : m_id(id),
			m_name(typeid(structure).name() +
			       name), m_data(std::move(data))
		{
			if (m_data->count("initialize") > 0)
				invoke((*m_data)["initialize"], var::make<structure>(this));
		}

		structure(const structure &s) : m_id(s.m_id), m_name(s.m_name),
			m_data(std::make_shared<map_t<string, var >>())
		{
			if (s.m_data->count("parent") > 0) {
				var &_p = (*s.m_data)["parent"];
				auto &_parent = _p.val<structure>(true);
				var p = copy(_p);
				auto &parent = p.val<structure>(true);
				m_data->emplace("parent", p);
				for (auto &it:*parent.m_data) {
					// Handle overriding
					var &v = (*s.m_data)[it.first];
					if (!(*_parent.m_data)[it.first].is_same(v))
						m_data->emplace(it.first, copy(v));
					else
						m_data->emplace(it.first, it.second);
				}
			}
			for (auto &it:*s.m_data)
				if (m_data->count(it.first) == 0)
					m_data->emplace(it.first, copy(it.second));
			if (m_data->count("duplicate") > 0)
				invoke((*m_data)["duplicate"], var::make<structure>(this), var::make<structure>(&s));
		}

		explicit structure(const structure *s) : m_shadow(true), m_id(s->m_id), m_name(s->m_name),
			m_data(s->m_data) {}

		~structure()
		{
			if (!m_shadow && m_data->count("finalize") > 0)
				invoke((*m_data)["finalize"], var::make<structure>(this));
		}

		bool operator==(const structure &s) const
		{
			if (s.m_id != m_id)
				return false;
			if (!m_shadow && m_data->count("finalize") > 0)
				return invoke((*m_data)["equal"], var::make<structure>(this),
				              var::make<structure>(&s)).const_val<bool>();
			else {
				for (auto &it:*m_data)
					if (it.first != "parent" && (*s.m_data)[it.first] != it.second)
						return false;
				return true;
			}
		}

		const domain_t &get_domain() const
		{
			return m_data;
		}

		const type_id &get_id() const
		{
			return m_id;
		}

		var &get_var(const std::string &name) const
		{
			if (m_data->count(name) > 0)
				return (*m_data)[name];
			else
				throw runtime_error("Struct \"" + m_name + "\" have no member called \"" + name + "\".");
		}
	};

	class struct_builder final {
		static std::size_t mCount;
		context_t mContext;
		type_id mTypeId;
		std::string mName;
		cov::tree<token_base *> mParent;
		std::deque<statement_base *> mMethod;
	public:
		struct_builder() = delete;

		struct_builder(context_t c, std::string name, cov::tree<token_base *> parent,
		               std::deque<statement_base *> method) : mContext(std::move(std::move(c))),
			mTypeId(typeid(structure), ++mCount),
			mName(std::move(name)),
			mParent(std::move(parent)),
			mMethod(std::move(method)) {}

		struct_builder(const struct_builder &) = default;

		~struct_builder() = default;

		static void reset_counter()
		{
			mCount = 0;
		}

		const type_id &get_id() const
		{
			return mTypeId;
		}

		var operator()();
	};

	std::size_t struct_builder::mCount = 0;

// Internal Garbage Collection
	template<typename T>
	class garbage_collector final {
		std::forward_list<T *> table_new;
		std::forward_list<T *> table_delete;
	public:
		garbage_collector() = default;

		garbage_collector(const garbage_collector &) = delete;

		~garbage_collector()
		{
			for (auto &ptr:table_delete)
				table_new.remove(ptr);
			for (auto &ptr:table_new)
				delete ptr;
		}

		void add(void *ptr)
		{
			table_new.push_front(static_cast<T *>(ptr));
		}

		void remove(void *ptr)
		{
			table_delete.push_front(static_cast<T *>(ptr));
		}
	};

// Namespace and extensions
	class name_space {
		domain_t m_data;
	public:
		name_space() : m_data(std::make_shared<map_t<string, var >>()) {}

		name_space(const name_space &) = delete;

		explicit name_space(domain_t dat) : m_data(std::move(dat)) {}

		virtual ~name_space() = default;

		name_space &add_var(const std::string &name, const var &var)
		{
			if (m_data->count(name) > 0)
				(*m_data)[name] = var;
			else
				m_data->emplace(name, var);
			return *this;
		}

		var &get_var(const std::string &name)
		{
			if (m_data->count(name) > 0)
				return (*m_data)[name];
			else
				throw runtime_error("Use of undefined variable \"" + name + "\".");
		}

		const var &get_var(const std::string &name) const
		{
			if (m_data->count(name) > 0)
				return (*m_data)[name];
			else
				throw runtime_error("Use of undefined variable \"" + name + "\".");
		}

		domain_t get_domain() const
		{
			return m_data;
		}
	};

	namespace dll_resources {
		const char* dll_main_entrance = "__CS_EXTENSION_MAIN__";
		typedef void(*dll_main_entrance_t)(name_space*, process_context*);
	}

	class extension final:public name_space {
		cov::dll m_dll;
	public:
		extension()=delete;
		extension(const extension&)=delete;
		explicit extension(const std::string& path):m_dll(path)
		{
			using namespace dll_resources;
			dll_main_entrance_t dll_main=reinterpret_cast<dll_main_entrance_t>(m_dll.get_address(dll_main_entrance));
			if(dll_main!=nullptr) {
				dll_main(this, current_process);
			}
			else
				throw lang_error("Incompatible DLL.");
		}
	};

	var make_namespace(const namespace_t &ns)
	{
		return var::make_protect<namespace_t>(ns);
	}

	template<typename T, typename...ArgsT>
	namespace_t make_shared_namespace(ArgsT&&...args)
	{
		return std::make_shared<T>(std::forward<ArgsT>(args)...);
	}

	var &type::get_var(const std::string &name) const
		{
			if (extensions.get() != nullptr)
				return extensions->get_var(name);
			else
				throw runtime_error("Type does not support the extension");
		}

// Literal format
	number parse_number(const std::string &str)
	{
		int point_count = 0;
		for (auto &ch:str) {
			if (!std::isdigit(ch)) {
				if (ch != '.' || ++point_count > 1)
					throw runtime_error("Wrong literal format.");
			}
		}
		return std::stold(str);
	}
}
