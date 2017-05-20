#pragma once
#include "./runtime.hpp"
namespace cov_basic {
	void opt_expr(cov::tree<token_base*>&,cov::tree<token_base*>::iterator);
	bool optimizable(cov::tree<token_base*>::iterator it)
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
	void opt_expr(cov::tree<token_base*>& tree,cov::tree<token_base*>::iterator it)
	{
		if(!it.usable())
			return;
		token_base* token=it.data();
		if(token==nullptr)
			return;
		switch(token->get_type()) {
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
				for(auto& tree:dynamic_cast<token_array*>(token)->get_array())
					arr.push_back(parse_expr(tree.root()));
				token_value* token=new token_value(arr);
				it.data()=token;
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
