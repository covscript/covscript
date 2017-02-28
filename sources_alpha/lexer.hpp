#include "../include/mozart/any.hpp"
#include <string>
#include <deque>
namespace cov_basic
{
	enum class token_types
	{
		action,signal,id,value
	};
	enum class action_types
	{
		endblock_,endline_,define_,as_,if_,then_,else_,while_,do_,for_,break_,continue_,function_,return_
	};
	enum class signal_types
	{
		add_,sub_,mul_,div_,mod_,pow_,com_,dot_,und_,abo_,asi_,equ_,ueq_,aeq_,neq_,and_,or_,not_,slb_,srb_,mlb_,mrb_,llb_,lrb_
	};
	class token_base
	{
		public:
		token_base()=default;
		token_base(const token_base&)=default;
		virtual ~token_base()=default;
		virtual token_types get_type() const noexcept=0;
	};
	class token_action final:public token_base
	{
		action_types mType;
		public:
		token_action()=delete;
		token_action(action_types t):mType(t){}
		virtual token_types get_type() const noexcept{return token_types::action;}
		action_types get_action() const noexcept{return this->mType;}
	};
	class token_signal final:public token_base
	{
		signal_types mType;
		public:
		token_signal()=delete;
		token_signal(signal_types t):mType(t){}
		virtual token_types get_type() const noexcept{return token_types::signal;}
		signal_types get_signal() const noexcept{return this->mType;}
	};
	class token_id final:public token_base
	{
		std::string mId;
		public:
		token_id()=delete;
		token_id(const std::string& id):mId(id){}
		virtual token_types get_type() const noexcept{return token_types::id;}
		const std::string& get_id() const noexcept{return this->mId;}
	};
	class token_value final:public token_base
	{
		cov::any mVal;
		public:
		token_value()=delete;
		token_value(const cov::any& val):mVal(val){}
		virtual token_types get_type() const noexcept{return token_types::value;}
		const cov::any& get_value() const noexcept{return this->mVal;}
	};
	void lexer(const std::deque<char>& buff,std::deque<token_base*>& tokens)
	{
		std::string tmp;
		token_types type=token_types::null;
		bool inside_str=false;
		for(std::size_t i=0;i<buff.size();)
		{
			if(inside_str)
			{
				if(buff[i]=='\"')
				{
					if(!tmp.empty())
						tokens.push_back(new token_value(tmp));
					inside_str=false;
				}else
					tmp+=buff[i];
				++i;
				continue;
			}
			switch(type)
			{
			case token_types::null:
				if(issignal(buff[i]))
				{
					type=token_types::signal;
					continue;
				}
				if(isdigital(buff[i]))
				{
					type=token_types::value;
					continue;
				}
				if(isalpha(buff[i]))
				{
					type=token_types::id;
					continue;
				}
				if(buff[i]=='\n')
				{
					tokens.push_back(new token_signal(signal_types::endline_));
					++i;
					continue;
				}
				break;
			case token_types::id:
				if(std::isalnum(buff[i]))
				{
					tmp+=buff[i];
					++i;
					continue;
				}
				if(isaction(tmp))
				{
					tokens.push_back(new token_action(fitaction(tmp)));
					tmp.clear();
					break;
				}
			}
		}
	}
}