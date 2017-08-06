#pragma once
namespace cs_impl {
	class any;
}
namespace cs {
	class token_base;
	class statement_base;
	class callable;
	class name_space;
	class name_space_holder;
	using var=cs_impl::any;
	using number=long double;
	using boolean=bool;
	using string=std::string;
	using list=std::list<var>;
	using array=std::deque<var>;
	using pair=std::pair<var,var>;
	using hash_map=std::unordered_map<var,var>;
	using native_interface=callable;
	using extension=name_space;
	using extension_holder=name_space_holder;
	using name_space_t=std::shared_ptr<name_space_holder>;
	using extension_t=std::shared_ptr<extension_holder>;
}
