#pragma once
/*
* Covariant Script Code Generating
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
#include <covscript/impl/symbols.hpp>
#include <covscript/impl/statement.hpp>

namespace cs {
	class method_expression final : public method_base {
	public:
		using method_base::method_base;

		method_types get_type() const noexcept override
		{
			return method_types::single;
		}

		statement_types get_target_type() const noexcept override
		{
			return statement_types::expression_;
		}

		statement_base *translate(const context_t &, const std::deque<std::deque<token_base *>> &) override;
	};

	class method_import final : public method_base {
		statement_base *mResult = nullptr;
	public:
		using method_base::method_base;

		method_types get_type() const noexcept override
		{
			return method_types::single;
		}

		statement_types get_target_type() const noexcept override
		{
			return statement_types::import_;
		}

		void preprocess(const context_t &, const std::deque<std::deque<token_base *>> &) override;

		statement_base *translate(const context_t &, const std::deque<std::deque<token_base *>> &) override;
	};

	class method_package final : public method_base {
	public:
		using method_base::method_base;

		method_types get_type() const noexcept override
		{
			return method_types::jit_command;
		}

		statement_types get_target_type() const noexcept override
		{
			return statement_types::package_;
		}

		statement_base *translate(const context_t &, const std::deque<std::deque<token_base *>> &) override;
	};

	class method_involve final : public method_base {
		statement_base *mResult = nullptr;
	public:
		using method_base::method_base;

		method_types get_type() const noexcept override
		{
			return method_types::single;
		}

		statement_types get_target_type() const noexcept override
		{
			return statement_types::involve_;
		}

		void preprocess(const context_t &, const std::deque<std::deque<token_base *>> &) override;

		statement_base *translate(const context_t &, const std::deque<std::deque<token_base *>> &) override;
	};

	class method_var final : public method_base {
	public:
		using method_base::method_base;

		method_types get_type() const noexcept override
		{
			return method_types::single;
		}

		statement_types get_target_type() const noexcept override
		{
			return statement_types::var_;
		}

		void preprocess(const context_t &, const std::deque<std::deque<token_base *>> &) override;

		statement_base *translate(const context_t &, const std::deque<std::deque<token_base *>> &) override;
	};

	class method_constant final : public method_base {
		statement_base *mResult = nullptr;
	public:
		using method_base::method_base;

		method_types get_type() const noexcept override
		{
			return method_types::single;
		}

		statement_types get_target_type() const noexcept override
		{
			return statement_types::constant_;
		}

		void preprocess(const context_t &, const std::deque<std::deque<token_base *>> &) override;

		statement_base *translate(const context_t &, const std::deque<std::deque<token_base *>> &) override;
	};

	class method_end final : public method_base {
	public:
		using method_base::method_base;

		method_types get_type() const noexcept override
		{
			return method_types::single;
		}

		statement_types get_target_type() const noexcept override
		{
			return statement_types::end_;
		}

		statement_base *translate(const context_t &, const std::deque<std::deque<token_base *>> &) override;
	};

	class method_block final : public method_base {
	public:
		using method_base::method_base;

		method_types get_type() const noexcept override
		{
			return method_types::block;
		}

		statement_types get_target_type() const noexcept override
		{
			return statement_types::block_;
		}

		statement_base *translate(const context_t &, const std::deque<std::deque<token_base *>> &) override;
	};

	class method_namespace final : public method_base {
	public:
		using method_base::method_base;

		method_types get_type() const noexcept override
		{
			return method_types::block;
		}

		statement_types get_target_type() const noexcept override
		{
			return statement_types::namespace_;
		}

		statement_base *translate(const context_t &, const std::deque<std::deque<token_base *>> &) override;
	};

	class method_if final : public method_base {
	public:
		using method_base::method_base;

		method_types get_type() const noexcept override
		{
			return method_types::block;
		}

		statement_types get_target_type() const noexcept override
		{
			return statement_types::if_;
		}

		statement_base *translate(const context_t &, const std::deque<std::deque<token_base *>> &) override;
	};

	class method_else final : public method_base {
	public:
		using method_base::method_base;

		method_types get_type() const noexcept override
		{
			return method_types::single;
		}

		statement_types get_target_type() const noexcept override
		{
			return statement_types::else_;
		}

		void preprocess(const context_t &, const std::deque<std::deque<token_base *>> &) override;

		statement_base *translate(const context_t &, const std::deque<std::deque<token_base *>> &) override;
	};

	class method_switch final : public method_base {
	public:
		using method_base::method_base;

		method_types get_type() const noexcept override
		{
			return method_types::block;
		}

		statement_types get_target_type() const noexcept override
		{
			return statement_types::switch_;
		}

		statement_base *translate(const context_t &, const std::deque<std::deque<token_base *>> &) override;
	};

	class method_case final : public method_base {
	public:
		using method_base::method_base;

		method_types get_type() const noexcept override
		{
			return method_types::block;
		}

		statement_types get_target_type() const noexcept override
		{
			return statement_types::case_;
		}

		statement_base *translate(const context_t &, const std::deque<std::deque<token_base *>> &) override;
	};

	class method_default final : public method_base {
	public:
		using method_base::method_base;

		method_types get_type() const noexcept override
		{
			return method_types::block;
		}

		statement_types get_target_type() const noexcept override
		{
			return statement_types::default_;
		}

		statement_base *translate(const context_t &, const std::deque<std::deque<token_base *>> &) override;
	};

	class method_while final : public method_base {
	public:
		using method_base::method_base;

		method_types get_type() const noexcept override
		{
			return method_types::block;
		}

		statement_types get_target_type() const noexcept override
		{
			return statement_types::while_;
		}

		statement_base *translate(const context_t &, const std::deque<std::deque<token_base *>> &) override;
	};

	class method_until final : public method_base {
	public:
		using method_base::method_base;

		method_types get_type() const noexcept override
		{
			return method_types::single;
		}

		statement_types get_target_type() const noexcept override
		{
			return statement_types::until_;
		}

		statement_base *translate(const context_t &, const std::deque<std::deque<token_base *>> &) override;
	};

	class method_loop final : public method_base {
	public:
		using method_base::method_base;

		method_types get_type() const noexcept override
		{
			return method_types::block;
		}

		statement_types get_target_type() const noexcept override
		{
			return statement_types::loop_;
		}

		statement_base *translate(const context_t &, const std::deque<std::deque<token_base *>> &) override;
	};

	class method_for final : public method_base {
	public:
		using method_base::method_base;

		method_types get_type() const noexcept override
		{
			return method_types::block;
		}

		statement_types get_target_type() const noexcept override
		{
			return statement_types::for_;
		}

		void preprocess(const context_t &, const std::deque<std::deque<token_base *>> &) override;

		statement_base *translate(const context_t &, const std::deque<std::deque<token_base *>> &) override;
	};

	class method_for_do final : public method_base {
	public:
		using method_base::method_base;

		method_types get_type() const noexcept override
		{
			return method_types::single;
		}

		statement_types get_target_type() const noexcept override
		{
			return statement_types::for_;
		}

		statement_base *translate(const context_t &, const std::deque<std::deque<token_base *>> &) override;
	};

	class method_foreach final : public method_base {
	public:
		using method_base::method_base;

		method_types get_type() const noexcept override
		{
			return method_types::block;
		}

		statement_types get_target_type() const noexcept override
		{
			return statement_types::foreach_;
		}

		void preprocess(const context_t &, const std::deque<std::deque<token_base *>> &) override;

		statement_base *translate(const context_t &, const std::deque<std::deque<token_base *>> &) override;
	};

	class method_foreach_do final : public method_base {
	public:
		using method_base::method_base;

		method_types get_type() const noexcept override
		{
			return method_types::single;
		}

		statement_types get_target_type() const noexcept override
		{
			return statement_types::foreach_;
		}

		statement_base *translate(const context_t &, const std::deque<std::deque<token_base *>> &) override;
	};

	class method_break final : public method_base {
	public:
		using method_base::method_base;

		method_types get_type() const noexcept override
		{
			return method_types::single;
		}

		statement_types get_target_type() const noexcept override
		{
			return statement_types::break_;
		}

		statement_base *translate(const context_t &, const std::deque<std::deque<token_base *>> &) override;
	};

	class method_continue final : public method_base {
	public:
		using method_base::method_base;

		method_types get_type() const noexcept override
		{
			return method_types::single;
		}

		statement_types get_target_type() const noexcept override
		{
			return statement_types::continue_;
		}

		statement_base *translate(const context_t &, const std::deque<std::deque<token_base *>> &) override;
	};

	class method_function final : public method_base {
	public:
		using method_base::method_base;

		method_types get_type() const noexcept override
		{
			return method_types::block;
		}

		statement_types get_target_type() const noexcept override
		{
			return statement_types::function_;
		}

		void preprocess(const context_t &, const std::deque<std::deque<token_base *>> &) override;

		statement_base *translate(const context_t &, const std::deque<std::deque<token_base *>> &) override;
	};

	class method_return final : public method_base {
	public:
		using method_base::method_base;

		method_types get_type() const noexcept override
		{
			return method_types::single;
		}

		statement_types get_target_type() const noexcept override
		{
			return statement_types::return_;
		}

		statement_base *translate(const context_t &, const std::deque<std::deque<token_base *>> &) override;
	};

	class method_return_no_value final : public method_base {
	public:
		using method_base::method_base;

		method_types get_type() const noexcept override
		{
			return method_types::single;
		}

		statement_types get_target_type() const noexcept override
		{
			return statement_types::return_;
		}

		statement_base *translate(const context_t &, const std::deque<std::deque<token_base *>> &) override;
	};

	class method_struct : public method_base {
	public:
		using method_base::method_base;

		method_types get_type() const noexcept override
		{
			return method_types::block;
		}

		statement_types get_target_type() const noexcept override
		{
			return statement_types::struct_;
		}

		void preprocess(const context_t &, const std::deque<std::deque<token_base *>> &) override;

		statement_base *translate(const context_t &, const std::deque<std::deque<token_base *>> &) override;
	};

	class method_try final : public method_base {
	public:
		using method_base::method_base;

		method_types get_type() const noexcept override
		{
			return method_types::block;
		}

		statement_types get_target_type() const noexcept override
		{
			return statement_types::try_;
		}

		statement_base *translate(const context_t &, const std::deque<std::deque<token_base *>> &) override;
	};

	class method_catch final : public method_base {
	public:
		using method_base::method_base;

		method_types get_type() const noexcept override
		{
			return method_types::single;
		}

		statement_types get_target_type() const noexcept override
		{
			return statement_types::catch_;
		}

		statement_base *translate(const context_t &, const std::deque<std::deque<token_base *>> &) override;
	};

	class method_throw final : public method_base {
	public:
		using method_base::method_base;

		method_types get_type() const noexcept override
		{
			return method_types::single;
		}

		statement_types get_target_type() const noexcept override
		{
			return statement_types::throw_;
		}

		statement_base *translate(const context_t &, const std::deque<std::deque<token_base *>> &) override;
	};
}
