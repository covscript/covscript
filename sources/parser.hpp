#pragma once
#include "./lexer.hpp"
#include <fstream>
#include <memory>
#include <utility>
#include <list>
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
		{signal_types::add_,10},{signal_types::sub_,10},{signal_types::mul_,11},{signal_types::div_,11},{signal_types::mod_,12},{signal_types::pow_,12},{signal_types::dot_,16},
		{signal_types::und_,9},{signal_types::abo_,9},{signal_types::asi_,-2},{signal_types::equ_,9},{signal_types::ueq_,9},{signal_types::aeq_,9},{signal_types::neq_,9},
		{signal_types::and_,7},{signal_types::or_,7},{signal_types::not_,8},{signal_types::inc_,13},{signal_types::dec_,13},{signal_types::fcall_,14},{signal_types::access_,15}
	};
	int get_signal_level(token_base* ptr)
	{
		if(ptr==nullptr)
			throw syntax_error("Get the level of null token.");
		if(ptr->get_type()!=token_types::signal)
			throw syntax_error("Get the level of non-signal token.");
		return signal_level_map.match(dynamic_cast<token_signal*>(ptr)->get_signal());
	}
	signal_types signal_left_associative[] = {
		signal_types::und_,signal_types::abo_,signal_types::asi_,signal_types::equ_,signal_types::ueq_,signal_types::aeq_,signal_types::neq_,signal_types::and_,signal_types::or_
	};
	bool is_left_associative(token_base* ptr)
	{
		if(ptr==nullptr)
			throw syntax_error("Get the level of null token.");
		if(ptr->get_type()!=token_types::signal)
			throw syntax_error("Get the level of non-signal token.");
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
					throw syntax_error("Internal Error(Nullptr Access).");
				if(mbl->get_list().size()!=1)
					throw syntax_error("There are no more elements in middle bracket.");
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
					throw syntax_error("Do not allow empty parentheses.");
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
				throw syntax_error("Wrong format of expression.");
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
			throw syntax_error("Symbols do not match the object.");
		for(auto& obj:objects) {
			if(obj!=nullptr&&obj->get_type()==token_types::sblist) {
				token_sblist* sbl=dynamic_cast<token_sblist*>(obj);
				if(sbl->get_list().size()!=1)
					throw syntax_error("There are no more elements in small bracket.");
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
					tree.emplace_root_left(it,signals.at(i));
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
			token_base* obj=raw.front();
			if(obj!=nullptr&&obj->get_type()==token_types::sblist) {
				token_sblist* sbl=dynamic_cast<token_sblist*>(obj);
				if(sbl->get_list().size()!=1)
					throw syntax_error("There are no more elements in small bracket.");
				cov::tree<token_base*> t;
				gen_tree(t,sbl->get_list().front());
				obj=new token_expr(t);
			}
			tree.emplace_root_left(tree.root(),obj);
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
			if(ptr->get_type()==token_types::action||ptr->get_type()==token_types::endline) {
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
	void translate_into_statements(std::deque<token_base*>& tokens,std::deque<statement_base*>& statements);
	enum class statement_types {
	    expression_,import_,block_,define_,if_,else_,while_,for_,break_,continue_,struct_,function_,return_,end_
	};
	class statement_base {
		static garbage_collector<statement_base> gc;
		std::size_t mLineNum=0;
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
		statement_base()=default;
		statement_base(const statement_base&)=default;
		statement_base(token_base* ptr):mLineNum(dynamic_cast<token_endline*>(ptr)->get_num()) {}
		virtual ~statement_base()=default;
		virtual statement_types get_type() const noexcept=0;
		virtual std::size_t get_line_num() const noexcept final
		{
			return this->mLineNum;
		}
		virtual void run()=0;
	};
	garbage_collector<statement_base> statement_base::gc;
	class statement_expression final:public statement_base {
		cov::tree<token_base*> mTree;
	public:
		statement_expression()=delete;
		statement_expression(const cov::tree<token_base*>& tree,token_base* ptr):statement_base(ptr),mTree(tree) {}
		virtual statement_types get_type() const noexcept override
		{
			return statement_types::expression_;
		}
		virtual void run() override;
	};
	class statement_import final:public statement_base {
		std::deque<statement_base*> mBlock;
	public:
		statement_import()=delete;
		statement_import(const std::string& path,token_base* ptr):statement_base(ptr)
		{
			std::deque<char> buff;
			std::deque<token_base*> tokens;
			std::ifstream in(path);
			std::string line;
			std::size_t line_num=0;
			while(std::getline(in,line)) {
				++line_num;
				if(line.empty())
					continue;
				if(line[0]=='#')
					continue;
				for(auto& c:line)
					buff.push_back(c);
				try {
					translate_into_tokens(buff,tokens);
				} catch(const syntax_error& se) {
					throw syntax_error(line_num,se.what());
				}
				tokens.push_back(new token_endline(line_num));
				buff.clear();
			}
			translate_into_statements(tokens,mBlock);
		}
		virtual statement_types get_type() const noexcept override
		{
			return statement_types::import_;
		}
		virtual void run() override;
	};
	class statement_define final:public statement_base {
		cov::tree<token_base*> mTree;
		token_id* mType=nullptr;
	public:
		statement_define()=delete;
		statement_define(const cov::tree<token_base*>& tree,token_base* ptr):statement_base(ptr),mTree(tree) {}
		statement_define(const cov::tree<token_base*>& tree,token_base* typ,token_base* ptr):statement_base(ptr),mTree(tree),mType(dynamic_cast<token_id*>(typ)) {}
		virtual statement_types get_type() const noexcept override
		{
			return statement_types::define_;
		}
		virtual void run() override;
	};
	class statement_break final:public statement_base {
	public:
		statement_break()=default;
		statement_break(token_base* ptr):statement_base(ptr) {}
		virtual statement_types get_type() const noexcept override
		{
			return statement_types::break_;
		}
		virtual void run() override;
	};
	class statement_continue final:public statement_base {
	public:
		statement_continue()=default;
		statement_continue(token_base* ptr):statement_base(ptr) {}
		virtual statement_types get_type() const noexcept override
		{
			return statement_types::continue_;
		}
		virtual void run() override;
	};
	class statement_block final:public statement_base {
		std::deque<statement_base*> mBlock;
	public:
		statement_block()=delete;
		statement_block(const std::deque<statement_base*>& block,token_base* ptr):statement_base(ptr),mBlock(block) {}
		virtual statement_types get_type() const noexcept override
		{
			return statement_types::block_;
		}
		virtual void run() override;
	};
	class statement_if final:public statement_base {
		cov::tree<token_base*> mTree;
		std::deque<statement_base*>* mBlock=nullptr;
		std::deque<statement_base*>* mElseBlock=nullptr;
	public:
		statement_if()=delete;
		statement_if(const cov::tree<token_base*>& tree,std::deque<statement_base*>* btrue,std::deque<statement_base*>* bfalse,token_base* ptr):statement_base(ptr),mTree(tree),mBlock(btrue),mElseBlock(bfalse) {}
		virtual ~statement_if()
		{
			delete mBlock;
			delete mElseBlock;
		}
		virtual statement_types get_type() const noexcept override
		{
			return statement_types::if_;
		}
		virtual void run() override;
	};
	class statement_else final:public statement_base {
	public:
		statement_else()=default;
		virtual statement_types get_type() const noexcept override
		{
			return statement_types::else_;
		}
		virtual void run() override {}
	};
	class statement_while final:public statement_base {
		cov::tree<token_base*> mTree;
		std::deque<statement_base*> mBlock;
	public:
		statement_while()=delete;
		statement_while(const cov::tree<token_base*>& tree,const std::deque<statement_base*>& b,token_base* ptr):statement_base(ptr),mTree(tree),mBlock(b) {}
		virtual statement_types get_type() const noexcept override
		{
			return statement_types::while_;
		}
		virtual void run() override;
	};
	class statement_struct final:public statement_base {
		std::string mName;
		struct_builder mBuilder;
	public:
		statement_struct()=delete;
		statement_struct(const std::string& name,const std::deque<statement_base*>& method,token_base* ptr):statement_base(ptr),mName(name),mBuilder(method) {}
		virtual statement_types get_type() const noexcept override
		{
			return statement_types::struct_;
		}
		virtual void run() override;
	};
	class statement_function final:public statement_base {
		std::string mName;
		function mFunc;
	public:
		statement_function()=delete;
		statement_function(const std::string& name,const std::deque<std::string>& args,const std::deque<statement_base*>& body,token_base* ptr):statement_base(ptr),mName(name),mFunc(args,body) {}
		virtual statement_types get_type() const noexcept override
		{
			return statement_types::function_;
		}
		virtual void run() override;
	};
	class statement_return final:public statement_base {
		cov::tree<token_base*> mTree;
	public:
		statement_return()=delete;
		statement_return(const cov::tree<token_base*>& tree,token_base* ptr):statement_base(ptr),mTree(tree) {}
		virtual statement_types get_type() const noexcept override
		{
			return statement_types::return_;
		}
		virtual void run() override;
	};
	class statement_end final:public statement_base {
	public:
		statement_end()=default;
		virtual statement_types get_type() const noexcept override
		{
			return statement_types::end_;
		}
		virtual void run() override {}
	};
	enum class grammar_type {
	    null,single,block
	};
	struct method_type final {
		using function_type=std::function<statement_base*(const std::deque<std::deque<token_base*>>&)>;
		grammar_type type=grammar_type::null;
		function_type function;
	};
	class translator_type final {
	public:
		using data_type=std::pair<std::deque<token_base*>,method_type>;
		static bool compare(const token_base* a,const token_base* b)
		{
			if(a==nullptr)
				return b==nullptr;
			if(b==nullptr)
				return a==nullptr;
			if(a->get_type()!=b->get_type())
				return false;
			if(a->get_type()==token_types::action) {
				return dynamic_cast<const token_action*>(a)->get_action()==dynamic_cast<const token_action*>(b)->get_action();
			} else
				return true;
		}
	private:
		std::list<std::shared_ptr<data_type>> m_data;
	public:
		translator_type()=default;
		translator_type(const translator_type&)=delete;
		~translator_type()=default;
		void add_method(const std::deque<token_base*>& grammar,const method_type& method)
		{
			m_data.emplace_back(std::make_shared<data_type>(grammar,method));
		}
		method_type& match(const std::deque<token_base*>& raw)
		{
			if(raw.size()<=1)
				throw syntax_error("Grammar error.");
			std::list<std::shared_ptr<data_type>> stack;
			for(auto& it:m_data)
				if(this->compare(it->first.front(),raw.front()))
					stack.push_back(it);
			stack.remove_if([&](const std::shared_ptr<data_type>& dat) {
				return dat->first.size()!=raw.size();
			});
			stack.remove_if([&](const std::shared_ptr<data_type>& dat) {
				for(std::size_t i=1; i<raw.size()-1; ++i) {
					if(!compare(raw.at(i),dat->first.at(i)))
						return true;
				}
				return false;
			});
			if(stack.empty())
				throw syntax_error("Uknow grammar.");
			if(stack.size()>1)
				throw syntax_error("Ambiguous grammar.");
			return stack.front()->second;
		}
	};
	static translator_type translator;
}