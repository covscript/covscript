#pragma once
/*
* Covariant Script Symbols
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
#include "./core.hpp"

namespace cs {
	enum class token_types {
		null,endline,action,signal,id,value,sblist,mblist,lblist,expr,arglist,array
	};
	enum class action_types {
		import_,package_,namespace_,struct_,block_,endblock_,var_,constant_,if_,else_,switch_,case_,default_,while_,loop_,until_,for_,to_,step_,iterate_,break_,continue_,function_,return_,try_,catch_,throw_
	};
	enum class signal_types {
		add_,sub_,mul_,div_,mod_,pow_,com_,dot_,und_,abo_,asi_,equ_,ueq_,aeq_,neq_,and_,or_,not_,inc_,dec_,pair_,slb_,srb_,mlb_,mrb_,llb_,lrb_,esb_,emb_,elb_,fcall_,access_,typeid_,new_,gcnew_,arrow_,lambda_,escape_,minus_,vardef_
	};
	template<typename Key,typename T>
	class mapping final {
		std::map<Key,T> mDat;
	public:
		mapping(std::initializer_list<std::pair<const Key, T>> l):mDat(l) {}
		bool exsist(const Key& k)
		{
			return mDat.count(k)>0;
		}
		T match(const Key& k)
		{
			if(!exsist(k))
				throw syntax_error("Undefined Mapping.");
			return mDat.at(k);
		}
	};
	class token_base {
		static garbage_collector<token_base> gc;
	public:
		static void* operator new(std::size_t size)
		{
			void* ptr=::operator new(size);
			gc.add(ptr);
			return ptr;
		}
		static void operator delete(void* ptr)
		{
			gc.remove(ptr);
			::operator delete(ptr);
		}
		token_base()=default;
		token_base(const token_base&)=default;
		virtual ~token_base()=default;
		virtual token_types get_type() const noexcept=0;
	};
	garbage_collector<token_base> token_base::gc;
	class token_endline final:public token_base {
		std::size_t mNum=0;
		std::string mFile="<Unknown>";
		std::string mCode="<Unknown>";
	public:
		token_endline()=default;
		token_endline(std::size_t num):mNum(num) {}
		token_endline(std::size_t num,const std::string& file):mNum(num),mFile(file) {}
		token_endline(std::size_t num,const std::string& file,const std::string& code):mNum(num),mFile(file),mCode(code) {}
		virtual token_types get_type() const noexcept override
		{
			return token_types::endline;
		}
		std::size_t get_num() const noexcept
		{
			return this->mNum;
		}
		const std::string& get_file() const noexcept
		{
			return this->mFile;
		}
		const std::string& get_code() const noexcept
		{
			return this->mCode;
		}
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
		action_types get_action() const noexcept
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
		signal_types get_signal() const noexcept
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
		const std::string& get_id() const noexcept
		{
			return this->mId;
		}
	};
	class token_value final:public token_base {
		var mVal;
		static std::deque<std::deque<token_value*>> mList;
	public:
		token_value()=delete;
		token_value(const var& val):mVal(val)
		{
			if(!mList.empty()&&!mVal.is_protect()) {
				mVal.protect();
				mList.front().push_back(this);
			}
		}
		virtual token_types get_type() const noexcept override
		{
			return token_types::value;
		}
		var& get_value() noexcept
		{
			return this->mVal;
		}
		static void clean()
		{
			mList.emplace_front();
		}
		static void mark()
		{
			for(auto& it:mList.front())
				it->mVal.constant();
			mList.pop_front();
		}
	};
	std::deque<std::deque<token_value*>> token_value::mList;
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
	mapping<std::string,signal_types> signal_map= {
		{"+",signal_types::add_},{"-",signal_types::sub_},{"*",signal_types::mul_},{"/",signal_types::div_},{"%",signal_types::mod_},{"^",signal_types::pow_},{">",signal_types::abo_},{"<",signal_types::und_},
		{"=",signal_types::asi_},{"&&",signal_types::and_},{"||",signal_types::or_},{"!",signal_types::not_},{"==",signal_types::equ_},{"!=",signal_types::neq_},{">=",signal_types::aeq_},{"<=",signal_types::ueq_},
		{"(",signal_types::slb_},{")",signal_types::srb_},{"[",signal_types::mlb_},{"]",signal_types::mrb_},{"{",signal_types::llb_},{"}",signal_types::lrb_},{",",signal_types::com_},{".",signal_types::dot_},
		{"()",signal_types::esb_},{"[]",signal_types::emb_},{"{}",signal_types::elb_},{"++",signal_types::inc_},{"--",signal_types::dec_},{":",signal_types::pair_},{"->",signal_types::arrow_}
	};
	mapping<std::string,action_types> action_map= {
		{"import",action_types::import_},{"package",action_types::package_},{"namespace",action_types::namespace_},{"struct",action_types::struct_},{"block",action_types::block_},{"end",action_types::endblock_},{"var",action_types::var_},{"const",action_types::constant_},{"if",action_types::if_},{"else",action_types::else_},{"switch",action_types::switch_},{"case",action_types::case_},{"default",action_types::default_},
		{"while",action_types::while_},{"until",action_types::until_},{"loop",action_types::loop_},{"for",action_types::for_},{"to",action_types::to_},{"step",action_types::step_},{"iterate",action_types::iterate_},{"break",action_types::break_},{"continue",action_types::continue_},{"function",action_types::function_},{"return",action_types::return_},{"try",action_types::try_},{"catch",action_types::catch_},{"throw",action_types::throw_}
	};
	enum class constant_values {
		current_namespace,global_namespace
	};
	mapping<std::string,std::function<token_base*()>> reserved_map= {
		{"and",[]()->token_base*{return new token_signal(signal_types::and_);}},{"or",[]()->token_base*{return new token_signal(signal_types::or_);}},{"not",[]()->token_base*{return new token_signal(signal_types::not_);}},{"typeid",[]()->token_base*{return new token_signal(signal_types::typeid_);}},{"new",[]()->token_base*{return new token_signal(signal_types::new_);}},{"gcnew",[]()->token_base*{return new token_signal(signal_types::gcnew_);}},
		{"current",[]()->token_base*{return new token_value(constant_values::current_namespace);}},{"global",[]()->token_base*{return new token_value(constant_values::global_namespace);}},{"null",[]()->token_base*{return new token_value(null_pointer);}},
		{"true",[]()->token_base*{return new token_value(true);}},{"false",[]()->token_base*{return new token_value(false);}}
	};
	mapping<char,char> escape_map= {
		{'a','\a'},{'b','\b'},{'f','\f'},{'n','\n'},{'r','\r'},{'t','\t'},{'v','\v'},{'\\','\\'},{'\'','\''},{'\"','\"'},{'0','\0'}
	};
	constexpr char signals[]= {
		'+','-','*','/','%','^',',','.','>','<','=','&','|','!','(',')','[',']','{','}',':'
	};
	bool issignal(char ch)
	{
		for(auto&c:signals)
			if(c==ch)
				return true;
		return false;
	}
}
