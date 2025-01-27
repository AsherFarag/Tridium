#pragma once
#include <tuple>

namespace Tridium {

	template<typename _Type1, typename _Type2>
	using Pair = std::pair<_Type1, _Type2>;

	template<typename... _Types>
	using Tuple = std::tuple<_Types...>;

}