#pragma once
#include "../cni.hpp"
static cs::extension hash_map_ext;
static cs::extension_t hash_map_ext_shared=std::make_shared<cs::extension_holder>(&hash_map_ext);
namespace cs_impl {
	template<>cs::extension_t& get_ext<cs::hash_map>()
	{
		return hash_map_ext_shared;
	}
}
namespace hash_map_cs_ext {
	using namespace cs;
// Capacity
	bool empty(const hash_map& map)
	{
		return map.empty();
	}
	number size(const hash_map& map)
	{
		return map.size();
	}
// Modifiers
	void clear(hash_map& map)
	{
		map.clear();
	}
	void insert(hash_map& map,const var& key,const var& val)
	{
		if(map.count(key)>0)
			map.at(key).swap(copy(val),true);
		else
			map.emplace(copy(key),copy(val));
	}
	void erase(hash_map& map,const var& key)
	{
		map.erase(key);
	}
// Lookup
	var at(hash_map& map,const var& key)
	{
		return map.at(key);
	}
	bool exist(hash_map& map,const var& key)
	{
		return map.count(key)>0;
	}
	void init()
	{
		hash_map_ext.add_var("empty",var::make_protect<native_interface>(cni(empty),true));
		hash_map_ext.add_var("size",var::make_protect<native_interface>(cni(size),true));
		hash_map_ext.add_var("clear",var::make_protect<native_interface>(cni(clear),true));
		hash_map_ext.add_var("insert",var::make_protect<native_interface>(cni(insert),true));
		hash_map_ext.add_var("erase",var::make_protect<native_interface>(cni(erase),true));
		hash_map_ext.add_var("at",var::make_protect<native_interface>(cni(at),true));
		hash_map_ext.add_var("exist",var::make_protect<native_interface>(cni(exist),true));
	}
}
