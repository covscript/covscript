/*
 * Covariant Script Instance
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
 * Copyright (C) 2017-2026 Michael Lee(李登淳)
 *
 * This software is registered with the National Copyright Administration
 * of the People's Republic of China(Registration Number: 2020SR0408026)
 * and is protected by the Copyright Law of the People's Republic of China.
 *
 * Email:   mikecovlee@163.com
 * Github:  https://github.com/mikecovlee
 * Website: http://covscript.org.cn
 */
#include <covscript/impl/system.hpp>
#include <covscript/covscript.hpp>

namespace cs
{
	const std::string &statement_base::get_file_path() const noexcept
	{
		return context->file_path;
	}

	const std::string &statement_base::get_package_name() const noexcept
	{
		return context->package_name;
	}

	const std::string &statement_base::get_raw_code() const noexcept
	{
		return context->get_file_line(line_num);
	}

	namespace_t instance_type::source_import(const std::string &path)
	{
		const std::string &module_key = cs_impl::file_system::normalize_path(path);
		if (context->compiler->modules.count(module_key) > 0)
			return context->compiler->modules[module_key];
		if (cs_impl::file_system::is_exe(path))
		{
			// is extension file
			namespace_t module = std::make_shared<extension>(path);
			context->compiler->modules.emplace(module_key, module);
			return module;
		}
		else
		{
			// is package file
			context_t rt = create_subcontext(context);
			namespace_t module = std::make_shared<name_space>();
			context->compiler->modules.emplace(module_key, module);
			try
			{
				{
					context_swap_guard guard(*rt->compiler, rt.get());
					rt->instance->compile(path);
					rt->instance->interpret();
				}
				*module = *rt->instance->storage.get_namespace();
				// The context now owns the subcontext (breaking the module cycle).
				context->subcontexts.push_back(rt);
				return module;
			}
			catch (...)
			{
				context->compiler->modules.erase(module_key);
				throw;
			}
		}
	}

	namespace_t instance_type::import(const std::string &path, const std::string &name)
	{
		std::vector<std::string> collection;
		{
			std::string tmp;
			for (auto &ch : path)
			{
				if (ch == cs::path_delimiter)
				{
					collection.push_back(tmp);
					tmp.clear();
				}
				else
					tmp.push_back(ch);
			}
			collection.push_back(tmp);
		}
		std::exception_ptr eptr = nullptr;
		for (auto &it : collection)
		{
			std::string package_path = it + path_separator + name;
			const std::string &module_key = cs_impl::file_system::normalize_path(package_path);
			if (context->compiler->modules.count(module_key) > 0)
				return context->compiler->modules[module_key];
			if (std::ifstream(package_path + ".csp"))
			{
				context_t rt = create_subcontext(context);
				rt->compiler->import_csym(package_path + ".csp", package_path + ".csym");
				namespace_t module = std::make_shared<name_space>();
				context->compiler->modules.emplace(module_key, module);
				try
				{
					{
						context_swap_guard guard(*rt->compiler, rt.get());
						rt->instance->compile(package_path + ".csp");
						rt->instance->interpret();
					}
					if (rt->package_name.empty())
						throw runtime_error("The imported file is not a package (it has no 'package' declaration)");
					if (rt->package_name != name)
						throw runtime_error("The package name declared in the file does not match the file name");
					*module = *rt->instance->storage.get_namespace();
					// The context now owns the subcontext (breaking the module cycle).
					context->subcontexts.push_back(rt);
					return module;
				}
				catch (...)
				{
					context->compiler->modules.erase(module_key);
					throw;
				}
			}
			else if (std::ifstream(package_path + ".cse"))
			{
				try
				{
					namespace_t module = std::make_shared<extension>(package_path + ".cse");
					context->compiler->modules.emplace(module_key, module);
					return module;
				}
				catch (...)
				{
					eptr = std::current_exception();
				}
			}
		}
		if (eptr != nullptr)
			std::rethrow_exception(eptr);
		throw fatal_error("No such file or directory");
	}

	void instance_type::compile(const std::string &path)
	{
		// Read from file
		context->file_path = path;
		std::ifstream in(path, std::ios::binary);
		if (!in.is_open())
			throw fatal_error(path + ": No such file or directory");
		compile(in);
	}

