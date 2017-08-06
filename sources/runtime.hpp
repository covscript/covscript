#pragma once
#include "./parser.hpp"
namespace cs {
	class domain_manager {
	public:
		using domain_t=std::shared_ptr<std::unordered_map<string,cs::any>>;
	private:
		std::deque<domain_t> m_data;
		std::deque<domain_t> m_this;
	public:
		domain_manager()
		{
			m_data.emplace_front(std::make_shared<std::unordered_map<string,cs::any>>());
		}
		domain_manager(const domain_manager&)=delete;
		~domain_manager()=default;
		void add_domain()
		{
			m_data.emplace_front(std::make_shared<std::unordered_map<string,cs::any>>());
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
			if(m_data.size()>1)
				m_data.pop_front();
		}
		void remove_this()
		{
			if(m_this.size()>1)
				m_this.pop_front();
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
		cs::any& get_var(const string& name)
		{
			for(auto& domain:m_data)
				if(domain->count(name)>0)
					return (*domain)[name];
			throw syntax_error("Use of undefined variable \""+name+"\".");
		}
		cs::any& get_var_current(const string& name)
		{
			if(m_data.front()->count(name)>0)
				return (*m_data.front())[name];
			throw syntax_error("Use of undefined variable \""+name+"\" in current domain.");
		}
		cs::any& get_var_global(const string& name)
		{
			if(m_data.back()->count(name)>0)
				return (*m_data.back())[name];
			throw syntax_error("Use of undefined variable \""+name+"\" in global domain.");
		}
		cs::any& get_var_this(const string& name)
		{
			if(m_this.front()->count(name)>0)
				return (*m_this.front())[name];
			throw syntax_error("Use of undefined variable \""+name+"\" in current object.");
		}
		void add_var(const string& name,const cs::any& var)
		{
			if(var_exsist_current(name))
				get_var(name)=var;
			else
				m_data.front()->emplace(name,var);
		}
		void add_var_global(const string& name,const cs::any& var)
		{
			if(var_exsist_global(name))
				get_var_global(name)=var;
			else
				m_data.back()->emplace(name,var);
		}
		void add_struct(const std::string& name,const struct_builder& builder)
		{
			add_var(name,cs::any::make_protect<type>(builder,builder.get_hash()));
		}
		void add_type(const std::string& name,const std::function<cs::any()>& func,std::size_t hash)
		{
			add_var(name,cs::any::make_protect<type>(func,hash));
		}
		void add_type(const std::string& name,const std::function<cs::any()>& func,std::size_t hash,extension_t ext)
		{
			add_var(name,cs::any::make_protect<type>(func,hash,ext));
		}
	};
	struct runtime_type final {
		domain_manager storage;
		domain_manager constant_storage;
	};
	std::unique_ptr<runtime_type> runtime=nullptr;
	cs::any parse_add(const cs::any& a,const cs::any& b)
	{
		if(a.type()==typeid(number)&&b.type()==typeid(number))
			return a.const_val<number>()+b.const_val<number>();
		else if(a.type()==typeid(string))
			return cs::any::make<std::string>(a.const_val<string>()+b.to_string());
		else
			throw syntax_error("Unsupported operator operations(Add).");
	}
	cs::any parse_sub(const cs::any& a,const cs::any& b)
	{
		if(!a.usable()&&b.type()==typeid(number))
			return -b.const_val<number>();
		else if(a.type()==typeid(number)&&b.type()==typeid(number))
			return a.const_val<number>()-b.const_val<number>();
		else
			throw syntax_error("Unsupported operator operations(Sub).");
	}
	cs::any parse_mul(const cs::any& a,const cs::any& b)
	{
		if(a.type()==typeid(number)&&b.type()==typeid(number))
			return a.const_val<number>()*b.const_val<number>();
		else
			throw syntax_error("Unsupported operator operations(Mul).");
	}
	cs::any parse_div(const cs::any& a,const cs::any& b)
	{
		if(a.type()==typeid(number)&&b.type()==typeid(number))
			return a.const_val<number>()/b.const_val<number>();
		else
			throw syntax_error("Unsupported operator operations(Div).");
	}
	cs::any parse_mod(const cs::any& a,const cs::any& b)
	{
		if(a.type()==typeid(number)&&b.type()==typeid(number))
			return number(long(a.const_val<number>())%long(b.const_val<number>()));
		else
			throw syntax_error("Unsupported operator operations(Mod).");
	}
	cs::any parse_pow(const cs::any& a,const cs::any& b)
	{
		if(a.type()==typeid(number)&&b.type()==typeid(number))
			return number(std::pow(a.const_val<number>(),b.const_val<number>()));
		else
			throw syntax_error("Unsupported operator operations(Pow).");
	}
	cs::any parse_dot(const cs::any& a,token_base* b)
	{
		if(b==nullptr)
			throw internal_error("Null Pointer Accessed.");
		else if(b->get_type()!=token_types::id)
			throw syntax_error("Unsupported operator operations(Dot).");
		else if(a.type()==typeid(constant_values)) {
			switch (a.const_val<constant_values>()) {
			case constant_values::global_namespace:
				return runtime->storage.get_var_global(dynamic_cast<token_id*>(b)->get_id());
				break;
			case constant_values::current_namespace:
				return runtime->storage.get_var_current(dynamic_cast<token_id*>(b)->get_id());
				break;
			case constant_values::this_object:
				return runtime->storage.get_var_this(dynamic_cast<token_id*>(b)->get_id());
				break;
			default:
				throw syntax_error("Unsupported operator operations(Dot).");
				break;
			}
		}
		else if(a.type()==typeid(extension_t))
			return a.val<extension_t>(true)->get_var(dynamic_cast<token_id*>(b)->get_id());
		else if(a.type()==typeid(structure))
			return a.val<structure>(true).get_var(dynamic_cast<token_id*>(b)->get_id());
		else if(a.type()==typeid(type))
			return a.val<type>(true).get_var(dynamic_cast<token_id*>(b)->get_id());
		else
			return cs::any::make<callable>(object_method(a,a.get_ext()->get_var(dynamic_cast<token_id*>(b)->get_id())),true);
	}
	cs::any parse_typeid(token_base* a,const cs::any& b)
	{
		if(a!=nullptr)
			throw syntax_error("Wrong format of new expression.");
		else if(b.type()==typeid(type))
			return b.const_val<type>().id;
		else if(b.type()==typeid(structure))
			return b.const_val<structure>().get_hash();
		else
			return cs_any::hash<std::string>(b.type().name());
	}
	cs::any parse_und(const cs::any& a,const cs::any& b)
	{
		if(a.type()==typeid(number)&&b.type()==typeid(number))
			return boolean(a.const_val<number>()<b.const_val<number>());
		else
			throw syntax_error("Unsupported operator operations(Und).");
	}
	cs::any parse_abo(const cs::any& a,const cs::any& b)
	{
		if(a.type()==typeid(number)&&b.type()==typeid(number))
			return boolean(a.const_val<number>()>b.const_val<number>());
		else
			throw syntax_error("Unsupported operator operations(Abo).");
	}
	cs::any parse_ueq(const cs::any& a,const cs::any& b)
	{
		if(a.type()==typeid(number)&&b.type()==typeid(number))
			return boolean(a.const_val<number>()<=b.const_val<number>());
		else
			throw syntax_error("Unsupported operator operations(Ueq).");
	}
	cs::any parse_aeq(const cs::any& a,const cs::any& b)
	{
		if(a.type()==typeid(number)&&b.type()==typeid(number))
			return boolean(a.const_val<number>()>=b.const_val<number>());
		else
			throw syntax_error("Unsupported operator operations(Aeq).");
	}
	cs::any parse_asi(cs::any a,const cs::any& b)
	{
		a.swap(copy(b),true);
		return a;
	}
	cs::any parse_pair(const cs::any& a,const cs::any& b)
	{
		if(a.type()!=typeid(pair)&&b.type()!=typeid(pair))
			return cs::any::make<pair>(copy(a),copy(b));
		else
			throw syntax_error("Unsupported operator operations(Pair).");
	}
	cs::any parse_equ(const cs::any& a,const cs::any& b)
	{
		return boolean(a==b);
	}
	cs::any parse_neq(const cs::any& a,const cs::any& b)
	{
		return boolean(a!=b);
	}
	cs::any parse_and(const cs::any& a,const cs::any& b)
	{
		if(a.type()==typeid(boolean)&&b.type()==typeid(boolean))
			return boolean(a.const_val<boolean>()&&b.const_val<boolean>());
		else
			throw syntax_error("Unsupported operator operations(And).");
	}
	cs::any parse_or(const cs::any& a,const cs::any& b)
	{
		if(a.type()==typeid(boolean)&&b.type()==typeid(boolean))
			return boolean(a.const_val<boolean>()||b.const_val<boolean>());
		else
			throw syntax_error("Unsupported operator operations(Or).");
	}
	cs::any parse_not(token_base* a,const cs::any& b)
	{
		if(a!=nullptr)
			throw syntax_error("Wrong format of not expression.");
		else if(b.type()==typeid(boolean))
			return boolean(!b.const_val<boolean>());
		else
			throw syntax_error("Unsupported operator operations(Not).");
	}
	cs::any parse_inc(cs::any a,cs::any b)
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
	cs::any parse_dec(cs::any a,cs::any b)
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
	cs::any parse_fcall(const cs::any& a,cs::any b)
	{
		if(a.type()==typeid(callable))
			return a.const_val<callable>().call(b.val<array>(true));
		else if(a.type()==typeid(function))
			return a.const_val<function>().call(b.val<array>(true));
		else
			throw syntax_error("Unsupported operator operations(Fcall).");
	}
	cs::any parse_access(cs::any a,const cs::any& b)
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
	bool define_var=false;
	cs::any parse_expr(const cov::tree<token_base*>::iterator& it)
	{
		if(!it.usable())
			throw internal_error("The expression tree is not available.");
		token_base* token=it.data();
		if(token==nullptr)
			return cs::any();
		switch(token->get_type()) {
		case token_types::id: {
			const std::string& id=dynamic_cast<token_id*>(token)->get_id();
			if(define_var) {
				if(!runtime->storage.var_exsist_current(id))
					runtime->storage.add_var(id,number(0));
				define_var=false;
			}
			return runtime->storage.get_var(id);
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
			bool is_map=true;
			for(auto& tree:dynamic_cast<token_array*>(token)->get_array()) {
				const cs::any& val=parse_expr(tree.root());
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
				return cs::any::make<hash_map>(std::move(map));
			}
			else
				return cs::any::make<array>(std::move(arr));
		}
		case token_types::arglist: {
			array arr;
			for(auto& tree:dynamic_cast<token_arglist*>(token)->get_arglist())
				arr.push_back(parse_expr(tree.root()));
			return cs::any::make<array>(std::move(arr));
		}
		case token_types::signal: {
			token_signal* ps=dynamic_cast<token_signal*>(token);
			if(define_var&&ps->get_signal()!=signal_types::asi_)
				throw syntax_error("Use of other signal in var definition.");
			switch(ps->get_signal()) {
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
			case signal_types::typeid_:
				return parse_typeid(it.left().data(),parse_expr(it.right()));
				break;
			case signal_types::und_:
				return parse_und(parse_expr(it.left()),parse_expr(it.right()));
				break;
			case signal_types::abo_:
				return parse_abo(parse_expr(it.left()),parse_expr(it.right()));
				break;
			case signal_types::asi_: {
				cs::any left=parse_expr(it.left());
				return parse_asi(left,parse_expr(it.right()));
				break;
			}
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
				return parse_not(it.left().data(),parse_expr(it.right()));
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
}
