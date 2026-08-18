/*
 * Covariant Script Programming Language
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Copyright (C) 2017-2026 Michael Lee(李登淳)
 *
 * This software is registered with the National Copyright Administration
 * of the People's Republic of China(Registration Number: 2020SR0408026)
 * and is protected by the Copyright Law of the People's Republic of China.
 *
 * Email:   mikecovlee@163.com
 * Github:  https://github.com/mikecovlee
 * Website: http://covscript.org.cn
 */
#include <covscript/impl/codegen.hpp>
#include <covscript/impl/system.hpp>
#include <covscript/covscript.hpp>

#include <filesystem>

#ifdef COVSCRIPT_PLATFORM_WIN32

#include <shlobj.h>

#pragma comment(lib, "shell32.lib")

#else

#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>

#endif

#ifdef _MSC_VER
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0501
#endif
#include <windows.h>
#include <Dbghelp.h>
#pragma comment(lib, "DbgHelp")
namespace cs_impl
{
	std::string cxx_demangle(const char *name)
	{
		char buffer[1024];
		DWORD length = UnDecorateSymbolName(name, buffer, sizeof(buffer), 0);
		if (length > 0)
			return std::string(buffer, length);
		else
			return name;
	}
} // namespace cs_impl
#elif defined __GNUC__

#include <cxxabi.h>

namespace cs_impl
{
	std::string cxx_demangle(const char *name)
	{
		char buffer[1024] = {0};
		size_t size = sizeof(buffer);
		int status;
		char *ret = abi::__cxa_demangle(name, buffer, &size, &status);
		if (ret != nullptr)
			return std::string(ret);
		else
			return name;
	}
} // namespace cs_impl
#endif

namespace cs_impl
{
#ifdef CS_ENABLE_PROFILING
	volatile std::size_t op_perf[40];
#endif
	cs::namespace_t member_visitor_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t except_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t array_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t array_iterator_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t number_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t char_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t math_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t math_const_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t list_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t list_iterator_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t hash_set_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t hash_map_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t pair_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t time_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t future_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t fiber_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t context_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t runtime_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t string_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t iostream_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t seekdir_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t openmode_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t charbuff_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t istream_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t ostream_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t system_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t console_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t file_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t path_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t path_type_ext = cs::make_shared_namespace<cs::name_space>();
	cs::namespace_t path_info_ext = cs::make_shared_namespace<cs::name_space>();
} // namespace cs_impl

namespace cs
{
	void exception::relocate_to_csym(const csym_info &csym)
	{
		if (mLine == 0 || mLine > csym.map.size())
			throw fatal_error("Invalid line number when relocating symbols in cSYM file");
		std::size_t relocated_line = csym.map[mLine - 1];
		if (relocated_line >= csym.codes.size())
			throw fatal_error("Malformed cSYM file: relocated line number is out of range");
		if (relocated_line > 0)
		{
			const std::string &relocated_code = csym.codes[relocated_line - 1];
			mStr = compose_what(csym.file, relocated_line, relocated_code, mWhat);
		}
		else
			mStr = compose_what_internal(csym.file, mWhat);
	}

	void process_context::cleanup_context()
	{
		if (current_process == nullptr)
			return;
		while (!current_process->stack.empty())
			current_process->stack.pop_no_return();
#ifdef CS_DEBUGGER
		while (!current_process->stack_backtrace.empty())
			current_process->stack_backtrace.pop_no_return();
#endif
	}

	bool process_context::on_process_exit_default_handler(void *code)
	{
		cleanup_context();
		// Pools not collected here (live callables reference them); std::exit
		// reclaims everything.
		std::exit(*static_cast<int *>(code));
		return true;
	}

	std::shared_ptr<process_context> process_context::current_owner()
	{
		// Nearest script fiber's process on the chain; null when the owner is the root.
		for (auto &f : fiber_context::current()->stack)
			if (auto p = f->get_process())
				return p;
		return nullptr;
	}

