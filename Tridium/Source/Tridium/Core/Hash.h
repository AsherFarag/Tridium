#pragma once

namespace Tridium {

	namespace HashAlgorithms {
		constexpr size_t fnv1a( const char* str, size_t hash = 2166136261U )
		{
			return *str ? fnv1a( str + 1, ( hash ^ static_cast<size_t>( *str ) ) * 16777619U ) : hash;
		}
	}



	inline constexpr size_t HashString( const char* str ) 
	{
		return HashAlgorithms::fnv1a( str );
	}

	inline constexpr size_t operator"" _H( const char* str, size_t ) 
	{
		return HashAlgorithms::fnv1a( str );
	}

	namespace Hashing {

		template<typename T>
		inline constexpr size_t Hash( const T& a_Value )
		{
			return 0;
		}

		inline constexpr size_t HashString( const char* str )
		{
			return HashAlgorithms::fnv1a( str );
		}

		inline constexpr size_t HashCombine( size_t lhs, size_t rhs )
		{
			return lhs ^ ( rhs + 0x9e3779b9 + ( lhs << 6 ) + ( lhs >> 2 ) );
		}
	}
}