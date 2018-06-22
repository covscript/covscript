#pragma once
/*
* Covariant Mozart Utility Library: Switcher
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
* Version: 17.4.1
*/
#include "./base.hpp"
#include "./any.hpp"
#include "./function.hpp"
#include "./tuple.hpp"
#include <unordered_map>
#include <forward_list>

namespace cov {
	class switcher final {
	public:
		typedef cov::function<void()> case_type;
	private:
		std::unordered_map<cov::any, cov::function<void()>> mCases;
		cov::function<void()> mDefault;
		const cov::any mCondition;
	public:
		switcher() = delete;

		switcher(const cov::any &val) : mCondition(val) {}

		switcher(switcher &&) noexcept = delete;

		switcher(const switcher &) = delete;

		~switcher() = default;

		void add_case(const cov::any &head, const case_type &body)
		{
			if (object::show_warning && head.type() != mCondition.type())
				throw cov::warning("W0001");
			if (mCases.count(head) == 0)
				mCases.emplace(head, body);
			else if (object::show_warning)
				throw cov::warning("W0002");
		}

		void add_default(const case_type &body)
		{
			if (mDefault.callable() && object::show_warning)
				throw cov::warning("W0002");
			mDefault = body;
		}

		void perform()
		{
			if (mCases.count(mCondition) > 0 && mCases.at(mCondition).callable())
				mCases.at(mCondition)();
			else if (mDefault.callable())
				mDefault.call();
		}
	};

	class switcher_stack final {
		std::forward_list<switcher *> mStack;
	public:
		switcher_stack() = default;

		switcher_stack(switcher_stack &&) noexcept = delete;

		switcher_stack(const switcher_stack &) = delete;

		~switcher_stack()
		{
			for (auto &it:mStack)
				delete it;
		}

		switcher &top()
		{
			return *mStack.front();
		}

		void push(const cov::any &val)
		{
			mStack.push_front(new switcher(val));
		}

		void pop()
		{
			delete mStack.front();
			mStack.pop_front();
		}
	};

	switcher_stack cov_switchers;
}

#define CovSwitch(obj) cov::cov_switchers.push(obj);
#define EndCovSwitch cov::cov_switchers.top().perform();cov::cov_switchers.pop();
#define CovCase(obj) cov::cov_switchers.top().add_case(obj,[&]{
#define CovDefault cov::cov_switchers.top().add_default([&]{
#define EndCovCase });
