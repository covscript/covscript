#include "./mozart/tree.hpp"
#include "./lexer.hpp"
namespace cov_basic {
	class token_expr final:public token_base {
		cov::tree<token_base*> mTree;
	public:
		token_expr()=delete;
		token_expr(const cov::tree<token_base*>& tree):mTree(tree) {}
		virtual token_types get_type() const noexcept override
		{
			return token_types::expr;
		}
		cov::tree<token_base*>& get_tree() noexcept {
			return this->mTree;
		}
	};
	class token_arglist final:public token_base {
		std::deque<cov::tree<token_base*>> mTreeList;
	public:
		token_arglist()=default;
		token_arglist(const std::deque<cov::tree<token_base*>>& tlist):mTreeList(tlist) {}
		virtual token_types get_type() const noexcept override
		{
			return token_types::arglist;
		}
		std::deque<cov::tree<token_base*>>& get_arglist() noexcept {
			return this->mTreeList;
		}
	};
	class token_array final:public token_base {
		std::deque<cov::tree<token_base*>> mTreeList;
	public:
		token_array()=default;
		token_array(const std::deque<cov::tree<token_base*>>& tlist):mTreeList(tlist) {}
		virtual token_types get_type() const noexcept override
		{
			return token_types::array;
		}
		std::deque<cov::tree<token_base*>>& get_array() noexcept {
			return this->mTreeList;
		}
	};
	mapping<signal_types,int> signal_level_map = {
		{signal_types::add_,10},{signal_types::sub_,10},{signal_types::mul_,11},{signal_types::div_,11},{signal_types::mod_,12},{signal_types::pow_,12},{signal_types::dot_,14},
		{signal_types::und_,9},{signal_types::abo_,9},{signal_types::asi_,-2},{signal_types::equ_,9},{signal_types::ueq_,9},{signal_types::aeq_,9},{signal_types::neq_,9},
		{signal_types::and_,7},{signal_types::or_,7},{signal_types::not_,8},{signal_types::inc_,13},{signal_types::dec_,13},{signal_types::fcall_,0},{signal_types::access_,-1}
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

	void kill_brackets(std::deque<token_base*>&);
	void gen_tree(cov::tree<token_base*>&,std::deque<token_base*>&);
	void kill_expr(std::deque<token_base*>&);

	void kill_brackets(std::deque<token_base*>& tokens)
	{
		std::deque<token_base*> oldt;
		std::swap(tokens,oldt);
		tokens.clear();
		bool expected_fcall=false;
		for(auto& ptr:oldt) {
			switch(ptr->get_type()) {
			case token_types::action:
				expected_fcall=false;
				break;
			case token_types::id:
				expected_fcall=true;
				break;
			case token_types::value:
				expected_fcall=false;
				break;
			case token_types::sblist: {
				for(auto& list:dynamic_cast<token_sblist*>(ptr)->get_list())
					kill_brackets(list);
				if(expected_fcall) {
					std::deque<cov::tree<token_base*>> tlist;
					for(auto& list:dynamic_cast<token_sblist*>(ptr)->get_list()) {
						cov::tree<token_base*> tree;
						gen_tree(tree,list);
						tlist.push_back(tree);
					}
					tokens.push_back(new token_signal(signal_types::fcall_));
					tokens.push_back(new token_arglist(tlist));
					continue;
				} else {
					expected_fcall=true;
					break;
				}
			}
			case token_types::mblist: {
				token_mblist* mbl=dynamic_cast<token_mblist*>(ptr);
				if(mbl==nullptr)
					throw;
				if(mbl->get_list().size()!=1)
					throw;
				kill_brackets(mbl->get_list().front());
				cov::tree<token_base*> tree;
				gen_tree(tree,mbl->get_list().front());
				tokens.push_back(new token_signal(signal_types::access_));
				tokens.push_back(new token_expr(tree));
				expected_fcall=true;
				continue;
			}
			case token_types::lblist: {
				for(auto& list:dynamic_cast<token_lblist*>(ptr)->get_list())
					kill_brackets(list);
				std::deque<cov::tree<token_base*>> tlist;
				for(auto& list:dynamic_cast<token_lblist*>(ptr)->get_list()) {
					cov::tree<token_base*> tree;
					gen_tree(tree,list);
					tlist.push_back(tree);
				}
				tokens.push_back(new token_array(tlist));
				expected_fcall=false;
				continue;
			}
			case token_types::signal: {
				expected_fcall=false;
				switch(dynamic_cast<token_signal*>(ptr)->get_signal()) {
				case signal_types::esb_:
					tokens.push_back(new token_signal(signal_types::fcall_));
					tokens.push_back(new token_arglist());
					continue;
				case signal_types::emb_:
					throw;
					break;
				case signal_types::elb_:
					tokens.push_back(new token_array());
					continue;
				}
			}
			}
			tokens.push_back(ptr);
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
		if(objects.empty()||signals.empty()||objects.size()!=signals.size()+1)
			throw std::logic_error("Symbols do not match the object.");
		for(auto& obj:objects) {
			if(obj!=nullptr&&obj->get_type()==token_types::sblist) {
				token_sblist* sbl=dynamic_cast<token_sblist*>(obj);
				if(sbl->get_list().size()!=1)
					throw;
				cov::tree<token_base*> t;
				gen_tree(t,sbl->get_list().front());
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
	void gen_tree(cov::tree<token_base*>& tree,std::deque<token_base*>& raw)
	{
		tree.clear();
		if(raw.size()==1) {
			tree.emplace_root_left(tree.root(),raw.front());
		} else {
			std::deque<token_base*> signals,objects;
			split_token(raw,signals,objects);
			build_tree(tree,signals,objects);
		}
	}
	void kill_expr(std::deque<token_base*>& tokens)
	{
		std::deque<token_base*> oldt,expr;
		std::swap(tokens,oldt);
		tokens.clear();
		for(auto& ptr:oldt) {
			if(ptr->get_type()==token_types::action) {
				if(!expr.empty()) {
					cov::tree<token_base*> tree;
					gen_tree(tree,expr);
					tokens.push_back(new token_expr(tree));
					expr.clear();
				}
				tokens.push_back(ptr);
			} else
				expr.push_back(ptr);
		}
	}
}