	void instance_type::compile(std::istream &in)
	{
		// Install this context's process (module imports run code here).
		process_run_scope scope(context);
		// Replace the previous program: owning statements and their token arena
		// are freed here.
		release_statements();
		// Create the token arena for this compilation; all tokens produced by the
		// lexer/parser/codegen are allocated into it.
		m_unit = std::make_shared<compile_unit>();
		std::shared_ptr<compile_unit> saved_unit = context->current_unit;
		context->current_unit = m_unit;
		// Roll back lambdas registered by a failed compilation.
		std::size_t store_base = functions.size();
		// Read from file
		std::deque<char> buff;
		for (int ch = in.get(); in; ch = in.get())
			buff.push_back(ch);
		std::deque<std::deque<token_base *>> ast;
		// Constant pool scoped to this unit: a nested compile must not wipe it.
		std::size_t pool_base = context->compiler->save_pool();
		value_guard<std::size_t> loop_guard(context->compiler->loop_depth, 0);
		// Scope the import FIFO to this unit so nested compiles don't disturb it.
		std::size_t import_base = context->compiler->import_results.size();
		try
		{
			context->compiler->build_ast(buff, ast);
			context->compiler->code_gen(ast, statements);
			context->compiler->utilize_metadata();
		}
		catch (...)
		{
			context->compiler->restore_pool(pool_base);
			context->compiler->clear_import_results(import_base);
			context->current_unit = saved_unit;
			// Free half-generated statements so a failed program can't linger.
			statement_base::delete_children(statements);
			functions.resize(store_base);
			release_unit();
			throw;
		}
		context->compiler->restore_pool(pool_base);
		context->compiler->clear_import_results(import_base);
		context->current_unit = saved_unit;
	}

	// Only the outermost interpret clears the value stack.
	static thread_local std::size_t interpret_depth = 0;

	void instance_type::interpret()
	{
		process_run_scope scope(context);
		if (interpret_depth == 0)
		{
			// Defensive: stale frames from an interrupted run (RAII balances the
			// stack normally). none: leave, warning: clear, strict: fail.
			if (!current_process->stack.empty())
			{
				cs_impl::debug_guard(
				    "[interpret] function value stack is not empty at program entry; "
				    "stale frames from an interrupted run");
				if (cs_impl::get_debug_mode() == cs_impl::debug_mode::warning)
					while (!current_process->stack.empty())
						current_process->stack.pop_no_return();
			}
#ifdef CS_DEBUGGER
			while (!current_process->stack_backtrace.empty())
				current_process->stack_backtrace.pop_no_return();
#endif
		}
		value_guard<std::size_t> depth_guard(interpret_depth, interpret_depth + 1);
		// Run the instruction
		for (auto &ptr : statements)
		{
			try
			{
				ptr->run();
			}
			catch (const lang_error &le)
			{
				if (le.has_location())
					throw exception(le.line(), le.file(), le.code(), std::string("Uncaught exception: ") + le.what());
				throw fatal_error(std::string("Uncaught exception: ") + le.what());
			}
			catch (const cs::exception &)
			{
				throw;
			}
			catch (const std::exception &e)
			{
				throw exception(ptr->get_line_num(), ptr->get_file_path(), ptr->get_raw_code(), exception_message(e));
			}
		}
	}

	void instance_type::dump_ast(std::ostream &stream)
	{
		stream << "< Covariant Script AST Dump >\n< BeginMetaData >\n< Version: " << current_process->version
		       << " >\n< Standard Version: "
		       << current_process->std_version
		       << " >\n< Import Path: \""
		       << current_process->import_path
		       << "\" >\n";
#ifdef COVSCRIPT_PLATFORM_WIN32
		stream << "< Platform: Win32 >\n";
#else
		stream << "< Platform: Unix >\n";
#endif
		stream << "< EndMetaData >\n";
		for (auto &ptr : statements)
			ptr->dump(stream);
		stream << std::flush;
	}

	void instance_type::check_declar_var(tree_type<token_base *>::iterator it, bool regist)
	{
		if (it.data() == nullptr)
			throw internal_error("Null pointer accessed.");
		if (it.data()->get_type() == token_types::parallel)
		{
			auto &parallel_list = static_cast<token_parallel *>(it.data())->get_parallel();
			for (auto &t : parallel_list)
				check_declar_var(t.root(), regist);
		}
		else
		{
			token_base *root = it.data();
			if (root == nullptr || root->get_type() != token_types::id)
				throw runtime_error("Invalid variable declaration: expected an identifier");
			if (regist)
				storage.add_record(static_cast<token_id *>(root)->get_id().get_id());
		}
	}

