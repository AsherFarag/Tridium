#pragma once
#include "Types.h"
#include <Tridium/Utils/TypeTraits.h>

namespace Tridium {

	using hash_t = uint32_t;

	namespace HashAlgorithms {
		constexpr hash_t fnv1a( const char* a_String, hash_t a_Hash = 2166136261U )
		{
			for ( ; *a_String; ++a_String )
			{
				a_Hash ^= *a_String;
				a_Hash *= 16777619U;
			}
			return a_Hash;
		}

		constexpr hash_t fnv1a( const char* a_Data, size_t a_Length, hash_t a_Hash = 2166136261U )
		{
			for ( size_t i = 0; i < a_Length; ++i )
			{
				a_Hash ^= a_Data[i];
				a_Hash *= 16777619U;
			}
			return a_Hash;
		}
	}

	namespace Hashing {

		inline constexpr hash_t Hash( const char* a_Data, size_t a_Length )
		{
			return HashAlgorithms::fnv1a( a_Data, a_Length );
		}

		inline constexpr hash_t HashString( const char* a_String )
		{
			return HashAlgorithms::fnv1a( a_String );
		}

		inline constexpr hash_t HashCombine( hash_t a_Left, hash_t a_Right )
		{
			return a_Left ^ ( a_Right + 0x9e3779b9 + ( a_Left << 6 ) + ( a_Left >> 2 ) );
		}

		template <typename T>
		constexpr hash_t TypeHash()
		{
			return Hash( StrippedTypeName<T>().data(), StrippedTypeName<T>().size() );
		}

	}

	inline constexpr hash_t operator"" _H( const char* a_String, size_t a_Length )
	{
		return Hashing::Hash( a_String, a_Length );
	}
}