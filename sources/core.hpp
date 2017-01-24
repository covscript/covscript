#pragma once
#include "../include/mozart/mozart.hpp"
#include "../include/darwin/darwin.hpp"
#include <stdexcept>
#include <string>
#include <deque>
namespace cov_basic {
	enum class signs {
	    Null,And,Or,Not,Above,Under,
	    Equ,NotEqu,AboveEqu,UnderEqu
	};
	using number=long double;
	using boolean=bool;
	using string=std::string;
	using array=std::deque<cov::any>;
	class function {
		std::deque<string> mArgs;
		std::deque<string> mBuf;
	public:
		mutable number* retval=nullptr;
		function()=delete;
		function(const std::deque<string>& args,const std::deque<string>& buf):mArgs(args),mBuf(buf) {}
		~function()=default;
		number call(array&) const;
	};
	class native_interface {
		cov::function<number(array&)> mFunc;
	public:
		native_interface()=delete;
		native_interface(const cov::function<number(array&)>& func):mFunc(func) {}
		~native_interface()=default;
		number call(array& args) const {
			return mFunc(args);
		}
	};
	class domain_manager {
		std::deque<std::deque<cov::tuple<string,cov::any>>> m_data;
	public:
		domain_manager() {
			m_data.emplace_front();
		}
		domain_manager(const domain_manager&)=delete;
		~domain_manager()=default;
		void add_domain() {
			m_data.emplace_front();
		}
		void remove_domain() {
			if(m_data.size()>1)
				m_data.pop_front();
		}
		bool var_exsist(const string& name) {
			for(auto& domain:m_data)
				for(auto& var:domain)
					if(var.get<0>()==name)
						return true;
			return false;
		}
		bool var_exsist_current(const string& name) {
			for(auto& var:m_data.front())
				if(var.get<0>()==name)
					return true;
			return false;
		}
		bool var_exsist_global(const string& name) {
			for(auto& var:m_data.back())
				if(var.get<0>()==name)
					return true;
			return false;
		}
		cov::any& get_var(const string& name) {
			for(auto& domain:m_data)
				for(auto& var:domain)
					if(var.get<0>()==name)
						return var.get<1>();
			Darwin_Error("Use of undefined variable.");
		}
		cov::any& get_var_global(const string& name) {
			for(auto& var:m_data.back())
				if(var.get<0>()==name)
					return var.get<1>();
			Darwin_Error("Use of undefined variable.");
		}
		void add_var(const string& name,const cov::any& var) {
			if(var_exsist_current(name))
				get_var(name)=var;
			else
				m_data.front().push_front({name,var});
		}
		void add_var_global(const string& name,const cov::any& var) {
			if(var_exsist_global(name))
				get_var_global(name)=var;
			else
				m_data.back().push_front({name,var});
		}
	};
	static domain_manager storage;
	void split_str(char signal,const string& str,std::deque<string>& data)
	{
		string tmp;
		bool is_str=false;
		bool is_block=false;
		for(auto& c:str) {
			if(c=='\"') {
				is_str=is_str?false:true;
				tmp+=c;
				continue;
			}
			if(c=='{') {
				is_block=true;
				tmp+=c;
				continue;
			}
			if(c=='}') {
				is_block=false;
				tmp+=c;
				continue;
			}
			if(is_str||is_block) {
				tmp+=c;
				continue;
			}
			if(std::isspace(c))
				continue;
			if(c==signal) {
				if(!tmp.empty()) {
					data.push_back(tmp);
					tmp.clear();
				}
				continue;
			}
			tmp+=c;
		}
		if(!tmp.empty())
			data.push_back(tmp);
	}
	signs match_signal(const string& str)
	{
		signs ret;
		Switch(str) {
			Default {
				ret=signs::Null;
			} EndCase;
			Case("&&") {
				ret=signs::And;
			}
			EndCase;
			Case("||") {
				ret=signs::Or;
			}
			EndCase;
			Case("!") {
				ret=signs::Not;
			}
			EndCase;
			Case(">") {
				ret=signs::Above;
			}
			EndCase;
			Case("<") {
				ret=signs::Under;
			}
			EndCase;
			Case("==") {
				ret=signs::Equ;
			}
			EndCase;
			Case("!=") {
				ret=signs::NotEqu;
			}
			EndCase;
			Case(">=") {
				ret=signs::AboveEqu;
			}
			EndCase;
			Case("<=") {
				ret=signs::UnderEqu;
			}
			EndCase;
		}
		EndSwitch;
		return ret;
	}
	bool is_signal(char signal)
	{
		switch(signal) {
		case '>':
			break;
		case '<':
			break;
		case '=':
			break;
		case '!':
			break;
		case '&':
			break;
		case '|':
			break;
		default:
			return false;
		}
		return true;
	}
	bool is_bool_exp(const string& str)
	{
		bool inside=false;
		for(auto& ch:str) {
			if(ch=='\"') {
				inside=inside?false:true;
				continue;
			}
			if(!inside&&is_signal(ch))
				return true;
		}
		return false;
	}
	bool is_str(const string& str)
	{
		bool inside=false;
		for(auto& ch:str) {
			if(ch=='\"') {
				inside=inside?false:true;
				continue;
			}
			if(!inside&&!std::isspace(ch))
				return false;
		}
		return true;
	}
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
	bool compute_boolean(const string&);
	number compute_number(const string&);
	cov::any infer_value(const string& str)
	{
		if(str.empty()||str=="Null")
			return cov::any();
		if(exsist(str))
			return get_value(str);
		if(str.find('{')!=string::npos) {
			array arr;
			auto lpos=str.find('{')+1;
			auto rpos=str.rfind('}');
			std::deque<string> buf;
			split_str(',',str.substr(lpos,rpos-lpos),buf);
			for(auto& it:buf)
				arr.push_back(infer_value(it));
			return arr;
		}
		if(str.find('[')!=string::npos) {
			auto fpos=str.rfind('[')+1;
			auto spos=str.rfind(']');
			return infer_value(str.substr(0,fpos-1)).const_val<array>().at(std::size_t(infer_value(str.substr(fpos,spos-fpos)).const_val<number>()));
		}
		if(str.find('\"')!=string::npos&&is_str(str))
			return str.substr(str.find('\"')+1,str.rfind('\"')-str.find('\"')-1);
		if(str=="True")
			return true;
		if(str=="False")
			return false;
		if(is_bool_exp(str))
			return compute_boolean(str);
		else
			return compute_number(str);
	}
	bool compute_boolean(const string & exp)
	{
		bool reverse = false;
		bool is_str = false;
		std::deque < signs > signals;
		std::deque < string > conditions;
		string tmp;
		for (int i = 0; i < exp.size();) {
			if (std::isspace(exp[i])&&!is_str) {
				++i;
				continue;
			}
			if(exp[i]=='\"') {
				is_str=is_str?false:true;
				tmp+=exp[i];
				++i;
				continue;
			}
			if (is_signal(exp[i])&&(i<exp.size()?(is_signal(exp[i+1])?true:exp[i]!='!'):exp[i]!='!')) {
				if(!tmp.empty())
					conditions.push_back(tmp);
				tmp.clear();
				string currentSignal(1,exp[i]);
				if(i<exp.size()) {
					if(is_signal(exp[++i]))
						currentSignal+=exp[i];
					else
						tmp+=exp[i];
				}
				signals.push_back(match_signal(currentSignal));
				++i;
				continue;
			}
			if(i<exp.size()&&exp[i]=='!'&&exp[i+1]=='(') {
				reverse=true;
				++i;
				continue;
			}
			tmp+=exp[i];
			++i;
		}
		if(!tmp.empty())
			conditions.push_back(tmp);
		cov::any val;
		auto parse=[](const string& str) {
			cov::any value;
			if(str[0]=='!') {
				value=infer_value(str.substr(1));
				if(value.type()!=typeid(bool))
					Darwin_Error("Can not convert value to boolean.");
				value=value.const_val<bool>()?false:true;
			} else
				value=infer_value(str);
			return value;
		};
		val=parse(conditions.front());
		conditions.pop_front();
		for(auto &it:conditions) {
			cov::any v=parse(it);
			switch(signals.front()) {
			default:
				Darwin_Error("Operator does not recognize.");
				break;
			case signs::And:
				val=(v.const_val<bool>()&&val.const_val<bool>());
				break;
			case signs::Or:
				val=(v.const_val<bool>()||val.const_val<bool>());
				break;
			case signs::Above:
				if(val.type()==v.type()) {
					if(val.type()==typeid(number)) {
						val=(val.const_val<number>()>v.const_val<number>());
						break;
					}
				}
				Darwin_Error("Compare value which have different type.");
				break;
			case signs::Under:
				if(val.type()==v.type()) {
					if(val.type()==typeid(number)) {
						val=(val.const_val<number>()<v.const_val<number>());
						break;
					}
				}
				Darwin_Error("Compare value which have different type.");
				break;
			case signs::Equ:
				val=(val==v);
				break;
			case signs::NotEqu:
				val=(val!=v);
				break;
			case signs::AboveEqu:
				if(val.type()==v.type()) {
					if(val.type()==typeid(number)) {
						val=(val.const_val<number>()>=v.const_val<number>());
						break;
					}
				}
				Darwin_Error("Compare value which have different type.");
				break;
			case signs::UnderEqu:
				if(val.type()==v.type()) {
					if(val.type()==typeid(number)) {
						val=(val.const_val<number>()<=v.const_val<number>());
						break;
					}
				}
				Darwin_Error("Compare value which have different type.");
				break;
			}
			signals.pop_front();
		}
		return val.const_val<bool>();
	}
	number compute_number(const string& exp)
	{
		bool reverse = false;
		std::deque<number> nums;
		std::deque<char>operators;
		string tmp;
		for (int i = 0; i < exp.size();) {
			if (std::isspace(exp[i])) {
				++i;
				continue;
			}
			if (exp[i] == '(') {
				int level(1), pos(++i);
				for (; pos < exp.size() && level > 0; ++pos) {
					if (exp[pos] == '(')
						++level;
					if (exp[pos] == ')')
						--level;
				}
				if (level > 0)
					Darwin_Error("The lack of corresponding brackets.");
				nums.push_back(compute_number(exp.substr(i, pos - i - 1)));
				i = pos;
				continue;
			}
			if (std::ispunct(exp[i])&&exp[i]!=':') {
				if (nums.empty()) {
					switch (exp[i]) {
					case '+':
						reverse = false;
						break;
					case '-':
						reverse = true;
						break;
					default:
						Darwin_Error("Operator does not recognize.");
					}
					++i;
					continue;
				}
				operators.push_back(exp[i]);
				++i;
				continue;
			}
			if (std::isdigit(exp[i]) || exp[i] == '.') {
				tmp.clear();
				for (; i < exp.size() && (isdigit(exp[i]) || exp[i] == '.'); ++i)
					tmp += exp[i];
				nums.push_back(std::stof(tmp));
				continue;
			}
			if (std::isalpha(exp[i]) || exp[i]==':') {
				tmp.clear();
				for (; i < exp.size() && (std::isalnum(exp[i]) || exp[i] == '_' || exp[i]==':'); ++i)
					tmp += exp[i];
				cov::any obj=infer_value(tmp);
				if (obj.type()==typeid(function)||obj.type()==typeid(native_interface)) {
					int level(1), pos(++i);
					for (; pos < exp.size() && level > 0; ++pos) {
						if (exp[pos] == '(')
							++level;
						if (exp[pos] == ')')
							--level;
					}
					if (level > 0)
						Darwin_Error("The lack of corresponding brackets.");
					std::deque <cov::any> args;
					if(pos-i>1) {
						std::deque<string> buf;
						split_str(',',exp.substr(i, pos - i - 1),buf);
						for(auto& it:buf)
							args.push_back(infer_value(it));
						/*string arglist = exp.substr(i, pos - i - 1);
						string temp;
						bool is_str=false;
						for (int i = 0; i < arglist.size(); ++i) {
							if(arglist[i]=='(')
								++level;
							if(arglist[i]==')')
								--level;
							if(arglist[i]=='\"')
								is_str=is_str?false:true;
							if (is_str || level>0 || arglist[i] != ',') {
								temp += arglist[i];
							} else {
								args.push_back(infer_value(temp));
								temp.clear();
							}
						}
						args.push_back(infer_value(temp));*/
					}
					Switch(obj.type()) {
						Case(typeid(function)) {
							nums.push_back(obj.const_val<function>().call(args));
						}
						EndCase;
						Case(typeid(native_interface)) {
							nums.push_back(obj.const_val<native_interface>().call(args));
						}
						EndCase;
					}
					EndSwitch;
					i = pos;
					continue;
				}
				nums.push_back(infer_value(tmp).const_val<number>());
				continue;
			}
			Darwin_Error("Operator does not recognize.");
		}
		if (nums.empty())
			return -1;
		number left = nums.front();
		number right = 0;
		char signal = 0;
		nums.pop_front();
		for (auto & current:nums) {
			switch (operators.front()) {
			case '+': {
				if (right != 0) {
					switch (signal) {
					case '+':
						left += right;
						break;
					case '-':
						left -= right;
						break;
					}
				}
				right = current;
				signal = '+';
				break;
			}
			case '-': {
				if (right != 0) {
					switch (signal) {
					case '+':
						left += right;
						break;
					case '-':
						left -= right;
						break;
					}
				}
				right = current;
				signal = '-';
				break;
			}
			case '*': {
				if (right != 0)
					right *= current;
				else
					left *= current;
				break;
			}
			case '/': {
				if (right != 0)
					right /= current;
				else
					left /= current;
				break;
			}
			default:
				Darwin_Error("Operator does not recognize.");
			}
			operators.pop_front();
		}
		number result = 0;
		switch (signal) {
		case '+':
			result = left + right;
			break;
		case '-':
			result = left - right;
			break;
		default:
			result = left;
			break;
		}
		if (reverse)
			result = -result;
		return result;
	}
	enum class statements {
	    If,While,For,Function
	};
	std::deque<cov::tuple<statements,std::deque<string>>> buffer;
	int bracket_count=0;
	std::deque<const function*> current_func;
	void parse_define(const string& raw_str)
	{
		string str=raw_str.substr(raw_str.find("Define")+6);
		string name;
		bool have_val=false;
		for(auto& ch:str) {
			if(std::isspace(ch))
				continue;
			if(ch=='=') {
				have_val=true;
				break;
			}
			name+=ch;
		}
		if(have_val)
			storage.add_var(name,infer_value(raw_str.substr(raw_str.find('=')+1)));
		else
			storage.add_var(name,cov::any(number(0)));
	}
	void parse_assign(const string& raw_str)
	{
		string name;
		for(auto& ch:raw_str) {
			if(std::isspace(ch))
				continue;
			if(ch=='=')
				break;
			name+=ch;
		}
		if(name.find('[')!=string::npos) {
			std::size_t pos=0;
			auto fpos=name.find('[');
			auto spos=name.find(']');
			cov::any* arr=&get_value(name.substr(0,fpos));
			std::string expr=name.substr(fpos+1,spos-fpos-1);
			while((fpos=name.find('[',spos+1))!=string::npos) {
				pos=compute_number(expr);
				if(pos>=arr->const_val<array>().size())
					arr->val<array>(true).resize(pos+1);
				arr=&arr->val<array>(true).at(pos);
				if((spos=name.find(']',spos+1))==string::npos)
					Darwin_Error("The lack of corresponding brackets.");
				expr=name.substr(fpos+1,spos-fpos-1);
			}
			pos=compute_number(expr);
			if(pos>=arr->const_val<array>().size())
				arr->val<array>(true).resize(pos+1);
			arr->val<array>(true).at(compute_number(expr)).assign(infer_value(raw_str.substr(raw_str.find('=')+1)));
		} else {
			get_value(name).assign(infer_value(raw_str.substr(raw_str.find('=')+1)),false);
		}
	}
	void parse_return(const string& str)
	{
		if(current_func.empty())
			Darwin_Error("Return in non-function area.");
		current_func.front()->retval=new number(compute_number(str.substr(str.find("Return")+6)));
	}
	void parse_if(const std::deque<string>&);
	void parse_while(const std::deque<string>&);
	void parse_for(const std::deque<string>&);
	void parse_function(const std::deque<string>&);
	void parse(const string& str)
	{
		if(bracket_count!=0) {
			if(str.find("If")!=string::npos||str.find("While")!=string::npos||str.find("For")!=string::npos||str.find("Function")!=string::npos)
				++bracket_count;
			else if(str.find("End")!=string::npos) {
				--bracket_count;
				if(bracket_count==0) {
					switch(buffer.front().get<0>()) {
					case statements::If:
						parse_if(buffer.front().get<1>());
						buffer.pop_front();
						break;
					case statements::While:
						parse_while(buffer.front().get<1>());
						buffer.pop_front();
						break;
					case statements::For:
						parse_for(buffer.front().get<1>());
						buffer.pop_front();
						break;
					case statements::Function:
						parse_function(buffer.front().get<1>());
						buffer.pop_front();
						break;
					}
					return;
				}
			}
			buffer.front().get<1>().push_back(str);
			return;
		}
		if(str.find("Define")!=string::npos) {
			parse_define(str);
			return;
		}
		if(str.find("Return")!=string::npos) {
			parse_return(str);
			return;
		}
		if(str.find("If")!=string::npos) {
			buffer.push_front(cov::tuple<statements,std::deque<string>>(statements::If,std::deque<string>({str})));
			++bracket_count;
			return;
		}
		if(str.find("While")!=string::npos) {
			buffer.push_front(cov::tuple<statements,std::deque<string>>(statements::While,std::deque<string>({str})));
			++bracket_count;
			return;
		}
		if(str.find("For")!=string::npos) {
			buffer.push_front(cov::tuple<statements,std::deque<string>>(statements::For,std::deque<string>({str})));
			++bracket_count;
			return;
		}
		if(str.find("Function")!=string::npos) {
			buffer.push_front(cov::tuple<statements,std::deque<string>>(statements::Function,std::deque<string>({str})));
			++bracket_count;
			return;
		}
		if(str.find('=')!=string::npos) {
			parse_assign(str);
			return;
		}
		if(is_bool_exp(str))
			compute_boolean(str);
		else
			compute_number(str);
	}
	void parse_if(const std::deque<string>& raw_buf)
	{
		if(raw_buf.empty())
			Darwin_Error("Parse empty code body.");
		storage.add_domain();
		string head=raw_buf.front();
		auto if_pos=head.find("If");
		auto then_pos=head.rfind("Then");
		bool result=compute_boolean(head.substr(if_pos+2,then_pos-if_pos-2));
		std::size_t pos=0;
		for(std::size_t i=1; i<raw_buf.size(); ++i) {
			if(raw_buf.at(i).find("Else")!=string::npos) {
				pos=i;
				break;
			}
		}
		if(pos==0)
			pos=raw_buf.size();
		if(result) {
			for(std::size_t i=1; i<pos; ++i)
				parse(raw_buf.at(i));
		} else {
			for(std::size_t i=pos+1; i<raw_buf.size(); ++i)
				parse(raw_buf.at(i));
		}
		storage.remove_domain();
	}
	void parse_while(const std::deque<string>& raw_buf)
	{
		if(raw_buf.empty())
			Darwin_Error("Parse empty code body.");
		storage.add_domain();
		string head=raw_buf.front();
		auto while_pos=head.find("While")+5;
		auto do_pos=head.rfind("Do");
		string condition=head.substr(while_pos,do_pos-while_pos);
		while(compute_boolean(condition)) {
			for(std::size_t i=1; i<raw_buf.size(); ++i)
				parse(raw_buf.at(i));
		}
		storage.remove_domain();
	}
	void parse_for(const std::deque<string>& raw_buf)
	{
		if(raw_buf.empty())
			Darwin_Error("Parse empty code body.");
		storage.add_domain();
		string head=raw_buf.front();
		auto for_pos=head.find("For")+3;
		std::deque<string> body;
		split_str(',',head.substr(for_pos),body);
		if(body.size()!=3)
			Darwin_Error("Wrong size of arguments.");
		storage.add_var(body.at(0),infer_value(body.at(1)));
		while(storage.get_var(body.at(0)).val<number>()<=infer_value(body.at(2)).val<number>()) {
			for(std::size_t i=1; i<raw_buf.size(); ++i)
				parse(raw_buf.at(i));
			++storage.get_var(body.at(0)).val<number>();
		}
		storage.remove_domain();
	}
	void parse_function(const std::deque<string>& raw_buf)
	{
		if(raw_buf.empty())
			Darwin_Error("Parse empty code body.");
		string head=raw_buf.front();
		string name;
		std::deque<string> arglist;
		auto lpos=head.find('(')+1;
		auto rpos=head.rfind(')');
		if(lpos!=rpos)
			split_str(',',head.substr(lpos,rpos-lpos),arglist);
		for(auto i=head.find("Function")+8; i<lpos-1; ++i)
			if(!std::isspace(head.at(i)))
				name+=head.at(i);
		storage.add_var_global(name,function(arglist,std::deque<string>(raw_buf.begin()+1,raw_buf.end())));
	}
	number function::call(array& args) const
	{
		if(args.size()!=mArgs.size())
			Darwin_Error("Wrong size of arguments");
		current_func.push_front(this);
		storage.add_domain();
		for(std::size_t i=0; i<args.size(); ++i)
			storage.add_var(mArgs.at(i),args.at(i));
		for(auto& it:mBuf) {
			parse(it);
			if(retval!=nullptr) {
				storage.remove_domain();
				current_func.pop_front();
				number ret=*retval;
				delete retval;
				retval=nullptr;
				return ret;
			}
		}
		storage.remove_domain();
		current_func.pop_front();
		return number(0);
	}
}
namespace std {
	template<>std::string to_string<bool>(const bool& v)
	{
		if(v)
			return "True";
		else
			return "False";
	}
	template<>std::string to_string<cov_basic::array>(const cov_basic::array& a)
	{
		if(a.empty())
			return "{}";
		std::string tmp="{";
		for(std::size_t i=0; i<a.size()-1; ++i)
			tmp=tmp+a.at(i).to_string()+",";
		return tmp+a.at(a.size()-1).to_string()+"}";
	}
}