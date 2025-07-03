#pragma once
#include <unordered_map>

namespace Tridium {

	template<typename _Key, typename _Value,
		typename Hash = std::hash<_Key>,
		typename KeyEqual = std::equal_to<_Key>,
		typename _Allocator = std::allocator<std::pair<const _Key, _Value>>>
	using UnorderedMap = std::unordered_map<_Key, _Value, Hash, KeyEqual, _Allocator>;
}