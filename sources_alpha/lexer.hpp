#pragma once
#include "./mozart/any.hpp"
#include <stdexcept>
#include <string>
#include <deque>
#include <map>
namespace cov_basic {
	enum class token_types {
		null,action,signal,id,value,sblist,mblist,lblist,expr,arglist,array
	};
	enum class action_types {
		endblock_,endline_,define_,as_,if_,then_,else_,while_,do_,for_,break_,continue_,function_,return_
	};
	enum class signal_types {
		add_,sub_,mul_,div_,mod_,pow_,com_,dot_,und_,abo_,asi_,equ_,ueq_,aeq_,neq_,and_,or_,not_,inc_,dec_,slb_,srb_,mlb_,mrb_,llb_,lrb_,esb_,emb_,elb_,fcall_,access_
	};
	class token_base {
	public:
		token_base()=default;
		token_base(const token_base&)=default;
		virtual ~token_base()=default;
		virtual token_types get_type() const noexcept=0;
	};
	class token_action final:public token_base {
		action_types mType;
	public:
		token_action()=delete;
		token_action(action_types t):mType(t) {}
		virtual token_types get_type() const noexcept override
		{
			return token_types::action;
		}
		action_types& get_action() noexcept
		{
			return this->mType;
		}
	};
	class token_signal final:public token_base {
		signal_types mType;
	public:
		token_signal()=delete;
		token_signal(signal_types t):mType(t) {}
		virtual token_types get_type() const noexcept override
		{
			return token_types::signal;
		}
		signal_types& get_signal() noexcept
		{
			return this->mType;
		}
	};
	class token_id final:public token_base {
		std::string mId;
	public:
		token_id()=delete;
		token_id(const std::string& id):mId(id) {}
		virtual token_types get_type() const noexcept override
		{
			return token_types::id;
		}
		std::string& get_id() noexcept
		{
			return this->mId;
		}
	};
	class token_value final:public token_base {
		cov::any mVal;
	public:
		token_value()=delete;
		token_value(const cov::any& val):mVal(val) {}
		virtual token_types get_type() const noexcept override
		{
			return token_types::value;
		}
		cov::any& get_value() noexcept
		{
			return this->mVal;
		}
	};
	class token_sblist final:public token_base {
		std::deque<std::deque<token_base*>> mList;
	public:
		token_sblist()=delete;
		token_sblist(const std::deque<std::deque<token_base*>>& list):mList(list) {}
		virtual token_types get_type() const noexcept override
		{
			return token_types::sblist;
		}
		std::deque<std::deque<token_base*>>& get_list() noexcept
		{
			return this->mList;
		}
	};
	class token_mblist final:public token_base {
		std::deque<std::deque<token_base*>> mList;
	public:
		token_mblist()=delete;
		token_mblist(const std::deque<std::deque<token_base*>>& list):mList(list) {}
		virtual token_types get_type() const noexcept override
		{
			return token_types::mblist;
		}
		std::deque<std::deque<token_base*>>& get_list() noexcept
		{
			return this->mList;
		}
	};
	class token_lblist final:public token_base {
		std::deque<std::deque<token_base*>> mList;
	public:
		token_lblist()=delete;
		token_lblist(const std::deque<std::deque<token_base*>>& list):mList(list) {}
		virtual token_types get_type() const noexcept override
		{
			return token_types::lblist;
		}
		std::deque<std::deque<token_base*>>& get_list() noexcept
		{
			return this->mList;
		}
	};
	template<typename Key,typename T>
	class mapping final {
		std::map<Key,T> mDat;
	public:
		mapping(std::initializer_list<std::pair<const Key, T>> l):mDat(l) {}
		bool exsist(const Key& k)
		{
			return mDat.find(k)!=mDat.end();
		}
		T match(const Key& k)
		{
			if(!exsist(k))
				throw std::logic_error("Undefined Mapping.");
			return mDat.at(k);
		}
	};
	mapping<std::string,signal_types> signal_map= {
		{"+",signal_types::add_},{"-",signal_types::sub_},{"*",signal_types::mul_},{"/",signal_types::div_},{"%",signal_types::mod_},{"^",signal_types::pow_},{">",signal_types::abo_},{"<",signal_types::und_},
		{"=",signal_types::asi_},{"&&",signal_types::and_},{"||",signal_types::or_},{"!",signal_types::not_},{"==",signal_types::equ_},{"!=",signal_types::neq_},{">=",signal_types::aeq_},{"<=",signal_types::ueq_},
		{"(",signal_types::slb_},{")",signal_types::srb_},{"[",signal_types::mlb_},{"]",signal_types::mrb_},{"{",signal_types::llb_},{"}",signal_types::lrb_},{",",signal_types::com_},{".",signal_types::dot_},
		{"()",signal_types::esb_},{"[]",signal_types::emb_},{"{}",signal_types::elb_},{"++",signal_types::inc_},{"--",signal_types::dec_}
	};
	mapping<std::string,action_types> action_map= {
		{"End",action_types::endblock_},{"Define",action_types::define_},{"As",action_types::as_},{"If",action_types::if_},{"Then",action_types::then_},{"Else",action_types::else_},{"While",action_types::while_},
		{"Do",action_types::do_},/*{"For",action_types::for_},*/{"Break",action_types::break_},{"Continue",action_types::continue_},{"Function",action_types::function_},{"Return",action_types::return_}
	};
	mapping<std::string,bool> boolean_map= {
		{"True",true},{"False",false}
	};
	char signals[]= {
		'+','-','*','/','%','^',',','.','>','<','=','&','|','!','(',')','[',']','{','}'
	};
	bool issignal(char ch)
	{
		for(auto&c:signals)
			if(c==ch)
				return true;
		return false;
	}
	void translate_into_tokens(const std::deque<char>& buff,std::deque<token_base*>& tokens)
	{
		std::string tmp;
		token_types type=token_types::null;
		bool inside_str=false;
		for(std::size_t i=0; i<buff.size();) {
			if(inside_str) {
				if(buff[i]=='\"') {
					tokens.push_back(new token_value(tmp));
					tmp.clear();
					inside_str=false;
				} else {
					tmp+=buff[i];
				}
				++i;
				continue;
			}
			switch(type) {
			case token_types::null:
				if(buff[i]=='\n') {
					tokens.push_back(new token_action(action_types::endline_));
					++i;
					continue;
				}
				if(std::isspace(buff[i])) {
					++i;
					continue;
				}
				if(issignal(buff[i])) {
					type=token_types::signal;
					continue;
				}
				if(std::isdigit(buff[i])) {
					type=token_types::value;
					continue;
				}
				if(std::isalpha(buff[i])) {
					type=token_types::id;
					continue;
				}
				throw std::logic_error("Uknown signal.");
				break;
			case token_types::id:
				if(std::isalnum(buff[i])||buff[i]=='_') {
					tmp+=buff[i];
					++i;
					continue;
				}
				type=token_types::null;
				if(action_map.exsist(tmp)) {
					tokens.push_back(new token_action(action_map.match(tmp)));
					tmp.clear();
					break;
				}
				if(boolean_map.exsist(tmp)) {
					tokens.push_back(new token_value(boolean_map.match(tmp)));
					tmp.clear();
					break;
				}
				tokens.push_back(new token_id(tmp));
				tmp.clear();
				break;
			case token_types::signal:
				if(issignal(buff[i])) {
					if(tmp.size()==1) {
						if(signal_map.exsist(tmp+buff[i])) {
							tokens.push_back(new token_signal(signal_map.match(tmp+buff[i])));
							tmp.clear();
						} else {
							if(signal_map.exsist(tmp)) {
								tokens.push_back(new token_signal(signal_map.match(tmp)));
								tmp.clear();
							}
							tmp+=buff[i];
						}
					} else
						tmp+=buff[i];
					++i;
					continue;
				}
				type=token_types::null;
				if(!tmp.empty())
					tokens.push_back(new token_signal(signal_map.match(tmp)));
				tmp.clear();
				break;
			case token_types::value:
				if(std::isdigit(buff[i])||buff[i]=='.') {
					tmp+=buff[i];
					++i;
					continue;
				}
				if(std::isalnum(buff[i])||buff[i]=='_') {
					type=token_types::id;
					tmp+=buff[i];
					++i;
					continue;
				}
				type=token_types::null;
				tokens.push_back(new token_value(std::stold(tmp)));
				tmp.clear();
				break;
			}
			if(buff[i]=='\"') {
				inside_str=true;
				++i;
				continue;
			}
		}
		switch(type) {
		case token_types::id:
			if(action_map.exsist(tmp)) {
				tokens.push_back(new token_action(action_map.match(tmp)));
				break;
			}
			if(boolean_map.exsist(tmp)) {
				tokens.push_back(new token_value(boolean_map.match(tmp)));
				break;
			}
			tokens.push_back(new token_id(tmp));
			break;
		case token_types::signal:
			tokens.push_back(new token_signal(signal_map.match(tmp)));
			break;
		case token_types::value:
			tokens.push_back(new token_value(std::stold(tmp)));
			break;
		}
	}
	void process_brackets(std::deque<token_base*>& tokens)
	{
		std::deque<token_base*> oldt;
		std::swap(tokens,oldt);
		tokens.clear();
		std::deque<std::deque<token_base*>> blist;
		std::deque<token_base*> btokens;
		std::deque<int> blist_stack;
		for(auto& ptr:oldt) {
			if(ptr->get_type()==token_types::signal) {
				switch(dynamic_cast<token_signal*>(ptr)->get_signal()) {
				case signal_types::slb_:
					blist_stack.push_front(1);
					if(blist_stack.size()==1)
						continue;
					break;
				case signal_types::mlb_:
					blist_stack.push_front(2);
					if(blist_stack.size()==1)
						continue;
					break;
				case signal_types::llb_:
					blist_stack.push_front(3);
					if(blist_stack.size()==1)
						continue;
					break;
				case signal_types::srb_:
					if(blist_stack.empty())
						throw;
					if(blist_stack.front()!=1)
						throw;
					blist_stack.pop_front();
					if(blist_stack.empty()) {
						process_brackets(btokens);
						blist.push_back(btokens);
						tokens.push_back(new token_sblist(blist));
						blist.clear();
						btokens.clear();
						continue;
					}
					break;
				case signal_types::mrb_:
					if(blist_stack.empty())
						throw;
					if(blist_stack.front()!=2)
						throw;
					blist_stack.pop_front();
					if(blist_stack.empty()) {
						process_brackets(btokens);
						blist.push_back(btokens);
						tokens.push_back(new token_mblist(blist));
						blist.clear();
						btokens.clear();
						continue;
					}
					break;
				case signal_types::lrb_:
					if(blist_stack.empty())
						throw;
					if(blist_stack.front()!=3)
						throw;
					blist_stack.pop_front();
					if(blist_stack.empty()) {
						process_brackets(btokens);
						blist.push_back(btokens);
						tokens.push_back(new token_lblist(blist));
						blist.clear();
						btokens.clear();
						continue;
					}
					break;
				case signal_types::com_:
					if(blist_stack.size()==1) {
						process_brackets(btokens);
						blist.push_back(btokens);
						btokens.clear();
						continue;
					} else
						btokens.push_back(ptr);
					break;
				}
			}
			if(blist_stack.size()==0)
				tokens.push_back(ptr);
			else
				btokens.push_back(ptr);
		}
	}
}
