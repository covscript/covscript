#pragma once
/*
* Covariant Script Exceptions
*
* Licensed under the Covariant Innovation General Public License,
* Version 1.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* https://covariant.cn/licenses/LICENSE-1.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
* Copyright (C) 2019 Michael Lee(李登淳)
* Email: mikecovlee@163.com
* Github: https://github.com/mikecovlee
*/
#include <exception>
#include <stdexcept>
#include <utility>

namespace cs {
	class exception final : public std::exception {
		std::string mWhat;
	public:
		exception() = delete;

		exception(std::size_t line, const std::string &file, const std::string &code, const std::string &what) noexcept:
			mWhat("File \"" + file + "\", line " + std::to_string(line) + "\n\t" + code + "\n\t^\n" + what) {}

		exception(const exception &) = default;

		exception(exception &&) noexcept = default;

		~exception() override = default;

		exception &operator=(const exception &) = default;

		exception &operator=(exception &&) = default;

		const char *what() const noexcept override
		{
			return this->mWhat.c_str();
		}
	};

	class runtime_error final : public std::exception {
		std::string mWhat = "Runtime Error";
	public:
		runtime_error() = default;

		explicit runtime_error(const std::string &str) noexcept:
			mWhat("Runtime Error: " + str) {}

		runtime_error(const runtime_error &) = default;

		runtime_error(runtime_error &&) noexcept = default;

		~runtime_error() override = default;

		runtime_error &operator=(const runtime_error &) = default;

		runtime_error &operator=(runtime_error &&) = default;

		const char *what() const noexcept override
		{
			return this->mWhat.c_str();
		}
	};

	class internal_error final : public std::exception {
		std::string mWhat = "Internal Error";
	public:
		internal_error() = default;

		explicit internal_error(const std::string &str) noexcept:
			mWhat("Internal Error: " + str) {}

		internal_error(const internal_error &) = default;

		internal_error(internal_error &&) noexcept = default;

		~internal_error() override = default;

		internal_error &operator=(const internal_error &) = default;

		internal_error &operator=(internal_error &&) = default;

		const char *what() const noexcept override
		{
			return this->mWhat.c_str();
		}
	};

	class lang_error final {
		std::string mWhat;
	public:
		lang_error() = default;

		explicit lang_error(std::string str) noexcept:
			mWhat(std::move(str)) {}

		lang_error(const lang_error &) = default;

		lang_error(lang_error &&) noexcept = default;

		~lang_error() = default;

		lang_error &operator=(const lang_error &) = default;

		lang_error &operator=(lang_error &&) = default;

		const char *what() const noexcept
		{
			return this->mWhat.c_str();
		}
	};

	class fatal_error final : public std::exception {
		std::string mWhat = "Fatal Error";
	public:
		fatal_error() = default;

		explicit fatal_error(const std::string &str) noexcept:
			mWhat("Fatal Error: " + str) {}

		fatal_error(const fatal_error &) = default;

		fatal_error(fatal_error &&) noexcept = default;

		~fatal_error() override = default;

		fatal_error &operator=(const fatal_error &) = default;

		fatal_error &operator=(fatal_error &&) = default;

		const char *what() const noexcept override
		{
			return this->mWhat.c_str();
		}
	};

	class forward_exception final : public std::exception {
		std::string mWhat;
	public:
		forward_exception() = delete;

		explicit forward_exception(const char *str) noexcept: mWhat(str) {}

		forward_exception(const forward_exception &) = default;

		forward_exception(forward_exception &&) noexcept = default;

		~forward_exception() override = default;

		forward_exception &operator=(const forward_exception &) = default;

		forward_exception &operator=(forward_exception &&) = default;

		const char *what() const noexcept override
		{
			return this->mWhat.c_str();
		}
	};
}
