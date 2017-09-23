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
#include "./parser.hpp"
#include "./statement.hpp"

namespace cs {
	/*
	* Grammar Types
	* Null: Do not use!
	* Single: Single Line Statement.
	* Block: Statement with code block.
	* Jit Command: Statement whitch execute in compile time.
	*/
	enum class method_types {
		null, single, block, jit_command
	};

	class method_base {
		static garbage_collector<method_base> gc;
	public:
		static void *operator new(std::size_t size)
		{
			void *ptr = ::operator new(size);
			gc.add(ptr);
			return ptr;
		}

		static void operator delete(void *ptr)
		{
			gc.remove(ptr);
			::operator delete(ptr);
		}

		method_base() = default;

		method_base(const method_base &) = default;

		virtual ~method_base() = default;

		virtual method_types get_type() const noexcept=0;

		virtual statement_types get_target_type() const noexcept=0;

		virtual void preprocess(const std::deque<std::deque<token_base *>> &) {}

		virtual statement_base *translate(const std::deque<std::deque<token_base *>> &)=0;
	};

	garbage_collector<method_base> method_base::gc;

	class method_expression final : public method_base {
	public:
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

	class translator_type final {
	public:
		using data_type=std::pair<std::deque<token_base *>, method_base *>;

		static bool compare(const token_base *a, const token_base *b)
		{
			if (a == nullptr)
				return b == nullptr;
			if (b == nullptr)
				return a == nullptr;
			if (a->get_type() != b->get_type())
				return false;
			return !(a->get_type() == token_types::action) || static_cast<const token_action *>(a)->get_action() == static_cast<const token_action *>(b)->get_action();
		}

	private:
		std::list<std::shared_ptr<data_type>> m_data;
	public:
		translator_type() = default;

		translator_type(const translator_type &) = delete;

		~translator_type() = default;

		void add_method(const std::deque<token_base *> &grammar, method_base *method)
		{
			m_data.emplace_back(std::make_shared<data_type>(grammar, method));
		}

		method_base *match(const std::deque<token_base *> &raw)
		{
			if (raw.size() <= 1)
				throw syntax_error("Grammar error.");
			std::list<std::shared_ptr<data_type>> stack;
			for (auto &it:m_data)
				if (this->compare(it->first.front(), raw.front()))
					stack.push_back(it);
			stack.remove_if([&](const std::shared_ptr<data_type> &dat) {
				return dat->first.size() != raw.size();
			});
			stack.remove_if([&](const std::shared_ptr<data_type> &dat) {
				for (std::size_t i = 1; i < raw.size() - 1; ++i) {
					if (!compare(raw.at(i), dat->first.at(i)))
						return true;
				}
				return false;
			});
			if (stack.empty())
				throw syntax_error("Uknow grammar.");
			if (stack.size() > 1)
				throw syntax_error("Ambiguous grammar.");
			return stack.front()->second;
		}
	};

	static translator_type translator;
}

#include "./runtime.hpp"

namespace cs {
	statement_base *method_expression::translate(const std::deque<std::deque<token_base *>> &raw)
	{
		return new statement_expression(dynamic_cast<token_expr *>(raw.front().front())->get_tree(), raw.front().back());
	}

	statement_base *method_import::translate(const std::deque<std::deque<token_base *>> &raw)
	{
		runtime_t rt = covscript(dynamic_cast<token_value *>(dynamic_cast<token_expr *>(raw.front().at(1))->get_tree().root().data())->get_value().const_val<string>());
		if (rt->package_name.empty())
			throw syntax_error("Only packages are allowed to import.");
		runtime->storage.add_var(rt->package_name, var::make_protect<extension_t>(std::make_shared<extension_holder>(rt->storage.get_global())));
		return nullptr;
	}

	statement_base *method_package::translate(const std::deque<std::deque<token_base *>> &raw)
	{
		if (!runtime->package_name.empty())
			throw syntax_error("Redefinition of package");
		runtime->package_name = dynamic_cast<token_id *>(dynamic_cast<token_expr *>(raw.front().at(1))->get_tree().root().data())->get_id();
		runtime->storage.add_var_global(runtime->package_name, var::make_protect<extension_t>(std::make_shared<extension_holder>(runtime->storage.get_global())));
		return nullptr;
	}

