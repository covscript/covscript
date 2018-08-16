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
// Version
	static const std::string version = COVSCRIPT_VERSION_STR;
	static const number std_version = COVSCRIPT_STD_VERSION;
// Output Precision
	static int output_precision = 8;
	static int *output_precision_ref = &output_precision;
// Import Path
	static std::string import_path = ".";
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
		instance_type *instance = nullptr;
		std::deque<string> file_buff;
		string file_path = "<Unknown>";
		string package_name;

		context_type() = delete;

		context_type(instance_type *iptr) : instance(iptr) {}

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

		callable(const function_type &func, bool constant = false) : mFunc(func), mType(constant ? types::constant
			        : types::normal) {}

		callable(const function_type &func, types type) : mFunc(func), mType(type) {}

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

		function(context_t c, const std::vector<std::string> &args, const std::deque<statement_base *> &body)
			: mContext(
			      c), mArgs(args), mBody(body) {}

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

		object_method(const var &obj, const var &func, bool constant = false) : object(obj), callable(func),
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

		pointer(const var &v) : data(v) {}

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
		extension_t extensions;
		type_id id;

		type() = delete;

		type(const std::function<var()> &c, const type_id &i) : constructor(c), id(i) {}

		type(const std::function<var()> &c, const type_id &i, extension_t ext) : constructor(c), id(i),
			extensions(ext) {}

		var &get_var(const std::string &) const;
	};

