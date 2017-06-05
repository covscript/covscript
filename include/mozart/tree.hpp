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
* Copyright (C) 2017 Michael Lee(李登淳)
* Email: mikecovlee@163.com
* Github: https://github.com/mikecovlee
*
* Version: 17.2.1
*/
#include "./base.hpp"
#include <utility>
namespace cov {
	template<typename T>
	class tree final {
		struct tree_node final {
			tree_node* root=nullptr;
			tree_node* left=nullptr;
			tree_node* right=nullptr;
			T data;
			tree_node()=default;
			tree_node(const tree_node&)=default;
			tree_node(tree_node&&) noexcept=default;
			tree_node(tree_node* a,tree_node* b,tree_node* c,const T& dat):root(a),left(b),right(c),data(dat) {}
			template<typename...Args_T>tree_node(tree_node* a,tree_node* b,tree_node* c,Args_T&&...args):root(a),left(b),right(c),data(std::forward<Args_T>(args)...) {}
			~tree_node()=default;
		};
		static tree_node* copy(tree_node* raw,tree_node* root=nullptr)
		{
			if(raw==nullptr) return nullptr;
			tree_node* node=new tree_node(root,nullptr,nullptr,raw->data);
			node->left=copy(raw->left,node);
			node->right=copy(raw->right,node);
			return node;
		}
		static void destory(tree_node* raw)
		{
			if(raw!=nullptr) {
				destory(raw->left);
				destory(raw->right);
				delete raw;
			}
		}
		tree_node* mRoot=nullptr;
	public:
		class iterator final {
			friend class tree;
			tree_node* mData=nullptr;
		public:
			iterator()=default;
			iterator(tree_node* ptr):mData(ptr) {}
			iterator(const iterator&)=default;
			iterator(iterator&&) noexcept=default;
			~iterator()=default;
			iterator& operator=(const iterator&)=default;
			iterator& operator=(iterator&&) noexcept=default;
			bool usable() const noexcept
			{
				return this->mData!=nullptr;
			}
			T& data()
			{
				if(!this->usable())
					throw cov::error("E000E");
				return this->mData->data;
			}
			const T& data() const
			{
				if(!this->usable())
					throw cov::error("E000E");
				return this->mData->data;
			}
			iterator root()
			{
				if(!this->usable())
					throw cov::error("E000E");
				return iterator(this->mData->root);
			}
			iterator left()
			{
				if(!this->usable())
					throw cov::error("E000E");
				return iterator(this->mData->left);
			}
			iterator right()
			{
				if(!this->usable())
					throw cov::error("E000E");
				return iterator(this->mData->right);
			}
		};
		class const_iterator final {
			friend class tree;
			const tree_node* mData=nullptr;
		public:
			const_iterator()=default;
			const_iterator(const tree_node* ptr):mData(ptr) {}
			const_iterator(const iterator& it):mData(it.mData) {}
			const_iterator(const const_iterator&)=default;
			const_iterator(const_iterator&&) noexcept=default;
			~const_iterator()=default;
			const_iterator& operator=(const const_iterator&)=default;
			const_iterator& operator=(const_iterator&&) noexcept=default;
			bool usable() const noexcept
			{
				return this->mData!=nullptr;
			}
			const T& data() const
			{
				if(!this->usable())
					throw cov::error("E000E");
				return this->mData->data;
			}
			const_iterator root() const
			{
				if(!this->usable())
					throw cov::error("E000E");
				return const_iterator(this->mData->root);
			}
			const_iterator left() const
			{
				if(!this->usable())
					throw cov::error("E000E");
				return const_iterator(this->mData->left);
			}
			const_iterator right() const
			{
				if(!this->usable())
					throw cov::error("E000E");
				return const_iterator(this->mData->right);
			}
		};
		void swap(tree& t)
		{
			tree_node* ptr=this->mRoot;
			this->mRoot=t.mRoot;
			t.mRoot=ptr;
		}
		void swap(tree&& t) noexcept
		{
			tree_node* ptr=this->mRoot;
			this->mRoot=t.mRoot;
			t.mRoot=ptr;
		}
		tree()=default;
		tree(iterator it):mRoot(copy(it.mData)) {}
		tree(const tree& t):mRoot(copy(t.mRoot)) {}
		tree(tree&& t) noexcept:
			mRoot(nullptr)
		{
			swap(t);
		}
		~tree()
		{
			destory(this->mRoot);
		}
		tree& operator=(const tree& t)
		{
			destory(this->mRoot);
			this->mRoot=copy(t.mRoot);
			return *this;
		}
		tree& operator=(tree&& t) noexcept
		{
			swap(t);
			return *this;
		}
		void assign(const tree& t)
		{
			destory(this->mRoot);
			this->mRoot=copy(t.mRoot);
		}
		bool empty() const noexcept
		{
			return this->mRoot==nullptr;
		}
		void clear()
		{
			destory(this->mRoot);
			this->mRoot=nullptr;
		}
		iterator root()
		{
			return iterator(this->mRoot);
		}
		const_iterator root() const
		{
			return const_iterator(this->mRoot);
		}
		const_iterator croot() const
		{
			return const_iterator(this->mRoot);
		}
		iterator insert_root_left(iterator it,const T& data)
		{
			if(it.mData==mRoot) {
				mRoot=new tree_node(nullptr,mRoot,nullptr,data);
				return iterator(mRoot);
			}
			if(!it.usable())
				throw cov::error("E000E");
			tree_node* node=new tree_node(it.mData->root,it.mData,nullptr,data);
			if(it.mData->root->left==it.mData)
				it.mData->root->left=node;
			else
				it.mData->root->right=node;
			return iterator(node);
		}
		iterator insert_root_right(iterator it,const T& data)
		{
			if(it.mData==mRoot) {
				mRoot=new tree_node(nullptr,nullptr,mRoot,data);
				return iterator(mRoot);
			}
			if(!it.usable())
				throw cov::error("E000E");
			tree_node* node=new tree_node(it.mData->root,nullptr,it.mData,data);
			if(it.mData->root->left==it.mData)
				it.mData->root->left=node;
			else
				it.mData->root->right=node;
			return iterator(node);
		}
		iterator insert_left_left(iterator it,const T& data)
		{
			if(!it.usable())
				throw cov::error("E000E");
			tree_node* node=new tree_node(it.mData,it.mData->left,nullptr,data);
			if(it.mData->left!=nullptr)
				it.mData->left->root=node;
			it.mData->left=node;
			return iterator(node);
		}
		iterator insert_left_right(iterator it,const T& data)
		{
			if(!it.usable())
				throw cov::error("E000E");
			tree_node* node=new tree_node(it.mData,nullptr,it.mData->left,data);
			if(it.mData->left!=nullptr)
				it.mData->left->root=node;
			it.mData->left=node;
			return iterator(node);
		}
		iterator insert_right_left(iterator it,const T& data)
		{
			if(!it.usable())
				throw cov::error("E000E");
			tree_node* node=new tree_node(it.mData,it.mData->right,nullptr,data);
			if(it.mData->right!=nullptr)
				it.mData->right->root=node;
			it.mData->right=node;
			return iterator(node);
		}
		iterator insert_right_right(iterator it,const T& data)
		{
			if(!it.usable())
				throw cov::error("E000E");
			tree_node* node=new tree_node(it.mData,nullptr,it.mData->right,data);
			if(it.mData->right!=nullptr)
				it.mData->right->root=node;
			it.mData->right=node;
			return iterator(node);
		}
		template<typename...Args>iterator emplace_root_left(iterator it,Args&&...args)
		{
			if(it.mData==mRoot) {
				mRoot=new tree_node(nullptr,mRoot,nullptr,std::forward<Args>(args)...);
				return iterator(mRoot);
			}
			if(!it.usable())
				throw cov::error("E000E");
			tree_node* node=new tree_node(it.mData->root,it.mData,nullptr,std::forward<Args>(args)...);
			if(it.mData->root->left==it.mData)
				it.mData->root->left=node;
			else
				it.mData->root->right=node;
			return iterator(node);
		}
		template<typename...Args>iterator emplace_root_right(iterator it,Args&&...args)
		{
			if(it.mData==mRoot) {
				mRoot=new tree_node(nullptr,nullptr,mRoot,std::forward<Args>(args)...);
				return iterator(mRoot);
			}
			if(!it.usable())
				throw cov::error("E000E");
			tree_node* node=new tree_node(it.mData->root,nullptr,it.mData,std::forward<Args>(args)...);
			if(it.mData->root->left==it.mData)
				it.mData->root->left=node;
			else
				it.mData->root->right=node;
			return iterator(node);
		}
		template<typename...Args>iterator emplace_left_left(iterator it,Args&&...args)
		{
			if(!it.usable())
				throw cov::error("E000E");
			tree_node* node=new tree_node(it.mData,it.mData->left,nullptr,std::forward<Args>(args)...);
			if(it.mData->left!=nullptr)
				it.mData->left->root=node;
			it.mData->left=node;
			return iterator(node);
		}
		template<typename...Args>iterator emplace_left_right(iterator it,Args&&...args)
		{
			if(!it.usable())
				throw cov::error("E000E");
			tree_node* node=new tree_node(it.mData,nullptr,it.mData->left,std::forward<Args>(args)...);
			if(it.mData->left!=nullptr)
				it.mData->left->root=node;
			it.mData->left=node;
			return iterator(node);
		}
		template<typename...Args>iterator emplace_right_left(iterator it,Args&&...args)
		{
			if(!it.usable())
				throw cov::error("E000E");
			tree_node* node=new tree_node(it.mData,it.mData->right,nullptr,std::forward<Args>(args)...);
			if(it.mData->right!=nullptr)
				it.mData->right->root=node;
			it.mData->right=node;
			return iterator(node);
		}
		template<typename...Args>iterator emplace_right_right(iterator it,Args&&...args)
		{
			if(!it.usable())
				throw cov::error("E000E");
			tree_node* node=new tree_node(it.mData,nullptr,it.mData->right,std::forward<Args>(args)...);
			if(it.mData->right!=nullptr)
				it.mData->right->root=node;
			it.mData->right=node;
			return iterator(node);
		}
		iterator erase(iterator it)
		{
			if(!it.usable())
				throw cov::error("E000E");
			iterator root(it.mData->root);
			destory(it.mData);
			return root;
		}
		iterator erase_left(iterator it)
		{
			if(!it.usable())
				throw cov::error("E000E");
			destory(it.mData->left);
			it.mData->left=nullptr;
			return it;
		}
		iterator erase_right(iterator it)
		{
			if(!it.usable())
				throw cov::error("E000E");
			destory(it.mData->right);
			it.mData->right=nullptr;
			return it;
		}
	};
}
