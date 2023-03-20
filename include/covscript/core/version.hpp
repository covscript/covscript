#pragma once
/*
* Covariant Script Version Info
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
* Copyright (C) 2017-2023 Michael Lee(李登淳)
*
* This software is registered with the National Copyright Administration
* of the People's Republic of China(Registration Number: 2020SR0408026)
* and is protected by the Copyright Law of the People's Republic of China.
*
* Email:   lee@covariant.cn, mikecovlee@163.com
* Github:  https://github.com/mikecovlee
* Website: http://covscript.org.cn
*
* Version Format:
*   1 . 0 . 0 [Version Code](Preview/Unstable/Stable [Patch 1]) Build 1
*   |   |   |                               |
*   |   | Minor                           Status
*   | Major
* Master
*
*/
#define COVSCRIPT_VERSION_NUM 3,4,2,3
#define COVSCRIPT_VERSION_STR "3.4.2 Castor fiber(Unstable) Build 3"
#define COVSCRIPT_STD_VERSION 210506
#define COVSCRIPT_API_VERSION 230301
#define COVSCRIPT_ABI_VERSION 230301
#define CS_VERSION_STR_MIXER(VER) #VER
#define CS_GET_VERSION_STR(VER) CS_VERSION_STR_MIXER(VER)