	// A constant's RHS must be a folded token_value or a lambda (token_lambda).
	static bool is_constant_rhs(token_base *rhs)
	{
		return rhs != nullptr && (rhs->get_type() == token_types::value || rhs->get_type() == token_types::lambda);
	}

	void instance_type::check_define_var(tree_type<token_base *>::iterator it, bool regist, bool constant)
	{
		if (it.data() == nullptr)
			throw internal_error("Null pointer accessed.");
		if (it.data()->get_type() == token_types::parallel)
		{
			auto &parallel_list = static_cast<token_parallel *>(it.data())->get_parallel();
			for (auto &t : parallel_list)
				check_define_var(t.root(), regist, constant);
		}
		else
		{
			token_base *root = it.data();
			if (root == nullptr)
				throw internal_error("Null pointer accessed.");
			if (root->get_type() != token_types::signal)
				throw runtime_error("Invalid variable definition: expected '<id> = <value>' or a structured binding");
			switch (static_cast<token_signal *>(root)->get_signal())
			{
				case signal_types::asi_:
				{
					token_base *left = it.left().data();
					token_base *right = it.right().data();
					if (left == nullptr || right == nullptr || left->get_type() != token_types::id)
						throw runtime_error("Invalid variable definition: the left-hand side must be an identifier");
					if (constant && !is_constant_rhs(right))
						throw runtime_error("A constant must be initialized with a constant value");
					if (regist)
						storage.add_record(static_cast<token_id *>(left)->get_id().get_id());
					break;
				}
				case signal_types::bind_:
				{
					token_base *right = it.right().data();
					if (constant && !is_constant_rhs(right))
						throw runtime_error("A constant structured binding must be initialized with a constant value");
					check_define_structured_binding(it.left(), regist);
					break;
				}
				default:
					throw runtime_error("Invalid variable definition: expected '<id> = <value>' or a structured binding");
			}
		}
	}

	var instance_type::fold_constant(tree_type<token_base *>::iterator rhs, bool constant)
	{
		token_base *ptr = rhs.data();
		return constant && ptr != nullptr && ptr->get_type() == token_types::value
		           ? static_cast<token_value *>(ptr)->get_value()
		           : parse_expr(rhs);
	}

	void instance_type::parse_define_var(tree_type<token_base *>::iterator it, bool constant, bool link)
	{
		if (it.data()->get_type() == token_types::parallel)
		{
			auto &parallel_list = static_cast<token_parallel *>(it.data())->get_parallel();
			for (auto &t : parallel_list)
				parse_define_var(t.root(), constant, link);
		}
		else
		{
			token_base *root = it.data();
			switch (static_cast<token_signal *>(root)->get_signal())
			{
				case signal_types::asi_:
				{
					var val = fold_constant(it.right(), constant);
					storage.add_var_no_return(static_cast<token_id *>(it.left().data())->get_id(),
					                          constant || link ? val : copy(val),
					                          constant);
					break;
				}
				case signal_types::bind_:
				{
					parse_define_structured_binding(it, constant, link);
					break;
				}
				default:
					throw runtime_error("Invalid variable definition: expected '<id> = <value>' or a structured binding");
			}
		}
	}

	void
	instance_type::check_define_structured_binding(tree_type<token_base *>::iterator it, bool regist)
	{
		for (auto &p_it : static_cast<token_parallel *>(it.data())->get_parallel())
		{
			token_base *root = p_it.root().data();
			if (root == nullptr)
				throw runtime_error("Invalid structured binding: empty binding target");
			if (root->get_type() != token_types::id)
			{
				if (root->get_type() == token_types::parallel)
					check_define_structured_binding(p_it.root(), regist);
				else
					throw runtime_error("Invalid structured binding: each target must be an identifier");
			}
			else if (regist)
				storage.add_record(static_cast<token_id *>(root)->get_id().get_id());
		}
	}

	void
	instance_type::parse_define_structured_binding(tree_type<token_base *>::iterator it, bool constant, bool link)
	{
		std::function<void(tree_type<token_base *>::iterator, const var &)> process;
		process = [&process, this, constant, link](tree_type<token_base *>::iterator it, const var &val)
		{
			auto &pl = static_cast<token_parallel *>(it.data())->get_parallel();
			if (!val.is_type_of<array>())
				throw runtime_error("Structured binding requires an array on the right-hand side");
			auto &arr = val.const_val<array>();
			if (pl.size() != arr.size())
				throw runtime_error("Structured binding mismatch: the number of variables does not match the number of array elements");
			for (std::size_t i = 0; i < pl.size(); ++i)
			{
				if (pl[i].root().data()->get_type() == token_types::parallel)
					process(pl[i].root(), arr[i]);
				else
					storage.add_var_no_return(static_cast<token_id *>(pl[i].root().data())->get_id(),
					                          constant || link ? arr[i] : copy(arr[i]), constant);
			}
		};
		var val = fold_constant(it.right(), constant);
		process(it.left(), val);
	}