	statement_base *method_var::translate(const std::deque<std::deque<token_base *>> &raw)
	{
		cov::tree<token_base *> &tree = dynamic_cast<token_expr *>(raw.front().at(1))->get_tree();
		define_var_profile dvp;
		parse_define_var(tree, dvp);
		return new statement_var(dvp, raw.front().back());
	}

	statement_base *method_constant::translate(const std::deque<std::deque<token_base *>> &raw)
	{
		cov::tree<token_base *> &tree = dynamic_cast<token_expr *>(raw.front().at(2))->get_tree();
		define_var_profile dvp;
		parse_define_var(tree, dvp);
		if (dvp.expr.root().data()->get_type() != token_types::value)
			throw syntax_error("Constant variable must have an constant value.");
		runtime->storage.add_var(dvp.id, static_cast<token_value *>(dvp.expr.root().data())->get_value());
		return nullptr;
	}

	statement_base *method_end::translate(const std::deque<std::deque<token_base *>> &raw)
	{
		return new statement_end;
	}

	statement_base *method_block::translate(const std::deque<std::deque<token_base *>> &raw)
	{
		std::deque<statement_base *> body;
		kill_action({raw.begin() + 1, raw.end()}, body);
		return new statement_block(body, raw.front().back());
	}

	statement_base *method_namespace::translate(const std::deque<std::deque<token_base *>> &raw)
	{
		std::deque<statement_base *> body;
		kill_action({raw.begin() + 1, raw.end()}, body);
		for (auto &ptr:body)
			if (ptr->get_type() != statement_types::var_ && ptr->get_type() != statement_types::function_ && ptr->get_type() != statement_types::namespace_ && ptr->get_type() != statement_types::struct_)
				throw syntax_error("Wrong grammar for namespace definition.");
		return new statement_namespace(dynamic_cast<token_expr *>(raw.front().at(1))->get_tree().root().data(), body, raw.front().back());
	}

	statement_base *method_if::translate(const std::deque<std::deque<token_base *>> &raw)
	{
		bool have_else = false;
		std::deque<statement_base *> body;
		kill_action({raw.begin() + 1, raw.end()}, body);
		for (auto &ptr:body) {
			if (ptr->get_type() == statement_types::else_) {
				if (!have_else)
					have_else = true;
				else
					throw syntax_error("Multi Else Grammar.");
			}
		}
		if (have_else) {
			std::deque<statement_base *> body_true;
			std::deque<statement_base *> body_false;
			bool now_place = true;
			for (auto &ptr:body) {
				if (ptr->get_type() == statement_types::else_) {
					now_place = false;
					continue;
				}
				if (now_place)
					body_true.push_back(ptr);
				else
					body_false.push_back(ptr);
			}
			return new statement_ifelse(dynamic_cast<token_expr *>(raw.front().at(1))->get_tree(), body_true, body_false, raw.front().back());
		}
		else
			return new statement_if(dynamic_cast<token_expr *>(raw.front().at(1))->get_tree(), body, raw.front().back());
	}

	statement_base *method_else::translate(const std::deque<std::deque<token_base *>> &raw)
	{
		return new statement_else;
	}

	statement_base *method_switch::translate(const std::deque<std::deque<token_base *>> &raw)
	{
		std::deque<statement_base *> body;
		kill_action({raw.begin() + 1, raw.end()}, body);
		statement_block *dptr = nullptr;
		std::unordered_map<var, statement_block *> cases;
		for (auto &it:body) {
			if (it == nullptr)
				throw internal_error("Access Null Pointer.");
			else if (it->get_type() == statement_types::case_) {
				statement_case *scptr = dynamic_cast<statement_case *>(it);
				if (cases.count(scptr->get_tag()) > 0)
					throw syntax_error("Redefinition of case.");
				cases.emplace(scptr->get_tag(), scptr->get_block());
			}
			else if (it->get_type() == statement_types::default_) {
				statement_default *sdptr = dynamic_cast<statement_default *>(it);
				if (dptr != nullptr)
					throw syntax_error("Redefinition of default case.");
				dptr = sdptr->get_block();
			}
			else
				throw syntax_error("Wrong format of switch statement.");
		}
		return new statement_switch(dynamic_cast<token_expr *>(raw.front().at(1))->get_tree(), cases, dptr, raw.front().back());
	}

