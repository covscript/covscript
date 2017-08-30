#pragma once

#include "../cni.hpp"
#include <cstdlib>

static cs::extension runtime_ext;
namespace runtime_cs_ext {
	using namespace cs;

	void info()
	{
		std::cout << "Covariant Script Programming Language Interpreter Version " << cs::version << "\n"
		          "Copyright (C) 2017 Michael Lee.All rights reserved.\nThis program comes with ABSOLUTELY NO WARRANTY; for details see <http://www.gnu.org/licenses/>.\n"
		          "This is free software, and you are welcome to redistribute it under certain conditions.\nPlease visit <http://covscript.org/> for more information." << std::endl;
	}

	number time()
	{
		return cov::timer::time(cov::timer::time_unit::milli_sec);
	}

	void delay(number time)
	{
		cov::timer::delay(cov::timer::time_unit::milli_sec, time);
	}

	number rand(number b, number e)
	{
		return cov::rand<number>(b, e);
	}

	number randint(number b, number e)
	{
		return cov::rand<long>(b, e);
	}

	var exception(const string &str)
	{
		return var::make<lang_error>(str);
	}

	extension_t load_extension(const string &path)
	{
		return std::make_shared<extension_holder>(path);
	}

	std::size_t hash(const var &val)
	{
		return val.hash();
	}

	using expression_t=cov::tree<token_base *>;

	var build(const string &expr)
	{
		std::deque<char> buff;
		std::deque<token_base *> tokens;
		expression_t tree;
		for (auto &ch:expr)
			buff.push_back(ch);
		translate_into_tokens(buff, tokens);
		process_brackets(tokens);
		kill_brackets(tokens);
		gen_tree(tree, tokens);
		return var::make<expression_t>(tree);
	}

	var solve(expression_t &tree)
	{
		return parse_expr(tree.root());
	}

	void init()
	{
		runtime_ext.add_var("std_version", var::make_constant<number>(std_version));
		runtime_ext.add_var("info", var::make_protect<callable>(cni(info)));
		runtime_ext.add_var("time", var::make_protect<callable>(cni(time)));
		runtime_ext.add_var("delay", var::make_protect<callable>(cni(delay)));
		runtime_ext.add_var("rand", var::make_protect<callable>(cni(rand)));
		runtime_ext.add_var("randint", var::make_protect<callable>(cni(randint)));
		runtime_ext.add_var("exception", var::make_protect<callable>(cni(exception)));
		runtime_ext.add_var("load_extension", var::make_protect<callable>(cni(load_extension), true));
		runtime_ext.add_var("hash", var::make_protect<callable>(cni(hash), true));
		runtime_ext.add_var("build", var::make_protect<callable>(cni(build)));
		runtime_ext.add_var("solve", var::make_protect<callable>(cni(solve)));
	}
}