	void instance_type::parse_using(tree_type<token_base *>::iterator it, bool override)
	{
		if (it.data()->get_type() == token_types::parallel)
		{
			auto &parallel_list = static_cast<token_parallel *>(it.data())->get_parallel();
			for (auto &t : parallel_list)
				parse_using(t.root(), override);
		}
		else
		{
			var ns = context->instance->parse_expr(it, true);
			if (ns.is_type_of<namespace_t>())
				context->instance->storage.involve_domain(ns.const_val<namespace_t>()->get_domain(), override);
			else
				throw runtime_error("Invalid 'using' statement: the target must be a namespace");
		}
	}

	repl::repl(context_t c)
	    : context(std::move(c))
	{
		context->file_path = "<REPL_ENV>";
		context->compiler->fold_expr = false;
		context->instance->storage.add_buildin_var("quit", cs::make_cni([]()
		{
			int code = 0;
			cs::current_process->on_process_exit.touch(&code);
		}));
	}

	void repl::interpret(const string &code, std::deque<token_base *> &line)
	{
		statement_base *sptr = nullptr;
		// Roll back lambdas registered by a failed line.
		std::size_t store_base = context->instance->functions.size();
		try
		{
			method_base *m = context->compiler->match_method(line);
			switch (m->get_type())
			{
				case method_types::null:
					throw runtime_error("Unrecognized statement");
					break;
				case method_types::single:
				{
					if (!methods.empty())
					{
						method_base *expected_method = nullptr;
						if (m->get_target_type() == statement_types::end_)
						{
							context->instance->storage.remove_set();
							domain_type domain = std::move(context->instance->storage.get_domain());
							context->instance->storage.remove_domain();
							// Pop before postprocess so a throw can't double-pop.
							expected_method = methods.top();
							methods.pop();
							expected_method->postprocess(context.get(), domain);
						}
						if (methods.empty())
						{
							if (m->get_target_type() == statement_types::end_)
								sptr = static_cast<method_end *>(m)->translate_end(expected_method, context.get(), tmp,
								                                                   line);
							else
								sptr = expected_method->translate(context.get(), tmp);
							// Loop closed: release depth after translation (break/continue
							// in the body still need loop_depth > 0 while it re-translates).
							if (expected_method != nullptr && compiler_type::is_loop_block(expected_method))
								--context->compiler->loop_depth;
							tmp.clear();
						}
						else
						{
							// Deferred block: release its depth now to keep the scan balanced.
							if (m->get_target_type() == statement_types::end_ && compiler_type::is_loop_block(expected_method))
								--context->compiler->loop_depth;
							m->preprocess(context.get(), {line});
							tmp.push_back(line);
						}
					}
					else
					{
						if (m->get_target_type() == statement_types::end_)
							throw runtime_error("Unexpected 'end': there is no open block to close");
						else
						{
							m->preprocess(context.get(), {line});
							sptr = m->translate(context.get(), {line});
						}
					}
				}
				break;
				case method_types::block:
				{
					methods.push(m);
					context->instance->storage.add_domain();
					context->instance->storage.add_set();
					if (compiler_type::is_loop_block(m))
						++context->compiler->loop_depth;
					methods.top()->preprocess(context.get(), {line});
					tmp.push_back(line);
				}
				break;
				case method_types::jit_command:
					m->translate(context.get(), {line});
					break;
			}
			if (sptr != nullptr)
			{
				echo ? sptr->repl_run() : sptr->run();
				delete sptr;
				// The catch handlers below also `delete sptr`; null it out so a
				// throw after this point cannot double-free the statement.
				sptr = nullptr;
			}
			// The top-level statement is complete: release its token arena (a
			// function retained in the store keeps it alive via its m_unit).
			if (methods.empty())
			{
				context->current_unit = nullptr;
				release_unit();
			}
		}
		catch (const lang_error &le)
		{
			delete sptr;
			reset_status();
			context->instance->functions.resize(store_base);
			context->compiler->utilize_metadata();
			context->instance->storage.clear_set();
			if (le.has_location())
				throw exception(le.line(), le.file(), le.code(), std::string("Uncaught exception: ") + le.what());
			throw fatal_error(std::string("Uncaught exception: ") + le.what());
		}
		catch (const cs::exception &)
		{
			delete sptr;
			reset_status();
			context->instance->functions.resize(store_base);
			context->compiler->utilize_metadata();
			context->instance->storage.clear_set();
			throw;
		}
		catch (const std::exception &e)
		{
			delete sptr;
			reset_status();
			context->compiler->utilize_metadata();
			context->instance->storage.clear_set();
			throw exception(line_num, context->file_path, code, exception_message(e));
		}
		context->compiler->utilize_metadata();
		context->instance->storage.clear_set();
	}