	statement_base *method_case::translate(const std::deque<std::deque<token_base *>> &raw)
	{
		cov::tree<token_base *> &tree = dynamic_cast<token_expr *>(raw.front().at(1))->get_tree();
		if (tree.root().data()->get_type() != token_types::value)
			throw syntax_error("Case Tag must be a constant value.");
		std::deque<statement_base *> body;
		kill_action({raw.begin() + 1, raw.end()}, body);
		return new statement_case(dynamic_cast<token_value *>(tree.root().data())->get_value(), body, raw.front().back());
	}

	statement_base *method_default::translate(const std::deque<std::deque<token_base *>> &raw)
	{
		std::deque<statement_base *> body;
		kill_action({raw.begin() + 1, raw.end()}, body);
		return new statement_default(body, raw.front().back());
	}

	statement_base *method_while::translate(const std::deque<std::deque<token_base *>> &raw)
	{
		std::deque<statement_base *> body;
		kill_action({raw.begin() + 1, raw.end()}, body);
		return new statement_while(dynamic_cast<token_expr *>(raw.front().at(1))->get_tree(), body, raw.front().back());
	}

	statement_base *method_until::translate(const std::deque<std::deque<token_base *>> &raw)
	{
		return new statement_until(dynamic_cast<token_expr *>(raw.front().at(1)), raw.front().back());
	}

	statement_base *method_loop::translate(const std::deque<std::deque<token_base *>> &raw)
	{
		std::deque<statement_base *> body;
		kill_action({raw.begin() + 1, raw.end()}, body);
		if (!body.empty() && body.back()->get_type() == statement_types::until_) {
			token_expr *expr = dynamic_cast<statement_until *>(body.back())->get_expr();
			body.pop_back();
			return new statement_loop(expr, body, raw.front().back());
		}
		else
			return new statement_loop(nullptr, body, raw.front().back());
	}

	void method_for_step::preprocess(const std::deque<std::deque<token_base *>> &raw)
	{
		cov::tree<token_base *> &tree = dynamic_cast<token_expr *>(raw.front().at(1))->get_tree();
		define_var_profile dvp;
		parse_define_var(tree, dvp);
		runtime->storage.add_record(dvp.id);
	}

	statement_base *method_for_step::translate(const std::deque<std::deque<token_base *>> &raw)
	{
		std::deque<statement_base *> body;
		kill_action({raw.begin() + 1, raw.end()}, body);
		return new statement_for(dynamic_cast<token_expr *>(raw.front().at(1))->get_tree(), dynamic_cast<token_expr *>(raw.front().at(3))->get_tree(), dynamic_cast<token_expr *>(raw.front().at(5))->get_tree(), body, raw.front().back());
	}

	void method_for::preprocess(const std::deque<std::deque<token_base *>> &raw)
	{
		cov::tree<token_base *> &tree = dynamic_cast<token_expr *>(raw.front().at(1))->get_tree();
		define_var_profile dvp;
		parse_define_var(tree, dvp);
		runtime->storage.add_record(dvp.id);
	}

	statement_base *method_for::translate(const std::deque<std::deque<token_base *>> &raw)
	{
		std::deque<statement_base *> body;
		kill_action({raw.begin() + 1, raw.end()}, body);
		cov::tree<token_base *> tree_step;
		tree_step.emplace_root_left(tree_step.root(), new token_value(number(1)));
		return new statement_for(dynamic_cast<token_expr *>(raw.front().at(1))->get_tree(), dynamic_cast<token_expr *>(raw.front().at(3))->get_tree(), tree_step, body, raw.front().back());
	}

