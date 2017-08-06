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
	class syntax_error final:public std::exception {
		std::string mWhat="Covariant Script Syntax Error";
	public:
		syntax_error()=default;
		syntax_error(const std::string& str) noexcept:
			mWhat("\nCovariant Script Syntax Error:\n"+str) {}
		syntax_error(std::size_t line,const std::string& str) noexcept:
			mWhat("\nIn line "+std::to_string(line)+":"+str) {}
		syntax_error(const std::string& file,std::size_t line,const std::string& str) noexcept:
			mWhat("\nIn file \""+file+"\",line "+std::to_string(line)+":"+str) {}
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
	class internal_error final:public std::exception {
		std::string mWhat="Covariant Script Internal Error";
	public:
		internal_error()=default;
		internal_error(const std::string& str) noexcept:
			mWhat("\nCovariant Script Internal Error:\n"+str) {}
		internal_error(std::size_t line,const std::string& str) noexcept:
			mWhat("\nIn line "+std::to_string(line)+":"+str) {}
		internal_error(const std::string& file,std::size_t line,const std::string& str) noexcept:
			mWhat("\nIn file \""+file+"\",line "+std::to_string(line)+":"+str) {}
		internal_error(const internal_error&)=default;
		internal_error(internal_error&&)=default;
		virtual ~internal_error()=default;
		internal_error& operator=(const internal_error&)=default;
		internal_error& operator=(internal_error&&)=default;
		virtual const char* what() const noexcept override
		{
			return this->mWhat.c_str();
		}
	};
	class lang_error final:public std::exception {
		std::string mWhat="Covariant Script Language Error";
	public:
		lang_error()=default;
		lang_error(const std::string& str) noexcept:
			mWhat("\nCovariant Script Language Error:\n"+str) {}
		lang_error(std::size_t line,const std::string& str) noexcept:
			mWhat("\nIn line "+std::to_string(line)+":"+str) {}
		lang_error(const std::string& file,std::size_t line,const std::string& str) noexcept:
			mWhat("\nIn file \""+file+"\",line "+std::to_string(line)+":"+str) {}
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
	class fatal_error final:public std::exception {
		std::string mWhat="Covariant Script Fatal Error";
	public:
		fatal_error()=default;
		fatal_error(const std::string& str) noexcept:mWhat("\nCovariant Script Fatal Error:\n"+str+"\nCompilation terminated.") {}
		fatal_error(const fatal_error&)=default;
		fatal_error(fatal_error&&)=default;
		virtual ~fatal_error()=default;
		fatal_error& operator=(const fatal_error&)=default;
		fatal_error& operator=(fatal_error&&)=default;
		virtual const char* what() const noexcept override
		{
			return this->mWhat.c_str();
		}
	};
}
