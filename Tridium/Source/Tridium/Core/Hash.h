#pragma once
#include "Types.h"
#include <Tridium/Utils/TypeTraits.h>

namespace Tridium {

	using hash_t = uint32_t;

	namespace HashAlgorithms {
		constexpr hash_t fnv1a( const char* str, hash_t hash = 2166136261U )
		{
			for ( ; *str; ++str )
			{
				hash ^= *str;
				hash *= 16777619U;
			}
			return hash;
		}
	}

	namespace Hashing {

		inline constexpr hash_t Hash( const char* str )
		{
			return HashAlgorithms::fnv1a( str );
		}

		inline constexpr hash_t HashString( const char* str )
		{
			return HashAlgorithms::fnv1a( str );
		}

		inline constexpr hash_t HashCombine( hash_t lhs, hash_t rhs )
		{
			return lhs ^ ( rhs + 0x9e3779b9 + ( lhs << 6 ) + ( lhs >> 2 ) );
		}

		template <typename T>
		constexpr hash_t TypeHash()
		{
			return Hash( StrippedTypeName<T>().data() );
		}

	}

	inline constexpr hash_t operator"" _H( const char* str, size_t )
	{
		return Hashing::Hash( str );
	}
}