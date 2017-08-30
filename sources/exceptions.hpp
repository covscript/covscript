#pragma once
/*
* Covariant Script Exceptions
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
*/
#include <exception>
#include <stdexcept>

namespace cs {
	class exception final : public std::exception {
		std::string mWhat;
	public:
		exception() = delete;

		exception(std::size_t line, const std::string &file, const std::string &code, const std::string &what) noexcept:
			mWhat("  File \"" + file + "\", line " + std::to_string(line) + "\n    " + code + "\n    ^\n" + what) {}

		exception(const exception &) = default;

		exception(exception &&) = default;

		virtual ~exception() = default;

		exception &operator=(const exception &)= default;

		exception &operator=(exception &&)= default;

		virtual const char *what() const noexcept override
		{
			return this->mWhat.c_str();
		}
	};

	class syntax_error final : public std::exception {
		std::string mWhat = "Syntax Error";
	public:
		syntax_error() = default;

		syntax_error(const std::string &str) noexcept:
			mWhat("Syntax Error: " + str) {}

		syntax_error(const syntax_error &) = default;

		syntax_error(syntax_error &&) = default;

		virtual ~syntax_error() = default;

		syntax_error &operator=(const syntax_error &)= default;

		syntax_error &operator=(syntax_error &&)= default;

		virtual const char *what() const noexcept override
		{
			return this->mWhat.c_str();
		}
	};

	class internal_error final : public std::exception {
		std::string mWhat = "Internal Error";
	public:
		internal_error() = default;

		internal_error(const std::string &str) noexcept:
			mWhat("Internal Error: " + str) {}

		internal_error(const internal_error &) = default;

		internal_error(internal_error &&) = default;

		virtual ~internal_error() = default;

		internal_error &operator=(const internal_error &)= default;

		internal_error &operator=(internal_error &&)= default;

		virtual const char *what() const noexcept override
		{
			return this->mWhat.c_str();
		}
	};

	class lang_error final {
		std::string mWhat;
	public:
		lang_error() = default;

		lang_error(const std::string &str) noexcept:
			mWhat(str) {}

		lang_error(const lang_error &) = default;

		lang_error(lang_error &&) = default;

		~lang_error() = default;

		lang_error &operator=(const lang_error &)= default;

		lang_error &operator=(lang_error &&)= default;

		const char *what() const noexcept
		{
			return this->mWhat.c_str();
		}
	};

	class fatal_error final : public std::exception {
		std::string mWhat = "Fatal Error";
	public:
		fatal_error() = default;

		fatal_error(const std::string &str) noexcept:
			mWhat("Fatal Error: " + str) {}

		fatal_error(const fatal_error &) = default;

		fatal_error(fatal_error &&) = default;

		virtual ~fatal_error() = default;

		fatal_error &operator=(const fatal_error &)= default;

		fatal_error &operator=(fatal_error &&)= default;

		virtual const char *what() const noexcept override
		{
			return this->mWhat.c_str();
		}
	};
}
