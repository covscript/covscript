#pragma once
/*
* Covariant Script OS Support
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
#if defined(_WIN32) || defined(WIN32)

#include "./win32_impl.hpp"

#else

#include "./unix_impl.hpp"

#endif

#include <covscript/core/core.hpp>
#include <covscript_impl/dirent/dirent.hpp>
#include <sys/stat.h>
#include <string>
#include <sstream>
#include <vector>

static std::vector<std::string> split(const std::string &string, char delimiter, bool drop_last)
{
    size_t start = 0;
    size_t end;
    std::vector<std::string> results;

    while ((end = string.find(delimiter, start)) != std::string::npos) {
        results.push_back(string.substr(start, end - start));
        start = end + 1;
    }

    if (!drop_last) {
        results.push_back(string.substr(start));
    }
    return results;
}

namespace cs_impl {
    namespace filesystem {
        static int get_file_type(const std::string &path)
        {
            struct stat s{};
            if (stat(path.c_str(), &s) != 0) {
                return DT_UNKNOWN;
            }
            if (S_ISDIR(s.st_mode)) {
                return DT_DIR;

            } else if (S_ISREG(s.st_mode)) {
                return DT_REG;

            } else if (S_ISLNK(s.st_mode)) {
                return DT_LNK;

            } else if (S_ISFIFO(s.st_mode)) {
                return DT_FIFO;

            } else if (S_ISBLK(s.st_mode)) {
                return DT_BLK;

            } else if (S_ISCHR(s.st_mode)) {
                return DT_CHR;

            } else if (S_ISSOCK(s.st_mode)) {
                return DT_SOCK;

            }
            return DT_UNKNOWN;
        }

        static bool is_directory(const std::string &path)
        {
            return get_file_type(path) == DT_DIR;
        }

        static bool is_regular_file(const std::string &path)
        {
            return get_file_type(path) == DT_REG;
        }

        static bool is_block_file(const std::string &path)
        {
            return get_file_type(path) == DT_BLK;
        }

        static bool is_char_file(const std::string &path)
        {
            return get_file_type(path) == DT_CHR;
        }

        static bool is_link_file(const std::string &path)
        {
            return get_file_type(path) == DT_LNK;
        }

        static bool is_fifo_file(const std::string &path)
        {
            return get_file_type(path) == DT_FIFO;
        }

        static bool mkdir_dirs(const std::vector<std::string> &dirs, mode_t mode)
        {
            std::stringstream ss;
            for (const auto &dir : dirs) {
                ss << dir << cs::path_separator;
                if (is_directory(ss.str())) {
                    continue;
                }

                if (!mkdir_impl(ss.str(), mode)) {
                    return false;
                }
            }
            return true;
        }

        static bool mkdir_recursive(const std::string &path, mode_t mode)
        {
            return mkdir_dirs(split(path, cs::path_separator, false), mode);
        }

        static bool mkdir_parent(const std::string &path, mode_t mode)
        {
            return mkdir_dirs(split(path, cs::path_separator, true), mode);
        }

        static bool chmod_recursive(const std::string &path, mode_t mode)
        {
            const auto &dirs = split(path, cs::path_separator, false);
            std::stringstream ss;
            for (const auto &dir : dirs) {
                ss << dir << cs::path_separator;
                if (is_directory(ss.str())) {
                    continue;
                }

                if (!chmod_impl(ss.str(), mode)) {
                    return false;
                }
            }
            return true;
        }
    }
}