	bool process_context::is_related(const process_context *other) const noexcept
	{
		if (other == nullptr)
			return false;
		for (const process_context *p = this; p != nullptr; p = p->m_parent.get())
			if (p == other)
				return true;
		for (const process_context *p = other; p != nullptr; p = p->m_parent.get())
			if (p == this)
				return true;
		return false;
	}

	std::shared_ptr<process_context> process_context::fork(const std::shared_ptr<process_context> &parent)
	{
		// Share the parent's fiber chain; a null parent means the parent is the root.
		process_context *src = parent ? parent.get() : current_process;
		if (src == nullptr)
			throw fatal_error("cannot fork a fiber without an active process");
		std::shared_ptr<process_context> new_process(
		    std::make_shared<process_context>(src->child_stack_size(), src->fiber_cxt));
		new_process->output_precision = src->output_precision;
		new_process->import_path = src->import_path;
		// Keep the parent (or the fork source without a fiber chain) alive so
		// the forwarders below can reach it and is_related() can trace it back.
		new_process->m_parent = parent ? parent : src->shared_from_this();
		// The parent (or the fork source) receives forwarded exit/sigint events.
		std::shared_ptr<process_context> parent_ref = new_process->m_parent;
		new_process->on_process_exit.add_listener([parent_ref](void *data) -> bool
		{
			if (parent_ref)
				return parent_ref->on_process_exit.touch(data);
			return false;
		});
		new_process->on_process_sigint.add_listener([parent_ref](void *data) -> bool
		{
			if (parent_ref)
				return parent_ref->on_process_sigint.touch(data);
			return false;
		});
		new_process->std_eh_callback = src->std_eh_callback;
		new_process->cs_eh_callback = src->cs_eh_callback;
		return new_process;
	}

	current_process_ref current_process;

	// Host process accessor handed to extension DLLs.
	process_context *current_process_host_accessor(void *)
	{
		return current_process;
	}

	context_type::~context_type()
	{
		// Run finalizers while the runtime is still usable. clear_global()
		// uses safe_rewind() to destroy globals in reverse order, removing
		// each slot's name before its destructor runs, so finalizers see a
		// consistent shrinking table.
		if (process != nullptr)
			process->teardown_ctx = this;
		if (instance != nullptr)
			instance->storage.clear_global();
		if (process != nullptr)
			process->teardown_ctx = nullptr;
		// Drop module namespaces/subcontexts so their arenas release at teardown.
		// Only the compiler-owning context may clear the cache: subcontexts share
		// the parent's compiler, so a failed module import must not wipe modules
		// the parent already imported successfully.
		if (owns_compiler && compiler != nullptr)
		{
			// Clear each module domain so circular cross-refs drop.
			for (auto &kv : compiler->modules)
				kv.second->get_domain().clear();
			compiler->modules.clear();
		}
		subcontexts.clear();
	}

	signal_control global_signals;

	type_node *alloc_type_node(process_context *p)
	{
		// Per-process pool: node addresses stay unique for the process lifetime.
		if (p != nullptr)
		{
			p->type_nodes.emplace_back();
			return &p->type_nodes.back();
		}
		// A bare (process-less) thread's fallback: thread_local so concurrent
		// process-less threads don't race on the pool.
		static thread_local std::deque<type_node> fallback_pool;
		fallback_pool.emplace_back();
		return &fallback_pool.back();
	}

	std::size_t callable::argument_count() const
	{
		if (mFunc.target_type() == typeid(function_ptr))
			return mFunc.target<function_ptr>()->fptr->argument_count();
		else if (mFunc.target_type() == typeid(cs_impl::cni))
			return mFunc.target<cs_impl::cni>()->argument_count();
		else
			throw lang_error("The target value is not a function");
	}

	void copy_no_return(var &val)
	{
		if (!val.is_rvalue())
		{
			// clone() itself rebinds a self-referencing lambda's `self` borrow
			// to the clone's own proxy (see any::clone/rebind).
			val.clone();
			val.detach();
		}
		else
			val.mark_trivial();
	}

