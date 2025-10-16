#pragma once
#include <unordered_set>
#include <Tridium/Utils/TypeTraits.h>

namespace Tridium {

	template <typename _Elem, typename _Hash = Hash<_Elem>, typename _Pred = Equal<_Elem>>
	using UnorderedSet = std::unordered_set<_Elem, _Hash, _Pred>;

} // namespace Tridium