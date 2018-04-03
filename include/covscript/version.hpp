#pragma once
/*
* Covariant Script Version Info
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
* Copyright (C) 2018 Michael Lee(李登淳)
* Email: mikecovlee@163.com
* Github: https://github.com/mikecovlee
*
* Version Format:
*   1 . 0 . 0 . 0 (Preview/Unstable/Stable)
*   |   |   |   |            |
*   |   |   |   |          Status
*   |   |   | Build
*   |   | Minor
*   | Major
* Master
*
*/
#define COVSCRIPT_VERSION_NUM 1,2,2,6
#define COVSCRIPT_VERSION_STR "1.2.2.6(Unstable)"
#define COVSCRIPT_STD_VERSION 180401
#if defined(__WIN32) || defined(WIN32)
#define COVSCRIPT_PLATFORM_WIN32
#endif