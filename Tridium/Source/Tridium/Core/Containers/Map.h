#pragma once
#include <map>
#include <unordered_map>

namespace Tridium {

	template<typename Key, typename Value>
	using Map = std::map<Key, Value>;

	template<typename Key, typename Value>
	using UnorderedMap = std::unordered_map<Key, Value>;

}