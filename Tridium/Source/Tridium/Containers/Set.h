#pragma once
#include <set>
#include <unordered_set>

namespace Tridium {

	template <typename _Elem, typename _Pred = std::less<_Elem>>
	using Set = std::set<_Elem, _Pred>;

	template <typename _Elem, typename _Hash = std::hash<_Elem>, typename _Pred = std::equal_to<_Elem>>
	using UnorderedSet = std::unordered_set<_Elem, _Hash, _Pred>;

}