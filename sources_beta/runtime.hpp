#pragma once
#include "./parser.hpp"
#include <cmath>
namespace cov_basic {
	class domain_manager {
		std::deque<std::deque<cov::tuple<string,cov::any>>> m_data;
	public:
		domain_manager()
		{
			m_data.emplace_front();
		}
		domain_manager(const domain_manager&)=delete;
		~domain_manager()=default;
		void add_domain()
		{
			m_data.emplace_front();
		}
		void remove_domain()
		{
			if(m_data.size()>1)
				m_data.pop_front();
		}
		bool var_exsist(const string& name)
		{
			for(auto& domain:m_data)
				for(auto& var:domain)
					if(var.get<0>()==name)
						return true;
			return false;
		}
		bool var_exsist_current(const string& name)
		{
			for(auto& var:m_data.front())
				if(var.get<0>()==name)
					return true;
			return false;
		}
		bool var_exsist_global(const string& name)
		{
			for(auto& var:m_data.back())
				if(var.get<0>()==name)
					return true;
			return false;
		}
		cov::any& get_var(const string& name)
		{
			for(auto& domain:m_data)
				for(auto& var:domain)
					if(var.get<0>()==name)
						return var.get<1>();
			throw syntax_error("Use of undefined variable.");
		}
		cov::any& get_var_global(const string& name)
		{
			for(auto& var:m_data.back())
				if(var.get<0>()==name)
					return var.get<1>();
			throw syntax_error("Use of undefined variable.");
		}
		void add_var(const string& name,const cov::any& var)
		{
			if(var_exsist_current(name))
				get_var(name)=var;
			else
				m_data.front().push_front({name,var});
		}
		void add_var_global(const string& name,const cov::any& var)
		{
			if(var_exsist_global(name))
				get_var_global(name)=var;
			else
				m_data.back().push_front({name,var});
		}
	};
	domain_manager storage;
	cov::any& get_value(const string& name)
	{
		auto pos=name.find("::");
		if(pos!=string::npos&&name.substr(0,pos)=="Global") {
			string n=name.substr(pos+2);
			if(storage.var_exsist_global(n))
				return storage.get_var_global(n);
		}
		return storage.get_var(name);
	}
	bool exsist(const string& name)
	{
		auto pos=name.find("::");
		if(pos!=string::npos&&name.substr(0,pos)=="Global") {
			string n=name.substr(pos+2);
			return storage.var_exsist_global(n);
		}
		return storage.var_exsist(name);
	}
	cov::any parse_add(const cov::any& a,const cov::any& b)
	{
		if(a.type()==typeid(number)) {
			if(b.type()==typeid(number)) {
				return a.const_val<number>()+b.const_val<number>();
			} else if(b.type()==typeid(boolean)) {
				return a.const_val<number>()+number(b.const_val<boolean>());
			} else
				throw;
		}
		if(a.type()==typeid(boolean)) {
			if(b.type()==typeid(number)) {
				return number(a.const_val<boolean>())+b.const_val<number>();
			} else if(b.type()==typeid(boolean)) {
				return number(a.const_val<boolean>())+number(b.const_val<boolean>());
			} else
				throw;
		}
		if(a.type()==typeid(string)) {
			return std::string(a.to_string()+b.to_string());
		}
		throw;
	}
	cov::any parse_sub(const cov::any& a,const cov::any& b)
	{
		if(a.type()==typeid(number)) {
			if(b.type()==typeid(number)) {
				return a.const_val<number>()-b.const_val<number>();
			} else if(b.type()==typeid(boolean)) {
				return a.const_val<number>()-number(b.const_val<boolean>());
			} else
				throw;
		}
		if(a.type()==typeid(boolean)) {
			if(b.type()==typeid(number)) {
				return number(a.const_val<boolean>())-b.const_val<number>();
			} else if(b.type()==typeid(boolean)) {
				return number(a.const_val<boolean>())-number(b.const_val<boolean>());
			} else
				throw;
		}
		throw;
	}
	cov::any parse_mul(const cov::any& a,const cov::any& b)
	{
		if(a.type()==typeid(number)) {
			if(b.type()==typeid(number)) {
				return number(a.const_val<number>()*b.const_val<number>());
			} else
				throw;
		}
		throw;
	}
	cov::any parse_div(const cov::any& a,const cov::any& b)
	{
		if(a.type()==typeid(number)) {
			if(b.type()==typeid(number)) {
				return number(a.const_val<number>()/b.const_val<number>());
			} else
				throw;
		}
		throw;
	}
	cov::any parse_mod(const cov::any& a,const cov::any& b)
	{
		if(a.type()==typeid(number)) {
			if(b.type()==typeid(number)) {
				return number(long(a.const_val<number>())%long(b.const_val<number>()));
			} else
				throw;
		}
		throw;
	}
	cov::any parse_pow(const cov::any& a,const cov::any& b)
	{
		if(a.type()==typeid(number)) {
			if(b.type()==typeid(number)) {
				return number(std::pow(a.const_val<number>(),b.const_val<number>()));
			} else
				throw;
		}
		throw;
	}
	cov::any parse_dot(const cov::any& a,token_base* b)
	{
		if(b==nullptr)
			throw;
		if(b->get_type()!=token_types::id)
			throw;
		std::string id=dynamic_cast<token_id*>(b)->get_id();
		if(a.type()==typeid(number)) {
			id="number_"+id;
			return get_value(id);
		} else if(a.type()==typeid(boolean)) {
			id="boolean_"+id;
			return get_value(id);
		} else if(a.type()==typeid(string)) {
			id="array_"+id;
			return get_value(id);
		} else if(a.type()==typeid(array)) {
			id="array_"+id;
			return get_value(id);
		}
		throw;
	}
	cov::any parse_und(const cov::any& a,const cov::any& b)
	{
		if(a.type()==typeid(number)) {
			if(b.type()==typeid(number)) {
				return boolean(a.const_val<number>()<b.const_val<number>());
			} else if(b.type()==typeid(boolean)) {
				return boolean(a.const_val<number>()<number(b.const_val<boolean>()));
			} else
				throw;
		}
		if(a.type()==typeid(boolean)) {
			if(b.type()==typeid(number)) {
				return boolean(number(a.const_val<boolean>())<b.const_val<number>());
			} else if(b.type()==typeid(boolean)) {
				return boolean(number(a.const_val<boolean>())<number(b.const_val<boolean>()));
			} else
				throw;
		}
		throw;
	}
	cov::any parse_abo(const cov::any& a,const cov::any& b)
	{
		if(a.type()==typeid(number)) {
			if(b.type()==typeid(number)) {
				return boolean(a.const_val<number>()>b.const_val<number>());
			} else if(b.type()==typeid(boolean)) {
				return boolean(a.const_val<number>()>number(b.const_val<boolean>()));
			} else
				throw;
		}
		if(a.type()==typeid(boolean)) {
			if(b.type()==typeid(number)) {
				return boolean(number(a.const_val<boolean>())>b.const_val<number>());
			} else if(b.type()==typeid(boolean)) {
				return boolean(number(a.const_val<boolean>())>number(b.const_val<boolean>()));
			} else
				throw;
		}
		throw;
	}
	cov::any parse_ueq(const cov::any& a,const cov::any& b)
	{
		if(a.type()==typeid(number)) {
			if(b.type()==typeid(number)) {
				return boolean(a.const_val<number>()<=b.const_val<number>());
			} else if(b.type()==typeid(boolean)) {
				return boolean(a.const_val<number>()<=number(b.const_val<boolean>()));
			} else
				throw;
		}
		if(a.type()==typeid(boolean)) {
			if(b.type()==typeid(number)) {
				return boolean(number(a.const_val<boolean>())<=b.const_val<number>());
			} else if(b.type()==typeid(boolean)) {
				return boolean(number(a.const_val<boolean>())<=number(b.const_val<boolean>()));
			} else
				throw;
		}
		throw;
	}
	cov::any parse_aeq(const cov::any& a,const cov::any& b)
	{
		if(a.type()==typeid(number)) {
			if(b.type()==typeid(number)) {
				return boolean(a.const_val<number>()>=b.const_val<number>());
			} else if(b.type()==typeid(boolean)) {
				return boolean(a.const_val<number>()>=number(b.const_val<boolean>()));
			} else
				throw;
		}
		if(a.type()==typeid(boolean)) {
			if(b.type()==typeid(number)) {
				return boolean(number(a.const_val<boolean>())>=b.const_val<number>());
			} else if(b.type()==typeid(boolean)) {
				return boolean(number(a.const_val<boolean>())>=number(b.const_val<boolean>()));
			} else
				throw;
		}
		throw;
	}
	cov::any parse_asi(cov::any a,cov::any b)
	{
		b.clone();
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
				throw;
		}
		if(a.type()==typeid(number)) {
			if(b.type()==typeid(boolean)) {
				return boolean(boolean(a.const_val<number>())&&b.const_val<boolean>());
			} else if(b.type()==typeid(number)) {
				return boolean(number(a.const_val<number>())&&number(b.const_val<number>()));
			} else
				throw;
		}
		throw;
	}
	cov::any parse_or(const cov::any& a,const cov::any& b)
	{
		if(a.type()==typeid(boolean)) {
			if(b.type()==typeid(boolean)) {
				return boolean(a.const_val<boolean>()||b.const_val<boolean>());
			} else if(b.type()==typeid(number)) {
				return boolean(a.const_val<boolean>()||boolean(b.const_val<number>()));
			} else
				throw;
		}
		if(a.type()==typeid(number)) {
			if(b.type()==typeid(boolean)) {
				return boolean(boolean(a.const_val<number>())||b.const_val<boolean>());
			} else if(b.type()==typeid(number)) {
				return boolean(number(a.const_val<number>())||number(b.const_val<number>()));
			} else
				throw;
		}
		throw;
	}
	cov::any parse_not(const cov::any& b)
	{
		if(b.type()==typeid(number)) {
			return boolean(!b.const_val<number>());
		} else if(b.type()==typeid(boolean)) {
			return boolean(!b.const_val<boolean>());
		}
		throw;
	}
	cov::any parse_inc(token_base* a,token_base* b)
	{
		if(a==nullptr) {
			if(b==nullptr)
				throw;
			if(b->get_type()!=token_types::id)
				throw;
			std::string id=dynamic_cast<token_id*>(b)->get_id();
			cov::any& val=get_value(id);
			if(val.type()!=typeid(number))
				throw;
			return val=val.const_val<number>()+1;
		} else {
			if(b!=nullptr)
				throw;
			if(a->get_type()!=token_types::id)
				throw;
			std::string id=dynamic_cast<token_id*>(a)->get_id();
			cov::any& val=get_value(id);
			if(val.type()!=typeid(number))
				throw;
			return val=val.const_val<number>()+1;
		}
	}
	cov::any parse_dec(token_base* a,token_base* b)
	{
		if(a==nullptr) {
			if(b==nullptr)
				throw;
			if(b->get_type()!=token_types::id)
				throw;
			std::string id=dynamic_cast<token_id*>(b)->get_id();
			cov::any& val=get_value(id);
			if(val.type()!=typeid(number))
				throw;
			return val=val.const_val<number>()-1;
		} else {
			if(b!=nullptr)
				throw;
			if(a->get_type()!=token_types::id)
				throw;
			std::string id=dynamic_cast<token_id*>(a)->get_id();
			cov::any& val=get_value(id);
			if(val.type()!=typeid(number))
				throw;
			return val=val.const_val<number>()-1;
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
		if(posit>=arr.size())
		{
			for(std::size_t i=posit-arr.size()+1;i>0;--i)
				arr.emplace_back(number(0));
		}
		return arr.at(posit);
	}
	bool define_var=false;
	cov::any parse_expr(cov::tree<token_base*>::iterator it)
	{
		if(!it.usable())
			throw;
		token_base* token=it.data();
		if(token==nullptr)
			return cov::any();
		switch(token->get_type()) {
		case token_types::id:
		{
			std::string id=dynamic_cast<token_id*>(token)->get_id();
			if(!exsist(id))
			{
				if(define_var)
					storage.add_var(id,number(0));
				else
					throw syntax_error("Undefined variable.");
			}
			return get_value(id);
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
			for(auto& tree:dynamic_cast<token_array*>(token)->get_array())
				arr.push_back(parse_expr(tree.root()));
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
				return parse_inc(it.left().data(),it.right().data());
				break;
			case signal_types::dec_:
				return parse_dec(it.left().data(),it.right().data());
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
		throw;
	}
}
