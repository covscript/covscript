/*
* Covariant Script Sqlite Extension
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
*/
#include "../sqlite/sqlite.hpp"
#include "../headers/extension.hpp"
#include "../headers/cni.hpp"
#include "./string.hpp"

static cs::extension sqlite_ext;
static cs::extension sqlite_stmt_ext;
static cs::extension_t sqlite_ext_shared = cs::make_shared_extension(sqlite_ext);
static cs::extension_t sqlite_stmt_ext_shared = cs::make_shared_extension(sqlite_stmt_ext);
namespace cs_impl {
	template<>
	cs::extension_t &get_ext<cs_impl::sqlite>()
	{
		return sqlite_ext_shared;
	}

	template<>
	cs::extension_t &get_ext<cs_impl::sqlite::statement>()
	{
		return sqlite_stmt_ext_shared;
	}

	template<>
	constexpr const char *get_name_of_type<cs_impl::sqlite>()
	{
		return "cs::sqlite";
	}

	template<>
	constexpr const char *get_name_of_type<cs_impl::sqlite::data_type>()
	{
		return "cs::sqlite::data_type";
	}

	template<>
	constexpr const char *get_name_of_type<cs_impl::sqlite::statement>()
	{
		return "cs::sqlite::statement";
	}
}
namespace sqlite_cs_ext {
	using namespace cs;
	using cs_impl::sqlite;

// Sqlite Statement
	bool done(const sqlite::statement &stmt)
	{
		return stmt.done();
	}

	void reset(sqlite::statement &stmt)
	{
		stmt.reset();
	}

	void exec(sqlite::statement &stmt)
	{
		stmt.step();
	}

	number column_count(const sqlite::statement &stmt)
	{
		return stmt.column_count();
	}

	sqlite::data_type column_type(const sqlite::statement &stmt, number index)
	{
		return stmt.column_type(index);
	}

	string column_name(const sqlite::statement &stmt, number index)
	{
		return stmt.column_name(index);
	}

	string column_decltype(const sqlite::statement &stmt, number index)
	{
		return stmt.column_decltype(index);
	}

	number column_integer(const sqlite::statement &stmt, number index)
	{
		return stmt.column_integer(index);
	}

	number column_real(const sqlite::statement &stmt, number index)
	{
		return stmt.column_real(index);
	}

	string column_text(const sqlite::statement &stmt, number index)
	{
		return stmt.column_text(index);
	}

	number bind_param_count(const sqlite::statement &stmt)
	{
		return stmt.bind_param_count();
	}

	void bind_integer(sqlite::statement &stmt, number index, number data)
	{
		stmt.bind_integer(index, data);
	}

	void bind_real(sqlite::statement &stmt, number index, number data)
	{
		stmt.bind_real(index, data);
	}

	void bind_text(sqlite::statement &stmt, number index, const string &data)
	{
		stmt.bind_text(index, data);
	}

	void clear_bindings(sqlite::statement &stmt)
	{
		stmt.clear_bindings();
	}

// Sqlite
	sqlite open(const string &path)
	{
		return sqlite(path);
	}

	sqlite::statement prepare(sqlite &db, const string &sql)
	{
		return db.prepare(sql);
	}

	void init()
	{
		string_cs_ext::init();
		sqlite_ext.add_var("statement", var::make_protect<extension_t>(sqlite_stmt_ext_shared));
		sqlite_ext.add_var("integer", var::make_constant<sqlite::data_type>(sqlite::data_type::integer));
		sqlite_ext.add_var("real", var::make_constant<sqlite::data_type>(sqlite::data_type::real));
		sqlite_ext.add_var("text", var::make_constant<sqlite::data_type>(sqlite::data_type::text));
		sqlite_ext.add_var("open", var::make_protect<callable>(cni(open)));
		sqlite_ext.add_var("prepare", var::make_protect<callable>(cni(prepare)));
		sqlite_stmt_ext.add_var("done", var::make_protect<callable>(cni(done)));
		sqlite_stmt_ext.add_var("reset", var::make_protect<callable>(cni(reset)));
		sqlite_stmt_ext.add_var("exec", var::make_protect<callable>(cni(exec)));
		sqlite_stmt_ext.add_var("column_count", var::make_protect<callable>(cni(column_count)));
		sqlite_stmt_ext.add_var("column_type", var::make_protect<callable>(cni(column_type)));
		sqlite_stmt_ext.add_var("column_name", var::make_protect<callable>(cni(column_name)));
		sqlite_stmt_ext.add_var("column_decltype", var::make_protect<callable>(cni(column_decltype)));
		sqlite_stmt_ext.add_var("column_integer", var::make_protect<callable>(cni(column_integer)));
		sqlite_stmt_ext.add_var("column_real", var::make_protect<callable>(cni(column_real)));
		sqlite_stmt_ext.add_var("column_text", var::make_protect<callable>(cni(column_text)));
		sqlite_stmt_ext.add_var("bind_param_count", var::make_protect<callable>(cni(bind_param_count)));
		sqlite_stmt_ext.add_var("bind_integer", var::make_protect<callable>(cni(bind_integer)));
		sqlite_stmt_ext.add_var("bind_real", var::make_protect<callable>(cni(bind_real)));
		sqlite_stmt_ext.add_var("bind_text", var::make_protect<callable>(cni(bind_text)));
		sqlite_stmt_ext.add_var("clear_bindings", var::make_protect<callable>(cni(clear_bindings)));
	}
}

cs::extension *cs_extension()
{
	sqlite_cs_ext::init();
	return &sqlite_ext;
}