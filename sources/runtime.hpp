#pragma once
/*
* Covariant Script Runtime
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
* Copyright (C) 2017 Michael Lee(李登淳)
* Email: mikecovlee@163.com
* Github: https://github.com/mikecovlee
*/
#include "./parser.hpp"

namespace cs {
	class domain_manager {
	public:
		using domain_t=std::shared_ptr<std::unordered_map<string,var>>;
	private:
		std::deque<domain_t> m_data;
		std::deque<domain_t> m_this;
	public:
		domain_manager()
		{
			m_data.emplace_front(std::make_shared<std::unordered_map<string,var>>());
		}
		domain_manager(const domain_manager&)=delete;
		~domain_manager()=default;
		void add_domain()
		{
			m_data.emplace_front(std::make_shared<std::unordered_map<string,var>>());
		}
		void add_domain(const domain_t& dat)
		{
			m_data.emplace_front(dat);
		}
		void add_this(const domain_t& dat)
		{
			m_this.emplace_front(dat);
		}
		domain_t& get_domain()
		{
			return m_data.front();
		}
		void remove_domain()
		{
			m_data.pop_front();
		}
		void remove_this()
		{
			m_this.pop_front();
		}
		void clear_domain()
		{
			m_data.front()->clear();
		}
		bool var_exsist(const string& name)
		{
			for(auto& domain:m_data)
				if(domain->count(name)>0)
					return true;
			return false;
		}
		bool var_exsist_current(const string& name)
		{
			if(m_data.front()->count(name)>0)
				return true;
			return false;
		}
		bool var_exsist_global(const string& name)
		{
			if(m_data.back()->count(name)>0)
				return true;
			return false;
		}
		bool var_exsist_this(const string& name)
		{
			if(m_this.front()->count(name)>0)
				return true;
			return false;
		}
		var& get_var(const string& name)
		{
			for(auto& domain:m_data)
				if(domain->count(name)>0)
					return (*domain)[name];
			throw syntax_error("Use of undefined variable \""+name+"\".");
		}
		var& get_var_current(const string& name)
		{
			if(m_data.front()->count(name)>0)
				return (*m_data.front())[name];
			throw syntax_error("Use of undefined variable \""+name+"\" in current domain.");
		}
		var& get_var_global(const string& name)
		{
			if(m_data.back()->count(name)>0)
				return (*m_data.back())[name];
			throw syntax_error("Use of undefined variable \""+name+"\" in global domain.");
		}
		var& get_var_this(const string& name)
		{
			if(m_this.empty())
				throw syntax_error("Access this outside structure.");
			if(m_this.front()->count(name)>0)
				return (*m_this.front())[name];
			throw syntax_error("Use of undefined variable \""+name+"\" in current object.");
		}
		void add_var(const string& name,const var& var)
		{
			if(var_exsist_current(name))
				get_var(name)=var;
			else
				m_data.front()->emplace(name,var);
		}
		void add_var_global(const string& name,const var& var)
		{
			if(var_exsist_global(name))
				get_var_global(name)=var;
			else
				m_data.back()->emplace(name,var);
		}
		void add_struct(const std::string& name,const struct_builder& builder)
		{
			add_var(name,var::make_protect<type>(builder,builder.get_hash()));
		}
		void add_type(const std::string& name,const std::function<var()>& func,std::size_t hash)
		{
			add_var(name,var::make_protect<type>(func,hash));
		}
		void add_type(const std::string& name,const std::function<var()>& func,std::size_t hash,extension_t ext)
		{
			add_var(name,var::make_protect<type>(func,hash,ext));
		}
	};
	class runtime_type final {
	public:
		string package_name;
		domain_manager storage;
	};
	class runtime_manager final {
		std::deque<runtime_t> m_data;
	public:
		runtime_manager()=default;
		runtime_manager(const runtime_manager&)=delete;
		~runtime_manager()=default;
		void new_instance()
		{
			m_data.emplace_front(std::make_shared<runtime_type>());
		}
		runtime_t pop_instance()
		{
			runtime_t front=m_data.front();
			m_data.pop_front();
			return front;
		}
		runtime_type* operator->()
		{
			return m_data.front().get();
		}
	};
	std::shared_ptr<runtime_type> covscript(const std::string&);
	static runtime_manager runtime;
	var parse_add(const var& a,const var& b)
	{
		if(a.type()==typeid(number)&&b.type()==typeid(number))
			return a.const_val<number>()+b.const_val<number>();
		else if(a.type()==typeid(string))
			return var::make<std::string>(a.const_val<string>()+b.to_string());
		else
			throw syntax_error("Unsupported operator operations(Add).");
	}
	var parse_sub(const var& a,const var& b)
	{
		if(a.type()==typeid(number)&&b.type()==typeid(number))
			return a.const_val<number>()-b.const_val<number>();
		else
			throw syntax_error("Unsupported operator operations(Sub).");
	}
	var parse_minus(const var& b)
	{
		if(b.type()==typeid(number))
			return -b.const_val<number>();
		else
			throw syntax_error("Unsupported operator operations(Minus).");
	}
	var parse_mul(const var& a,const var& b)
	{
		if(a.type()==typeid(number)&&b.type()==typeid(number))
			return a.const_val<number>()*b.const_val<number>();
		else
			throw syntax_error("Unsupported operator operations(Mul).");
	}
	var parse_escape(const var& b)
	{
		if(b.type()==typeid(pointer)) {
			const pointer& ptr=b.const_val<pointer>();
			if(ptr.data.usable())
				return ptr.data;
			else
				throw syntax_error("Escape from null pointer.");
		}
		else
			throw syntax_error("Unsupported operator operations(Escape).");
	}
	var parse_div(const var& a,const var& b)
	{
		if(a.type()==typeid(number)&&b.type()==typeid(number))
			return a.const_val<number>()/b.const_val<number>();
		else
			throw syntax_error("Unsupported operator operations(Div).");
	}
	var parse_mod(const var& a,const var& b)
	{
		if(a.type()==typeid(number)&&b.type()==typeid(number))
			return number(long(a.const_val<number>())%long(b.const_val<number>()));
		else
			throw syntax_error("Unsupported operator operations(Mod).");
	}
	var parse_pow(const var& a,const var& b)
	{
		if(a.type()==typeid(number)&&b.type()==typeid(number))
			return number(std::pow(a.const_val<number>(),b.const_val<number>()));
		else
			throw syntax_error("Unsupported operator operations(Pow).");
	}
	var parse_dot(const var& a,token_base* b)
	{
		if(a.type()==typeid(constant_values)) {
			switch (a.const_val<constant_values>()) {
			case constant_values::global_namespace:
				return runtime->storage.get_var_global(static_cast<token_id*>(b)->get_id());
				break;
			case constant_values::current_namespace:
				return runtime->storage.get_var_current(static_cast<token_id*>(b)->get_id());
				break;
			case constant_values::this_object:
				return runtime->storage.get_var_this(static_cast<token_id*>(b)->get_id());
				break;
			default:
				throw syntax_error("Unsupported operator operations(Dot).");
				break;
			}
		}
		else if(a.type()==typeid(extension_t))
			return a.val<extension_t>(true)->get_var(static_cast<token_id*>(b)->get_id());
		else if(a.type()==typeid(structure))
			return a.val<structure>(true).get_var(static_cast<token_id*>(b)->get_id());
		else if(a.type()==typeid(type))
			return a.val<type>(true).get_var(static_cast<token_id*>(b)->get_id());
		else
			return var::make<callable>(object_method(a,a.get_ext()->get_var(static_cast<token_id*>(b)->get_id())),true);
	}
	var parse_arraw(const var& a,token_base* b)
	{
		if(a.type()==typeid(pointer))
			return parse_dot(a.const_val<pointer>().data,b);
		else
			throw syntax_error("Unsupported operator operations(Arraw).");
	}
	var parse_typeid(const var& b)
	{
		if(b.type()==typeid(type))
			return b.const_val<type>().id;
		else if(b.type()==typeid(structure))
			return b.const_val<structure>().get_hash();
		else
			return cs_impl::hash<std::string>(b.type().name());
	}
	var parse_new(const var& b)
	{
		if(b.type()==typeid(type))
			return b.const_val<type>().constructor();
		else
			throw syntax_error("Unsupported operator operations(New).");
	}
	var parse_gcnew(const var& b)
	{
		if(b.type()==typeid(type))
			return var::make<pointer>(b.const_val<type>().constructor());
		else
			throw syntax_error("Unsupported operator operations(GcNew).");
	}
	var parse_und(const var& a,const var& b)
	{
		if(a.type()==typeid(number)&&b.type()==typeid(number))
			return boolean(a.const_val<number>()<b.const_val<number>());
		else
			throw syntax_error("Unsupported operator operations(Und).");
	}
	var parse_abo(const var& a,const var& b)
	{
		if(a.type()==typeid(number)&&b.type()==typeid(number))
			return boolean(a.const_val<number>()>b.const_val<number>());
		else
			throw syntax_error("Unsupported operator operations(Abo).");
	}
	var parse_ueq(const var& a,const var& b)
	{
		if(a.type()==typeid(number)&&b.type()==typeid(number))
			return boolean(a.const_val<number>()<=b.const_val<number>());
		else
			throw syntax_error("Unsupported operator operations(Ueq).");
	}
	var parse_aeq(const var& a,const var& b)
	{
		if(a.type()==typeid(number)&&b.type()==typeid(number))
			return boolean(a.const_val<number>()>=b.const_val<number>());
		else
			throw syntax_error("Unsupported operator operations(Aeq).");
	}
	var parse_asi(var a,const var& b)
	{
		a.swap(copy(b),true);
		return a;
	}
	var parse_pair(const var& a,const var& b)
	{
		if(a.type()!=typeid(pair)&&b.type()!=typeid(pair))
			return var::make<pair>(copy(a),copy(b));
		else
			throw syntax_error("Unsupported operator operations(Pair).");
	}
	var parse_equ(const var& a,const var& b)
	{
		return boolean(a==b);
	}
	var parse_neq(const var& a,const var& b)
	{
		return boolean(a!=b);
	}
	var parse_and(const var& a,const var& b)
	{
		if(a.type()==typeid(boolean)&&b.type()==typeid(boolean))
			return boolean(a.const_val<boolean>()&&b.const_val<boolean>());
		else
			throw syntax_error("Unsupported operator operations(And).");
	}
	var parse_or(const var& a,const var& b)
	{
		if(a.type()==typeid(boolean)&&b.type()==typeid(boolean))
			return boolean(a.const_val<boolean>()||b.const_val<boolean>());
		else
			throw syntax_error("Unsupported operator operations(Or).");
	}
	var parse_not(const var& b)
	{
		if(b.type()==typeid(boolean))
			return boolean(!b.const_val<boolean>());
		else
			throw syntax_error("Unsupported operator operations(Not).");
	}
	var parse_inc(var a,var b)
	{
		if(a.usable()) {
			if(b.usable())
				throw syntax_error("Unsupported operator operations(Inc).");
			else
				return a.val<number>(true)++;
		}
		else {
			if(!b.usable())
				throw syntax_error("Unsupported operator operations(Inc).");
			else
				return ++b.val<number>(true);
		}
	}
	var parse_dec(var a,var b)
	{
		if(a.usable()) {
			if(b.usable())
				throw syntax_error("Unsupported operator operations(Dec).");
			else
				return a.val<number>(true)--;
		}
		else {
			if(!b.usable())
				throw syntax_error("Unsupported operator operations(Dec).");
			else
				return --b.val<number>(true);
		}
	}
	var parse_fcall(const var& a,var b)
	{
		if(a.type()==typeid(callable))
			return a.const_val<callable>().call(b.val<array>(true));
		else if(a.type()==typeid(function))
			return a.const_val<function>().call(b.val<array>(true));
		else
			throw syntax_error("Unsupported operator operations(Fcall).");
	}
	var parse_access(var a,const var& b)
	{
		if(a.type()==typeid(array)) {
			if(b.type()!=typeid(number))
				throw syntax_error("Index must be a number.");
			if(b.const_val<number>()<0)
				throw syntax_error("Index must above zero.");
			const array& carr=a.const_val<array>();
			std::size_t posit=b.const_val<number>();
			if(posit>=carr.size()) {
				array& arr=a.val<array>(true);
				for(std::size_t i=posit-arr.size()+1; i>0; --i)
					arr.emplace_back(number(0));
			}
			return carr.at(posit);
		}
		else if(a.type()==typeid(hash_map)) {
			const hash_map& cmap=a.const_val<hash_map>();
			if(cmap.count(b)==0)
				a.val<hash_map>(true).emplace(copy(b),number(0));
			return cmap.at(b);
		}
		else if(a.type()==typeid(string)) {
			if(b.type()!=typeid(number))
				throw syntax_error("Index must be a number.");
			return a.const_val<string>().at(b.const_val<number>());
		}
		else
			throw syntax_error("Access non-array or string object.");
	}
	var parse_expr(const cov::tree<token_base*>::iterator& it)
	{
		if(!it.usable())
			throw internal_error("The expression tree is not available.");
		token_base* token=it.data();
		if(token==nullptr)
			return var();
		switch(token->get_type()) {
		default:
			break;
		case token_types::id:
			return runtime->storage.get_var(static_cast<token_id*>(token)->get_id());
			break;
		case token_types::value:
			return static_cast<token_value*>(token)->get_value();
			break;
		case token_types::expr:
			return parse_expr(static_cast<token_expr*>(token)->get_tree().root());
			break;
		case token_types::array: {
			array arr;
			bool is_map=true;
			for(auto& tree:static_cast<token_array*>(token)->get_array()) {
				const var& val=parse_expr(tree.root());
				if(is_map&&val.type()!=typeid(pair))
					is_map=false;
				arr.push_back(copy(val));
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
						map[p.first]=p.second;
				}
				return var::make<hash_map>(std::move(map));
			}
			else
				return var::make<array>(std::move(arr));
		}
		case token_types::arglist: {
			array arr;
			for(auto& tree:static_cast<token_arglist*>(token)->get_arglist())
				arr.push_back(parse_expr(tree.root()));
			return var::make<array>(std::move(arr));
		}
		case token_types::signal: {
			switch(static_cast<token_signal*>(token)->get_signal()) {
			default:
				break;
			case signal_types::add_:
				return parse_add(parse_expr(it.left()),parse_expr(it.right()));
				break;
			case signal_types::sub_:
				return parse_sub(parse_expr(it.left()),parse_expr(it.right()));
				break;
			case signal_types::minus_:
				return parse_minus(parse_expr(it.right()));
				break;
			case signal_types::mul_:
				return parse_mul(parse_expr(it.left()),parse_expr(it.right()));
				break;
			case signal_types::escape_:
				return parse_escape(parse_expr(it.right()));
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
			case signal_types::arrow_:
				return parse_arraw(parse_expr(it.left()),it.right().data());
				break;
			case signal_types::typeid_:
				return parse_typeid(parse_expr(it.right()));
				break;
			case signal_types::new_:
				return parse_new(parse_expr(it.right()));
				break;
			case signal_types::gcnew_:
				return parse_gcnew(parse_expr(it.right()));
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
			case signal_types::pair_:
				return parse_pair(parse_expr(it.left()),parse_expr(it.right()));
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
		throw internal_error("Unrecognized expression.");
	}
	void parse_define_var(cov::tree<token_base*>& tree,define_var_profile& dvp)
	{
		const auto& it=tree.root();
		token_base* root=it.data();
		token_base* left=it.left().data();
		const auto& right=it.right();
		if(root==nullptr||left==nullptr||right.data()==nullptr||root->get_type()!=token_types::signal||static_cast<token_signal*>(root)->get_signal()!=signal_types::asi_||left->get_type()!=token_types::id)
			throw syntax_error("Wrong grammar for variable definition.");
		const std::string& id=static_cast<token_id*>(left)->get_id();
		if(runtime->storage.var_exsist_current(id))
			throw syntax_error("Redefination of variable.");
		dvp.id=id;
		dvp.expr=right;
	}
}
