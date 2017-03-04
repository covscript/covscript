#include "./mozart/tree.hpp"
#include "./lexer.hpp"
namespace cov_basic{
	void split_token(std::deque<token_base*>& raw,std::deque<token_base*>& signals,std::deque<token_base*>& objects)
	{
		bool request_signal=false;
		for(auto& ptr:raw)
		{
			if(ptr->get_type()==token_types::action)
				throw std::logic_error("Wrong format of expression.");
			if(ptr->get_type()==token_types::signal)
			{
				if(!request_signal)
					objects.push_back(nullptr);
				signals.push_back(ptr);
				request_signal=false;
			}else{
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
		tree.clear();
		tree.emplace_root_left(signals.front());
		tree.emplace_left_left(tree.root(),objects.front());
		for(std::size_t i=1;i<signals.size();++i)
		{
			for(iterator it=tree.root();it.usable();it=it.right())
			{
				if(!it.right().usable())
				{
					tree.emplace_right_right(it,objects.at(i));
					break;
				}
			}
			for(iterator it=tree.root();it.usable();it=it.right())
			{
				if(get_signal_level(it.data())==get_signal_level(signals.at(i))
				{
					tree.emplace_root_left(it,signals.at(i));
					break;
				}
				if(get_signal_level(it.data())<get_signal_level(signals.at(i))
				{
					tree.emplace_right_left(it,signals.at(i));
					break;
				}
			}
		}
	}
}