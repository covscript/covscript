#pragma once
#include "./parser.hpp"
#include <cmath>
namespace cov_basic {
	class domain_manager {
		std::unordered_map<string,std::function<cov::any()>> m_type;
		std::deque<std::unordered_map<string,cov::any>> m_data;
	public:
		domain_manager()
		{
			m_data.emplace_front();
		}
		domain_manager(const domain_manager&)=delete;
		~domain_manager()=default;
		void add_type(const std::string& name,const std::function<cov::any()>& func)
		{
			if(m_type.count(name)>0)
				throw syntax_error("Redefinition of type \""+name+"\".");
			else
				m_type.emplace(name,func);
		}
		void add_domain()
		{
			m_data.emplace_front();
		}
		std::unordered_map<string,cov::any>& get_domain()
		{
			return m_data.front();
		}
		void remove_domain()
		{
			if(m_data.size()>1)
				m_data.pop_front();
		}
		bool type_exsist(const string& name)
		{
			if(m_type.count(name)>0)
				return true;
			else
				return false;
		}
		bool var_exsist(const string& name)
		{
			for(auto& domain:m_data)
				if(domain.count(name)>0)
					return true;
			return false;
		}
		bool var_exsist_current(const string& name)
		{
			if(m_data.front().count(name)>0)
				return true;
			return false;
		}
		bool var_exsist_global(const string& name)
		{
			if(m_data.back().count(name)>0)
				return true;
			return false;
		}
		cov::any get_var_type(const string& type)
		{
			if(type_exsist(type))
				return m_type.at(type)();
			else
				throw syntax_error("Use of undefined type \""+type+"\".");
		}
		cov::any& get_var(const string& name)
		{
			for(auto& domain:m_data)
				if(domain.count(name)>0)
					return domain.at(name);
			throw syntax_error("Use of undefined variable \""+name+"\".");
		}
		cov::any& get_var_current(const string& name)
		{
			if(m_data.front().count(name)>0)
				return m_data.front().at(name);
			throw syntax_error("Use of undefined variable \""+name+"\" in current domain.");
		}
		cov::any& get_var_global(const string& name)
		{
			if(m_data.back().count(name)>0)
				return m_data.back().at(name);
			throw syntax_error("Use of undefined variable \""+name+"\" in global domain.");
		}
		void add_var(const string& name,const cov::any& var)
		{
			if(var_exsist_current(name))
				get_var(name)=var;
			else
				m_data.front().emplace(name,var);
		}
		void add_var_global(const string& name,const cov::any& var)
		{
			if(var_exsist_global(name))
				get_var_global(name)=var;
			else
				m_data.back().emplace(name,var);
		}
	};
	domain_manager storage;
	cov::any parse_add(const cov::any& a,const cov::any& b)
	{
		if(a.type()==typeid(number)) {
			if(b.type()==typeid(number)) {
				return a.const_val<number>()+b.const_val<number>();
			} else if(b.type()==typeid(boolean)) {
				return a.const_val<number>()+number(b.const_val<boolean>());
			} else
				throw syntax_error("Unsupported operator operations(Add).");
		}
		if(a.type()==typeid(boolean)) {
			if(b.type()==typeid(number)) {
				return number(a.const_val<boolean>())+b.const_val<number>();
			} else if(b.type()==typeid(boolean)) {
				return number(a.const_val<boolean>())+number(b.const_val<boolean>());
			} else
				throw syntax_error("Unsupported operator operations(Add).");
		}
		if(a.type()==typeid(string)) {
			return std::string(a.to_string()+b.to_string());
		}
		throw syntax_error("Unsupported operator operations(Add).");
	}
	cov::any parse_sub(const cov::any& a,const cov::any& b)
	{
		if(!a.usable()) {
			if(b.type()==typeid(number))
				return -b.const_val<number>();
			else
				throw syntax_error("Unsupported operator operations(Sub).");
		}
		if(a.type()==typeid(number)) {
			if(b.type()==typeid(number)) {
				return a.const_val<number>()-b.const_val<number>();
			} else if(b.type()==typeid(boolean)) {
				return a.const_val<number>()-number(b.const_val<boolean>());
			} else
				throw syntax_error("Unsupported operator operations(Sub).");
		}
		if(a.type()==typeid(boolean)) {
			if(b.type()==typeid(number)) {
				return number(a.const_val<boolean>())-b.const_val<number>();
			} else if(b.type()==typeid(boolean)) {
				return number(a.const_val<boolean>())-number(b.const_val<boolean>());
			} else
				throw syntax_error("Unsupported operator operations(Sub).");
		}
		throw syntax_error("Unsupported operator operations(Sub).");
	}
	cov::any parse_mul(const cov::any& a,const cov::any& b)
	{
		if(a.type()==typeid(number)) {
			if(b.type()==typeid(number)) {
				return number(a.const_val<number>()*b.const_val<number>());
			} else
				throw syntax_error("Unsupported operator operations(Mul).");
		}
		throw syntax_error("Unsupported operator operations(Mul).");
	}
	cov::any parse_div(const cov::any& a,const cov::any& b)
	{
		if(a.type()==typeid(number)) {
			if(b.type()==typeid(number)) {
				return number(a.const_val<number>()/b.const_val<number>());
			} else
				throw syntax_error("Unsupported operator operations(Div).");
		}
		throw syntax_error("Unsupported operator operations(Div).");
	}
	cov::any parse_mod(const cov::any& a,const cov::any& b)
	{
		if(a.type()==typeid(number)) {
			if(b.type()==typeid(number)) {
				return number(long(a.const_val<number>())%long(b.const_val<number>()));
			} else
				throw syntax_error("Unsupported operator operations(Mod).");
		}
		throw syntax_error("Unsupported operator operations(Mod).");
	}
	cov::any parse_pow(const cov::any& a,const cov::any& b)
	{
		if(a.type()==typeid(number)) {
			if(b.type()==typeid(number)) {
				return number(std::pow(a.const_val<number>(),b.const_val<number>()));
			} else
				throw syntax_error("Unsupported operator operations(Pow).");
		}
		throw syntax_error("Unsupported operator operations(Pow).");
	}
	cov::any parse_dot(const cov::any& a,token_base* b)
	{
		if(b==nullptr)
			throw syntax_error("Internal Error(Null Pointer Accessed).");
		if(b->get_type()!=token_types::id)
			throw syntax_error("Unsupported operator operations(Dot).");
		if(a.type()==typeid(structure))
			return a.val<structure>(true).get_var(dynamic_cast<token_id*>(b)->get_id());
		if(a.type()!=typeid(constant_values))
			throw syntax_error("Unsupported operator operations(Dot).");
		if(a.const_val<constant_values>()==constant_values::global_namespace)
			return storage.get_var_global(dynamic_cast<token_id*>(b)->get_id());
		else if(a.const_val<constant_values>()==constant_values::current_namespace)
			return storage.get_var_current(dynamic_cast<token_id*>(b)->get_id());
		else
			throw syntax_error("Unsupported operator operations(Dot).");
	}
	cov::any parse_und(const cov::any& a,const cov::any& b)
	{
		if(a.type()==typeid(number)) {
			if(b.type()==typeid(number)) {
				return boolean(a.const_val<number>()<b.const_val<number>());
			} else if(b.type()==typeid(boolean)) {
				return boolean(a.const_val<number>()<number(b.const_val<boolean>()));
			} else
				throw syntax_error("Unsupported operator operations(Und).");
		}
		if(a.type()==typeid(boolean)) {
			if(b.type()==typeid(number)) {
				return boolean(number(a.const_val<boolean>())<b.const_val<number>());
			} else if(b.type()==typeid(boolean)) {
				return boolean(number(a.const_val<boolean>())<number(b.const_val<boolean>()));
			} else
				throw syntax_error("Unsupported operator operations(Und).");
		}
		throw syntax_error("Unsupported operator operations(Und).");
	}
	cov::any parse_abo(const cov::any& a,const cov::any& b)
	{
		if(a.type()==typeid(number)) {
			if(b.type()==typeid(number)) {
				return boolean(a.const_val<number>()>b.const_val<number>());
			} else if(b.type()==typeid(boolean)) {
				return boolean(a.const_val<number>()>number(b.const_val<boolean>()));
			} else
				throw syntax_error("Unsupported operator operations(Abo).");
		}
		if(a.type()==typeid(boolean)) {
			if(b.type()==typeid(number)) {
				return boolean(number(a.const_val<boolean>())>b.const_val<number>());
			} else if(b.type()==typeid(boolean)) {
				return boolean(number(a.const_val<boolean>())>number(b.const_val<boolean>()));
			} else
				throw syntax_error("Unsupported operator operations(Abo).");
		}
		throw syntax_error("Unsupported operator operations(Abo).");
	}
	cov::any parse_ueq(const cov::any& a,const cov::any& b)
	{
		if(a.type()==typeid(number)) {
			if(b.type()==typeid(number)) {
				return boolean(a.const_val<number>()<=b.const_val<number>());
			} else if(b.type()==typeid(boolean)) {
				return boolean(a.const_val<number>()<=number(b.const_val<boolean>()));
			} else
				throw syntax_error("Unsupported operator operations(Ueq).");
		}
		if(a.type()==typeid(boolean)) {
			if(b.type()==typeid(number)) {
				return boolean(number(a.const_val<boolean>())<=b.const_val<number>());
			} else if(b.type()==typeid(boolean)) {
				return boolean(number(a.const_val<boolean>())<=number(b.const_val<boolean>()));
			} else
				throw syntax_error("Unsupported operator operations(Ueq).");
		}
		throw syntax_error("Unsupported operator operations(Ueq).");
	}
	cov::any parse_aeq(const cov::any& a,const cov::any& b)
	{
		if(a.type()==typeid(number)) {
			if(b.type()==typeid(number)) {
				return boolean(a.const_val<number>()>=b.const_val<number>());
			} else if(b.type()==typeid(boolean)) {
				return boolean(a.const_val<number>()>=number(b.const_val<boolean>()));
			} else
				throw syntax_error("Unsupported operator operations(Aeq).");
		}
		if(a.type()==typeid(boolean)) {
			if(b.type()==typeid(number)) {
				return boolean(number(a.const_val<boolean>())>=b.const_val<number>());
			} else if(b.type()==typeid(boolean)) {
				return boolean(number(a.const_val<boolean>())>=number(b.const_val<boolean>()));
			} else
				throw syntax_error("Unsupported operator operations(Aeq).");
		}
		throw syntax_error("Unsupported operator operations(Aeq).");
	}
	cov::any parse_asi(cov::any a,cov::any b)
	{
		a.assign(b,true);
		return b;
	}
	cov::any parse_equ(const cov::any& a,const cov::any& b)
	{
		return boolean(a==b);
	}
	cov::any parse_neq(const cov::any& a,const cov::any& b)
	{
		return boolean(a!=b);
	}
	cov::any parse_and(const cov::any& a,const cov::any& b)
	{
		if(a.type()==typeid(boolean)) {
			if(b.type()==typeid(boolean)) {
				return boolean(a.const_val<boolean>()&&b.const_val<boolean>());
			} else if(b.type()==typeid(number)) {
				return boolean(a.const_val<boolean>()&&boolean(b.const_val<number>()));
			} else
				throw syntax_error("Unsupported operator operations(And).");
		}
		if(a.type()==typeid(number)) {
			if(b.type()==typeid(boolean)) {
				return boolean(boolean(a.const_val<number>())&&b.const_val<boolean>());
			} else if(b.type()==typeid(number)) {
				return boolean(number(a.const_val<number>())&&number(b.const_val<number>()));
			} else
				throw syntax_error("Unsupported operator operations(And).");
		}
		throw syntax_error("Unsupported operator operations(And).");
	}
	cov::any parse_or(const cov::any& a,const cov::any& b)
	{
		if(a.type()==typeid(boolean)) {
			if(b.type()==typeid(boolean)) {
				return boolean(a.const_val<boolean>()||b.const_val<boolean>());
			} else if(b.type()==typeid(number)) {
				return boolean(a.const_val<boolean>()||boolean(b.const_val<number>()));
			} else
				throw syntax_error("Unsupported operator operations(Or).");
		}
		if(a.type()==typeid(number)) {
			if(b.type()==typeid(boolean)) {
				return boolean(boolean(a.const_val<number>())||b.const_val<boolean>());
			} else if(b.type()==typeid(number)) {
				return boolean(number(a.const_val<number>())||number(b.const_val<number>()));
			} else
				throw syntax_error("Unsupported operator operations(Or).");
		}
		throw syntax_error("Unsupported operator operations(Or).");
	}
	cov::any parse_not(const cov::any& b)
	{
		if(b.type()==typeid(number)) {
			return boolean(!b.const_val<number>());
		} else if(b.type()==typeid(boolean)) {
			return boolean(!b.const_val<boolean>());
		}
		throw syntax_error("Unsupported operator operations(Not).");
	}
	cov::any parse_inc(cov::any a,cov::any b)
	{
		if(a.usable()) {
			if(b.usable())
				throw syntax_error("Unsupported operator operations(Inc).");
			return a.val<number>(true)++;
		} else {
			if(!b.usable())
				throw syntax_error("Unsupported operator operations(Inc).");
			return ++b.val<number>(true);
		}
	}
	cov::any parse_dec(cov::any a,cov::any b)
	{
		if(a.usable()) {
			if(b.usable())
				throw syntax_error("Unsupported operator operations(Dec).");
			return a.val<number>(true)--;
		} else {
			if(!b.usable())
				throw syntax_error("Unsupported operator operations(Dec).");
			return --b.val<number>(true);
		}
	}
	cov::any parse_fcall(const cov::any& a,cov::any b)
	{
		array& args=b.val<array>(true);
		if(a.type()==typeid(function)) {
			return a.val<function>(true).call(args);
		} else if(a.type()==typeid(native_interface)) {
			return a.val<native_interface>(true).call(args);
		} else
			throw syntax_error("Call non-function object.");
	}
	cov::any parse_access(cov::any a,const cov::any& b)
	{
		if(a.type()!=typeid(array))
			throw syntax_error("Access non-array object.");
		if(b.type()!=typeid(number))
			throw syntax_error("Array index must be a number.");
		array& arr=a.val<array>(true);
		std::size_t posit=b.const_val<number>();
		if(posit>=arr.size()) {
			for(std::size_t i=posit-arr.size()+1; i>0; --i)
				arr.emplace_back(number(0));
		}
		return arr.at(posit);
	}
	bool define_var=false;
	cov::any parse_expr(cov::tree<token_base*>::iterator it)
	{
		if(!it.usable())
			throw syntax_error("Internal Error(The expression tree is not available).");
		token_base* token=it.data();
		if(token==nullptr)
			return cov::any();
		switch(token->get_type()) {
		case token_types::id: {
			std::string id=dynamic_cast<token_id*>(token)->get_id();
			if(!storage.var_exsist_current(id)&&define_var)
				storage.add_var(id,number(0));
			return storage.get_var(id);
			break;
		}
		case token_types::value:
			return dynamic_cast<token_value*>(token)->get_value();
			break;
		case token_types::expr:
			return parse_expr(dynamic_cast<token_expr*>(token)->get_tree().root());
			break;
		case token_types::array: {
			array arr;
			for(auto& tree:dynamic_cast<token_array*>(token)->get_array()) {
				cov::any val=parse_expr(tree.root());
				val.clone();
				arr.push_back(val);
			}
			return arr;
		}
		case token_types::arglist: {
			array arr;
			for(auto& tree:dynamic_cast<token_arglist*>(token)->get_arglist())
				arr.push_back(parse_expr(tree.root()));
			return arr;
		}
		case token_types::signal: {
			switch(dynamic_cast<token_signal*>(token)->get_signal()) {
			case signal_types::add_:
				return parse_add(parse_expr(it.left()),parse_expr(it.right()));
				break;
			case signal_types::sub_:
				return parse_sub(parse_expr(it.left()),parse_expr(it.right()));
				break;
			case signal_types::mul_:
				return parse_mul(parse_expr(it.left()),parse_expr(it.right()));
				break;
			case signal_types::div_:
				return parse_div(parse_expr(it.left()),parse_expr(it.right()));
				break;
			case signal_types::mod_:
				return parse_mod(parse_expr(it.left()),parse_expr(it.right()));
				break;
			case signal_types::pow_:
				return parse_pow(parse_expr(it.left()),parse_expr(it.right()));
				break;
			case signal_types::dot_:
				return parse_dot(parse_expr(it.left()),it.right().data());
				break;
			case signal_types::und_:
				return parse_und(parse_expr(it.left()),parse_expr(it.right()));
				break;
			case signal_types::abo_:
				return parse_abo(parse_expr(it.left()),parse_expr(it.right()));
				break;
			case signal_types::asi_:
				return parse_asi(parse_expr(it.left()),parse_expr(it.right()));
				break;
			case signal_types::equ_:
				return parse_equ(parse_expr(it.left()),parse_expr(it.right()));
				break;
			case signal_types::ueq_:
				return parse_ueq(parse_expr(it.left()),parse_expr(it.right()));
				break;
			case signal_types::aeq_:
				return parse_aeq(parse_expr(it.left()),parse_expr(it.right()));
				break;
			case signal_types::neq_:
				return parse_neq(parse_expr(it.left()),parse_expr(it.right()));
				break;
			case signal_types::and_:
				return parse_and(parse_expr(it.left()),parse_expr(it.right()));
				break;
			case signal_types::or_:
				return parse_or(parse_expr(it.left()),parse_expr(it.right()));
				break;
			case signal_types::not_:
				return parse_not(parse_expr(it.right()));
				break;
			case signal_types::inc_:
				return parse_inc(parse_expr(it.left()),parse_expr(it.right()));
				break;
			case signal_types::dec_:
				return parse_dec(parse_expr(it.left()),parse_expr(it.right()));
				break;
			case signal_types::fcall_:
				return parse_fcall(parse_expr(it.left()),parse_expr(it.right()));
				break;
			case signal_types::access_:
				return parse_access(parse_expr(it.left()),parse_expr(it.right()));
				break;
			}
		}
		}
		throw syntax_error("Internal Error(Unrecognized expression).");
	}
}
