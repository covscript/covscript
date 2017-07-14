#pragma once
#include "./parser.hpp"
namespace cs {
	class domain_manager {
	public:
		using domain_t=std::shared_ptr<std::unordered_map<string,cov::any>>;
	private:
		std::deque<domain_t> m_data;
		std::deque<domain_t> m_this;
	public:
		domain_manager()
		{
			m_data.emplace_front(std::make_shared<std::unordered_map<string,cov::any>>());
		}
		domain_manager(const domain_manager&)=delete;
		~domain_manager()=default;
		void add_domain()
		{
			m_data.emplace_front(std::make_shared<std::unordered_map<string,cov::any>>());
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
		cov::any& get_var(const string& name)
		{
			for(auto& domain:m_data)
				if(domain->count(name)>0)
					return domain->at(name);
			throw syntax_error("Use of undefined variable \""+name+"\".");
		}
		cov::any& get_var_current(const string& name)
		{
			if(m_data.front()->count(name)>0)
				return m_data.front()->at(name);
			throw syntax_error("Use of undefined variable \""+name+"\" in current domain.");
		}
		cov::any& get_var_global(const string& name)
		{
			if(m_data.back()->count(name)>0)
				return m_data.back()->at(name);
			throw syntax_error("Use of undefined variable \""+name+"\" in global domain.");
		}
		cov::any& get_var_this(const string& name)
		{
			if(m_this.front()->count(name)>0)
				return m_this.front()->at(name);
			throw syntax_error("Use of undefined variable \""+name+"\" in current object.");
		}
		void add_var(const string& name,const cov::any& var)
		{
			if(var_exsist_current(name))
				get_var(name)=var;
			else
				m_data.front()->emplace(name,var);
		}
		void add_var_global(const string& name,const cov::any& var)
		{
			if(var_exsist_global(name))
				get_var_global(name)=var;
			else
				m_data.back()->emplace(name,var);
		}
		void add_struct(const std::string& name,const struct_builder& builder)
		{
			add_var(name,cov::any::make_protect<type>(builder,builder.get_hash()));
		}
		void add_type(const std::string& name,const std::function<cov::any()>& func,std::size_t hash)
		{
			add_var(name,cov::any::make_protect<type>(func,hash));
		}
	};
	struct runtime_type final {
		domain_manager storage;
		domain_manager constant_storage;
		extension_t char_ext;
		extension_t string_ext;
		extension_t list_ext;
		extension_t array_ext;
		extension_t pair_ext;
		extension_t hash_map_ext;
	};
	std::unique_ptr<runtime_type> runtime=nullptr;
	var get_type_ext(const var& a,const string& name)
	{
		if(a.type()==typeid(char))
			return cov::any::make<callable>(object_method(a,runtime->char_ext->get_var(name)),true);
		else if(a.type()==typeid(string))
			return cov::any::make<callable>(object_method(a,runtime->string_ext->get_var(name)),true);
		else if(a.type()==typeid(list))
			return cov::any::make<callable>(object_method(a,runtime->list_ext->get_var(name)),true);
		else if(a.type()==typeid(array))
			return cov::any::make<callable>(object_method(a,runtime->array_ext->get_var(name)),true);
		else if(a.type()==typeid(pair))
			return cov::any::make<callable>(object_method(a,runtime->pair_ext->get_var(name)),true);
		else if(a.type()==typeid(hash_map))
			return cov::any::make<callable>(object_method(a,runtime->hash_map_ext->get_var(name)),true);
		else
			throw syntax_error("Unsupported type.");
	}
	cov::any parse_add(const cov::any& a,const cov::any& b)
	{
		if(a.type()==typeid(number)&&b.type()==typeid(number))
			return a.const_val<number>()+b.const_val<number>();
		else if(a.type()==typeid(string))
			return cov::any::make<std::string>(a.const_val<string>()+b.to_string());
		else
			throw syntax_error("Unsupported operator operations(Add).");
	}
	cov::any parse_sub(const cov::any& a,const cov::any& b)
	{
		if(!a.usable()&&b.type()==typeid(number))
			return -b.const_val<number>();
		else if(a.type()==typeid(number)&&b.type()==typeid(number))
			return a.const_val<number>()-b.const_val<number>();
		else
			throw syntax_error("Unsupported operator operations(Sub).");
	}
	cov::any parse_mul(const cov::any& a,const cov::any& b)
	{
		if(a.type()==typeid(number)&&b.type()==typeid(number))
			return a.const_val<number>()*b.const_val<number>();
		else
			throw syntax_error("Unsupported operator operations(Mul).");
	}
	cov::any parse_div(const cov::any& a,const cov::any& b)
	{
		if(a.type()==typeid(number)&&b.type()==typeid(number))
			return a.const_val<number>()/b.const_val<number>();
		else
			throw syntax_error("Unsupported operator operations(Div).");
	}
	cov::any parse_mod(const cov::any& a,const cov::any& b)
	{
		if(a.type()==typeid(number)&&b.type()==typeid(number))
			return number(long(a.const_val<number>())%long(b.const_val<number>()));
		else
			throw syntax_error("Unsupported operator operations(Mod).");
	}
	cov::any parse_pow(const cov::any& a,const cov::any& b)
	{
		if(a.type()==typeid(number)&&b.type()==typeid(number))
			return number(std::pow(a.const_val<number>(),b.const_val<number>()));
		else
			throw syntax_error("Unsupported operator operations(Pow).");
	}
	cov::any parse_dot(const cov::any& a,token_base* b)
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
		else
			return get_type_ext(a,dynamic_cast<token_id*>(b)->get_id());
	}
	cov::any parse_typeid(token_base* a,const cov::any& b)
	{
		if(a!=nullptr)
			throw syntax_error("Wrong format of new expression.");
		else if(b.type()==typeid(type))
			return b.const_val<type>().id;
		else if(b.type()==typeid(structure))
			return b.const_val<structure>().get_hash();
		else
			return cov::hash<std::string>(b.type().name());
	}
	cov::any parse_und(const cov::any& a,const cov::any& b)
	{
		if(a.type()==typeid(number)&&b.type()==typeid(number))
			return boolean(a.const_val<number>()<b.const_val<number>());
		else
			throw syntax_error("Unsupported operator operations(Und).");
	}
	cov::any parse_abo(const cov::any& a,const cov::any& b)
	{
		if(a.type()==typeid(number)&&b.type()==typeid(number))
			return boolean(a.const_val<number>()>b.const_val<number>());
		else
			throw syntax_error("Unsupported operator operations(Abo).");
	}
	cov::any parse_ueq(const cov::any& a,const cov::any& b)
	{
		if(a.type()==typeid(number)&&b.type()==typeid(number))
			return boolean(a.const_val<number>()<=b.const_val<number>());
		else
			throw syntax_error("Unsupported operator operations(Ueq).");
	}
	cov::any parse_aeq(const cov::any& a,const cov::any& b)
	{
		if(a.type()==typeid(number)&&b.type()==typeid(number))
			return boolean(a.const_val<number>()>=b.const_val<number>());
		else
			throw syntax_error("Unsupported operator operations(Aeq).");
	}
	cov::any parse_asi(cov::any a,const cov::any& b)
	{
		a.swap(copy(b),true);
		return a;
	}
	cov::any parse_pair(const cov::any& a,const cov::any& b)
	{
		if(a.type()!=typeid(pair)&&b.type()!=typeid(pair))
			return cov::any::make<pair>(copy(a),copy(b));
		else
			throw syntax_error("Unsupported operator operations(Pair).");
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
		if(a.type()==typeid(boolean)&&b.type()==typeid(boolean))
			return boolean(a.const_val<boolean>()&&b.const_val<boolean>());
		else
			throw syntax_error("Unsupported operator operations(And).");
	}
	cov::any parse_or(const cov::any& a,const cov::any& b)
	{
		if(a.type()==typeid(boolean)&&b.type()==typeid(boolean))
			return boolean(a.const_val<boolean>()||b.const_val<boolean>());
		else
			throw syntax_error("Unsupported operator operations(Or).");
	}
	cov::any parse_not(token_base* a,const cov::any& b)
	{
		if(a!=nullptr)
			throw syntax_error("Wrong format of not expression.");
		else if(b.type()==typeid(boolean))
			return boolean(!b.const_val<boolean>());
		else
			throw syntax_error("Unsupported operator operations(Not).");
	}
	cov::any parse_inc(cov::any a,cov::any b)
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
	cov::any parse_dec(cov::any a,cov::any b)
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
	cov::any parse_fcall(const cov::any& a,cov::any b)
	{
		if(a.type()==typeid(callable))
			return a.const_val<callable>().call(b.val<array>(true));
		else if(a.type()==typeid(function))
			return a.const_val<function>().call(b.val<array>(true));
		else
			throw syntax_error("Unsupported operator operations(Fcall).");
	}
	cov::any parse_access(cov::any a,const cov::any& b)
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
	cov::any parse_expr(const cov::tree<token_base*>::iterator& it)
	{
		if(!it.usable())
			throw internal_error("The expression tree is not available.");
		token_base* token=it.data();
		if(token==nullptr)
			return cov::any();
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
				const cov::any& val=parse_expr(tree.root());
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
						map.at(p.first)=p.second;
				}
				return cov::any::make<hash_map>(std::move(map));
			}
			else
				return cov::any::make<array>(std::move(arr));
		}
		case token_types::arglist: {
			array arr;
			for(auto& tree:dynamic_cast<token_arglist*>(token)->get_arglist())
				arr.push_back(parse_expr(tree.root()));
			return cov::any::make<array>(std::move(arr));
		}
		case token_types::signal: {
			token_signal* ps=dynamic_cast<token_signal*>(token);
			if(define_var&&ps->get_signal()!=signal_types::asi_)
				throw syntax_error("Use of other signal in define.");
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
				cov::any left=parse_expr(it.left());
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
