#pragma once
namespace cs {
	class name_space;
	class name_space_holder;
	using extension=name_space;
	using extension_holder=name_space_holder;
	using name_space_t=std::shared_ptr<name_space_holder>;
	using extension_t=std::shared_ptr<extension_holder>;
}
