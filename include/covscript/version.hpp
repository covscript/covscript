#pragma once
/*
* Covariant Script Version Info
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
* Copyright (C) 2018 Michael Lee(李登淳)
* Email: mikecovlee@163.com
* Github: https://github.com/mikecovlee
*
* Version Format:
*   1 . 0 . 0 [Version Code](Preview/Unstable/Stable) Build 1
*   |   |   |                           |
*   |   | Minor                       Status
*   | Major
* Master
*
*/
#define COVSCRIPT_VERSION_NUM 3,0,2,2
#define COVSCRIPT_VERSION_STR "3.0.2 Acipenser sinensis(Unstable) Build 2"
#define COVSCRIPT_STD_VERSION 190101
#define COVSCRIPT_ABI_VERSION 190101
#if defined(_WIN32) || defined(WIN32)
#define COVSCRIPT_PLATFORM_WIN32
#endif
