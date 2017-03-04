#pragma once
/*
* Covariant Mozart Utility Library: Two branches tree
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
* Copyright (C) 2016 Michael Lee(李登淳)
* Email: China-LDC@outlook.com
* Github: https://github.com/mikecovlee
* Website: http://ldc.atd3.cn
*
* Version: 17.1.1
*/
#include "./base.hpp"
namespace cov
{
	template<typename T>
	class tree final{
		struct tree_node final{
			T data;
			tree_node* root=nullptr;
			tree_node* left=nullptr;
			tree_node* right=nullptr;
		};
		static tree_node* copy(tree_node* raw,tree_node* root=nullptr)
		{
			if(raw==nullptr) return nullptr;
			tree_node* node=new tree_node{raw->data,root,nullptr,nullptr};
			node->left=copy(raw->left,node);
			node->right=copy(raw->right,node);
			return node;
		}
		static void destory(tree_node* raw)
		{
			if(raw!=nullptr)
			{
				destory(raw->left);
				destory(raw->right);
				delete raw;
			}
		}
		tree_node* mRoot=nullptr;
	public:
		class iterator final{
			friend class tree;
			tree_node* mData=nullptr;
		public:
			iterator()=default;
			iterator(tree_node* ptr):mData(ptr){}
			iterator(const iterator&)=default;
			~iterator()=default;
			bool usable() const noexcept{return this->mData!=nullptr;}
			T& data()
			{
				if(!this->usable())
					throw std::logic_error("Use of final tree node.");
				return this->mData->data;
			}
			const T& data() const
			{
				if(!this->usable())
					throw std::logic_error("Use of final tree node.");
				return this->mData->data;
			}
			iterator root()
			{
				if(!this->usable())
					throw std::logic_error("Use of final tree node.");
				return iterator(this->mData->root);
			}
			iterator left()
			{
				if(!this->usable())
					throw std::logic_error("Use of final tree node.");
				return iterator(this->mData->left);
			}
			iterator right()
			{
				if(!this->usable())
					throw std::logic_error("Use of final tree node.");
				return iterator(this->mData->right);
			}
		};
		class const_iterator final{
			friend class tree;
			const tree_node* mData=nullptr;
		public:
			const_iterator()=default;
			const_iterator(const tree_node* ptr):mData(ptr){}
			const_iterator(const iterator& it):mData(it.mData){}
			const_iterator(const const_iterator&)=default;
			~const_iterator()=default;
			bool usable() const noexcept{return this->mData!=nullptr;}
			const T& data() const
			{
				if(!this->usable())
					throw std::logic_error("Use of final tree node.");
				return this->mData->data;
			}
			const_iterator root() const
			{
				if(!this->usable())
					throw std::logic_error("Use of final tree node.");
				return const_iterator(this->mData->root);
			}
			const_iterator left() const
			{
				if(!this->usable())
					throw std::logic_error("Use of final tree node.");
				return const_iterator(this->mData->left);
			}
			const_iterator right() const
			{
				if(!this->usable())
					throw std::logic_error("Use of final tree node.");
				return const_iterator(this->mData->right);
			}
		};
		tree()=default;
		tree(iterator it):mRoot(copy(it.mData)){}
		tree(const tree& t):mRoot(copy(t.mRoot)){}
		~tree(){destory(this->mRoot);}
		bool empty() const noexcept{return this->mRoot==nullptr;}
		void clear(){destory(this->mRoot);this->mRoot=nullptr;}
		iterator root(){return iterator(this->mRoot);}
		const_iterator root() const{return const_iterator(this->mRoot);}
		const_iterator croot() const{return const_iterator(this->mRoot);}
		template<typename...Args>void emplace_root_left(Args&&...args)
		{
			mRoot=new tree_node{T(args...),nullptr,mRoot,nullptr};
		}
		template<typename...Args>void emplace_root_right(Args&&...args)
		{
			mRoot=new tree_node{T(args...),nullptr,nullptr,mRoot};
		}
		template<typename...Args>iterator emplace_root_left(iterator it,Args&&...args)
		{
			if(!it.usable())
				throw std::logic_error("Use of final tree node.");
			if(it.mData==mRoot)
			{
				mRoot=new tree_node{T(args...),nullptr,mRoot,nullptr};
				return iterator(mRoot);
			}
			tree_node* node=new tree_node{T(args...),it.mData->root,it.mData,nullptr};
			if(it.mData->root->left==it.mData)
				it.mData->root->left=node;
			else
				it.mData->root->right=node;
			return iterator(node);
		}
		template<typename...Args>iterator emplace_root_right(iterator it,Args&&...args)
		{
			if(!it.usable())
				throw std::logic_error("Use of final tree node.");
			if(it.mData==mRoot)
			{
				mRoot=new tree_node{T(args...),nullptr,nullptr,mRoot};
				return iterator(mRoot);
			}
			tree_node* node=new tree_node{T(args...),it.mData->root,nullptr,it.mData};
			if(it.mData->root->left==it.mData)
				it.mData->root->left=node;
			else
				it.mData->root->right=node;
			return iterator(node);
		}
		template<typename...Args>iterator emplace_left_left(iterator it,Args&&...args)
		{
			if(!it.usable())
				throw std::logic_error("Use of final tree node.");
			tree_node* node=new tree_node{T(args...),it.mData,it.mData->left,nullptr};
			if(it.mData->left!=nullptr)
				it.mData->left->root=node;
			it.mData->left=node;
			return iterator(node);
		}
		template<typename...Args>iterator emplace_left_right(iterator it,Args&&...args)
		{
			if(!it.usable())
				throw std::logic_error("Use of final tree node.");
			tree_node* node=new tree_node{T(args...),it.mData,nullptr,it.mData->left};
			if(it.mData->left!=nullptr)
				it.mData->left->root=node;
			it.mData->left=node;
			return iterator(node);
		}
		template<typename...Args>iterator emplace_right_left(iterator it,Args&&...args)
		{
			if(!it.usable())
				throw std::logic_error("Use of final tree node.");
			tree_node* node=new tree_node{T(args...),it.mData,it.mData->right,nullptr};
			if(it.mData->right!=nullptr)
				it.mData->right->root=node;
			it.mData->right=node;
			return iterator(node);
		}
		template<typename...Args>iterator emplace_right_right(iterator it,Args&&...args)
		{
			if(!it.usable())
				throw std::logic_error("Use of final tree node.");
			tree_node* node=new tree_node{T(args...),it.mData,nullptr,it.mData->right};
			if(it.mData->right!=nullptr)
				it.mData->right->root=node;
			it.mData->right=node;
			return iterator(node);
		}
		iterator remove(iterator it)
		{
			if(!it.usable())
				throw std::logic_error("Use of final tree node.");
			iterator root(it.mData->root);
			destory(it.mData);
			return root;
		}
		iterator remove_left(iterator it)
		{
			if(!it.usable())
				throw std::logic_error("Use of final tree node.");
			destory(it.mData->left);
			it.mData->left=nullptr;
			return it;
		}
		iterator remove_right(iterator it)
		{
			if(!it.usable())
				throw std::logic_error("Use of final tree node.");
			destory(it.mData->right);
			it.mData->right=nullptr;
			return it;
		}
	};
}