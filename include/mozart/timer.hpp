#pragma once
/*
* Covariant Mozart Utility Library: Timer
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
* Version: 17.1.0
*/
#include "./base.hpp"
#include "./function.hpp"
#include <thread>
#include <chrono>

namespace cov {
	class timer final {
		static std::chrono::time_point<std::chrono::high_resolution_clock> m_timer;
	public:
		typedef unsigned long timer_t;
		enum class time_unit {
			nano_sec, micro_sec, milli_sec, second, minute
		};

		static void reset()
		{
			m_timer = std::chrono::high_resolution_clock::now();
		}

		static timer_t time(time_unit unit)
		{
			switch (unit) {
			case time_unit::nano_sec:
				return std::chrono::duration_cast<std::chrono::nanoseconds>(
				           std::chrono::high_resolution_clock::now() - m_timer).count();
			case time_unit::micro_sec:
				return std::chrono::duration_cast<std::chrono::microseconds>(
				           std::chrono::high_resolution_clock::now() - m_timer).count();
			case time_unit::milli_sec:
				return std::chrono::duration_cast<std::chrono::milliseconds>(
				           std::chrono::high_resolution_clock::now() - m_timer).count();
			case time_unit::second:
				return std::chrono::duration_cast<std::chrono::seconds>(
				           std::chrono::high_resolution_clock::now() - m_timer).count();
			case time_unit::minute:
				return std::chrono::duration_cast<std::chrono::minutes>(
				           std::chrono::high_resolution_clock::now() - m_timer).count();
			}
			return 0;
		}

		static void delay(time_unit unit, timer_t time)
		{
			switch (unit) {
			case time_unit::nano_sec:
				std::this_thread::sleep_for(std::chrono::nanoseconds(time));
				break;
			case time_unit::micro_sec:
				std::this_thread::sleep_for(std::chrono::microseconds(time));
				break;
			case time_unit::milli_sec:
				std::this_thread::sleep_for(std::chrono::milliseconds(time));
				break;
			case time_unit::second:
				std::this_thread::sleep_for(std::chrono::seconds(time));
				break;
			case time_unit::minute:
				std::this_thread::sleep_for(std::chrono::minutes(time));
				break;
			}
		}

		static timer_t measure(time_unit unit, const cov::function<void()> &func)
		{
			timer_t begin(0), end(0);
			begin = time(unit);
			func();
			end = time(unit);
			return end - begin;
		}
	};

	std::chrono::time_point<std::chrono::high_resolution_clock>
	timer::m_timer(std::chrono::high_resolution_clock::now());
}
