#pragma once
/*
* Covariant Mozart Utility Library: Static Stack
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
* Copyright (C) 2018 Michael Lee(李登淳)
* Email: mikecovlee@163.com
* Github: https://github.com/mikecovlee
*
* Version: 17.6.1
*/
#include "./base.hpp"
#include <utility>
#include <array>

namespace cov {
	template<typename T, long Size>
	class static_stack final {
		std::array<T, Size> mData;
		long mOffset = -1;
	public:
		static_stack() = default;

		static_stack(const static_stack &) = default;

		static_stack(static_stack &&) noexcept = default;

		~static_stack() = default;

		bool empty() const
		{
			return mOffset == -1;
		}

		bool full() const
		{
			return mOffset == Size - 1;
		}

		T &top()
		{
			if (mOffset < 0)
				throw cov::error("E000H");
			return mData[mOffset];
		}

		const T &top() const
		{
			if (mOffset < 0)
				throw cov::error("E000H");
			return mData[mOffset];
		}

		void push(const T &data)
		{
			if (mOffset < Size - 1)
				mData[++mOffset] = data;
			else
				throw cov::error("E000I");
		}

		void pop()
		{
			if (mOffset >= 0)
				--mOffset;
			else
				throw cov::error("E000H");
		}
	};
}
