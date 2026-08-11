/*
 * Covariant Script OS Support: Common Functions
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Copyright (C) 2017-2026 Michael Lee(李登淳)
 *
 * This software is registered with the National Copyright Administration
 * of the People's Republic of China(Registration Number: 2020SR0408026)
 * and is protected by the Copyright Law of the People's Republic of China.
 *
 * Email:   mikecovlee@163.com
 * Github:  https://github.com/mikecovlee
 * Website: http://covscript.org.cn
 */

#include <covscript/impl/system.hpp>
#include <filesystem>
#include <fcntl.h>

namespace cs::fiber {
	class fiber_future final : public future_type {
		fiber_t mFiber;
		std::exception_ptr mException;

		void resume_fiber()
		{
			try {
				resume(mFiber, schedule_policy::no_backpressure);
			}
			catch (...) {
				mException = std::current_exception();
			}
		}

	public:
		fiber_future(fiber_t fiber)
			: mFiber(std::move(fiber)) {}

		bool wait_for(std::size_t ms) override
		{
			std::chrono::steady_clock::time_point end_time = std::chrono::steady_clock::now() + std::chrono::milliseconds(ms);
			while (mFiber->get_state() != fiber_state::finished) {
				auto now = std::chrono::steady_clock::now();
				if (now > end_time)
					break;
				resume_fiber();
				if (mFiber->get_state() == fiber_state::finished || mException != nullptr)
					break;
				auto remain_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
				                     end_time - std::chrono::steady_clock::now())
				                 .count();
				if (remain_ms <= 0)
					break;
				auto wait_time = static_cast<std::size_t>(remain_ms * current_process->fiber_cxt->busy_wait_coef);
				if (wait_time < current_process->fiber_cxt->busy_wait_min)
					wait_time = current_process->fiber_cxt->busy_wait_min;
				if (wait_time > static_cast<std::size_t>(remain_ms))
					wait_time = static_cast<std::size_t>(remain_ms);
				if (within())
					fiber::sleep_for(wait_time);
				else
					std::this_thread::sleep_for(std::chrono::milliseconds(wait_time));
			}
			return mFiber->get_state() == fiber_state::finished;
		}

		void wait() override
		{
			while (mFiber->get_state() != fiber_state::finished) {
				resume_fiber();
				if (mException != nullptr)
					break;
				// Only back off if the fiber is actively sleeping; a suspended
				// (yielded) fiber can be resumed again immediately.
				if (mFiber->get_state() == fiber_state::sleeping) {
					if (within())
						fiber::sleep_for(current_process->fiber_cxt->busy_wait_min);
					else
						std::this_thread::sleep_for(std::chrono::milliseconds(current_process->fiber_cxt->busy_wait_min));
				}
			}
		}

		var get() override
		{
			if (mFiber->get_state() != fiber_state::finished)
				wait();
			if (mException)
				std::rethrow_exception(mException);
			return mFiber->return_value();
		}
	};

	future_t get_future(const fiber_t &fiber)
	{
		return std::make_shared<fiber_future>(fiber);
	}
} // namespace cs::fiber

namespace cs_system_impl {
	bool mkdir_impl(const std::string &, unsigned int);

	bool chmod_impl(const std::string &, unsigned int);
} // namespace cs_system_impl

#ifdef COVSCRIPT_PLATFORM_WIN32

#include "./win32/common.cpp"

#else
#ifndef COVSCRIPT_PLATFORM_UNIX
#warning Compatible Mode for Unix Platform
#endif

#include "./unix/common.cpp"

#endif

namespace cs_system_impl {
	std::vector<std::string> split(const std::string &str, const cs::set_t<char> &set)
	{
		std::vector<std::string> results;
		std::string buff;
		for (auto ch : str) {
			if (set.count(ch) > 0) {
				if (!buff.empty()) {
					results.emplace_back(buff);
					buff.clear();
				}
			}
			else
				buff.push_back(ch);
		}
		if (!buff.empty()) {
			results.emplace_back(buff);
			buff.clear();
		}
		return std::move(results);
	}

