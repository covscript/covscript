#pragma once
/*
* Covariant Script Runtime Extension
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
*/
#include <covscript/mozart/timer.hpp>
#include <covscript/cni.hpp>
#include <cstdlib>

static cs::namespace_t context_ext = cs::make_shared_namespace<cs::name_space>();
namespace cs_impl {
	template<>
	cs::namespace_t &get_ext<cs::context_t>()
	{
		return context_ext;
	}

	template<>
	constexpr const char *get_name_of_type<cov::tree<cs::token_base *>>()
	{
		return "cs::expression";
	}
}
static cs::namespace_t runtime_ext = cs::make_shared_namespace<cs::name_space>();
namespace runtime_cs_ext {
	using namespace cs;

	string get_import_path()
	{
		return current_process->import_path;
	}

	void info()
	{
		std::cout << "Covariant Script Programming Language Interpreter\nVersion: " << current_process->version << "\n"
		          "Copyright (C) 2018 Michael Lee.All rights reserved.\n"
		          "Licensed under the Apache License, Version 2.0 (the \"License\");\n"
		          "you may not use this file except in compliance with the License.\n"
		          "You may obtain a copy of the License at\n"
		          "\n"
		          "http://www.apache.org/licenses/LICENSE-2.0\n"
		          "\n"
		          "Unless required by applicable law or agreed to in writing, software\n"
		          "distributed under the License is distributed on an \"AS IS\" BASIS,\n"
		          "WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.\n"
		          "See the License for the specific language governing permissions and\n"
		          "limitations under the License.\n"
		          "Please visit http://covscript.org for more information."
		          << std::endl;
	}

	number time()
	{
		return cov::timer::time(cov::timer::time_unit::milli_sec);
	}

	void delay(number time)
	{
		cov::timer::delay(cov::timer::time_unit::milli_sec, time);
	}

	var exception(const string &str)
	{
		return var::make<lang_error>(str);
	}

	std::size_t hash(const var &val)
	{
		return val.hash();
	}

	using expression_t=cov::tree<token_base *>;

	var build(const context_t &context, const string &expr)
	{
		std::deque<char> buff;
		expression_t tree;
		for (auto &ch:expr)
			buff.push_back(ch);
		context->compiler->build_expr(buff, tree);
		return var::make<expression_t>(tree);
	}

	var solve(const context_t &context, expression_t &tree)
	{
		return context->instance->parse_expr(tree.root());
	}

	var cmd_args(const context_t &context)
	{
		return context->cmd_args;
	}

	var advance_import(const context_t &context, const string &path, const string &name)
	{
		return make_namespace(context->instance->import(path, name));
	}

	void init()
	{
		(*runtime_ext)
		.add_var("std_version", var::make_constant<number>(current_process->std_version))
		.add_var("get_import_path", make_cni(get_import_path, true))
		.add_var("info", make_cni(info))
		.add_var("time", make_cni(time))
		.add_var("delay", make_cni(delay))
		.add_var("exception", make_cni(exception))
		.add_var("hash", make_cni(hash, true))
		.add_var("build", make_cni(build))
		.add_var("solve", make_cni(solve))
		.add_var("cmd_args", make_cni(cmd_args, true))
		.add_var("advance_import", make_cni(advance_import, true));
		(*context_ext)
		.add_var("build", make_cni(build))
		.add_var("solve", make_cni(solve))
		.add_var("cmd_args", make_cni(cmd_args, true))
		.add_var("advance_import", make_cni(advance_import, true));
	}
}
