#pragma once
/*
* Covariant Script Parser
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
#include "./lexer.hpp"

namespace cs {
	class token_expr final:public token_base {
		cov::tree<token_base*> mTree;
	public:
		token_expr()=delete;
		token_expr(const cov::tree<token_base*>& tree):mTree(tree) {}
		virtual token_types get_type() const noexcept override
		{
			return token_types::expr;
		}
		cov::tree<token_base*>& get_tree() noexcept
		{
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
		std::deque<cov::tree<token_base*>>& get_arglist() noexcept
		{
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
		std::deque<cov::tree<token_base*>>& get_array() noexcept
		{
			return this->mTreeList;
		}
	};
	mapping<signal_types,int> signal_level_map = {
		{signal_types::add_,10},{signal_types::sub_,10},{signal_types::mul_,11},{signal_types::div_,11},{signal_types::mod_,12},{signal_types::pow_,12},{signal_types::dot_,17},{signal_types::arrow_,17},
		{signal_types::und_,9},{signal_types::abo_,9},{signal_types::asi_,1},{signal_types::pair_,3},{signal_types::equ_,9},{signal_types::ueq_,9},{signal_types::aeq_,9},{signal_types::neq_,9},{signal_types::lambda_,2},{signal_types::vardef_,20},
		{signal_types::and_,7},{signal_types::or_,7},{signal_types::not_,8},{signal_types::inc_,13},{signal_types::dec_,13},{signal_types::fcall_,14},{signal_types::emb_,14},{signal_types::access_,15},{signal_types::typeid_,16},{signal_types::new_,16},{signal_types::gcnew_,16}
	};
	int get_signal_level(token_base* ptr)
	{
		if(ptr==nullptr)
			throw syntax_error("Get the level of null token.");
		if(ptr->get_type()!=token_types::signal)
			throw syntax_error("Get the level of non-signal token.");
		return signal_level_map.match(static_cast<token_signal*>(ptr)->get_signal());
	}
	constexpr signal_types signal_left_associative[] = {
		signal_types::und_,signal_types::abo_,signal_types::asi_,signal_types::equ_,signal_types::ueq_,signal_types::aeq_,signal_types::neq_,signal_types::and_,signal_types::or_
	};
	bool is_left_associative(token_base* ptr)
	{
		if(ptr==nullptr)
			throw syntax_error("Get the level of null token.");
		if(ptr->get_type()!=token_types::signal)
			throw syntax_error("Get the level of non-signal token.");
		signal_types s=static_cast<token_signal*>(ptr)->get_signal();
		for(auto& t:signal_left_associative)
			if(t==s)
				return true;
		return false;
	}

	void kill_brackets(std::deque<token_base*>&);
	void gen_tree(cov::tree<token_base*>&,std::deque<token_base*>&);
	void kill_expr(std::deque<token_base*>&);
	void optimize_expression(cov::tree<token_base*>&);

	void kill_brackets(std::deque<token_base*>& tokens)
	{
		std::deque<token_base*> oldt;
		std::swap(tokens,oldt);
		tokens.clear();
		bool expected_fcall=false;
		bool expected_lambda=false;
		for(auto& ptr:oldt) {
			switch(ptr->get_type()) {
			default:
				break;
			case token_types::action:
				expected_fcall=false;
				switch(static_cast<token_action*>(ptr)->get_action()) {
				default:
					break;
				case action_types::var_:
					tokens.push_back(ptr);
					tokens.push_back(new token_signal(signal_types::vardef_));
					continue;
				case action_types::struct_:
					tokens.push_back(ptr);
					tokens.push_back(new token_signal(signal_types::vardef_));
					continue;
				case action_types::function_:
					tokens.push_back(ptr);
					tokens.push_back(new token_signal(signal_types::vardef_));
					continue;
				case action_types::namespace_:
					tokens.push_back(ptr);
					tokens.push_back(new token_signal(signal_types::vardef_));
					continue;
				case action_types::catch_:
					tokens.push_back(ptr);
					tokens.push_back(new token_signal(signal_types::vardef_));
					continue;
				}
				break;
			case token_types::id:
				expected_fcall=true;
				break;
			case token_types::value:
				expected_fcall=false;
				break;
			case token_types::sblist: {
				for(auto& list:static_cast<token_sblist*>(ptr)->get_list())
					kill_brackets(list);
				if(expected_fcall) {
					std::deque<cov::tree<token_base*>> tlist;
					for(auto& list:static_cast<token_sblist*>(ptr)->get_list()) {
						cov::tree<token_base*> tree;
						gen_tree(tree,list);
						tlist.push_back(tree);
					}
					if(!expected_lambda)
						tokens.push_back(new token_signal(signal_types::fcall_));
					tokens.push_back(new token_arglist(tlist));
					continue;
				}
				else {
					expected_fcall=true;
					break;
				}
			}
			case token_types::mblist: {
				token_mblist* mbl=static_cast<token_mblist*>(ptr);
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
				for(auto& list:static_cast<token_lblist*>(ptr)->get_list())
					kill_brackets(list);
				std::deque<cov::tree<token_base*>> tlist;
				for(auto& list:static_cast<token_lblist*>(ptr)->get_list()) {
					cov::tree<token_base*> tree;
					gen_tree(tree,list);
					tlist.push_back(tree);
				}
				tokens.push_back(new token_array(tlist));
				expected_fcall=false;
				continue;
			}
			case token_types::signal: {
				switch(static_cast<token_signal*>(ptr)->get_signal()) {
				default:
					break;
				case signal_types::arrow_:
					if(expected_lambda) {
						tokens.push_back(new token_signal(signal_types::lambda_));
						expected_lambda=false;
						continue;
					}
					else
						break;
				case signal_types::esb_:
					if(expected_fcall) {
						if(!expected_lambda)
							tokens.push_back(new token_signal(signal_types::fcall_));
						tokens.push_back(new token_arglist());
					}
					else
						throw syntax_error("Do not allow standalone empty small parentheses.");
					expected_fcall=false;
					continue;
				case signal_types::emb_:
					expected_fcall=true;
					expected_lambda=true;
					tokens.push_back(ptr);
					continue;
				case signal_types::elb_:
					tokens.push_back(new token_array());
					expected_fcall=false;
					continue;
				}
				expected_lambda=false;
				expected_fcall=false;
				break;
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
			}
			else {
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
				token_sblist* sbl=static_cast<token_sblist*>(obj);
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
				token_sblist* sbl=static_cast<token_sblist*>(obj);
				if(sbl->get_list().size()!=1)
					throw syntax_error("There are no more elements in small bracket.");
				cov::tree<token_base*> t;
				gen_tree(t,sbl->get_list().front());
				obj=new token_expr(t);
			}
			tree.emplace_root_left(tree.root(),obj);
		}
		else {
			std::deque<token_base*> signals,objects;
			split_token(raw,signals,objects);
			build_tree(tree,signals,objects);
		}
		optimize_expression(tree);
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
			}
			else
				expr.push_back(ptr);
		}
	}
	void translate_into_statements(std::deque<token_base*>& tokens,std::deque<statement_base*>& statements);
	enum class statement_types {
		null,expression_,import_,package_,block_,namespace_,var_,constant_,if_,else_,switch_,case_,default_,while_,until_,loop_,for_,foreach_,break_,continue_,struct_,function_,return_,end_,try_,catch_,throw_
	};
	class statement_base {
		static garbage_collector<statement_base> gc;
	protected:
		std::size_t mLineNum=0;
		std::string mFilePath="<Unknown>";
		std::string mCode="<Unknown>";
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
		statement_base(token_base* ptr):mLineNum(static_cast<token_endline*>(ptr)->get_num()),mFilePath(static_cast<token_endline*>(ptr)->get_file()),mCode(static_cast<token_endline*>(ptr)->get_code()) {}
		virtual ~statement_base()=default;
		virtual statement_types get_type() const noexcept=0;
		virtual std::size_t get_line_num() const noexcept final
		{
			return this->mLineNum;
		}
		virtual const std::string& get_file_path() const noexcept final
		{
			return this->mFilePath;
		}
		virtual const std::string& get_code() const noexcept final
		{
			return this->mCode;
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
	class statement_var final:public statement_base {
		define_var_profile mDvp;
	public:
		statement_var()=delete;
		statement_var(const define_var_profile& dvp,token_base* ptr):statement_base(ptr),mDvp(dvp) {}
		virtual statement_types get_type() const noexcept override
		{
			return statement_types::var_;
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
	class statement_namespace final:public statement_base {
		std::string mName;
		std::deque<statement_base*> mBlock;
	public:
		statement_namespace()=delete;
		statement_namespace(token_base* tbp,const std::deque<statement_base*>& block,token_base* ptr):statement_base(ptr),mName(static_cast<token_id*>(tbp)->get_id()),mBlock(block) {}
		virtual statement_types get_type() const noexcept override
		{
			return statement_types::namespace_;
		}
		virtual void run() override;
	};
	class statement_if final:public statement_base {
		cov::tree<token_base*> mTree;
		std::deque<statement_base*> mBlock;
	public:
		statement_if()=delete;
		statement_if(const cov::tree<token_base*>& tree,const std::deque<statement_base*>& block,token_base* ptr):statement_base(ptr),mTree(tree),mBlock(block) {}
		virtual statement_types get_type() const noexcept override
		{
			return statement_types::if_;
		}
		virtual void run() override;
	};
	class statement_ifelse final:public statement_base {
		cov::tree<token_base*> mTree;
		std::deque<statement_base*> mBlock;
		std::deque<statement_base*> mElseBlock;
	public:
		statement_ifelse()=delete;
		statement_ifelse(const cov::tree<token_base*>& tree,const std::deque<statement_base*>& btrue,const std::deque<statement_base*>& bfalse,token_base* ptr):statement_base(ptr),mTree(tree),mBlock(btrue),mElseBlock(bfalse) {}
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
		virtual void run() override
		{
			throw syntax_error("Do not allowed standalone else statement.");
		}
	};
	class statement_switch final:public statement_base {
		cov::tree<token_base*> mTree;
		statement_block* mDefault=nullptr;
		std::unordered_map<var,statement_block*> mCases;
	public:
		statement_switch()=delete;
		statement_switch(const cov::tree<token_base*>& tree,const std::unordered_map<var,statement_block*>& cases,statement_block* dptr,token_base* ptr):statement_base(ptr),mTree(tree),mCases(cases),mDefault(dptr) {}
		virtual statement_types get_type() const noexcept override
		{
			return statement_types::switch_;
		}
		virtual void run() override;
	};
	class statement_case final:public statement_base {
		var mTag;
		statement_block* mBlock;
	public:
		statement_case()=delete;
		statement_case(const var& tag,const std::deque<statement_base*>& b,token_base* ptr):statement_base(ptr),mTag(copy(tag)),mBlock(new statement_block(b,ptr)) {}
		virtual statement_types get_type() const noexcept override
		{
			return statement_types::case_;
		}
		virtual void run() override
		{
			throw syntax_error("Do not allowed standalone case statement.");
		}
		const var& get_tag() const
		{
			return this->mTag;
		}
		statement_block* get_block() const
		{
			return this->mBlock;
		}
	};
	class statement_default final:public statement_base {
		statement_block* mBlock;
	public:
		statement_default()=delete;
		statement_default(const std::deque<statement_base*>& b,token_base* ptr):statement_base(ptr),mBlock(new statement_block(b,ptr)) {}
		virtual statement_types get_type() const noexcept override
		{
			return statement_types::default_;
		}
		virtual void run() override
		{
			throw syntax_error("Do not allowed standalone default statement.");
		}
		statement_block* get_block() const
		{
			return this->mBlock;
		}
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
	class statement_until final:public statement_base {
		token_expr* mExpr=nullptr;
	public:
		statement_until()=delete;
		statement_until(token_expr* expr,token_base* ptr):statement_base(ptr),mExpr(expr) {}
		virtual statement_types get_type() const noexcept override
		{
			return statement_types::until_;
		}
		token_expr* get_expr() const
		{
			return mExpr;
		}
		virtual void run() override
		{
			throw syntax_error("Do not allowed standalone until statement.");
		}
	};
	class statement_loop final:public statement_base {
		token_expr* mExpr=nullptr;
		std::deque<statement_base*> mBlock;
	public:
		statement_loop()=delete;
		statement_loop(token_expr* expr,const std::deque<statement_base*>& b,token_base* ptr):statement_base(ptr),mExpr(expr),mBlock(b) {}
		virtual statement_types get_type() const noexcept override
		{
			return statement_types::loop_;
		}
		virtual void run() override;
	};
	class statement_for final:public statement_base {
		define_var_profile mDvp;
		cov::tree<token_base*> mEnd;
		cov::tree<token_base*> mStep;
		std::deque<statement_base*> mBlock;
	public:
		statement_for()=delete;
		statement_for(cov::tree<token_base*>& tree0,const cov::tree<token_base*>& tree1,const cov::tree<token_base*>& tree2,const std::deque<statement_base*>& b,token_base* ptr):statement_base(ptr),mEnd(tree1),mStep(tree2),mBlock(b)
		{
			parse_define_var(tree0,mDvp);
		}
		virtual statement_types get_type() const noexcept override
		{
			return statement_types::for_;
		}
		virtual void run() override;
	};
	class statement_foreach final:public statement_base {
		std::string mIt;
		cov::tree<token_base*> mObj;
		std::deque<statement_base*> mBlock;
	public:
		statement_foreach()=delete;
		statement_foreach(const std::string& it,const cov::tree<token_base*>& tree1,const std::deque<statement_base*>& b,token_base* ptr):statement_base(ptr),mIt(it),mObj(tree1),mBlock(b) {}
		virtual statement_types get_type() const noexcept override
		{
			return statement_types::foreach_;
		}
		virtual void run() override;
	};
	class statement_struct final:public statement_base {
		std::string mName;
		struct_builder mBuilder;
	public:
		statement_struct()=delete;
		statement_struct(const std::string& name,const std::deque<statement_base*>& method,token_base* ptr):statement_base(ptr),mName(name),mBuilder(name,method) {}
		virtual statement_types get_type() const noexcept override
		{
			return statement_types::struct_;
		}
		virtual void run() override;
	};
	class statement_function final:public statement_base {
		std::string mName;
		function mFunc;
		bool mIsMemFn=false;
	public:
		statement_function()=delete;
		statement_function(const std::string& name,const std::deque<std::string>& args,const std::deque<statement_base*>& body,token_base* ptr):statement_base(ptr),mName(name),mFunc(args,body) {}
		virtual statement_types get_type() const noexcept override
		{
			return statement_types::function_;
		}
		void set_mem_fn()
		{
			mFunc.add_this();
			mIsMemFn=true;
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
		virtual void run() override
		{
			throw syntax_error("Do not allowed standalone end statement.");
		}
	};
	class statement_try final:public statement_base {
		std::string mName;
		std::deque<statement_base*> mTryBody;
		std::deque<statement_base*> mCatchBody;
	public:
		statement_try()=delete;
		statement_try(const std::string& name,const std::deque<statement_base*>& tbody,const std::deque<statement_base*>& cbody,token_base* ptr):statement_base(ptr),mName(name),mTryBody(tbody),mCatchBody(cbody) {}
		virtual statement_types get_type() const noexcept override
		{
			return statement_types::try_;
		}
		virtual void run() override;
	};
	class statement_catch final:public statement_base {
		std::string mName;
	public:
		statement_catch()=delete;
		statement_catch(const std::string& name,token_base* ptr):statement_base(ptr),mName(name) {}
		virtual statement_types get_type() const noexcept override
		{
			return statement_types::catch_;
		}
		const std::string& get_name() const
		{
			return this->mName;
		}
		virtual void run() override
		{
			throw syntax_error("Do not allowed standalone catch statement.");
		}
	};
	class statement_throw final:public statement_base {
		cov::tree<token_base*> mTree;
	public:
		statement_throw()=delete;
		statement_throw(const cov::tree<token_base*>& tree,token_base* ptr):statement_base(ptr),mTree(tree) {}
		virtual statement_types get_type() const noexcept override
		{
			return statement_types::throw_;
		}
		virtual void run() override;
	};
	/*
	* Grammar Types
	* Null: Do not use!
	* Single: Single Line Statement.
	* Block: Statement with code block.
	* Jit Command: Statement whitch execute in compile time.
	*/
	enum class grammar_types {
		null,single,block,jit_command
	};
	struct method_type final {
		using function_type=std::function<statement_base*(const std::deque<std::deque<token_base*>>&)>;
		grammar_types type=grammar_types::null;
		statement_types statement_type=statement_types::null;
		function_type init;
		function_type function;
		method_type(statement_types s,grammar_types g,const function_type& f):statement_type(s),type(g),init([](const std::deque<std::deque<token_base*>>&)->statement_base*
		{
			return nullptr;
		}),function(f) {}
		method_type(statement_types s,grammar_types g,const function_type& i,const function_type& f):statement_type(s),type(g),init(i),function(f) {}
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
				return static_cast<const token_action*>(a)->get_action()==static_cast<const token_action*>(b)->get_action();
			}
			else
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