	var copy(var val)
	{
		if (!val.is_rvalue())
		{
			val.clone();
			val.detach();
		}
		else
			val.mark_trivial();
		return val;
	}

	var lvalue(const var &val)
	{
		if (val.is_rvalue())
			val.mark_trivial();
		return val;
	}

	var rvalue(const var &val)
	{
		val.try_move();
		return val;
	}

	var try_move(const var &val)
	{
		val.try_move();
		return val;
	}

	var make_namespace(const namespace_t &ns)
	{
		return var::make_protect<namespace_t>(ns);
	}

	numeric parse_number(const std::string &str)
	{
		try
		{
			std::size_t pos = 0;
			if (str.find_first_of(".eE") != std::string::npos)
			{
				numeric_float val = std::stold(str, &pos);
				if (pos != str.size())
					throw lang_error("Invalid numeric literal: cannot parse the given string as a number");
				return val;
			}
			else
			{
				numeric_integer val = std::stoll(str, &pos);
				if (pos != str.size())
					throw lang_error("Invalid numeric literal: cannot parse the given string as a number");
				return val;
			}
		}
		catch (const lang_error &e)
		{
			throw;
		}
		catch (const std::exception &)
		{
			throw lang_error("Invalid numeric literal: cannot parse the given string as a number");
		}
	}

	std::string process_path(const std::string &raw)
	{
		auto pos0 = raw.find('\"');
		auto pos1 = raw.rfind('\"');
		if (pos0 != std::string::npos)
		{
			if (pos0 == pos1)
				throw cs::fatal_error("argument syntax error.");
			else
				return raw.substr(pos0 + 1, pos1 - pos0 - 1);
		}
		else
			return raw;
	}

#ifdef COVSCRIPT_PLATFORM_WIN32

	std::string get_sdk_path()
	{
#ifdef COVSCRIPT_HOME
		return COVSCRIPT_HOME;
#else
		const char *sdk_path = std::getenv("COVSCRIPT_HOME");
		if (sdk_path == nullptr)
		{
			CHAR path[MAX_PATH] = {0};
			if (SHGetFolderPathA(nullptr, CSIDL_PERSONAL, nullptr, SHGFP_TYPE_CURRENT, path) != S_OK)
				throw cs::fatal_error("Cannot locate the CovScript SDK directory: set COVSCRIPT_HOME");
			return process_path(std::string(path) + "\\CovScript");
		}
		else
			return process_path(sdk_path);
#endif
	}

#else

	std::string get_sdk_path()
	{
#ifdef COVSCRIPT_HOME
		return COVSCRIPT_HOME;
#else
		const char *sdk_path = std::getenv("COVSCRIPT_HOME");
		if (sdk_path == nullptr)
		{
			struct passwd *pw = getpwuid(getuid());
			if (pw == nullptr || pw->pw_dir == nullptr)
				throw cs::fatal_error("Cannot locate the CovScript SDK directory: set COVSCRIPT_HOME");
			return process_path(std::string(pw->pw_dir) + "/.covscript");
		}
		else
			return process_path(sdk_path);
#endif
	}

#endif

	std::string get_import_path()
	{
		const char *import_path = std::getenv("CS_IMPORT_PATH");
		std::string base_path = get_sdk_path() + cs::path_separator + "imports";
#ifdef COVSCRIPT_PLATFORM_HOME
		base_path += cs::path_delimiter + std::string(COVSCRIPT_PLATFORM_HOME) + cs::path_separator + "imports";
#endif
		if (import_path != nullptr)
			return process_path(std::string(import_path) + cs::path_delimiter + base_path);
		else
			return process_path(base_path);
	}

	void prepend_import_path(const std::string &script, cs::process_context *context)
	{
		if (script.empty())
		{
			return;
		}
		std::error_code ec;
		std::filesystem::path p = std::filesystem::absolute(script, ec);
		if (ec)
			return;
		std::filesystem::path dir = p.lexically_normal().parent_path();
		if (dir.empty())
			return;
		context->import_path = dir.string() + cs::path_delimiter + context->import_path;
	}

