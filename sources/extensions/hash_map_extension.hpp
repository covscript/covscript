#pragma once
#include "../cni.hpp"
static cov_basic::extension hash_map_ext;
namespace hash_map_cbs_ext {
	using namespace cov_basic;
	void clear(hash_map& map)
	{
		map.clear();
	}
	void insert(hash_map& map,const cov::any& key,const cov::any& val)
	{
		if(map.count(key)>0)
			map.at(key).swap(copy(val),true);
		else
			map.emplace(copy(key),copy(val));
	}
	void erase(hash_map& map,const cov::any& key)
	{
		map.erase(key);
	}
	bool exist(hash_map& map,const cov::any& key)
	{
		return map.count(key)>0;
	}
	cov::any at(hash_map& map,const cov::any& key)
	{
		return map.at(key);
	}
	number size(const hash_map& map)
	{
		return map.size();
	}
	void init()
	{
		hash_map_ext.add_var("clear",cov::any::make_protect<native_interface>(cni(clear),true));
		hash_map_ext.add_var("insert",cov::any::make_protect<native_interface>(cni(insert),true));
		hash_map_ext.add_var("erase",cov::any::make_protect<native_interface>(cni(erase),true));
		hash_map_ext.add_var("exist",cov::any::make_protect<native_interface>(cni(exist),true));
		hash_map_ext.add_var("at",cov::any::make_protect<native_interface>(cni(at),true));
		hash_map_ext.add_var("size",cov::any::make_protect<native_interface>(cni(size),true));
	}
}
