#pragma once
#include "./cni.hpp"
#include "./grammar.hpp"
#include "./extensions/system_extension.hpp"
#include "./extensions/runtime_extension.hpp"
#include "./extensions/char_extension.hpp"
#include "./extensions/string_extension.hpp"
#include "./extensions/list_extension.hpp"
#include "./extensions/array_extension.hpp"
#include "./extensions/pair_extension.hpp"
#include "./extensions/hash_map_extension.hpp"
#ifdef CS_MATH_EXT
#include "./extensions/math_extension.cpp"
#endif
#ifdef CS_FILE_EXT
#include "./extensions/file_extension.cpp"
#endif
#ifdef CS_DARWIN_EXT
#include "./extensions/darwin_extension.cpp"
#endif
#define add_function(name) cs::runtime->storage.add_var_global(#name,cs::var::make_protect<cs::native_interface>(cs::cni(name)));
#define add_function_const(name) cs::runtime->storage.add_var_global(#name,cs::var::make_protect<cs::native_interface>(cs::cni(name),true));
namespace cs {
// Internal Functions
	number to_integer(const var& val)
	{
		return val.to_integer();
	}
	string to_string(const var& val)
	{
		return val.to_string();
	}
	bool is_char(const var& val)
	{
		return val.type()==typeid(char);
	}
	bool is_number(const var& val)
	{
		return val.type()==typeid(number);
	}
	bool is_boolean(const var& val)
	{
		return val.type()==typeid(boolean);
	}
	bool is_string(const var& val)
	{
		return val.type()==typeid(string);
	}
	bool is_list(const var& val)
	{
		return val.type()==typeid(list);
	}
	bool is_array(const var& val)
	{
		return val.type()==typeid(array);
	}
	bool is_pair(const var& val)
	{
		return val.type()==typeid(pair);
	}
	bool is_hash_map(const var& val)
	{
		return val.type()==typeid(hash_map);
	}
	var clone(const var& val)
	{
		return copy(val);
	}
	void swap(var& a,var& b)
	{
		a.swap(b,true);
	}
	void init()
	{
		init_grammar();
		// Internal Types
		runtime->storage.add_type("char",[]()->var {return var::make<char>('\0');},cs_impl::hash<std::string>(typeid(char).name()),char_ext_shared);
		runtime->storage.add_type("number",[]()->var {return var::make<number>(0);},cs_impl::hash<std::string>(typeid(number).name()));
		runtime->storage.add_type("boolean",[]()->var {return var::make<boolean>(true);},cs_impl::hash<std::string>(typeid(boolean).name()));
		runtime->storage.add_type("string",[]()->var {return var::make<string>();},cs_impl::hash<std::string>(typeid(string).name()),string_ext_shared);
		runtime->storage.add_type("list",[]()->var {return var::make<list>();},cs_impl::hash<std::string>(typeid(list).name()),list_ext_shared);
		runtime->storage.add_type("array",[]()->var {return var::make<array>();},cs_impl::hash<std::string>(typeid(array).name()),array_ext_shared);
		runtime->storage.add_type("pair",[]()->var {return var::make<pair>(number(0),number(0));},cs_impl::hash<std::string>(typeid(pair).name()),pair_ext_shared);
		runtime->storage.add_type("hash_map",[]()->var {return var::make<hash_map>();},cs_impl::hash<std::string>(typeid(hash_map).name()),hash_map_ext_shared);
		// Add Internal Functions to storage
		add_function_const(to_integer);
		add_function_const(to_string);
		add_function_const(is_char);
		add_function_const(is_number);
		add_function_const(is_boolean);
		add_function_const(is_string);
		add_function_const(is_array);
		add_function_const(is_pair);
		add_function_const(is_hash_map);
		add_function(clone);
		add_function(swap);
		// Init the extensions
		system_cs_ext::init();
		runtime->storage.add_var("system",var::make_protect<std::shared_ptr<extension_holder>>(std::make_shared<extension_holder>(&system_ext)));
		runtime_cs_ext::init();
		runtime->storage.add_var("runtime",var::make_protect<std::shared_ptr<extension_holder>>(std::make_shared<extension_holder>(&runtime_ext)));
		char_cs_ext::init();
		string_cs_ext::init();
		list_cs_ext::init();
		array_cs_ext::init();
		pair_cs_ext::init();
		hash_map_cs_ext::init();
#ifdef CS_MATH_EXT
		math_cs_ext::init();
		runtime->storage.add_var("math",var::make_protect<std::shared_ptr<extension_holder>>(std::make_shared<extension_holder>(&math_ext)));
#endif
#ifdef CS_FILE_EXT
		file_cs_ext::init();
		runtime->storage.add_var("file",var::make_protect<std::shared_ptr<extension_holder>>(std::make_shared<extension_holder>(&file_ext)));
#endif
#ifdef CS_DARWIN_EXT
		darwin_cs_ext::init();
		runtime->storage.add_var("darwin",var::make_protect<std::shared_ptr<extension_holder>>(std::make_shared<extension_holder>(&darwin_ext)));
#endif
	}
	void reset()
	{
		runtime=std::unique_ptr<runtime_type>(new runtime_type);
		init();
	}
	void cs(const std::string& path)
	{
		token_value::clean();
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
				throw syntax_error(path,line_num,se.what());
			}
			catch(const std::exception& e) {
				throw internal_error(path,line_num,e.what());
			}
			tokens.push_back(new token_endline(line_num,path));
			buff.clear();
		}
		translate_into_statements(tokens,statements);
		token_value::mark();
		for(auto& ptr:statements) {
			try {
				ptr->run();
				if(break_block||continue_block)
					throw syntax_error("Can not run break or continue outside the loop.");
			}
			catch(const syntax_error& se) {
				throw syntax_error(ptr->get_file_path(),ptr->get_line_num(),se.what());
			}
			catch(const lang_error& le) {
				throw lang_error(ptr->get_file_path(),ptr->get_line_num(),le.what());
			}
			catch(const std::exception& e) {
				throw internal_error(ptr->get_file_path(),ptr->get_line_num(),e.what());
			}
		}
	}
}
