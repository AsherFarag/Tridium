#pragma once
#include <stack>
#include "Array.h"

namespace Tridium {

	template<typename _Value, typename _Container = std::vector<_Value>>
	using Stack = std::stack<_Value, _Container>;

}