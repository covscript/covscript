#include "./mozart/tree.hpp"
#include "./lexer.hpp"
namespace cov_basic {
	class token_fcall final:public token_base {
		token_id* mId=nullptr;
		token_sblist* mArg=nullptr;
	public:
		token_fcall()=delete;
		token_fcall(token_base* id,token_base* arg):mId(dynamic_cast<token_id*>(id)),mArg(dynamic_cast<token_sblist*>(arg)) {}
		virtual token_types get_type() const noexcept
		{
			return token_types::fcall;
		}
		token_id* get_id() noexcept
		{
			return this->mId;
		}
		token_sblist* get_arg() noexcept
		{
			return this->mArg;
		}
	};
	class token_access final:public token_base {
		token_id* mId=nullptr;
		token_mblist* mArg=nullptr;
	public:
		token_access()=delete;
		token_access(token_base* id,token_base* arg):mId(dynamic_cast<token_id*>(id)),mArg(dynamic_cast<token_mblist*>(arg)) {}
		virtual token_types get_type() const noexcept
		{
			return token_types::access;
		}
		token_id* get_id() noexcept
		{
			return this->mId;
		}
		token_mblist* get_arg() noexcept
		{
			return this->mArg;
		}
	};
	class token_array final:public token_base {
		token_lblist* mArg=nullptr;
	public:
		token_array()=delete;
		token_array(token_base* arg):mArg(dynamic_cast<token_lblist*>(arg)) {}
		virtual token_types get_type() const noexcept
		{
			return token_types::array;
		}
		token_lblist* get_arg() noexcept
		{
			return this->mArg;
		}
	};
	class token_expr final:public token_base {
		cov::tree<token_base*> mTree;
	public:
		token_expr()=delete;
		token_expr(const cov::tree<token_base*>& tree):mTree(tree) {}
		virtual token_types get_type() const noexcept
		{
			return token_types::expr;
		}
		cov::tree<token_base*>& get_tree() noexcept
		{
			return this->mTree;
		}
	};
	mapping<signal_types,int> signal_level_map = {
		{signal_types::add_,10},{signal_types::sub_,10},{signal_types::mul_,11},{signal_types::div_,11},{signal_types::mod_,12},{signal_types::pow_,12},{signal_types::dot_,14},
		{signal_types::und_,9},{signal_types::abo_,9},{signal_types::asi_,0},{signal_types::equ_,9},{signal_types::ueq_,9},{signal_types::aeq_,9},{signal_types::neq_,9},
		{signal_types::and_,7},{signal_types::or_,7},{signal_types::not_,8},{signal_types::inc_,13},{signal_types::dec_,13}
	};
	int get_signal_level(token_base* ptr)
	{
		if(ptr==nullptr)
			throw std::logic_error("Get the level of null token.");
		if(ptr->get_type()!=token_types::signal)
			throw std::logic_error("Get the level of non-signal token.");
		return signal_level_map.match(dynamic_cast<token_signal*>(ptr)->get_signal());
	}
	signal_types signal_left_associative[] = {
		signal_types::und_,signal_types::abo_,signal_types::asi_,signal_types::equ_,signal_types::ueq_,signal_types::aeq_,signal_types::neq_,signal_types::and_,signal_types::or_
	};
	bool is_left_associative(token_base* ptr)
	{
		if(ptr==nullptr)
			throw std::logic_error("Get the level of null token.");
		if(ptr->get_type()!=token_types::signal)
			throw std::logic_error("Get the level of non-signal token.");
		signal_types s=dynamic_cast<token_signal*>(ptr)->get_signal();
		for(auto& t:signal_left_associative)
			if(t==s)
				return true;
		return false;
	}
	void kill_brackets(std::deque<token_base*>& tokens)
	{
		std::deque<token_base*> oldt;
		std::swap(tokens,oldt);
		tokens.clear();
		token_base* t=nullptr;
		for(auto& ptr:oldt) {
			switch(ptr->get_type()) {
			case token_types::id:
				t=ptr;
				continue;
			case token_types::sblist:
				if(t!=nullptr) {
					for(auto& list:dynamic_cast<token_sblist*>(ptr)->get_list())
						kill_brackets(list);
					tokens.push_back(new token_fcall(t,ptr));
					t=nullptr;
					continue;
				}
				break;
			case token_types::mblist:
				if(t!=nullptr) {
					for(auto& list:dynamic_cast<token_mblist*>(ptr)->get_list())
						kill_brackets(list);
					tokens.push_back(new token_access(t,ptr));
					t=nullptr;
					continue;
				} else
					throw;
				break;
			case token_types::lblist:
				for(auto& list:dynamic_cast<token_lblist*>(ptr)->get_list())
					kill_brackets(list);
				tokens.push_back(new token_array(ptr));
				if(t!=nullptr) {
					tokens.push_back(t);
					t=nullptr;
				}
				continue;
			case token_types::signal:
				switch(dynamic_cast<token_signal*>(ptr)->get_signal()) {
				case signal_types::esb_:
					if(t!=nullptr) {
						tokens.push_back(new token_fcall(t,nullptr));
						t=nullptr;
						continue;
					} else
						throw;
					break;
				case signal_types::emb_:
					throw;
					break;
				case signal_types::elb_:
					tokens.push_back(new token_array(nullptr));
					if(t!=nullptr) {
						tokens.push_back(t);
						t=nullptr;
					}
					continue;
				}
				break;
			}
			if(t!=nullptr) {
				tokens.push_back(t);
				t=nullptr;
			}
			tokens.push_back(ptr);
		}
		if(t!=nullptr) {
			tokens.push_back(t);
			t=nullptr;
		}
	}
	void split_token(std::deque<token_base*>& raw,std::deque<token_base*>& signals,std::deque<token_base*>& objects)
	{
		bool request_signal=false;
		for(auto& ptr:raw) {
			if(ptr->get_type()==token_types::action)
				throw std::logic_error("Wrong format of expression.");
			if(ptr->get_type()==token_types::signal) {
				if(!request_signal)
					objects.push_back(nullptr);
				signals.push_back(ptr);
				request_signal=false;
			} else {
				objects.push_back(ptr);
				request_signal=true;
			}
		}
		if(!request_signal)
			objects.push_back(nullptr);
	}
	void build_tree(cov::tree<token_base*>& tree,std::deque<token_base*>& signals,std::deque<token_base*>& objects)
	{
		if(objects.size()!=signals.size()+1)
			throw std::logic_error("Symbols do not match the object.");
		for(auto& obj:objects)
		{
			if(obj!=nullptr&&obj->get_type()==token_types::sblist)
			{
				token_sblist* sbl=dynamic_cast<token_sblist*>(obj);
				if(sbl->get_list().size()!=1)
					throw;
				cov::tree<token_base*> t;
				std::deque<token_base*> s;
				std::deque<token_base*> o;
				split_token(sbl->get_list().front(),s,o);
				build_tree(t,s,o);
				obj=new token_expr(t);
			}
		}
		tree.clear();
		tree.emplace_root_left(tree.root(),signals.front());
		tree.emplace_left_left(tree.root(),objects.front());
		for(std::size_t i=1; i<signals.size(); ++i) {
			for(typename cov::tree<token_base*>::iterator it=tree.root(); it.usable(); it=it.right()) {
				if(!it.right().usable()) {
					tree.emplace_right_right(it,objects.at(i));
					break;
				}
			}
			for(typename cov::tree<token_base*>::iterator it=tree.root(); it.usable(); it=it.right()) {
				if(!it.right().usable()) {
					tree.emplace_root_left(it,signals.at(i));
					break;
				}
				if(get_signal_level(it.data())==get_signal_level(signals.at(i))) {
					if(is_left_associative(it.data()))
						tree.emplace_right_left(it,signals.at(i));
					else
						tree.emplace_root_left(it,signals.at(i));
					break;
				}
				if(get_signal_level(it.data())>get_signal_level(signals.at(i))) {
					tree.emplace_right_left(it,signals.at(i));
					break;
				}
			}
		}
		for(typename cov::tree<token_base*>::iterator it=tree.root(); it.usable(); it=it.right()) {
			if(!it.right().usable()) {
				tree.emplace_right_right(it,objects.back());
				break;
			}
		}
	}
	void kill_expr(std::deque<token_base*>& tokens)
	{
		std::deque<token_base*> oldt,expr;
		std::swap(tokens,oldt);
		tokens.clear();
		for(auto& ptr:oldt)
		{
			if(ptr->get_type()==token_types::action)
			{
				if(!expr.empty())
				{
					cov::tree<token_base*> tree;
					std::deque<token_base*> signals,objects;
					split_token(expr,signals,objects);
					build_tree(tree,signals,objects);
					tokens.push_back(new token_expr(tree));
					expr.clear();
				}
				tokens.push_back(ptr);
			}else
				expr.push_back(ptr);
		}
	}
}
