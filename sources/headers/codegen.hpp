#pragma once
/*
* Covariant Script Code Gen
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
* Copyright (C) 2017 Michael Lee(李登淳)
* Email: mikecovlee@163.com
* Github: https://github.com/mikecovlee
*/
#include "./symbols.hpp"
#include "./statement.hpp"

namespace cs {
	class method_expression final : public method_base {
	public:
		using method_base::method_base;

		virtual method_types get_type() const noexcept override
		{
			return method_types::single;
		}

		virtual statement_types get_target_type() const noexcept override
		{
			return statement_types::expression_;
		}

		virtual statement_base *translate(const std::deque<std::deque<token_base *>> &) override;
	};

	class method_import final : public method_base {
	public:
		using method_base::method_base;

		virtual method_types get_type() const noexcept override
		{
			return method_types::jit_command;
		}

		virtual statement_types get_target_type() const noexcept override
		{
			return statement_types::import_;
		}

		virtual statement_base *translate(const std::deque<std::deque<token_base *>> &) override;
	};

	class method_package final : public method_base {
	public:
		using method_base::method_base;

		virtual method_types get_type() const noexcept override
		{
			return method_types::jit_command;
		}

		virtual statement_types get_target_type() const noexcept override
		{
			return statement_types::package_;
		}

		virtual statement_base *translate(const std::deque<std::deque<token_base *>> &) override;
	};

	class method_var final : public method_base {
	public:
		using method_base::method_base;

		virtual method_types get_type() const noexcept override
		{
			return method_types::single;
		}

		virtual statement_types get_target_type() const noexcept override
		{
			return statement_types::var_;
		}

		virtual statement_base *translate(const std::deque<std::deque<token_base *>> &) override;
	};

	class method_constant final : public method_base {
	public:
		using method_base::method_base;

		virtual method_types get_type() const noexcept override
		{
			return method_types::jit_command;
		}

		virtual statement_types get_target_type() const noexcept override
		{
			return statement_types::constant_;
		}

		virtual statement_base *translate(const std::deque<std::deque<token_base *>> &) override;
	};

	class method_end final : public method_base {
	public:
		using method_base::method_base;

		virtual method_types get_type() const noexcept override
		{
			return method_types::single;
		}

		virtual statement_types get_target_type() const noexcept override
		{
			return statement_types::end_;
		}

		virtual statement_base *translate(const std::deque<std::deque<token_base *>> &) override;
	};

	class method_block final : public method_base {
	public:
		using method_base::method_base;

		virtual method_types get_type() const noexcept override
		{
			return method_types::block;
		}

		virtual statement_types get_target_type() const noexcept override
		{
			return statement_types::block_;
		}

		virtual statement_base *translate(const std::deque<std::deque<token_base *>> &) override;
	};

	class method_namespace final : public method_base {
	public:
		using method_base::method_base;

		virtual method_types get_type() const noexcept override
		{
			return method_types::block;
		}

		virtual statement_types get_target_type() const noexcept override
		{
			return statement_types::namespace_;
		}

		virtual statement_base *translate(const std::deque<std::deque<token_base *>> &) override;
	};

	class method_if final : public method_base {
	public:
		using method_base::method_base;

		virtual method_types get_type() const noexcept override
		{
			return method_types::block;
		}

		virtual statement_types get_target_type() const noexcept override
		{
			return statement_types::if_;
		}

		virtual statement_base *translate(const std::deque<std::deque<token_base *>> &) override;
	};

	class method_else final : public method_base {
	public:
		using method_base::method_base;

		virtual method_types get_type() const noexcept override
		{
			return method_types::single;
		}

		virtual statement_types get_target_type() const noexcept override
		{
			return statement_types::else_;
		}

		virtual statement_base *translate(const std::deque<std::deque<token_base *>> &) override;
	};

	class method_switch final : public method_base {
	public:
		using method_base::method_base;

		virtual method_types get_type() const noexcept override
		{
			return method_types::block;
		}

		virtual statement_types get_target_type() const noexcept override
		{
			return statement_types::switch_;
		}

		virtual statement_base *translate(const std::deque<std::deque<token_base *>> &) override;
	};

	class method_case final : public method_base {
	public:
		using method_base::method_base;

		virtual method_types get_type() const noexcept override
		{
			return method_types::block;
		}

		virtual statement_types get_target_type() const noexcept override
		{
			return statement_types::case_;
		}

		virtual statement_base *translate(const std::deque<std::deque<token_base *>> &) override;
	};

	class method_default final : public method_base {
	public:
		using method_base::method_base;

		virtual method_types get_type() const noexcept override
		{
			return method_types::block;
		}

		virtual statement_types get_target_type() const noexcept override
		{
			return statement_types::default_;
		}

		virtual statement_base *translate(const std::deque<std::deque<token_base *>> &) override;
	};

	class method_while final : public method_base {
	public:
		using method_base::method_base;

		virtual method_types get_type() const noexcept override
		{
			return method_types::block;
		}

		virtual statement_types get_target_type() const noexcept override
		{
			return statement_types::while_;
		}

		virtual statement_base *translate(const std::deque<std::deque<token_base *>> &) override;
	};