// Maybe here the name is unnecessary
	class structure final {
		bool m_shadow = false;
		std::string m_name;
		domain_t m_data;
		type_id m_id;
	public:
		structure() = delete;

		structure(const type_id &id, const std::string &name, const domain_t &data) : m_id(id),
			m_name(typeid(structure).name() +
			       name), m_data(data)
		{
			if (m_data->count("initialize") > 0)
				invoke((*m_data)["initialize"], var::make<structure>(this));
		}

		structure(const structure &s) : m_id(s.m_id), m_name(s.m_name),
			m_data(std::make_shared<map_t<string, var >>())
		{
			if (s.m_data->count("parent") > 0) {
				var &_p = (*s.m_data)["parent"];
				structure &_parent = _p.val<structure>(true);
				var p = copy(_p);
				structure &parent = p.val<structure>(true);
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

		struct_builder(context_t c, const std::string &name, const cov::tree<token_base *> &parent,
		               const std::deque<statement_base *> &method) : mContext(c),
			mTypeId(typeid(structure), ++mCount),
			mName(name),
			mParent(parent),
			mMethod(method) {}

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

// Exception Handler
	struct exception_handler final {
		static std_exception_handler std_eh_callback;
		static cs_exception_handler cs_eh_callback;

		static void cs_defalt_exception_handler(const lang_error &e)
		{
			throw e;
		}

		static void std_defalt_exception_handler(const std::exception &e)
		{
			throw forward_exception(e.what());
		}
	};

	cs_exception_handler exception_handler::cs_eh_callback = exception_handler::cs_defalt_exception_handler;
	std_exception_handler exception_handler::std_eh_callback = exception_handler::std_defalt_exception_handler;

// Namespace and extensions
	class name_space final {
		domain_t m_data;
	public:
		name_space() : m_data(std::make_shared<map_t<string, var >>()) {}

		name_space(const name_space &) = delete;

		explicit name_space(const domain_t &dat) : m_data(dat) {}

		~name_space() = default;

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

		domain_t get_domain() const
		{
			return m_data;
		}
	};

	class name_space_holder final {
		bool m_local;
		name_space *m_ns = nullptr;
		cov::dll m_dll;
	public:
		name_space_holder() = delete;

		name_space_holder(const name_space_holder &) = delete;

		name_space_holder(const domain_t &dat) : m_local(true), m_ns(new name_space(dat)) {}

		name_space_holder(name_space *ptr) : m_local(false), m_ns(ptr) {}

		name_space_holder(const std::string &path) : m_local(false), m_dll(path)
		{
			m_ns = reinterpret_cast<extension_entrance_t>(m_dll.get_address("__CS_EXTENSION__"))(output_precision_ref,
			        exception_handler::cs_eh_callback,
			        exception_handler::std_eh_callback);
		}

		~name_space_holder()
		{
			if (m_local)
				delete m_ns;
		}

		var &get_var(const std::string &name)
		{
			if (m_ns == nullptr)
				throw internal_error("Use of nullptr of extension.");
			return m_ns->get_var(name);
		}

		domain_t get_domain() const
		{
			if (m_ns == nullptr)
				throw internal_error("Use of nullptr of extension.");
			return m_ns->get_domain();
		}
	};

	var make_namespace(const name_space_t &ns)
	{
		return var::make_protect<name_space_t>(ns);
	}

	name_space_t make_shared_namespace(name_space &ns)
	{
		return std::make_shared<name_space_holder>(&ns);
	}

// Implement
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

	var parse_value(const std::string &str)
	{
		if (str == "true")
			return true;
		if (str == "false")
			return false;
		try {
			return parse_number(str);
		}
		catch (...) {
			return str;
		}
		return str;
	}
}
namespace cs_impl {
// Detach
	template<>
	void detach<cs::pair>(cs::pair &val)
	{
		cs::copy_no_return(val.first);
		cs::copy_no_return(val.second);
	}

	template<>
	void detach<cs::list>(cs::list &val)
	{
		for (auto &it:val)
			cs::copy_no_return(it);
	}

	template<>
	void detach<cs::array>(cs::array &val)
	{
		for (auto &it:val)
			cs::copy_no_return(it);
	}

	template<>
	void detach<cs::hash_map>(cs::hash_map &val)
	{
		for (auto &it:val)
			cs::copy_no_return(it.second);
	}

// To String
	template<>
	std::string to_string<cs::number>(const cs::number &val)
	{
		std::stringstream ss;
		std::string str;
		ss << std::setprecision(*cs::output_precision_ref) << val;
		ss >> str;
		return std::move(str);
	}

	template<>
	std::string to_string<char>(const char &c)
	{
		return std::string(1, c);
	}

	template<>
	std::string to_string<cs::type_id>(const cs::type_id &id)
	{
		if (id.type_hash != 0)
			return cxx_demangle(id.type_idx.name()) + "_" + to_string(id.type_hash);
		else
			return cxx_demangle(id.type_idx.name());
	}

// To Integer
	template<>
	long to_integer<std::string>(const std::string &str)
	{
		for (auto &ch:str) {
			if (!std::isdigit(ch))
				throw cs::runtime_error("Wrong literal format.");
		}
		return std::stol(str);
	}

// Type name
	template<>
	constexpr const char *get_name_of_type<cs::context_t>()
	{
		return "cs::context";
	}

	template<>
	constexpr const char *get_name_of_type<cs::var>()
	{
		return "cs::var";
	}

	template<>
	constexpr const char *get_name_of_type<cs::number>()
	{
		return "cs::number";
	}

	template<>
	constexpr const char *get_name_of_type<cs::boolean>()
	{
		return "cs::boolean";
	}

	template<>
	constexpr const char *get_name_of_type<cs::pointer>()
	{
		return "cs::pointer";
	}

	template<>
	constexpr const char *get_name_of_type<char>()
	{
		return "cs::char";
	}

	template<>
	constexpr const char *get_name_of_type<cs::string>()
	{
		return "cs::string";
	}

	template<>
	constexpr const char *get_name_of_type<cs::list>()
	{
		return "cs::list";
	}

	template<>
	constexpr const char *get_name_of_type<cs::array>()
	{
		return "cs::array";
	}

	template<>
	constexpr const char *get_name_of_type<cs::pair>()
	{
		return "cs::pair";
	}

	template<>
	constexpr const char *get_name_of_type<cs::hash_map>()
	{
		return "cs::hash_map";
	}

	template<>
	constexpr const char *get_name_of_type<cs::type>()
	{
		return "cs::type";
	}

	template<>
	constexpr const char *get_name_of_type<cs::name_space_t>()
	{
		return "cs::namespace";
	}

	template<>
	constexpr const char *get_name_of_type<cs::callable>()
	{
		return "cs::function";
	}

	template<>
	constexpr const char *get_name_of_type<cs::structure>()
	{
		return "cs::structure";
	}

	template<>
	constexpr const char *get_name_of_type<cs::lang_error>()
	{
		return "cs::exception";
	}

	template<>
	constexpr const char *get_name_of_type<cs::istream>()
	{
		return "cs::istream";
	}

	template<>
	constexpr const char *get_name_of_type<cs::ostream>()
	{
		return "cs::ostream";
	}
}