	void method_foreach::preprocess(const std::deque<std::deque<token_base *>> &raw)
	{
		cov::tree<token_base *> &t = dynamic_cast<token_expr *>(raw.front().at(1))->get_tree();
		if (t.root().data() == nullptr)
			throw internal_error("Null pointer accessed.");
		if (t.root().data()->get_type() != token_types::id)
			throw syntax_error("Wrong grammar(foreach)");
		runtime->storage.add_record(dynamic_cast<token_id *>(t.root().data())->get_id());
	}

	statement_base *method_foreach::translate(const std::deque<std::deque<token_base *>> &raw)
	{
		cov::tree<token_base *> &t = dynamic_cast<token_expr *>(raw.front().at(1))->get_tree();
		if (t.root().data() == nullptr)
			throw internal_error("Null pointer accessed.");
		if (t.root().data()->get_type() != token_types::id)
			throw syntax_error("Wrong grammar(foreach)");
		const std::string &it = dynamic_cast<token_id *>(t.root().data())->get_id();
		std::deque<statement_base *> body;
		kill_action({raw.begin() + 1, raw.end()}, body);
		return new statement_foreach(it, dynamic_cast<token_expr *>(raw.front().at(3))->get_tree(), body, raw.front().back());
	}

	statement_base *method_break::translate(const std::deque<std::deque<token_base *>> &raw)
	{
		return new statement_break(raw.front().back());
	}

	statement_base *method_continue::translate(const std::deque<std::deque<token_base *>> &raw)
	{
		return new statement_continue(raw.front().back());
	}

	statement_base *method_function::translate(const std::deque<std::deque<token_base *>> &raw)
	{
		cov::tree<token_base *> &t = dynamic_cast<token_expr *>(raw.front().at(1))->get_tree();
		if (t.root().data() == nullptr)
			throw internal_error("Null pointer accessed.");
		if (t.root().data()->get_type() != token_types::signal || dynamic_cast<token_signal *>(t.root().data())->get_signal() != signal_types::fcall_)
			throw syntax_error("Wrong grammar for function definition.");
		if (t.root().left().data() == nullptr)
			throw internal_error("Null pointer accessed.");
		if (t.root().left().data()->get_type() != token_types::id)
			throw syntax_error("Wrong grammar for function definition.");
		if (t.root().right().data() == nullptr)
			throw internal_error("Null pointer accessed.");
		if (t.root().right().data()->get_type() != token_types::arglist)
			throw syntax_error("Wrong grammar for function definition.");
		std::string name = dynamic_cast<token_id *>(t.root().left().data())->get_id();
		std::deque<std::string> args;
		for (auto &it:dynamic_cast<token_arglist *>(t.root().right().data())->get_arglist()) {
			if (it.root().data() == nullptr)
				throw internal_error("Null pointer accessed.");
			if (it.root().data()->get_type() != token_types::id)
				throw syntax_error("Wrong grammar for function definition.");
			const std::string &str = dynamic_cast<token_id *>(it.root().data())->get_id();
			for (auto &it:args)
				if (it == str)
					throw syntax_error("Redefinition of function argument.");
			args.push_back(str);
		}
		std::deque<statement_base *> body;
		kill_action({raw.begin() + 1, raw.end()}, body);
		return new statement_function(name, args, body, raw.front().back());
	}

	statement_base *method_return::translate(const std::deque<std::deque<token_base *>> &raw)
	{
		return new statement_return(dynamic_cast<token_expr *>(raw.front().at(1))->get_tree(), raw.front().back());
	}

	statement_base *method_return_no_value::translate(const std::deque<std::deque<token_base *>> &raw)
	{
		cov::tree<token_base *> tree;
		tree.emplace_root_left(tree.root(), new token_value(number(0)));
		return new statement_return(tree, raw.front().back());
	}

	statement_base *method_struct::translate(const std::deque<std::deque<token_base *>> &raw)
	{
		cov::tree<token_base *> &t = dynamic_cast<token_expr *>(raw.front().at(1))->get_tree();
		if (t.root().data() == nullptr)
			throw internal_error("Null pointer accessed.");
		if (t.root().data()->get_type() != token_types::id)
			throw syntax_error("Wrong grammar for struct definition.");
		std::string name = dynamic_cast<token_id *>(t.root().data())->get_id();
		std::deque<statement_base *> body;
		kill_action({raw.begin() + 1, raw.end()}, body);
		for (auto &ptr:body) {
			switch (ptr->get_type()) {
			default:
				throw syntax_error("Wrong grammar for struct definition.");
			case statement_types::var_:
				break;
			case statement_types::function_:
				static_cast<statement_function *>(ptr)->set_mem_fn();
				break;
			}
		}
		return new statement_struct(name, body, raw.front().back());
	}

