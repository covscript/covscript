#pragma once
#include "./runtime.hpp"
namespace cs {
	void opt_expr(cov::tree<token_base*>&,cov::tree<token_base*>::iterator);
	bool optimizable(const cov::tree<token_base*>::iterator& it)
	{
		if(!it.usable())
			return false;
		token_base* token=it.data();
		if(token==nullptr)
			return true;
		switch(token->get_type()) {
		case token_types::value:
			return true;
			break;
		}
		return false;
	}
	bool constant_var=false;
	void opt_expr(cov::tree<token_base*>& tree,cov::tree<token_base*>::iterator it)
	{
		if(!it.usable())
			return;
		token_base* token=it.data();
		if(token==nullptr)
			return;
		switch(token->get_type()) {
		case token_types::id: {
			const std::string& id=dynamic_cast<token_id*>(token)->get_id();
			if(runtime->constant_storage.var_exsist(id))
				it.data()=new token_value(runtime->constant_storage.get_var(id));
			else if(runtime->storage.var_exsist(id)&&runtime->storage.get_var(id).is_protect())
				it.data()=new token_value(runtime->storage.get_var(id));
			return;
			break;
		}
		case token_types::expr: {
			cov::tree<token_base*>& t=dynamic_cast<token_expr*>(it.data())->get_tree();
			optimize_expression(t);
			if(optimizable(t.root())) {
				it.data()=t.root().data();
			}
			return;
			break;
		}
		case token_types::array: {
			bool is_optimizable=true;
			for(auto& tree:dynamic_cast<token_array*>(token)->get_array()) {
				optimize_expression(tree);
				if(is_optimizable&&!optimizable(tree.root()))
					is_optimizable=false;
			}
			if(is_optimizable) {
				array arr;
				bool is_map=true;
				token_value* t=nullptr;
				for(auto& tree:dynamic_cast<token_array*>(token)->get_array()) {
					const cov::any& val=parse_expr(tree.root());
					if(is_map&&val.type()!=typeid(pair))
						is_map=false;
					arr.push_back((new token_value(copy(val)))->get_value());
				}
				if(arr.empty())
					is_map=false;
				if(is_map) {
					hash_map map;
					for(auto& it:arr) {
						pair& p=it.val<pair>(true);
						if(map.count(p.first)==0)
							map.emplace(p.first,p.second);
						else
							map.at(p.first)=p.second;
					}
					t=new token_value(cov::any::make<hash_map>(std::move(map)));
				}
				else
					t=new token_value(cov::any::make<array>(std::move(arr)));
				it.data()=t;
			}
			return;
			break;
		}
		case token_types::arglist: {
			for(auto& tree:dynamic_cast<token_arglist*>(token)->get_arglist())
				optimize_expression(tree);
			return;
			break;
		}
		case token_types::signal: {
			switch(dynamic_cast<token_signal*>(token)->get_signal()) {
			case signal_types::dot_: {
				opt_expr(tree,it.left());
				opt_expr(tree,it.right());
				token_base* lptr=it.left().data();
				token_base* rptr=it.right().data();
				if(lptr!=nullptr&&lptr->get_type()==token_types::value&&rptr!=nullptr&&rptr->get_type()==token_types::id) {
					cov::any& a=dynamic_cast<token_value*>(lptr)->get_value();
					if(a.type()==typeid(extension_t))
						it.data()=new token_value(a.val<extension_t>(true)->get_var(dynamic_cast<token_id*>(rptr)->get_id()));
					else {
						token_base* orig_ptr=it.data();
						try {
							it.data()=new token_value(get_type_ext(a,dynamic_cast<token_id*>(rptr)->get_id()));
						}
						catch(const syntax_error& se) {
							it.data()=orig_ptr;
						}
					}

				}
				return;
				break;
			}
			case signal_types::fcall_: {
				opt_expr(tree,it.left());
				opt_expr(tree,it.right());
				token_base* lptr=it.left().data();
				token_base* rptr=it.right().data();
				if(lptr!=nullptr&&lptr->get_type()==token_types::value&&rptr!=nullptr&&rptr->get_type()==token_types::arglist) {
					cov::any& a=dynamic_cast<token_value*>(lptr)->get_value();
					if(a.type()==typeid(callable)&&a.const_val<callable>().is_constant()) {
						bool is_optimizable=true;
						for(auto& tree:dynamic_cast<token_arglist*>(rptr)->get_arglist()) {
							if(is_optimizable&&!optimizable(tree.root()))
								is_optimizable=false;
						}
						if(is_optimizable) {
							array arr;
							for(auto& tree:dynamic_cast<token_arglist*>(rptr)->get_arglist())
								arr.push_back(parse_expr(tree.root()));
							it.data()=new token_value(a.val<callable>(true).call(arr));
						}
					}
				}
				return;
				break;
			}
			case signal_types::asi_: {
				opt_expr(tree,it.left());
				opt_expr(tree,it.right());
				token_base* lptr=it.left().data();
				token_base* rptr=it.right().data();
				if(constant_var&&lptr!=nullptr&&lptr->get_type()==token_types::id&&rptr!=nullptr&&rptr->get_type()==token_types::value) {
					runtime->constant_storage.add_var(dynamic_cast<token_id*>(lptr)->get_id(),dynamic_cast<token_value*>(rptr)->get_value());
					it.data()=rptr;
				}
				return;
				break;
			}
			}
		}
		}
		opt_expr(tree,it.left());
		opt_expr(tree,it.right());
		if(optimizable(it.left())&&optimizable(it.right())) {
			token_value* token=new token_value(parse_expr(it));
			tree.erase_left(it);
			tree.erase_right(it);
			it.data()=token;
		}
	}
	void optimize_expression(cov::tree<token_base*>& tree)
	{
		opt_expr(tree,tree.root());
	}
}
