#pragma once
/*
* Covariant Mozart Utility Library: Static Stack
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

		std::size_t size() const
		{
			return mOffset + 1;
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

		auto begin() const
		{
			return mData.rbegin() + (Size - size());
		}

		auto end() const
		{
			return mData.rend();
		}
	};
}