	statement_base *method_try::translate(const std::deque<std::deque<token_base *>> &raw)
	{
		std::deque<statement_base *> body;
		kill_action({raw.begin() + 1, raw.end()}, body);
		std::string name;
		std::deque<statement_base *> tbody, cbody;
		bool founded = false;
		for (auto &ptr:body) {
			if (ptr->get_type() == statement_types::catch_) {
				name = static_cast<statement_catch *>(ptr)->get_name();
				founded = true;
				continue;
			}
			if (founded)
				cbody.push_back(ptr);
			else
				tbody.push_back(ptr);
		}
		if (!founded)
			throw syntax_error("Wrong grammar for try statement.");
		return new statement_try(name, tbody, cbody, raw.front().back());
	}

	statement_base *method_catch::translate(const std::deque<std::deque<token_base *>> &raw)
	{
		cov::tree<token_base *> &t = dynamic_cast<token_expr *>(raw.front().at(1))->get_tree();
		if (t.root().data() == nullptr)
			throw internal_error("Null pointer accessed.");
		if (t.root().data()->get_type() != token_types::id)
			throw syntax_error("Wrong grammar for catch statement.");
		return new statement_catch(dynamic_cast<token_id *>(t.root().data())->get_id(), raw.front().back());
	}

	statement_base *method_throw::translate(const std::deque<std::deque<token_base *>> &raw)
	{
		return new statement_throw(dynamic_cast<token_expr *>(raw.front().at(1))->get_tree(), raw.front().back());
	}

	void kill_action(std::deque<std::deque<token_base *>> lines, std::deque<statement_base *> &statements, bool raw)
	{
		std::deque<std::deque<token_base *>> tmp;
		method_base *method = nullptr;
		token_endline *endsig = nullptr;
		int level = 0;
		for (auto &line:lines) {
			endsig = dynamic_cast<token_endline *>(line.back());
			try {
				if (raw) {
					process_brackets(line);
					kill_brackets(line);
					kill_expr(line);
				}
				method_base *m = translator.match(line);
				switch (m->get_type()) {
				case method_types::null:
					throw syntax_error("Null type of grammar.");
					break;
				case method_types::single: {
					if (level > 0) {
						if (m->get_target_type() == statement_types::end_) {
							runtime->storage.remove_set();
							runtime->storage.remove_domain();
							--level;
						}
						if (level == 0) {
							statements.push_back(method->translate(tmp));
							tmp.clear();
							method = nullptr;
						}
						else
							tmp.push_back(line);
					}
					else
						statements.push_back(m->translate({line}));
				}
				break;
				case method_types::block: {
					if (level == 0)
						method = m;
					++level;
					runtime->storage.add_domain();
					runtime->storage.add_set();
					m->preprocess({line});
					tmp.push_back(line);
				}
				break;
				case method_types::jit_command:
					m->translate({line});
					break;
				}
			}
			catch (const cs::exception &e) {
				throw e;
			}
			catch (const std::exception &e) {
				throw exception(endsig->get_num(), endsig->get_file(), endsig->get_code(), e.what());
			}
		}
		if (level != 0)
			throw syntax_error("Lack of the \"end\" signal.");
	}

	void translate_into_statements(std::deque<token_base *> &tokens, std::deque<statement_base *> &statements)
	{
		std::deque<std::deque<token_base *>> lines;
		std::deque<token_base *> tmp;
		for (auto &ptr:tokens) {
			tmp.push_back(ptr);
			if (ptr != nullptr && ptr->get_type() == token_types::endline) {
				if (tmp.size() > 1)
					lines.push_back(tmp);
				tmp.clear();
			}
		}
		if (tmp.size() > 1)
			lines.push_back(tmp);
		tmp.clear();
		kill_action(lines, statements, true);
	}
}