	void repl::run(const string &code)
	{
		if (code.empty())
			return;
		process_run_scope scope(context);
		std::deque<char> buff;
		for (auto &ch : code)
			buff.push_back(ch);
		// A new top-level statement starts when no block is open: record the
		// import FIFO base and start a fresh token arena for the whole statement.
		if (methods.empty())
		{
			import_base = context->compiler->import_results.size();
			m_unit = std::make_shared<compile_unit>();
			context->current_unit = m_unit;
		}
		try
		{
			std::deque<std::deque<token_base *>> ast;
			context->compiler->clear_metadata();
			context->compiler->build_line(buff, ast, line_num, encoding);
			for (auto &line : ast)
				interpret(code, line);
		}
		catch (const lang_error &le)
		{
			reset_status();
			if (le.has_location())
				throw exception(le.line(), le.file(), le.code(), std::string("Uncaught exception: ") + le.what());
			throw fatal_error(std::string("Uncaught exception: ") + le.what());
		}
		catch (const cs::exception &)
		{
			reset_status();
			throw;
		}
		catch (const std::exception &e)
		{
			reset_status();
			throw exception(line_num, context->file_path, code, exception_message(e));
		}
	}

	void repl::exec(const string &code)
	{
		// Preprocess
		++line_num;
		int mode = 0;
		for (auto &ch : code)
		{
			if (mode == 0)
			{
				if (!std::isspace(static_cast<unsigned char>(ch)))
				{
					switch (ch)
					{
						case '#':
							context->file_buff.emplace_back();
							return;
						case '@':
							mode = 1;
							break;
						default:
							mode = -1;
					}
				}
			}
			else if (mode == 1)
			{
				if (!std::isspace(static_cast<unsigned char>(ch)))
					cmd_buff.push_back(ch);
			}
			else
				break;
		}
		switch (mode)
		{
			default:
				break;
			case 0:
				return;
			case 1:
			{
				std::string cmd;
				std::swap(cmd_buff, cmd);
				if (cmd == "begin" && !multi_line)
				{
					multi_line = true;
					context->file_buff.emplace_back();
				}
				else if (cmd == "end" && multi_line)
				{
					multi_line = false;
					std::string line;
					std::swap(line_buff, line);
					this->run(line);
				}
				else
				{
					auto pos = cmd.find(':');
					std::string arg;
					if (pos != std::string::npos)
					{
						arg = cmd.substr(pos + 1);
						cmd = cmd.substr(0, pos);
					}
					if (cmd == "exit")
					{
						int code = 0;
						current_process->on_process_exit.touch(&code);
					}
					else if (cmd == "charset")
					{
						if (arg == "ascii")
							encoding = charset::ascii;
						else if (arg == "utf8")
							encoding = charset::utf8;
						else if (arg == "gbk")
							encoding = charset::gbk;
						else
							throw exception(line_num, context->file_path, "@" + cmd + ": " + arg,
							                "Unavailable encoding.");
					}
					else if (cmd == "require")
					{
						std::string version_str = CS_GET_VERSION_STR(COVSCRIPT_STD_VERSION);
						if (arg > version_str)
							throw exception(line_num, context->file_path, "@" + cmd + ": " + arg,
							                "Newer Language Standard required: " + arg + ", now on " + version_str);
					}
					else
						throw exception(line_num, context->file_path, "@" + cmd + (arg.empty() ? "" : ": " + arg),
						                "Invalid preprocessor command");
					context->file_buff.emplace_back();
				}
				return;
			}
		}
		if (multi_line)
		{
			context->file_buff.emplace_back();
			if (!line_buff.empty())
				line_buff.append("\n");
			line_buff.append(code);
		}
		else
		{
			context->file_buff.emplace_back(code);
			this->run(code);
		}
	}
} // namespace cs