	class method_until final : public method_base {
	public:
		using method_base::method_base;

		virtual method_types get_type() const noexcept override
		{
			return method_types::single;
		}

		virtual statement_types get_target_type() const noexcept override
		{
			return statement_types::until_;
		}

		virtual statement_base *translate(const std::deque<std::deque<token_base *>> &) override;
	};

	class method_loop final : public method_base {
	public:
		using method_base::method_base;

		virtual method_types get_type() const noexcept override
		{
			return method_types::block;
		}

		virtual statement_types get_target_type() const noexcept override
		{
			return statement_types::loop_;
		}

		virtual statement_base *translate(const std::deque<std::deque<token_base *>> &) override;
	};

	class method_for final : public method_base {
	public:
		using method_base::method_base;

		virtual method_types get_type() const noexcept override
		{
			return method_types::block;
		}

		virtual statement_types get_target_type() const noexcept override
		{
			return statement_types::for_;
		}

		virtual void preprocess(const std::deque<std::deque<token_base *>> &) override;

		virtual statement_base *translate(const std::deque<std::deque<token_base *>> &) override;
	};

	class method_for_step final : public method_base {
	public:
		using method_base::method_base;

		virtual method_types get_type() const noexcept override
		{
			return method_types::block;
		}

		virtual statement_types get_target_type() const noexcept override
		{
			return statement_types::for_;
		}

		virtual void preprocess(const std::deque<std::deque<token_base *>> &) override;

		virtual statement_base *translate(const std::deque<std::deque<token_base *>> &) override;
	};

	class method_foreach final : public method_base {
	public:
		using method_base::method_base;

		virtual method_types get_type() const noexcept override
		{
			return method_types::block;
		}

		virtual statement_types get_target_type() const noexcept override
		{
			return statement_types::foreach_;
		}

		virtual void preprocess(const std::deque<std::deque<token_base *>> &) override;

		virtual statement_base *translate(const std::deque<std::deque<token_base *>> &) override;
	};

	class method_break final : public method_base {
	public:
		using method_base::method_base;

		virtual method_types get_type() const noexcept override
		{
			return method_types::single;
		}

		virtual statement_types get_target_type() const noexcept override
		{
			return statement_types::break_;
		}

		virtual statement_base *translate(const std::deque<std::deque<token_base *>> &) override;
	};

	class method_continue final : public method_base {
	public:
		using method_base::method_base;

		virtual method_types get_type() const noexcept override
		{
			return method_types::single;
		}

		virtual statement_types get_target_type() const noexcept override
		{
			return statement_types::continue_;
		}

		virtual statement_base *translate(const std::deque<std::deque<token_base *>> &) override;
	};

	class method_function final : public method_base {
	public:
		using method_base::method_base;

		virtual method_types get_type() const noexcept override
		{
			return method_types::block;
		}

		virtual statement_types get_target_type() const noexcept override
		{
			return statement_types::function_;
		}

		virtual statement_base *translate(const std::deque<std::deque<token_base *>> &) override;
	};

	class method_return final : public method_base {
	public:
		using method_base::method_base;

		virtual method_types get_type() const noexcept override
		{
			return method_types::single;
		}

		virtual statement_types get_target_type() const noexcept override
		{
			return statement_types::return_;
		}

		virtual statement_base *translate(const std::deque<std::deque<token_base *>> &) override;
	};

	class method_return_no_value final : public method_base {
	public:
		using method_base::method_base;

		virtual method_types get_type() const noexcept override
		{
			return method_types::single;
		}

		virtual statement_types get_target_type() const noexcept override
		{
			return statement_types::return_;
		}

		virtual statement_base *translate(const std::deque<std::deque<token_base *>> &) override;
	};

	class method_struct final : public method_base {
	public:
		using method_base::method_base;

		virtual method_types get_type() const noexcept override
		{
			return method_types::block;
		}

		virtual statement_types get_target_type() const noexcept override
		{
			return statement_types::struct_;
		}

		virtual statement_base *translate(const std::deque<std::deque<token_base *>> &) override;
	};

	class method_try final : public method_base {
	public:
		using method_base::method_base;

		virtual method_types get_type() const noexcept override
		{
			return method_types::block;
		}

		virtual statement_types get_target_type() const noexcept override
		{
			return statement_types::try_;
		}

		virtual statement_base *translate(const std::deque<std::deque<token_base *>> &) override;
	};

	class method_catch final : public method_base {
	public:
		using method_base::method_base;

		virtual method_types get_type() const noexcept override
		{
			return method_types::single;
		}

		virtual statement_types get_target_type() const noexcept override
		{
			return statement_types::catch_;
		}

		virtual statement_base *translate(const std::deque<std::deque<token_base *>> &) override;
	};

	class method_throw final : public method_base {
	public:
		using method_base::method_base;

		virtual method_types get_type() const noexcept override
		{
			return method_types::single;
		}

		virtual statement_types get_target_type() const noexcept override
		{
			return statement_types::throw_;
		}

		virtual statement_base *translate(const std::deque<std::deque<token_base *>> &) override;
	};
}
