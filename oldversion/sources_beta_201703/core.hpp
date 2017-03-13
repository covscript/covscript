#pragma once
#include "../include/mozart/mozart.hpp"
#include "../include/darwin/darwin.hpp"
#include <stdexcept>
#include <string>
#include <deque>
namespace cov_basic {
	enum class operators {
		null,add,cut,mul,div,mod,pow,_and_,_or_,abo,und,equ,neq,aeq,ueq
	};
	using number=long double;
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
		number call(array& args) const
		{
			return mFunc(args);
		}
	};
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
			Darwin_Error("Use of undefined variable.");
		}
		cov::any& get_var_global(const string& name)
		{
			for(auto& var:m_data.back())
				if(var.get<0>()==name)
					return var.get<1>();
			Darwin_Error("Use of undefined variable.");
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
	static domain_manager storage;
	void split_str(char signal,const string& str,std::deque<string>& data)
	{
		string tmp;
		int brackets_level=0;
		int blocks_level=0;
		bool is_str=false;
		for(auto& c:str) {
			if(c=='\"') {
				is_str=is_str?false:true;
				tmp+=c;
				continue;
			}
			if(c=='(') {
				++brackets_level;
				tmp+=c;
				continue;
			}
			if(c==')') {
				--brackets_level;
				tmp+=c;
				continue;
			}
			if(c=='{') {
				++blocks_level;
				tmp+=c;
				continue;
			}
			if(c=='}') {
				--blocks_level;
				tmp+=c;
				continue;
			}
			if(is_str||brackets_level>0||blocks_level>0) {
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
		if(brackets_level!=0||blocks_level!=0)
			Darwin_Error("The lack of corresponding brackets.");
		if(!tmp.empty())
			data.push_back(tmp);
	}
	inline int get_operator_level(operators op)
	{
		switch(op) {
		case operators::null:
			return 10;
		case operators::add:
			return 1;
		case operators::cut:
			return 1;
		case operators::mul:
			return 2;
		case operators::div:
			return 2;
		case operators::mod:
			return 3;
		case operators::pow:
			return 4;
		case operators::_and_:
			return -2;
		case operators::_or_:
			return -2;
		case operators::abo:
			return -1;
		case operators::und:
			return -1;
		case operators::equ:
			return -1;
		case operators::neq:
			return -1;
		case operators::aeq:
			return -1;
		case operators::ueq:
			return -1;
		}
	};
	inline operators parse_operators(const std::string& str)
	{
		operators op=operators::null;
		CovSwitch(str) {
			CovCase("+") {
				op=operators::add;
			}
			EndCovCase;
			CovCase("-") {
				op=operators::cut;
			}
			EndCovCase;
			CovCase("*") {
				op=operators::mul;
			}
			EndCovCase;
			CovCase("/") {
				op=operators::div;
			}
			EndCovCase;
			CovCase("%") {
				op=operators::mod;
			}
			EndCovCase;
			CovCase("^") {
				op=operators::pow;
			}
			EndCovCase;
			CovCase("&&") {
				op=operators::_and_;
			}
			EndCovCase;
			CovCase("||") {
				op=operators::_or_;
			}
			EndCovCase;
			CovCase(">") {
				op=operators::abo;
			}
			EndCovCase;
			CovCase("<") {
				op=operators::und;
			}
			EndCovCase;
			CovCase("==") {
				op=operators::equ;
			}
			EndCovCase;
			CovCase("!=") {
				op=operators::neq;
			}
			EndCovCase;
			CovCase(">=") {
				op=operators::aeq;
			}
			EndCovCase;
			CovCase("<=") {
				op=operators::ueq;
			}
			EndCovCase;
		}
		EndCovSwitch;
		return op;
	}
	inline std::string get_operator_string(operators op)
	{
		switch(op) {
		case operators::null:
			return "Null";
		case operators::add:
			return "+";
		case operators::cut:
			return "-";
		case operators::mul:
			return "*";
		case operators::div:
			return "/";
		case operators::mod:
			return "%";
		case operators::pow:
			return "^";
		case operators::_and_:
			return "&&";
		case operators::_or_:
			return "||";
		case operators::abo:
			return ">";
		case operators::und:
			return "<";
		case operators::equ:
			return "==";
		case operators::neq:
			return "!=";
		case operators::aeq:
			return ">=";
		case operators::ueq:
			return "<=";
		}
	}
	inline bool is_logic_signal(char ch)
	{
		switch(ch) {
		case '+':
			return true;
		case '-':
			return true;
		case '*':
			return true;
		case '/':
			return true;
		case '%':
			return true;
		case '^':
			return true;
		case '&':
			return true;
		case '|':
			return true;
		case '=':
			return true;
		case '>':
			return true;
		case '<':
			return true;
		}
		return false;
	}
	class tree_node_base {
	protected:
		tree_node_base* mRoot=nullptr;
		tree_node_base* mLeft=nullptr;
		tree_node_base* mRight=nullptr;
	public:
		tree_node_base()=default;
		tree_node_base(tree_node_base* root,tree_node_base* left,tree_node_base* right):mRoot(root),mLeft(left),mRight(right) {}
		tree_node_base(const tree_node_base&)=default;
		virtual ~tree_node_base()=default;
		virtual std::type_index get_type() const noexcept final
		{
			return typeid(*this);
		}
		virtual tree_node_base* get_root() noexcept final
		{
			return this->mRoot;
		}
		virtual void set_root(tree_node_base* node) noexcept final
		{
			this->mRoot=node;
		}
		virtual tree_node_base* get_left_branch() noexcept final
		{
			return this->mLeft;
		}
		virtual void set_left_branch(tree_node_base* node) noexcept final
		{
			this->mLeft=node;
		}
		virtual tree_node_base* get_right_branch() noexcept final
		{
			return this->mRight;
		}
		virtual void set_right_branch(tree_node_base* node) noexcept final
		{
			this->mRight=node;
		}
	};
	class tree_op_node:public tree_node_base {
	protected:
		operators mData=operators::null;
	public:
		tree_op_node()=default;
		tree_op_node(tree_node_base* root,tree_node_base* left,tree_node_base* right,operators data):tree_node_base(root,left,right),mData(data) {}
		virtual ~tree_op_node()=default;
		operators get_data() noexcept
		{
			return this->mData;
		}
		void set_data(operators op) noexcept
		{
			this->mData=op;
		}
	};
	class tree_val_node:public tree_node_base {
	protected:
		number mData;
	public:
		tree_val_node()=default;
		tree_val_node(tree_node_base* root,tree_node_base* left,tree_node_base* right,number data):tree_node_base(root,left,right),mData(data) {}
		virtual ~tree_val_node()=default;
		number get_data() noexcept
		{
			return this->mData;
		}
		void set_data(number val) noexcept
		{
			this->mData=val;
		}
	};
	tree_node_base* generate_tree(const std::deque<number>& val_stack,const std::deque<operators>& op_stack)
	{
		auto val=val_stack.begin();
		auto op=op_stack.begin();
		tree_op_node* root=new tree_op_node;
		if(op!=op_stack.end())
			root->set_data(*op);
		root->set_left_branch(new tree_val_node(root,nullptr,nullptr,*val));
		++val;
		++op;
		while(val!=val_stack.end()) {
			if(op==op_stack.end()) {
				for(tree_node_base* current=root; true; current=current->get_right_branch()) {
					if(current->get_right_branch()==nullptr) {
						current->set_right_branch(new tree_val_node(current,nullptr,nullptr,*val));
						break;
					}
				}
				++val;
				continue;
			}
			if(get_operator_level(*op)<get_operator_level(root->get_data())) {
				tree_op_node* new_root=new tree_op_node(nullptr,root,nullptr,*op);
				root->set_right_branch(new tree_val_node(root,nullptr,nullptr,*val));
				root->set_root(new_root);
				root=new_root;
				++val;
				++op;
				continue;
			}
			if(get_operator_level(*op)==get_operator_level(root->get_data())) {
				tree_op_node* node=new tree_op_node(root,nullptr,nullptr,*op);
				node->set_left_branch(new tree_val_node(node,nullptr,nullptr,*val));
				root->set_right_branch(node);
				++val;
				++op;
				continue;
			}
			if(get_operator_level(*op)>get_operator_level(root->get_data())) {
				for(tree_node_base* current=root; true; current=current->get_right_branch()) {
					if(current->get_right_branch()==nullptr) {
						tree_op_node* node=new tree_op_node(current,nullptr,nullptr,*op);
						node->set_left_branch(new tree_val_node(node,nullptr,nullptr,*val));
						current->set_right_branch(node);
						break;
					}
					tree_op_node* ptr=dynamic_cast<tree_op_node*>(current->get_right_branch());
					if(get_operator_level(*op)<=get_operator_level(ptr->get_data())) {
						tree_op_node* node=new tree_op_node(current,nullptr,nullptr,*op);
						node->set_left_branch(current->get_right_branch());
						current->get_right_branch()->set_root(node);
						current->set_right_branch(node);
						break;
					}
				}
				++val;
				++op;
				continue;
			}
		}
		return root;
	}
	number parse_tree(tree_node_base* tree)
	{
		if(tree!=nullptr) {
			if(tree->get_type()==typeid(tree_val_node))
				return dynamic_cast<tree_val_node*>(tree)->get_data();
			else {
				tree_op_node* node=dynamic_cast<tree_op_node*>(tree);
				switch(node->get_data()) {
				case operators::null:
					return parse_tree(node->get_left_branch());
				case operators::add:
					return parse_tree(node->get_left_branch())+parse_tree(node->get_right_branch());
				case operators::cut:
					return parse_tree(node->get_left_branch())-parse_tree(node->get_right_branch());
				case operators::mul:
					return parse_tree(node->get_left_branch())*parse_tree(node->get_right_branch());
				case operators::div:
					return parse_tree(node->get_left_branch())/parse_tree(node->get_right_branch());
				case operators::mod:
					return long(parse_tree(node->get_left_branch()))%long(parse_tree(node->get_right_branch()));
				case operators::pow:
					return std::pow(parse_tree(node->get_left_branch()),parse_tree(node->get_right_branch()));
				case operators::_and_:
					return parse_tree(node->get_left_branch())&&parse_tree(node->get_right_branch());
				case operators::_or_:
					return parse_tree(node->get_left_branch())||parse_tree(node->get_right_branch());
				case operators::abo:
					return parse_tree(node->get_left_branch())>parse_tree(node->get_right_branch());
				case operators::und:
					return parse_tree(node->get_left_branch())<parse_tree(node->get_right_branch());
				case operators::equ:
					return parse_tree(node->get_left_branch())==parse_tree(node->get_right_branch());
				case operators::neq:
					return parse_tree(node->get_left_branch())!=parse_tree(node->get_right_branch());
				case operators::aeq:
					return parse_tree(node->get_left_branch())>=parse_tree(node->get_right_branch());
				case operators::ueq:
					return parse_tree(node->get_left_branch())<=parse_tree(node->get_right_branch());
				}
			}
		}
		return 0;
	}
	void destory_tree(tree_node_base* tree)
	{
		if(tree!=nullptr) {
			destory_tree(tree->get_left_branch());
			destory_tree(tree->get_right_branch());
			delete tree;
		}
	}
	bool is_empty(const string& str)
	{
		for(auto& ch:str) {
			if(!std::isspace(ch))
				return false;
		}
		return true;
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
	number parse_expression(const string&);
	cov::any parse_value(const string&);
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
	cov::any parse_value(const std::string& str)
	{
		if(str.empty()||str=="Null")
			return cov::any();
		if(exsist(str))
			return get_value(str);
		if(str=="true"||str=="True"||str=="TRUE")
			return number(true);
		if(str=="false"||str=="False"||str=="FALSE")
			return number(false);
		if(str.find('{')!=string::npos) {
			array arr;
			auto lpos=str.find('{')+1;
			auto rpos=str.rfind('}');
			std::deque<string> buf;
			split_str(',',str.substr(lpos,rpos-lpos),buf);
			for(auto& it:buf)
				arr.push_back(parse_value(it));
			return arr;
		}
		if(str.find('[')!=string::npos) {
			auto fpos=str.rfind('[')+1;
			auto spos=str.rfind(']');
			return parse_value(str.substr(0,fpos-1)).const_val<array>().at(std::size_t(parse_expression(str.substr(fpos,spos-fpos))));
		}
		if(str.find('\"')!=string::npos&&is_str(str))
			return str.substr(str.find('\"')+1,str.rfind('\"')-str.find('\"')-1);
		return parse_expression(str);
	}
	number parse_expression(const std::string& expr)
	{
		if(is_empty(expr))
			return 0;
		std::deque<number> val_stack;
		std::deque<operators> op_stack;
		// 分解表达式并入栈
		std::string tmp;
		bool is_str=false;
		bool is_signal=true;
		for(std::size_t i=0; i<expr.size();) {
			if(expr[i]=='\"') {
				is_str=is_str?false:true;
				tmp+=expr[i];
				++i;
				continue;
			}
			if(!is_str) {
				if(expr[i]=='(') {
					if(is_signal) {
						if(!tmp.empty())
							op_stack.push_back(parse_operators(tmp));
						tmp.clear();
						is_signal=false;
						int brackets_count=1;
						for(++i; i<expr.size(); ++i) {
							if(expr[i]=='\"')
								is_str=is_str?false:true;
							if(!is_str) {
								if(expr[i]=='(')
									++brackets_count;
								if(expr[i]==')')
									--brackets_count;
								if(brackets_count>0)
									tmp+=expr[i];
								else
									break;
							} else
								tmp+=expr[i];
						}
						++i;
						if(brackets_count!=0)
							Darwin_Error("The lack of corresponding brackets.");
						if(!tmp.empty())
							val_stack.push_back(parse_expression(tmp));
						tmp.clear();
					} else {
						cov::any& val=get_value(tmp);
						tmp.clear();
						int brackets_count=1;
						for(++i; i<expr.size(); ++i) {
							if(expr[i]=='\"')
								is_str=is_str?false:true;
							if(!is_str) {
								if(expr[i]=='(')
									++brackets_count;
								if(expr[i]==')')
									--brackets_count;
								if(brackets_count>0)
									tmp+=expr[i];
								else
									break;
							} else
								tmp+=expr[i];
						}
						++i;
						if(brackets_count!=0)
							Darwin_Error("The lack of corresponding brackets.");
						std::deque<string> buf;
						array args;
						split_str(',',tmp,buf);
						for(auto& it:buf)
							args.push_back(parse_value(it));
						CovSwitch(val.type()) {
							CovCase(typeid(function)) {
								val_stack.push_back(val.const_val<function>().call(args));
							}
							EndCovCase;
							CovCase(typeid(native_interface)) {
								val_stack.push_back(val.const_val<native_interface>().call(args));
							}
							EndCovCase;
						}
						EndCovSwitch;
						tmp.clear();
					}
					continue;
				}
				if(std::isspace(expr[i])) {
					++i;
					continue;
				}
				if(is_logic_signal(expr[i])) {
					if(!is_signal) {
						if(!tmp.empty()) {
							try {
								val_stack.push_back(std::stold(tmp));
							} catch(...) {
								val_stack.push_back(parse_value(tmp).const_val<number>());
							}
						}
						tmp.clear();
						is_signal=true;
					}
				} else {
					if(is_signal) {
						if(!tmp.empty())
							op_stack.push_back(parse_operators(tmp));
						tmp.clear();
						is_signal=false;
					}
				}
			}
			tmp+=expr[i];
			++i;
		}
		if(!tmp.empty()) {
			try {
				val_stack.push_back(std::stold(tmp));
			} catch(...) {
				val_stack.push_back(parse_value(tmp).const_val<number>());
			}
		}
		// 解栈建树
		tree_node_base* tree=generate_tree(val_stack,op_stack);
		number result=parse_tree(tree);
		destory_tree(tree);
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
			storage.add_var(name,parse_value(raw_str.substr(raw_str.find('=')+1)));
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
				pos=parse_expression(expr);
				if(pos>=arr->val<array>().size())
					arr->val<array>(true).resize(pos+1);
				arr=&arr->val<array>(true).at(pos);
				if((spos=name.find(']',spos+1))==string::npos)
					Darwin_Error("The lack of corresponding brackets.");
				expr=name.substr(fpos+1,spos-fpos-1);
			}
			pos=parse_expression(expr);
			if(pos>=arr->val<array>().size())
				arr->val<array>(true).resize(pos+1);
			arr->val<array>(true).at(parse_expression(expr)).assign(parse_value(raw_str.substr(raw_str.find('=')+1)));
		} else {
			get_value(name).assign(parse_value(raw_str.substr(raw_str.find('=')+1)),false);
		}
	}
	void parse_return(const string& str)
	{
		if(current_func.empty())
			Darwin_Error("Return in non-function area.");
		current_func.front()->retval=new number(parse_expression(str.substr(str.find("Return")+6)));
	}
	void parse_if(const std::deque<string>&);
	void parse_while(const std::deque<string>&);
	void parse_for(const std::deque<string>&);
	void parse_function(const std::deque<string>&);
	void parse(const string& str)
	{
		if(is_empty(str))
			return;
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
		parse_expression(str);
	}
	void parse_if(const std::deque<string>& raw_buf)
	{
		if(raw_buf.empty())
			Darwin_Error("Parse empty code body.");
		storage.add_domain();
		string head=raw_buf.front();
		auto if_pos=head.find("If");
		auto then_pos=head.rfind("Then");
		bool result=parse_expression(head.substr(if_pos+2,then_pos-if_pos-2));
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
		while(bool(parse_expression(condition))) {
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
		storage.add_var(body.at(0),parse_expression(body.at(1)));
		while(storage.get_var(body.at(0)).const_val<number>()<=parse_expression(body.at(2))) {
			for(std::size_t i=1; i<raw_buf.size(); ++i)
				parse(raw_buf.at(i));
			++storage.get_var(body.at(0)).val<number>(true);
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