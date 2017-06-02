#pragma once
#include "./grammar.hpp"
#include "./arglist.hpp"
#include "./extensions/system_extension.hpp"
#include "./extensions/runtime_extension.hpp"
#ifdef CBS_STRING_EXT
#include "./extensions/string_extension.cpp"
#endif
#ifdef CBS_ARRAY_EXT
#include "./extensions/array_extension.cpp"
#endif
#ifdef CBS_HASH_MAP_EXT
#include "./extensions/hash_map_extension.cpp"
#endif
#ifdef CBS_MATH_EXT
#include "./extensions/math_extension.cpp"
#endif
#ifdef CBS_FILE_EXT
#include "./extensions/file_extension.cpp"
#endif
#ifdef CBS_DARWIN_EXT
#include "./extensions/darwin_extension.cpp"
#endif
#define add_function(name) cov_basic::runtime->storage.add_var_global(#name,cov::any::make_constant<cov_basic::native_interface>(name));
#define add_function_name(name,func) cov_basic::runtime->storage.add_var_global(name,cov::any::make_constant<cov_basic::native_interface>(func));
namespace cov_basic {
// Internal Functions
	cov::any size_of(array& args)
	{
		if(args.size()!=1)
			throw syntax_error("Wrong size of arguments.");
		if(args.front().type()==typeid(array))
			return number(args.front().const_val<array>().size());
		else if(args.front().type()==typeid(hash_map))
			return number(args.front().const_val<hash_map>().size());
		else if(args.front().type()==typeid(string))
			return number(args.front().const_val<string>().size());
		else
			throw syntax_error("Get size of non-array or string object.");
	}
	cov::any to_integer(array& args)
	{
		if(args.size()!=1)
			throw syntax_error("Wrong size of arguments.");
		if(args.front().type()==typeid(number))
			return number(long(args.at(0).const_val<number>()));
		else if(args.front().type()==typeid(string))
			return number(std::stold(args.at(0).const_val<string>()));
		else
			throw syntax_error("Wrong type of arguments.(Request Number or String)");
	}
	cov::any to_string(array& args)
	{
		if(args.size()!=1)
			throw syntax_error("Wrong size of arguments.");
		return args.front().to_string();
	}
	cov::any is_number(array& args)
	{
		if(args.size()!=1)
			throw syntax_error("Wrong size of arguments.");
		return args.front().type()==typeid(number);
	}
	cov::any is_boolean(array& args)
	{
		if(args.size()!=1)
			throw syntax_error("Wrong size of arguments.");
		return args.front().type()==typeid(boolean);
	}
	cov::any is_string(array& args)
	{
		if(args.size()!=1)
			throw syntax_error("Wrong size of arguments.");
		return args.front().type()==typeid(string);
	}
	cov::any is_array(array& args)
	{
		if(args.size()!=1)
			throw syntax_error("Wrong size of arguments.");
		return args.front().type()==typeid(array);
	}
	cov::any is_linker(array& args)
	{
		if(args.size()!=1)
			throw syntax_error("Wrong size of arguments.");
		return args.front().type()==typeid(linker);
	}
	cov::any is_hash_map(array& args)
	{
		if(args.size()!=1)
			throw syntax_error("Wrong size of arguments.");
		return args.front().type()==typeid(hash_map);
	}
	cov::any clone(array& args)
	{
		if(args.size()!=1)
			throw syntax_error("Wrong size of arguments.");
		if(args.front().type()==typeid(linker))
			return copy(args.front().const_val<linker>().data);
		else
			return copy(args.front());
	}
	cov::any swap(array& args)
	{
		if(args.size()!=2)
			throw syntax_error("Wrong size of arguments.");
		args.at(0).swap(args.at(1),true);
		return number(0);
	}
	cov::any link(array& args)
	{
		if(args.size()!=1)
			throw syntax_error("Wrong size of arguments.");
		return std::move(cov::any::make<linker>(args.front()));
	}
	cov::any escape(array& args)
	{
		arglist::check<linker>(args);
		return args.front().val<linker>(true).data;
	}
	void init()
	{
		init_grammar();
		// Internal Types
		runtime->storage.add_type("char",[]()->cov::any {return cov::any::make<char>('\0');},cov::hash<std::string>(typeid(char).name()));
		runtime->storage.add_type("number",[]()->cov::any {return cov::any::make<number>(0);},cov::hash<std::string>(typeid(number).name()));
		runtime->storage.add_type("boolean",[]()->cov::any {return cov::any::make<boolean>(true);},cov::hash<std::string>(typeid(boolean).name()));
		runtime->storage.add_type("string",[]()->cov::any {return cov::any::make<string>();},cov::hash<std::string>(typeid(string).name()));
		runtime->storage.add_type("array",[]()->cov::any {return cov::any::make<array>();},cov::hash<std::string>(typeid(array).name()));
		runtime->storage.add_type("linker",[]()->cov::any {return cov::any::make<linker>();},cov::hash<std::string>(typeid(linker).name()));
		runtime->storage.add_type("hash_map",[]()->cov::any {return cov::any::make<hash_map>();},cov::hash<std::string>(typeid(hash_map).name()));
		// Add Internal Functions to storage
		add_function(to_integer);
		add_function(to_string);
		add_function(is_number);
		add_function(is_boolean);
		add_function(is_string);
		add_function(is_array);
		add_function(size_of);
		add_function(clone);
		add_function(swap);
		add_function(link);
		add_function(escape);
		// Init the extensions
		system_cbs_ext::init();
		runtime->storage.add_var("system",cov::any::make_constant<std::shared_ptr<extension_holder>>(std::make_shared<extension_holder>(&system_ext)));
		runtime_cbs_ext::init();
		runtime->storage.add_var("runtime",cov::any::make_constant<std::shared_ptr<extension_holder>>(std::make_shared<extension_holder>(&runtime_ext)));
#ifdef CBS_STRING_EXT
		string_cbs_ext::init();
		runtime->storage.add_var("string",cov::any::make_constant<std::shared_ptr<extension_holder>>(std::make_shared<extension_holder>(&string_ext)));
#endif
#ifdef CBS_ARRAY_EXT
		array_cbs_ext::init();
		runtime->storage.add_var("array",cov::any::make_constant<std::shared_ptr<extension_holder>>(std::make_shared<extension_holder>(&array_ext)));
#endif
#ifdef CBS_HASH_MAP_EXT
		hash_map_cbs_ext::init();
		runtime->storage.add_var("hash_map",cov::any::make_constant<std::shared_ptr<extension_holder>>(std::make_shared<extension_holder>(&hash_map_ext)));
#endif
#ifdef CBS_MATH_EXT
		math_cbs_ext::init();
		runtime->storage.add_var("math",cov::any::make_constant<std::shared_ptr<extension_holder>>(std::make_shared<extension_holder>(&math_ext)));
#endif
#ifdef CBS_FILE_EXT
		file_cbs_ext::init();
		runtime->storage.add_var("file",cov::any::make_constant<std::shared_ptr<extension_holder>>(std::make_shared<extension_holder>(&file_ext)));
#endif
#ifdef CBS_DARWIN_EXT
		darwin_cbs_ext::init();
		runtime->storage.add_var("darwin",cov::any::make_constant<std::shared_ptr<extension_holder>>(std::make_shared<extension_holder>(&darwin_ext)));
#endif
	}
	void reset()
	{
		runtime=std::unique_ptr<runtime_type>(new runtime_type);
	}
	void cov_basic(const std::string& path)
	{
		std::ios::sync_with_stdio(false);
		std::deque<char> buff;
		std::deque<token_base*> tokens;
		std::deque<statement_base*> statements;
		std::ifstream in(path);
		if(!in.is_open())
			throw fatal_error(path+": No such file or directory");
		std::string line;
		std::size_t line_num=0;
		while(std::getline(in,line)) {
			++line_num;
			if(line.empty())
				continue;
			bool is_note=false;
			for(auto& ch:line) {
				if(!std::isspace(ch)) {
					if(ch=='#')
						is_note=true;
					break;
				}
			}
			if(is_note)
				continue;
			for(auto& c:line)
				buff.push_back(c);
			try {
				translate_into_tokens(buff,tokens);
			}
			catch(const syntax_error& se) {
				throw syntax_error(line_num,se.what());
			}
			catch(const std::exception& e) {
				throw internal_error(line_num,e.what());
			}
			tokens.push_back(new token_endline(line_num));
			buff.clear();
		}
		init();
		translate_into_statements(tokens,statements);
		for(auto& ptr:statements) {
			try {
				ptr->run();
				if(break_block||continue_block)
					throw syntax_error("Can not run break or continue outside the loop.");
			}
			catch(const syntax_error& se) {
				throw syntax_error(ptr->get_line_num(),se.what());
			}
			catch(const lang_error& le) {
				throw lang_error(ptr->get_line_num(),le.what());
			}
			catch(const std::exception& e) {
				throw internal_error(ptr->get_line_num(),e.what());
			}
		}
		std::ios::sync_with_stdio(true);
	}
}
