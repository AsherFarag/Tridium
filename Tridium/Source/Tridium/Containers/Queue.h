#pragma once
#include <queue>

namespace Tridium
{
	//=====================================================
	// Queue
	//  A simple wrapper around std::queue.
	//=====================================================
	template<typename T, typename _Container = std::deque<T>>
	using Queue = std::queue<T, _Container>;

	//=====================================================
	// Priority Queue
	//  A simple wrapper around std::priority_queue.
	//=====================================================
	template<typename T, typename _Container = std::priority_queue<T>::container_type, typename _Compare = std::less<T>>
	using PriorityQueue = std::priority_queue<T, _Container, _Compare>;
}