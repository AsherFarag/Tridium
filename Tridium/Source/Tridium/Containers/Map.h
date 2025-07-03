#pragma once
#include <map>

namespace Tridium {

	template<typename _Key, typename _Value,
		typename _Compare = std::less<_Key>,
		typename _Allocator = std::allocator<std::pair<const _Key, _Value>>>
	using Map = std::map<_Key, _Value, _Compare, _Allocator>;

	template<typename _Key, typename _Value,
		typename _Compare = std::less<_Key>,
		typename _Allocator = std::allocator<std::pair<const _Key, _Value>>>
	using Multimap = std::multimap<_Key, _Value, _Compare, _Allocator>;
}