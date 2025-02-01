#pragma once
#include <variant>

namespace Tridium {

	template<typename... _Types>
	using Variant = std::variant<_Types...>;

}