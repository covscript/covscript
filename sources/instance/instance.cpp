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
* Copyright (C) 2017-2024 Michael Lee(李登淳)
*
* This software is registered with the National Copyright Administration
* of the People's Republic of China(Registration Number: 2020SR0408026)
* and is protected by the Copyright Law of the People's Republic of China.
*
* Email:   lee@covariant.cn, mikecovlee@163.com
* Github:  https://github.com/mikecovlee
* Website: http://covscript.org.cn
*/
#include <covscript_impl/system.hpp>
#include <covscript/covscript.hpp>

namespace cs {
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
		return context->file_buff.at(line_num - 1);
	}

	namespace_t instance_type::source_import(const std::string &path)
	{
		if (context->compiler->modules.count(path) > 0)
			return context->compiler->modules[path];
		if (cs_impl::file_system::is_exe(path)) {
			// is extension file
			namespace_t module = std::make_shared<extension>(path);
			context->compiler->modules.emplace(path, module);
			return module;
		}
		else {
			// is package file
			context_t rt = create_subcontext(context);
			rt->compiler->swap_context(rt);
			try {
				rt->instance->compile(path);
			}
			catch (...) {
				context->compiler->swap_context(context);
				throw;
			}
			context->compiler->swap_context(context);
			rt->instance->interpret();
			namespace_t module = std::make_shared<name_space>(*rt->instance->storage.get_namespace());
			context->compiler->modules.emplace(path, module);
			return module;
		}
	}

	namespace_t instance_type::import(const std::string &path, const std::string &name)
	{
		std::vector<std::string> collection;
		{
			std::string tmp;
			for (auto &ch: path) {
				if (ch == cs::path_delimiter) {
					collection.push_back(tmp);
					tmp.clear();
				}
				else
					tmp.push_back(ch);
			}
			collection.push_back(tmp);
		}
		for (auto &it: collection) {
			std::string package_path = it + path_separator + name;
			if (context->compiler->modules.count(package_path) > 0)
				return context->compiler->modules[package_path];
			if (std::ifstream(package_path + ".csp")) {
				context_t rt = create_subcontext(context);
				rt->compiler->import_csym(package_path + ".csp", package_path + ".csym");
				rt->compiler->swap_context(rt);
				try {
					rt->instance->compile(package_path + ".csp");
				}
				catch (...) {
					context->compiler->swap_context(context);
					throw;
				}
				context->compiler->swap_context(context);
				rt->instance->interpret();
				if (rt->package_name.empty())
					throw runtime_error("Target file is not a package.");
				if (rt->package_name != name)
					throw runtime_error("Package name is different from file name.");
				namespace_t module = std::make_shared<name_space>(*rt->instance->storage.get_namespace());
				context->compiler->modules.emplace(package_path, module);
				return module;
			}
			else if (std::ifstream(package_path + ".cse")) {
				namespace_t module = std::make_shared<extension>(package_path + ".cse");
				context->compiler->modules.emplace(package_path, module);
				return module;
			}
		}
		throw fatal_error("No such file or directory.");
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
		// Read from file
		std::deque<char> buff;
		for (int ch = in.get(); in; ch = in.get())
			buff.push_back(ch);
		std::deque<std::deque<token_base *>> ast;
		// Compile
		context->compiler->clear_metadata();
		context->compiler->build_ast(buff, ast);
		context->compiler->code_gen(ast, statements);
		context->compiler->utilize_metadata();
	}

	void instance_type::interpret()
	{
		// Run the instruction
		for (auto &ptr: statements) {
			try {
				ptr->run();
			}
			catch (const lang_error &le) {
				throw fatal_error(std::string("Uncaught exception: ") + le.what());
			}
			catch (const cs::exception &e) {
				throw e;
			}
			catch (const std::exception &e) {
				throw exception(ptr->get_line_num(), ptr->get_file_path(), ptr->get_raw_code(), e.what());
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
		for (auto &ptr: statements)
			ptr->dump(stream);
		stream << std::flush;
	}

	void instance_type::check_declar_var(tree_type<token_base *>::iterator it, bool regist)
	{
		if (it.data() == nullptr)
			throw internal_error("Null pointer accessed.");
		if (it.data()->get_type() == token_types::parallel) {
			auto &parallel_list = static_cast<token_parallel *>(it.data())->get_parallel();
			for (auto &t: parallel_list)
				check_declar_var(t.root(), regist);
		}
		else {
			token_base *root = it.data();
			if (root == nullptr || root->get_type() != token_types::id)
				throw runtime_error("Wrong grammar for variable declaration.");
			if (regist)
				storage.add_record(static_cast<token_id *>(root)->get_id());
		}
	}

	void instance_type::check_define_var(tree_type<token_base *>::iterator it, bool regist, bool constant)
	{
		if (it.data() == nullptr)
			throw internal_error("Null pointer accessed.");
		if (it.data()->get_type() == token_types::parallel) {
			auto &parallel_list = static_cast<token_parallel *>(it.data())->get_parallel();
			for (auto &t: parallel_list)
				check_define_var(t.root(), regist, constant);
		}
		else {
			token_base *root = it.data();
			if (root == nullptr)
				throw internal_error("Null pointer accessed.");
			if (root->get_type() != token_types::signal)
				throw runtime_error("Wrong grammar for variable definition(1).");
			switch (static_cast<token_signal *>(root)->get_signal()) {
			case signal_types::asi_: {
				token_base *left = it.left().data();
				token_base *right = it.right().data();
				if (left == nullptr || right == nullptr || left->get_type() != token_types::id)
					throw runtime_error("Wrong grammar for variable definition(2).");
				if (constant && right->get_type() != token_types::value)
					throw runtime_error("Wrong grammar for constant variable definition(3).");
				if (regist)
					storage.add_record(static_cast<token_id *>(left)->get_id());
				break;
			}
			case signal_types::bind_: {
				token_base *right = it.right().data();
				if (constant && (right == nullptr || right->get_type() != token_types::value))
					throw runtime_error("Wrong grammar for constant variable definition(4).");
				check_define_structured_binding(it.left(), regist);
				break;
			}
			default:
				throw runtime_error("Wrong grammar for variable definition(5).");
			}
		}
	}

	void instance_type::parse_define_var(tree_type<token_base *>::iterator it, bool constant, bool link)
	{
		if (it.data()->get_type() == token_types::parallel) {
			auto &parallel_list = static_cast<token_parallel *>(it.data())->get_parallel();
			for (auto &t: parallel_list)
				parse_define_var(t.root(), constant, link);
		}
		else {
			token_base *root = it.data();
			switch (static_cast<token_signal *>(root)->get_signal()) {
			case signal_types::asi_: {
				const var &val = constant ? static_cast<token_value *>(it.right().data())->get_value() : parse_expr(
				                     it.right());
				storage.add_var_no_return(static_cast<token_id *>(it.left().data())->get_id(),
				                          constant || link ? val : copy(val),
				                          constant);
				break;
			}
			case signal_types::bind_: {
				parse_define_structured_binding(it, constant, link);
				break;
			}
			default:
				throw runtime_error("Wrong grammar for variable definition(6).");
			}
		}
	}

	void
	instance_type::check_define_structured_binding(tree_type<token_base *>::iterator it, bool regist)
	{
		for (auto &p_it: static_cast<token_parallel *>(it.data())->get_parallel()) {
			token_base *root = p_it.root().data();
			if (root == nullptr)
				throw runtime_error("Wrong grammar for variable definition(7).");
			if (root->get_type() != token_types::id) {
				if (root->get_type() == token_types::parallel)
					check_define_structured_binding(p_it.root(), regist);
				else
					throw runtime_error("Wrong grammar for variable definition(8).");
			}
			else if (regist)
				storage.add_record(static_cast<token_id *>(root)->get_id());
		}
	}

	void
	instance_type::parse_define_structured_binding(tree_type<token_base *>::iterator it, bool constant, bool link)
	{
		std::function<void(tree_type<token_base *>::iterator, const var &)> process;
		process = [&process, this, constant, link](tree_type<token_base *>::iterator it, const var &val) {
			auto &pl = static_cast<token_parallel *>(it.data())->get_parallel();
			if (val.type() != typeid(array))
				throw runtime_error("Only support structured binding with array while variable definition.");
			auto &arr = val.const_val<array>();
			if (pl.size() != arr.size())
				throw runtime_error("Unmatched structured binding while variable definition.");
			for (std::size_t i = 0; i < pl.size(); ++i) {
				if (pl[i].root().data()->get_type() == token_types::parallel)
					process(pl[i].root(), arr[i]);
				else
					storage.add_var_no_return(static_cast<token_id *>(pl[i].root().data())->get_id(),
					                          constant || link ? arr[i] : copy(arr[i]), constant);
			}
		};
		const var &val = constant ? static_cast<token_value *>(it.right().data())->get_value() : parse_expr(it.right());
		process(it.left(), val);
	}

	void instance_type::parse_using(tree_type<token_base *>::iterator it, bool override)
	{
		if (it.data()->get_type() == token_types::parallel) {
			auto &parallel_list = static_cast<token_parallel *>(it.data())->get_parallel();
			for (auto &t: parallel_list)
				parse_using(t.root());
		}
		else {
			var ns = context->instance->parse_expr(it, true);
			if (ns.type() == typeid(namespace_t))
				context->instance->storage.involve_domain(ns.const_val<namespace_t>()->get_domain(), override);
			else
				throw runtime_error("Only support involve namespace.");
		}
	}

	repl::repl(context_t c) : context(std::move(c))
	{
		context->file_path = "<REPL_ENV>";
		context->compiler->fold_expr = false;
		context->instance->storage.add_buildin_var("quit", cs::make_cni([]() {
			int code = 0;
			cs::current_process->on_process_exit.touch(&code);
		}));
	}

	void repl::interpret(const string &code, std::deque<token_base *> &line)
	{
		statement_base *sptr = nullptr;
		try {
			method_base *m = context->compiler->match_method(line);
			switch (m->get_type()) {
			case method_types::null:
				throw runtime_error("Null type of grammar.");
				break;
			case method_types::single: {
				if (!methods.empty()) {
					method_base *expected_method = nullptr;
					if (m->get_target_type() == statement_types::end_) {
						context->instance->storage.remove_set();
						domain_type domain = std::move(context->instance->storage.get_domain());
						context->instance->storage.remove_domain();
						methods.top()->postprocess(context, domain);
						expected_method = methods.top();
						methods.pop();
					}
					if (methods.empty()) {
						if (m->get_target_type() == statement_types::end_)
							sptr = static_cast<method_end *>(m)->translate_end(expected_method, context, tmp,
							        line);
						else
							sptr = expected_method->translate(context, tmp);
						tmp.clear();
					}
					else {
						m->preprocess(context, {line});
						tmp.push_back(line);
					}
				}
				else {
					if (m->get_target_type() == statement_types::end_)
						throw runtime_error("Hanging end statement.");
					else {
						m->preprocess(context, {line});
						sptr = m->translate(context, {line});
					}
				}
			}
			break;
			case method_types::block: {
				methods.push(m);
				context->instance->storage.add_domain();
				context->instance->storage.add_set();
				methods.top()->preprocess(context, {line});
				tmp.push_back(line);
			}
			break;
			case method_types::jit_command:
				m->translate(context, {line});
				break;
			}
			if (sptr != nullptr)
				echo ? sptr->repl_run() : sptr->run();
		}
		catch (const lang_error &le) {
			reset_status();
			context->compiler->utilize_metadata();
			context->instance->storage.clear_set();
			throw fatal_error(std::string("Uncaught exception: ") + le.what());
		}
		catch (const cs::exception &e) {
			reset_status();
			context->compiler->utilize_metadata();
			context->instance->storage.clear_set();
			throw e;
		}
		catch (const std::exception &e) {
			reset_status();
			context->compiler->utilize_metadata();
			context->instance->storage.clear_set();
			throw exception(line_num, context->file_path, code, e.what());
		}
		context->compiler->utilize_metadata();
		context->instance->storage.clear_set();
	}

	void repl::run(const string &code)
	{
		if (code.empty())
			return;
		std::deque<char> buff;
		for (auto &ch: code)
			buff.push_back(ch);
		try {
			std::deque<std::deque<token_base *>> ast;
			context->compiler->clear_metadata();
			context->compiler->build_line(buff, ast, 1, encoding);
			for (auto &line: ast)
				interpret(code, line);
		}
		catch (const lang_error &le) {
			reset_status();
			throw fatal_error(std::string("Uncaught exception: ") + le.what());
		}
		catch (const cs::exception &e) {
			reset_status();
			throw e;
		}
		catch (const std::exception &e) {
			reset_status();
			throw exception(line_num, context->file_path, code, e.what());
		}
	}

	void repl::exec(const string &code)
	{
		// Preprocess
		++line_num;
		int mode = 0;
		for (auto &ch: code) {
			if (mode == 0) {
				if (!std::isspace(ch)) {
					switch (ch) {
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
			else if (mode == 1) {
				if (!std::isspace(ch))
					cmd_buff.push_back(ch);
			}
			else
				break;
		}
		switch (mode) {
		default:
			break;
		case 0:
			return;
		case 1: {
			std::string cmd;
			std::swap(cmd_buff, cmd);
			if (cmd == "begin" && !multi_line) {
				multi_line = true;
				context->file_buff.emplace_back();
			}
			else if (cmd == "end" && multi_line) {
				multi_line = false;
				std::string line;
				std::swap(line_buff, line);
				this->run(line);
			}
			else {
				auto pos = cmd.find(':');
				std::string arg;
				if (pos != std::string::npos) {
					arg = cmd.substr(pos + 1);
					cmd = cmd.substr(0, pos);
				}
				if (cmd == "exit") {
					int code = 0;
					current_process->on_process_exit.touch(&code);
				}
				else if (cmd == "charset") {
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
				else if (cmd == "require") {
					std::string version_str = CS_GET_VERSION_STR(COVSCRIPT_STD_VERSION);
					if (arg > version_str)
						throw exception(line_num, context->file_path, "@" + cmd + ": " + arg,
						                "Newer Language Standard required: " + arg + ", now on " + version_str);
				}
				else
					throw exception(line_num, context->file_path, "@" + cmd + (arg.empty() ? "" : ": " + arg),
					                "Wrong grammar for preprocessor command.");
				context->file_buff.emplace_back();
			}
			return;
		}
		}
		if (multi_line) {
			context->file_buff.emplace_back();
			line_buff.append(code);
		}
		else {
			context->file_buff.emplace_back(code);
			this->run(code);
		}
	}
}