	array parse_cmd_args(int argc, char *argv[])
	{
		cs::array arg;
		for (std::size_t i = 0; i < argc; ++i)
			arg.emplace_back(cs::var::make_constant<cs::string>(argv[i]));
		return std::move(arg);
	}

	// Internal Functions

	var range(vector &args)
	{
		switch (args.size())
		{
			case 1:
				cs_impl::check_args<numeric>(args);
				return var::make_constant<range_type>(0, args[0].const_val<numeric>(), 1);
			case 2:
				cs_impl::check_args<numeric, numeric>(args);
				return var::make_constant<range_type>(args[0].const_val<numeric>(), args[1].const_val<numeric>(), 1);
			case 3:
				cs_impl::check_args<numeric, numeric, numeric>(args);
				if (args[2].const_val<numeric>() == 0)
					throw cs::runtime_error("Range step cannot be zero");
				return var::make_constant<range_type>(args[0].const_val<numeric>(), args[1].const_val<numeric>(),
				                                      args[2].const_val<numeric>());
			default:
				throw cs::runtime_error(
				    "Wrong size of the arguments. Expected 1, 2 or 3, provided " + std::to_string(args.size()));
		}
	}

	numeric to_integer(const var &val)
	{
		return val.to_integer();
	}

	var to_string(const var &val)
	{
		if (val.is_type_of<string>())
			return val;
		else
			return val.to_string().extract();
	}

	string type(const var &a)
	{
		return a.get_type_name();
	}

	var clone(const var &val)
	{
		return copy(val);
	}

	var move(const var &val)
	{
		return rvalue(val);
	}

	void swap(var &a, var &b)
	{
		a.swap(b, true);
	}

	bool is_a(const type_id &a, const type_id &b)
	{
		return a.is_a(b);
	}

