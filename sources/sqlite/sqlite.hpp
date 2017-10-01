#pragma once
/*
* Covariant Script Sqlite
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
#include "../headers/exceptions.hpp"
#include <sqlite3.h>
#include <utility>
#include <string>
#include <memory>

namespace cs_impl {
	class sqlite final {
		class db_holder final {
		public:
			::sqlite3 *db;

			db_holder()
			{
				if (::sqlite3_open(":memory:", &db) != SQLITE_OK)
					throw cs::lang_error(::sqlite3_errmsg(db));
			}

			explicit db_holder(const char *path)
			{
				if (::sqlite3_open(path, &db) != SQLITE_OK)
					throw cs::lang_error(::sqlite3_errmsg(db));
			}

			db_holder(const db_holder &) = delete;

			~db_holder()
			{
				::sqlite3_close(db);
			}
		};

		class stmt_holder final {
		public:
			int errcode = SQLITE_OK;

			::sqlite3* host;

			::sqlite3_stmt *stmt;

			stmt_holder() = delete;

			explicit stmt_holder(::sqlite3 *db, const char *sql, std::size_t len):host(db)
			{
				if (::sqlite3_prepare_v2(host, sql, len, &stmt, nullptr) != SQLITE_OK)
					throw cs::lang_error(::sqlite3_errmsg(db));
			}

			stmt_holder(const stmt_holder &) = delete;

			~stmt_holder()
			{
				::sqlite3_finalize(stmt);
			}
		};

		std::shared_ptr<db_holder> m_db;
	public:
		enum class data_type {
			integer, real, text
		};

		class statement final {
			friend class sqlite;

			std::shared_ptr<stmt_holder> m_stmt;

			explicit statement(const std::shared_ptr<db_holder> &db, const std::string &sql) :m_stmt(std::make_shared<stmt_holder>(db->db, sql.c_str(), sql.size())) {}

		public:
			statement() = delete;

			statement(const statement &) = default;

			~statement() = default;

			bool done() const
			{
				return m_stmt->errcode == SQLITE_DONE;
			}

			void reset()
			{
				if (::sqlite3_reset(m_stmt->stmt)!=SQLITE_OK)
					throw cs::lang_error(::sqlite3_errmsg(m_stmt->host));
			}

			void step()
			{
				if ((m_stmt->errcode = ::sqlite3_step(m_stmt->stmt))==SQLITE_ERROR)
					throw cs::lang_error(::sqlite3_errmsg(m_stmt->host));
			}

			std::size_t column_count() const
			{
				return ::sqlite3_column_count(m_stmt->stmt);
			}

			data_type column_type(std::size_t index) const
			{
				switch (::sqlite3_column_type(m_stmt->stmt, index)) {
				case SQLITE_INTEGER:
					return data_type::integer;
				case SQLITE_FLOAT:
					return data_type::real;
				case SQLITE_TEXT:
					return data_type::text;
				default:
					throw cs::lang_error("Unsupported type.");
				}
			}

			std::string column_name(std::size_t index) const
			{
				return ::sqlite3_column_name(m_stmt->stmt, index);
			}

			std::string column_decltype(std::size_t index) const
			{
				return ::sqlite3_column_decltype(m_stmt->stmt, index);
			}

			int column_integer(std::size_t index) const
			{
				return ::sqlite3_column_int(m_stmt->stmt, index);
			}

			double column_real(std::size_t index) const
			{
				return ::sqlite3_column_double(m_stmt->stmt, index);
			}

			std::string column_text(std::size_t index) const
			{
				return reinterpret_cast<const char *>(::sqlite3_column_text(m_stmt->stmt, index));
			}

			std::size_t bind_param_count() const
			{
				return ::sqlite3_bind_parameter_count(m_stmt->stmt);
			}

			void bind_integer(std::size_t index,int data)
			{
				if (::sqlite3_bind_int(m_stmt->stmt,index,data)!=SQLITE_OK)
					throw cs::lang_error(::sqlite3_errmsg(m_stmt->host));
			}

			void bind_real(std::size_t index,double data)
			{
				if (::sqlite3_bind_double(m_stmt->stmt,index,data)!=SQLITE_OK)
					throw cs::lang_error(::sqlite3_errmsg(m_stmt->host));
			}

			void bind_text(std::size_t index,const std::string& data)
			{
				if (::sqlite3_bind_text(m_stmt->stmt,index,data.c_str(),data.size(),nullptr)!=SQLITE_OK)
					throw cs::lang_error(::sqlite3_errmsg(m_stmt->host));
			}

			void clear_bindings()
			{
				::sqlite3_clear_bindings(m_stmt->stmt);
			}
		};

		sqlite() : m_db(std::make_shared<db_holder>()) {}

		explicit sqlite(const std::string &path) : m_db(std::make_shared<db_holder>(path.c_str())) {}

		sqlite(const sqlite &) = default;

		~sqlite() = default;

		statement prepare(const std::string &sql)
		{
			return statement(m_db, sql);
		}
	};
}
