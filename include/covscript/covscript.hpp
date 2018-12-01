#pragma once
/*
* Covariant Script Programming Language
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
* http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*
* Copyright (C) 2018 Michael Lee(李登淳)
* Email: mikecovlee@163.com
* Github: https://github.com/mikecovlee
* Website: http://covscript.org
*/
#include <covscript/extension.hpp>
#include <covscript/cni.hpp>

namespace cs {
	std::string get_sdk_path();

	std::string process_path(const std::string &);

	std::string get_import_path();

	array parse_cmd_args(int, const char *[]);

	context_t create_context(const array &);

	context_t create_subcontext(const context_t &);

	template<typename>class function_invoker;

	template<typename RetT, typename...ArgsT>class function_invoker<RetT(ArgsT...)> {
		var m_func;
	public:
		function_invoker()=delete;
		function_invoker(const function_invoker&)=default;
		function_invoker(const context_t context, const std::string& expr)
		{
			cov::tree<token_base*> tree;
			std::deque<char> buff;
			for(auto& ch:expr)
				buff.push_back(ch);
			context->compiler->build_expr(buff, tree);
			m_func=context->instance->parse_expr(tree.root());
		}
		template<typename...ElementT>
		RetT operator()(ElementT&&...args) const
		{
			return cs_impl::convert_helper<RetT>::get_val(invoke(m_func, cs_impl::type_convertor<ElementT, ArgsT>::convert(std::forward<ElementT>(args))...));
		}
	};

	template<typename...ArgsT>class function_invoker<void(ArgsT...)> {
		var m_func;
	public:
		function_invoker()=delete;
		function_invoker(const function_invoker&)=default;
		function_invoker& operator=(const function_invoker&)=default;
		function_invoker(const context_t context, const std::string& expr)
		{
			cov::tree<token_base*> tree;
			std::deque<char> buff;
			for(auto& ch:expr)
				buff.push_back(ch);
			context->compiler->build_expr(buff, tree);
			m_func=context->instance->parse_expr(tree.root());
		}
		template<typename...ElementT>
		void operator()(ElementT&&...args) const
		{
			invoke(m_func, cs_impl::type_convertor<ElementT, ArgsT>::convert(std::forward<ElementT>(args))...);
		}
	};
}