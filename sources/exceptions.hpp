#pragma once
#include <exception>
#include <stdexcept>
namespace cov_basic {
	class syntax_error final:public std::exception {
		std::string mWhat="Covariant Basic Syntax Error";
	public:
		syntax_error()=default;
		syntax_error(const std::string& str) noexcept:
			mWhat("\nCovariant Basic Syntax Error:\n"+str) {}
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
		std::string mWhat="Covariant Basic Internal Error";
	public:
		internal_error()=default;
		internal_error(const std::string& str) noexcept:
			mWhat("\nCovariant Basic Internal Error:\n"+str) {}
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
		std::string mWhat="Covariant Basic Language Error";
	public:
		lang_error()=default;
		lang_error(const std::string& str) noexcept:
			mWhat("\nCovariant Basic Language Error:\n"+str) {}
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
		std::string mWhat="Covariant Basic Fatal Error";
	public:
		fatal_error()=default;
		fatal_error(const std::string& str) noexcept:mWhat("\nCovariant Basic Fatal Error:\n"+str+"\nCompilation terminated.") {}
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