	context_t create_context(const array &args, std::size_t stack_size)
	{
		context_t context = std::make_shared<context_type>();
		// Each context owns its own process, sized from the requested size, the
		// active process, or the default.
		std::size_t ss = stack_size ? stack_size
		                            : (current_process ? current_process->stack_size : COVSCRIPT_STACK_PRESERVE);
		context->process = std::make_shared<process_context>(ss, fiber_context::current());
		{
			// Extensions read current_process at load time.
			process_run_scope scope(context);
			cs_impl::init_extensions();
		}
		context->compiler = std::make_shared<compiler_type>(context.get());
		context->owns_compiler = true;
		context->instance = std::make_shared<instance_type>(context.get(), context->process->stack_size);
		context->cmd_args = cs::var::make_constant<cs::array>(args);
		// Default arena for tokens outside an explicit compile unit.
		context->current_unit = std::make_shared<compile_unit>();
		// Init Grammars
		(*context->compiler)
		    // Expression Grammar
		    .add_method({new token_expr(tree_type<token_base *>()), new token_endline(0)}, new method_expression)
		    // Import Grammar
		    .add_method({new token_action(action_types::import_), new token_expr(tree_type<token_base *>()),
		                 new token_endline(0)},
		                new method_import)
		    .add_method({new token_action(action_types::import_), new token_expr(tree_type<token_base *>()),
		                 new token_action(action_types::as_), new token_expr(tree_type<token_base *>()),
		                 new token_endline(0)},
		                new method_import_as)
		    // Package Grammar
		    .add_method({new token_action(action_types::package_), new token_expr(tree_type<token_base *>()),
		                 new token_endline(0)},
		                new method_package)
		    // Involve Grammar
		    .add_method({new token_action(action_types::using_), new token_expr(tree_type<token_base *>()),
		                 new token_endline(0)},
		                new method_involve)
		    // Var Grammar
		    .add_method({new token_action(action_types::var_), new token_expr(tree_type<token_base *>()),
		                 new token_endline(0)},
		                new method_var)
		    .add_method({new token_action(action_types::link_), new token_expr(tree_type<token_base *>()),
		                 new token_endline(0)},
		                new method_link)
		    .add_method({new token_action(action_types::constant_), new token_expr(tree_type<token_base *>()),
		                 new token_endline(0)},
		                new method_constant)
		    // End Grammar
		    .add_method({new token_action(action_types::endblock_), new token_endline(0)}, new method_end)
		    // Block Grammar
		    .add_method({new token_action(action_types::block_), new token_endline(0)}, new method_block)
		    // Namespace Grammar
		    .add_method({new token_action(action_types::namespace_), new token_expr(tree_type<token_base *>()),
		                 new token_endline(0)},
		                new method_namespace)
		    // If Grammar
		    .add_method({new token_action(action_types::if_), new token_expr(tree_type<token_base *>()),
		                 new token_endline(0)},
		                new method_if)
		    // Else Grammar
		    .add_method({new token_action(action_types::else_), new token_endline(0)}, new method_else)
		    // Switch Grammar
		    .add_method({new token_action(action_types::switch_), new token_expr(tree_type<token_base *>()),
		                 new token_endline(0)},
		                new method_switch)
		    // Case Grammar
		    .add_method({new token_action(action_types::case_), new token_expr(tree_type<token_base *>()),
		                 new token_endline(0)},
		                new method_case)
		    // Default Grammar
		    .add_method({new token_action(action_types::default_), new token_endline(0)},
		                new method_default)
		    // While Grammar
		    .add_method({new token_action(action_types::while_), new token_expr(tree_type<token_base *>()),
		                 new token_endline(0)},
		                new method_while)
		    // Until Grammar
		    .add_method({new token_action(action_types::until_), new token_expr(tree_type<token_base *>()),
		                 new token_endline(0)},
		                new method_until)
		    // Loop Grammar
		    .add_method({new token_action(action_types::loop_), new token_endline(0)}, new method_loop)
		    // For Grammar
		    .add_method({new token_action(action_types::for_), new token_expr(tree_type<token_base *>()),
		                 new token_endline(0)},
		                new method_for)
		    .add_method({new token_action(action_types::for_), new token_expr(tree_type<token_base *>()),
		                 new token_action(action_types::do_), new token_expr(tree_type<token_base *>()),
		                 new token_endline(0)},
		                new method_for_do)
		    .add_method({new token_action(action_types::foreach_), new token_expr(tree_type<token_base *>()),
		                 new token_action(action_types::in_), new token_expr(tree_type<token_base *>()),
		                 new token_endline(0)},
		                new method_foreach)
		    .add_method({new token_action(action_types::foreach_), new token_expr(tree_type<token_base *>()),
		                 new token_action(action_types::in_), new token_expr(tree_type<token_base *>()),
		                 new token_action(action_types::do_), new token_expr(tree_type<token_base *>()),
		                 new token_endline(0)},
		                new method_foreach_do)
		    // Break Grammar
		    .add_method({new token_action(action_types::break_), new token_endline(0)}, new method_break)
		    // Continue Grammar
		    .add_method({new token_action(action_types::continue_), new token_endline(0)},
		                new method_continue)
		    // Function Grammar
		    .add_method({new token_action(action_types::function_), new token_expr(tree_type<token_base *>()),
		                 new token_endline(0)},
		                new method_function)
		    .add_method({new token_action(action_types::function_), new token_expr(tree_type<token_base *>()),
		                 new token_action(action_types::override_), new token_endline(0)},
		                new method_function)
		    // Return Grammar
		    .add_method({new token_action(action_types::return_), new token_expr(tree_type<token_base *>()),
		                 new token_endline(0)},
		                new method_return)
		    .add_method({new token_action(action_types::return_), new token_endline(0)},
		                new method_return_no_value)
		    // Struct Grammar
		    .add_method({new token_action(action_types::struct_), new token_expr(tree_type<token_base *>()),
		                 new token_endline(0)},
		                new method_struct)
		    .add_method({new token_action(action_types::struct_), new token_expr(tree_type<token_base *>()),
		                 new token_action(action_types::extends_), new token_expr(tree_type<token_base *>()),
		                 new token_endline(0)},
		                new method_struct)
		    // Try Grammar
		    .add_method({new token_action(action_types::try_), new token_endline(0)}, new method_try)
		    // Catch Grammar
		    .add_method({new token_action(action_types::catch_), new token_expr(tree_type<token_base *>()),
		                 new token_endline(0)},
		                new method_catch)
		    // Throw Grammar
		    .add_method({new token_action(action_types::throw_), new token_expr(tree_type<token_base *>()),
		                 new token_endline(0)},
		                new method_throw);
		// Init Runtime
		context->instance->storage
		    // Internal Types
		    .add_buildin_type("char", []() -> var
		{ return var::make<char>('\0'); }, typeid(char), cs_impl::char_ext)
		    .add_buildin_type("number", []() -> var
		{ return var::make<numeric>(0); }, typeid(numeric), cs_impl::number_ext)
		    .add_buildin_type("integer", []() -> var
		{ return var::make<numeric>(0); }, typeid(numeric))
		    .add_buildin_type("float", []() -> var
		{ return var::make<numeric>(0.0); }, typeid(numeric))
		    .add_buildin_type("boolean", []() -> var
		{ return var::make<boolean>(true); }, typeid(boolean))
		    .add_buildin_type("pointer", []() -> var
		{ return var::make<pointer>(null_pointer); }, typeid(pointer))
		    .add_buildin_type("string", []() -> var
		{ return var::make<string>(); }, typeid(string), cs_impl::string_ext)
		    .add_buildin_type("list", []() -> var
		{ return var::make<list>(); }, typeid(list), cs_impl::list_ext)
		    .add_buildin_type("array", []() -> var
		{ return var::make<array>(); }, typeid(array), cs_impl::array_ext)
		    .add_buildin_type("pair", []() -> var
		{ return var::make<pair>(numeric(0), numeric(0)); }, typeid(pair), cs_impl::pair_ext)
		    .add_buildin_type("hash_set", []() -> var
		{ return var::make<hash_set>(); }, typeid(hash_set), cs_impl::hash_set_ext)
		    .add_buildin_type("hash_map", []() -> var
		{ return var::make<hash_map>(); }, typeid(hash_map), cs_impl::hash_map_ext)
		    // Context
		    .add_buildin_var("context", var::make_constant<context_type *>(context.get()))
		    // Add Internal Functions to storage
		    .add_buildin_var("range", var::make_protect<callable>(range, callable::types::request_fold))
		    .add_buildin_var("to_integer", make_cni(to_integer, true))
		    .add_buildin_var("to_string", make_cni(to_string, true))
		    .add_buildin_var("type", make_cni(type, true))
		    .add_buildin_var("clone", make_cni(clone))
		    .add_buildin_var("move", make_cni(move))
		    .add_buildin_var("swap", make_cni(swap, true))
		    .add_buildin_var("is_a", make_cni(is_a, true))
		    // Add extensions to storage
		    .add_buildin_var("exception", make_namespace(cs_impl::except_ext))
		    .add_buildin_var("iostream", make_namespace(cs_impl::iostream_ext))
		    .add_buildin_var("system", make_namespace(cs_impl::system_ext))
		    .add_buildin_var("future", make_namespace(cs_impl::future_ext))
		    .add_buildin_var("fiber", make_namespace(cs_impl::fiber_ext))
		    .add_buildin_var("runtime", make_namespace(cs_impl::runtime_ext))
		    .add_buildin_var("math", make_namespace(cs_impl::math_ext));
		return context;
	}

