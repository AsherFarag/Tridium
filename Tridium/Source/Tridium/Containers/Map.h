#pragma once
#include <map>
#include <unordered_map>
#include "String.h"

namespace Tridium {

	namespace Detail {

		template<typename... _Bases>
		struct Overload : _Bases... 
		{
			using is_transparent = void;
			using _Bases::operator()...;
		};

		struct CharPtrHash
		{
			auto operator()(const char* a_String) const noexcept -> std::size_t
			{
				return std::hash<StringView>{}(a_String);
			}
		};
	}

	using TransparentStringHash = Detail::Overload<
		std::hash<String>,
		std::hash<StringView>,
		Detail::CharPtrHash
	>;

	template<typename _Key, typename _Value,
		typename _Compare = std::less<_Key>,
		typename _Allocator = std::allocator<std::pair<const _Key, _Value>>>
	using Map = std::map<_Key, _Value, _Compare, _Allocator>;

	template<typename _Key, typename _Value,
		typename Hash = std::hash<_Key>,
		typename KeyEqual = std::equal_to<_Key>,
		typename _Allocator = std::allocator<std::pair<const _Key, _Value>>>
	using UnorderedMap = std::unordered_map<_Key, _Value, Hash, KeyEqual, _Allocator>;
}