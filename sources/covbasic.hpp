#pragma once
#include "./cni.hpp"
#include "./grammar.hpp"
#include "./extensions/system_extension.hpp"
#include "./extensions/runtime_extension.hpp"
#include "./extensions/types_extension.hpp"
#ifdef CBS_MATH_EXT
#include "./extensions/math_extension.cpp"
#endif
#ifdef CBS_FILE_EXT
#include "./extensions/file_extension.cpp"
#endif
#ifdef CBS_DARWIN_EXT
#include "./extensions/darwin_extension.cpp"
#endif
#define add_function(name) cov_basic::runtime->storage.add_var_global(#name,cov::any::make_protect<cov_basic::native_interface>(cov_basic::cni(name)));
#define add_function_const(name) cov_basic::runtime->storage.add_var_global(#name,cov::any::make_protect<cov_basic::native_interface>(cov_basic::cni(name),true));
namespace cov_basic {
// Internal Functions
	number to_integer(number n)
	{
		return long(n);
	}
	string to_string(const cov::any& val)
	{
		return val.to_string();
	}
	bool is_char(const cov::any& val)
	{
		return val.type()==typeid(char);
	}
	bool is_number(const cov::any& val)
	{
		return val.type()==typeid(number);
	}
	bool is_boolean(const cov::any& val)
	{
		return val.type()==typeid(boolean);
	}
	bool is_string(const cov::any& val)
	{
		return val.type()==typeid(string);
	}
	bool is_list(const cov::any& val)
	{
		return val.type()==typeid(list);
	}
	bool is_array(const cov::any& val)
	{
		return val.type()==typeid(array);
	}
	bool is_linker(const cov::any& val)
	{
		return val.type()==typeid(linker);
	}
	bool is_pair(const cov::any& val)
	{
		return val.type()==typeid(pair);
	}
	bool is_hash_map(const cov::any& val)
	{
		return val.type()==typeid(hash_map);
	}
	cov::any clone(const cov::any& val)
	{
		if(val.type()==typeid(linker))
			return copy(val.const_val<linker>().data);
		else
			return copy(val);
	}
	void swap(cov::any& a,cov::any& b)
	{
		a.swap(b,true);
	}
	cov::any link(const cov::any& val)
	{
		return cov::any::make<linker>(val);
	}
	cov::any escape(const cov::any& val)
	{
		return val.const_val<linker>().data;
	}
	void init()
	{
		init_grammar();
		// Internal Types
		runtime->storage.add_type("char",[]()->cov::any {return cov::any::make<char>('\0');},cov::hash<std::string>(typeid(char).name()));
		runtime->storage.add_type("number",[]()->cov::any {return cov::any::make<number>(0);},cov::hash<std::string>(typeid(number).name()));
		runtime->storage.add_type("boolean",[]()->cov::any {return cov::any::make<boolean>(true);},cov::hash<std::string>(typeid(boolean).name()));
		runtime->storage.add_type("string",[]()->cov::any {return cov::any::make<string>();},cov::hash<std::string>(typeid(string).name()));
		runtime->storage.add_type("list",[]()->cov::any {return cov::any::make<list>();},cov::hash<std::string>(typeid(list).name()));
		runtime->storage.add_type("array",[]()->cov::any {return cov::any::make<array>();},cov::hash<std::string>(typeid(array).name()));
		runtime->storage.add_type("linker",[]()->cov::any {return cov::any::make<linker>();},cov::hash<std::string>(typeid(linker).name()));
		runtime->storage.add_type("pair",[]()->cov::any {return cov::any::make<pair>(number(0),number(0));},cov::hash<std::string>(typeid(pair).name()));
		runtime->storage.add_type("hash_map",[]()->cov::any {return cov::any::make<hash_map>();},cov::hash<std::string>(typeid(hash_map).name()));
		// Add Internal Functions to storage
		add_function_const(to_integer);
		add_function_const(to_string);
		add_function_const(is_char);
		add_function_const(is_number);
		add_function_const(is_boolean);
		add_function_const(is_string);
		add_function_const(is_array);
		add_function_const(is_linker);
		add_function_const(is_pair);
		add_function_const(is_hash_map);
		add_function(clone);
		add_function(swap);
		add_function(link);
		add_function(escape);
		// Init the extensions
		system_cbs_ext::init();
		runtime->storage.add_var("system",cov::any::make_protect<std::shared_ptr<extension_holder>>(std::make_shared<extension_holder>(&system_ext)));
		runtime_cbs_ext::init();
		runtime->storage.add_var("runtime",cov::any::make_protect<std::shared_ptr<extension_holder>>(std::make_shared<extension_holder>(&runtime_ext)));
		types_cbs_ext::init();
		runtime->storage.add_var("types",cov::any::make_protect<std::shared_ptr<extension_holder>>(std::make_shared<extension_holder>(&types_ext)));
		char_cbs_ext::init();
		runtime->char_ext=std::make_shared<extension_holder>(&char_ext);
		string_cbs_ext::init();
		runtime->string_ext=std::make_shared<extension_holder>(&string_ext);
		list_cbs_ext::init();
		runtime->list_ext=std::make_shared<extension_holder>(&list_ext);
		array_cbs_ext::init();
		runtime->array_ext=std::make_shared<extension_holder>(&array_ext);
		pair_cbs_ext::init();
		runtime->pair_ext=std::make_shared<extension_holder>(&pair_ext);
		hash_map_cbs_ext::init();
		runtime->hash_map_ext=std::make_shared<extension_holder>(&hash_map_ext);
#ifdef CBS_MATH_EXT
		math_cbs_ext::init();
		runtime->storage.add_var("math",cov::any::make_protect<std::shared_ptr<extension_holder>>(std::make_shared<extension_holder>(&math_ext)));
#endif
#ifdef CBS_FILE_EXT
		file_cbs_ext::init();
		runtime->storage.add_var("file",cov::any::make_protect<std::shared_ptr<extension_holder>>(std::make_shared<extension_holder>(&file_ext)));
#endif
#ifdef CBS_DARWIN_EXT
		darwin_cbs_ext::init();
		runtime->storage.add_var("darwin",cov::any::make_protect<std::shared_ptr<extension_holder>>(std::make_shared<extension_holder>(&darwin_ext)));
#endif
	}
	void reset()
	{
		runtime=std::make_shared<runtime_type>();
		init();
	}
	void cov_basic(const std::string& path)
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
				thread::exec();
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