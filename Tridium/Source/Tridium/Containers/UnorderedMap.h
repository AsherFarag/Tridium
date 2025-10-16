#pragma once
#include <unordered_map>
#include <Tridium/Utils/TypeTraits.h>

namespace Tridium {

	template<typename _Key, typename _Value,
		typename Hash = Hash<_Key>,
		typename KeyEqual = Equal<_Key>,
		typename _Allocator = Allocator<std::pair<const _Key, _Value>>>
	using UnorderedMap = std::unordered_map<_Key, _Value, Hash, KeyEqual, _Allocator>;
}