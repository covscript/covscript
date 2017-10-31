#pragma once
/*
* Covariant Script Programming Language
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU Affero General Public License as published
* by the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU Affero General Public License for more details.
*
* You should have received a copy of the GNU Affero General Public License
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
#include "../../include/mozart/static_stack.hpp"
#include "../../include/mozart/random.hpp"
#include "../../include/mozart/timer.hpp"
#include "../../include/mozart/tree.hpp"
// LibDLL
#include "../../include/libdll/dll.hpp"
// STL
#include <unordered_map>
#include <unordered_set>
#include <forward_list>
#include <functional>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <istream>
#include <ostream>
#include <utility>
#include <cctype>
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
	static const char *const version = "1.1.4";
	static const number std_version = 20171101;
// Output Precision
	static int output_precision = 8;
// Import Path
	static const char *import_path = ".";

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
		using function_type=std::function<var(array &)>;
		enum class types {
			normal, constant, member_fn
		};
	private:
		function_type mFunc;
		types mType = types::normal;
	public:
		callable() = delete;

		callable(const callable &) = default;

		callable(const function_type &func, bool constant = false) : mFunc(func), mType(constant ? types::constant : types::normal) {}

		callable(const function_type &func, types type) : mFunc(func), mType(type) {}

		bool is_constant() const
		{
			return mType == types::constant;
		}

		bool is_member_fn() const
		{
			return mType == types::member_fn;
		}

		var call(array &args) const
		{
			return mFunc(args);
		}
	};

	class function final {
		context_t mContext;
		std::deque<std::string> mArgs;
		std::deque<statement_base *> mBody;
	public:
		function() = delete;

		function(const function &) = default;

		function(context_t c, const std::deque<std::string> &args, const std::deque<statement_base *> &body) : mContext(c), mArgs(args), mBody(body) {}

		~function() = default;

		var call(array &) const;

		var operator()(array &args) const
		{
			return call(args);
		}

		void add_this()
		{
			mArgs.push_front("this");
		}
	};

	class object_method final {
		var mObj;
		var mCallable;
	public:
		object_method() = delete;

		object_method(const var &obj, const var &callable) : mObj(obj), mCallable(callable) {}

		~object_method() = default;

		const var &get_callable() const
		{
			return mCallable;
		}

		var operator()(array &args) const
		{
			args.push_front(mObj);
			var retval = mCallable.const_val<callable>().call(args);
			args.pop_front();
			return retval;
		}
	};

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

	struct type final {
		std::function<var()> constructor;
		std::size_t id;
		extension_t extensions;

		type() = delete;

		type(const std::function<var()> &c, std::size_t i) : constructor(c), id(i) {}

		type(const std::function<var()> &c, std::size_t i, extension_t ext) : constructor(c), id(i), extensions(ext) {}

		var &get_var(const std::string &) const;
	};

	class structure final {
		std::size_t m_hash;
		std::string m_name;
		std::shared_ptr<std::unordered_map<string, var>> m_data;
	public:
		structure() = delete;

		structure(std::size_t hash, const std::string &name, const std::shared_ptr<std::unordered_map<string, var>> &data) : m_hash(hash), m_name(typeid(structure).name() + name), m_data(data) {}

		structure(const structure &s) : m_hash(s.m_hash), m_name(s.m_name), m_data(std::make_shared<std::unordered_map<string, var>>(*s.m_data))
		{
			for (auto &it:*m_data)
				it.second.clone();
		}

		~structure() = default;

		std::shared_ptr<std::unordered_map<string, var>> &get_domain()
		{
			return m_data;
		}

		std::size_t get_hash() const
		{
			return m_hash;
		}

		var &get_var(const std::string &name) const
		{
			if (m_data->count(name) > 0)
				return (*m_data)[name];
			else
				throw syntax_error("Struct \"" + m_name + "\" have no member called \"" + name + "\".");
		}
	};

	class struct_builder final {
		static std::size_t mCount;
		context_t mContext;
		std::size_t mHash;
		std::string mName;
		std::deque<statement_base *> mMethod;
	public:
		struct_builder() = delete;

		struct_builder(context_t c, const std::string &name, const std::deque<statement_base *> &method) : mContext(c), mHash(++mCount), mName(name), mMethod(method) {}

		struct_builder(const struct_builder &) = default;

		~struct_builder() = default;

		std::size_t get_hash() const
		{
			return mHash;
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
	class name_space final {
		domain_t m_data;
	public:
		name_space() : m_data(std::make_shared<std::unordered_map<string, var>>()) {}

		name_space(const name_space &) = delete;

		name_space(const domain_t &dat) : m_data(dat) {}

		~name_space() = default;

		void add_var(const std::string &name, const var &var)
		{
			if (m_data->count(name) > 0)
				(*m_data)[name] = var;
			else
				m_data->emplace(name, var);
		}

		var &get_var(const std::string &name)
		{
			if (m_data->count(name) > 0)
				return (*m_data)[name];
			else
				throw syntax_error("Use of undefined variable \"" + name + "\".");
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
			m_ns = reinterpret_cast<name_space *(*)()>(m_dll.get_address("__CS_EXTENSION__"))();
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
	};

	extension_t make_shared_extension(name_space &ns)
	{
		return std::make_shared<extension_holder>(&ns);
	}

// Implement
	var &type::get_var(const std::string &name) const
	{
		if (extensions.get() != nullptr)
			return extensions->get_var(name);
		else
			throw syntax_error("Type does not support the extension");
	}

// Literal format
	var parse_value(const std::string &str)
	{
		if (str == "true")
			return true;
		if (str == "false")
			return false;
		try {
			return std::stold(str);
		}
		catch (...) {
			return str;
		}
		return str;
	}

// Copy
	void copy_no_return(var &val)
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

	template<>
	std::string to_string<cs::number>(const cs::number &val)
	{
		std::stringstream ss;
		std::string str;
		ss << std::setprecision(cs::output_precision) << val;
		ss >> str;
		return std::move(str);
	}

	template<>
	std::string to_string<char>(const char &c)
	{
		return std::move(std::string(1, c));
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
