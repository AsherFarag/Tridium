#pragma once
#include <deque>

namespace Tridium
{
	//=====================================================
	// Deque
	//  A simple wrapper around std::deque.
	//  A deque is a double-ended queue that allows fast insertion and deletion at both ends.
	template<typename T, typename _Allocator = std::allocator<T>>
	using Deque = std::deque<T, _Allocator>;

}