	context_t create_subcontext(context_type *cxt)
	{
		context_t context = std::make_shared<context_type>();
		// A subcontext (module import) shares the parent's process.
		context->process = cxt->process;
		{
			process_run_scope scope(context); // transparent: same process
			cs_impl::init_extensions();
		}
		context->instance = std::make_shared<instance_type>(context.get(), cxt->instance->fiber_stack,
		                                                    context->process ? context->process->stack_size : COVSCRIPT_STACK_PRESERVE);
		context->compiler = cxt->compiler;
		context->cmd_args = cxt->cmd_args;
		context->current_unit = std::make_shared<compile_unit>();
		// Init Runtime
		context->instance->storage
		    // Internal Types
		    .add_buildin_type("char", []() -> var
		{ return var::make<char>('\0'); }, typeid(char), cs_impl::char_ext)
		    .add_buildin_type("number", []() -> var
		{ return var::make<numeric>(0); }, typeid(numeric))
		    .add_buildin_type("integer", []() -> var
		{ return var::make<numeric>(0); }, typeid(numeric))
		    .add_buildin_type("float", []() -> var
		{ return var::make<numeric>(0.0); }, typeid(numeric))
		    .add_buildin_type("boolean", []() -> var
		{ return var::make<boolean>(true); }, typeid(boolean))
		    .add_buildin_type("pointer", []() -> var
		{ return var::make<pointer>(null_pointer); }, typeid(pointer))
		    .add_buildin_type("string", []() -> var
		{ return var::make<string>(); }, typeid(string), cs_impl::string_ext)
		    .add_buildin_type("list", []() -> var
		{ return var::make<list>(); }, typeid(list), cs_impl::list_ext)
		    .add_buildin_type("array", []() -> var
		{ return var::make<array>(); }, typeid(array), cs_impl::array_ext)
		    .add_buildin_type("pair", []() -> var
		{ return var::make<pair>(numeric(0), numeric(0)); }, typeid(pair), cs_impl::pair_ext)
		    .add_buildin_type("hash_set", []() -> var
		{ return var::make<hash_set>(); }, typeid(hash_set), cs_impl::hash_set_ext)
		    .add_buildin_type("hash_map", []() -> var
		{ return var::make<hash_map>(); }, typeid(hash_map), cs_impl::hash_map_ext)
		    // Context
		    .add_buildin_var("context", var::make_constant<context_type *>(context.get()))
		    // Add Internal Functions to storage
		    .add_buildin_var("range", var::make_protect<callable>(range, callable::types::request_fold))
		    .add_buildin_var("to_integer", make_cni(to_integer, true))
		    .add_buildin_var("to_string", make_cni(to_string, true))
		    .add_buildin_var("type", make_cni(type, true))
		    .add_buildin_var("clone", make_cni(clone))
		    .add_buildin_var("move", make_cni(move))
		    .add_buildin_var("swap", make_cni(swap, true))
		    .add_buildin_var("is_a", make_cni(is_a, true))
		    // Add extensions to storage
		    .add_buildin_var("exception", make_namespace(cs_impl::except_ext))
		    .add_buildin_var("iostream", make_namespace(cs_impl::iostream_ext))
		    .add_buildin_var("system", make_namespace(cs_impl::system_ext))
		    .add_buildin_var("future", make_namespace(cs_impl::future_ext))
		    .add_buildin_var("fiber", make_namespace(cs_impl::fiber_ext))
		    .add_buildin_var("runtime", make_namespace(cs_impl::runtime_ext))
		    .add_buildin_var("math", make_namespace(cs_impl::math_ext));
		return context;
	}

	cs::var eval(const context_t &context, const std::string &expr)
	{
		process_run_scope scope(context);
		// Fresh arena so repeated eval() calls don't accumulate tokens.
		compile_unit_guard guard(context.get());
		tree_type<cs::token_base *> tree;
		std::deque<char> buff;
		for (auto &ch : expr)
			buff.push_back(ch);
		context->compiler->build_expr(buff, tree);
		return context->instance->parse_expr(tree.root());
	}
} // namespace cs
