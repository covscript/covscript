#pragma once
#include "../cni.hpp"
static cs::extension hash_map_ext;
namespace hash_map_cs_ext {
	using namespace cs;
	void clear(hash_map& map)
	{
		map.clear();
	}
	void insert(hash_map& map,const cs::any& key,const cs::any& val)
	{
		if(map.count(key)>0)
			map.at(key).swap(copy(val),true);
		else
			map.emplace(copy(key),copy(val));
	}
	void erase(hash_map& map,const cs::any& key)
	{
		map.erase(key);
	}
	bool exist(hash_map& map,const cs::any& key)
	{
		return map.count(key)>0;
	}
	cs::any at(hash_map& map,const cs::any& key)
	{
		return map.at(key);
	}
	number size(const hash_map& map)
	{
		return map.size();
	}
	void init()
	{
		hash_map_ext.add_var("clear",cs::any::make_protect<native_interface>(cni(clear),true));
		hash_map_ext.add_var("insert",cs::any::make_protect<native_interface>(cni(insert),true));
		hash_map_ext.add_var("erase",cs::any::make_protect<native_interface>(cni(erase),true));
		hash_map_ext.add_var("exist",cs::any::make_protect<native_interface>(cni(exist),true));
		hash_map_ext.add_var("at",cs::any::make_protect<native_interface>(cni(at),true));
		hash_map_ext.add_var("size",cs::any::make_protect<native_interface>(cni(size),true));
	}
}