	unsigned int parse_mode(const std::string &modeString)
	{
		if (modeString.empty())
			throw cs::lang_error("Invalid permission mode: empty string");
		const char *perm = modeString.c_str();
		unsigned int mode = 0;

		if (std::isdigit(static_cast<unsigned char>(perm[0]))) {
			const char *p = perm;
			while (*p) {
				if (*p < '0' || *p > '7')
					throw cs::lang_error("Invalid permission mode: expected octal digits 0-7");
				mode = mode * 8 + *p++ - '0';
			}
		}
		else {
			if (modeString.size() != 9)
				throw cs::lang_error("Invalid permission mode: expected 3 groups of 'rwx'");
			mode = (((perm[0] == 'r') * 4 | (perm[1] == 'w') * 2 | (perm[2] == 'x')) << 6) |
			       (((perm[3] == 'r') * 4 | (perm[4] == 'w') * 2 | (perm[5] == 'x')) << 3) |
			       (((perm[6] == 'r') * 4 | (perm[7] == 'w') * 2 | (perm[8] == 'x')));
		}
		return mode;
	}

	static bool mkdir_dirs(const std::vector<std::string> &dirs, unsigned int mode, bool absolute_path = false)
	{
		std::string path;
		if (absolute_path)
			path = cs::path_separator;
		for (const auto &dir : dirs) {
			path += dir + cs::path_separator;
			if (std::filesystem::is_directory(path))
				continue;
			if (!mkdir_impl(path, mode))
				return false;
		}
		return true;
	}
} // namespace cs_system_impl

#ifdef COVSCRIPT_PLATFORM_WIN32
constexpr char path_separator_reversed = '/';
constexpr char path_delimiter_reversed = ':';
#else
constexpr char path_separator_reversed = '\\';
constexpr char path_delimiter_reversed = ';';
#endif

namespace cs_impl {
	namespace file_system {
		bool exist(const std::string &path)
		{
			return std::filesystem::exists(path);
		}

		bool is_file(const std::string &path)
		{
			return std::filesystem::is_regular_file(path);
		}

		bool is_dir(const std::string &path)
		{
			return std::filesystem::is_directory(path);
		}

		bool is_absolute_path(const std::string &path)
		{
			return std::filesystem::path(path).is_absolute();
		}

		std::string normalize_path(const std::string &path)
		{
			std::error_code ec;
			std::filesystem::path p = std::filesystem::weakly_canonical(path, ec);
			if (ec) {
				p = std::filesystem::absolute(path, ec);
				if (ec)
					p = std::filesystem::path(path);
				p = p.lexically_normal();
			}
			return p.generic_string();
		}

		bool chmod_r(const std::string &path_input, const std::string &mode)
		{
			auto dirs = cs_system_impl::split(path_input, {'/', '\\'});
			std::string path;
			bool absolute = path_input.size() > 0 && (path_input[0] == '/' || path_input[0] == '\\');
			if (absolute)
				path = cs::path_separator;
			for (auto &dir : dirs) {
				if (dir.empty())
					continue; // Skip the empty component left by a leading separator
				path += dir + cs::path_separator;
				if (path.size() == 1 && path[0] == cs::path_separator) // Never chmod the filesystem root
					continue;
				if (!cs_system_impl::chmod_impl(path, cs_system_impl::parse_mode(mode)))
					return false;
			}
			return true;
		}

		bool chmod(const std::string &path, const std::string &mode)
		{
			return cs_system_impl::chmod_impl(path, cs_system_impl::parse_mode(mode));
		}

		bool move(const std::string &source, const std::string &dest)
		{
			std::error_code ec;
			std::filesystem::rename(source, dest, ec);
			return !ec;
		}

		bool copy(const std::string &source, const std::string &dest)
		{
			std::error_code ec;
			// copy_file ignores the recursive option; dispatch on the source type.
			if (std::filesystem::is_directory(source))
				std::filesystem::copy(source, dest,
				                      std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing, ec);
			else
				std::filesystem::copy_file(source, dest, std::filesystem::copy_options::overwrite_existing, ec);
			return !ec;
		}

// Note: remove is recursive and deletes a whole tree; false for nonexistent paths.
		bool remove(const std::string &path)
		{
			std::error_code ec;
			return std::filesystem::remove_all(path, ec) > 0 && !ec;
		}

		bool mkdir_p(const std::string &path)
		{
			std::error_code ec;
			std::filesystem::create_directories(path, ec);
			return !ec;
		}

		bool mkdir(std::string path)
		{
			std::error_code ec;
			std::filesystem::create_directory(path, ec);
			return !ec;
		}

		std::string get_current_dir()
		{
			return std::filesystem::current_path().string();
		}
	} // namespace file_system
} // namespace cs_impl
