#pragma once
#include <set>

namespace Tridium {

	template <typename _Elem, typename _Pred = std::less<_Elem>>
	using Set = std::set<_Elem, _Pred>;

} // namespace Tridium