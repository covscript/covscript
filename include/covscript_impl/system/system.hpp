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

#include <sys/stat.h>

namespace cs_impl {
    namespace filesystem {
        bool is_directory(const std::string &path) {
            struct stat s{};
            if (stat(path.c_str(), &s) == 0) {
                return S_ISDIR(s.st_mode);
            }
            return false;
        }
    }
}